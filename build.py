#! /usr/bin/env python3

import os, sys
import subprocess
import multiprocessing
import argparse

def set_ASAN_flags(cmake_config_cmd):
    # OSX
    if 'darwin' in sys.platform:
        cmake_config_cmd += [ '-DCMAKE_CXX_FLAGS='         + '-fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer',
                              '-DCMAKE_EXE_LINKER_FLAGS='  + '-fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer' ]
    # LINUX
    else:
        cmake_config_cmd += [ '-DCMAKE_CXX_FLAGS='         + '-fsanitize=address -fsanitize=leak -fno-omit-frame-pointer',
                              '-DCMAKE_EXE_LINKER_FLAGS='  + '-fsanitize=address -fsanitize=leak -fno-omit-frame-pointer' ]

def set_TSAN_flags(cmake_config_cmd):
    cmake_config_cmd += [ '-DCMAKE_CXX_FLAGS='         + '-fsanitize=thread',
                          '-DCMAKE_EXE_LINKER_FLAGS='  + '-fsanitize=thread' ]

def join_with_quotes(cmd):
    result=""
    for a in cmd:
        if result != "":
            result += " "
        if " " in a or ";" in a:
            result += "\"" + a + "\""
        else:
            result += a
    return result

def run_cmd(cmd, working_directory):
    global prev_working_directory
    global dry_run

    if dry_run:
        if working_directory!=prev_working_directory:
            prev_working_directory=working_directory
            print(join_with_quotes(["cd",working_directory]))
        print(join_with_quotes(cmd))
        return

    current_dir = os.getcwd()
    os.chdir(working_directory)
    print('\n####################################################################################################')
    print('# Running ' + ' '.join(cmd))
    print('# working_directory:' + os.getcwd())
    print('####################################################################################################\n')

    try:
        output = subprocess.check_call(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print('"' + ' '.join(e.cmd) + '" failed with return code ' + str(e.returncode))
        sys.exit(e.returncode)
    os.chdir(current_dir)

def build_external_lib(src_dir, build_dir, configuration, osx_target, extra_config_options, enable_ASAN, enable_TSAN):
    if configuration == 'RelWithAssert': # not available for dependencies, so build release
        configuration = 'Release'

    if 'darwin' in sys.platform or 'linux' in sys.platform:
        cmake_config_cmd = ['cmake',
                            src_dir,
                            '-DCMAKE_BUILD_TYPE='+configuration
                           ]
        if 'darwin' in sys.platform:
            cmake_config_cmd += ['-DCMAKE_OSX_DEPLOYMENT_TARGET='+osx_target]

        if enable_TSAN:
            set_TSAN_flags(cmake_config_cmd)
        if enable_ASAN:
            set_ASAN_flags(cmake_config_cmd)

        cmake_build_cmd = ['make', '-j'+str(multiprocessing.cpu_count())]

    else:
        cmake_config_cmd = ['cmake',
                            src_dir,
                            '-DCMAKE_BUILD_TYPE='+configuration,
                           ]

        cmake_build_cmd = ['cmake',
                           '--build', '.', # working directory is set in run_cmd
                           '--config', configuration,
                           '--',
                           '/m'
                           ]

    cmake_config_cmd += extra_config_options
    run_cmd(cmake_config_cmd, build_dir)
    run_cmd(cmake_build_cmd, build_dir)

def build_source(src_dir, build_dir, external_build_dir, configuration, osx_target, extra_config_options, enable_ASAN, enable_TSAN):
    cmake_config_cmd = ['cmake',
                        src_dir,
                        '-DCMAKE_BUILD_TYPE='+configuration,
                        '-DCMAKE_PREFIX_PATH=' + os.path.join(external_build_dir,'install') ]

    if 'darwin' in sys.platform or 'linux' in sys.platform:
        if 'darwin' in sys.platform:
            cmake_config_cmd += ['-DCMAKE_OSX_DEPLOYMENT_TARGET='+osx_target]

        if enable_TSAN:
            set_TSAN_flags(cmake_config_cmd)
        if enable_ASAN:
            set_ASAN_flags(cmake_config_cmd)

        cmake_build_cmd = ['make', '-j'+str(multiprocessing.cpu_count())]
    else:
        cmake_build_cmd = ['cmake',
                           '--build', '.', #working directory is set in run_cmd
                           '--config', configuration,
                           '--',
                           '/m']

    cmake_config_cmd += extra_config_options

    run_cmd(cmake_config_cmd, build_dir)
    run_cmd(cmake_build_cmd, build_dir)

if __name__ == "__main__":
    global prev_working_directory
    prev_working_directory=""
    global dry_run
    dry_run=True
    build_type = 'Release'
    source_dir = os.path.dirname(os.path.abspath(__file__))
    external_libs_dir = os.path.join(source_dir, 'external')
    build_dir = os.path.join(source_dir, 'build')
    external_build_dir = os.path.join(source_dir, 'build_external') # dependencies need to live outside the build_dir as DlbFixupBundle
                                                                           # cannot resolve rpath otherwise... strange,
                                                                           # we need to investigate why that is

    example_call = 'build.py --build_dir ' + build_dir  + ' --external_build_dir ' + external_build_dir + ' --build_type ' + build_type

    parser = argparse.ArgumentParser(description='Convenient script to build this project', usage=example_call)
    parser.add_argument('--build_type',
                        help='Release | Debug',
                        default='Release')
    parser.add_argument('--external_build_dir',
                        help='directory where to build external libraries (default: ' + external_build_dir +')',
                        default=external_build_dir)
    parser.add_argument('--build_dir',
                        help='directory where to build the project (default: ' + build_dir +')',
                        default=build_dir)
    parser.add_argument('--dry_run',
                        help='print the commands that would be executed without actually executing them',
                        action='store_true')
    parser.add_argument('--',
                        nargs='*',
                        dest='extra_cmake_args',
                         metavar = 'CMAKE_ARGUMENT',
                         help='extra arguments for cmake')

    if sys.platform in ['darwin']:
        out, err = subprocess.Popen(['/usr/bin/xcrun', '--show-sdk-version'],stdout=subprocess.PIPE,text=True).communicate()
        osx_target = out.strip()
        parser.add_argument('--osx_target',
                            help='osx deployment target (default: ' + osx_target +')',
                            default=osx_target)
        parser.add_argument('--enable_ASAN',
                            help='enable address sanitizer',
                            action='store_true')
        parser.add_argument('--enable_TSAN',
                            help='enable thread sanitizer',
                            action='store_true')

    # separate arguments for the script and arguments for cmake (passed after a "--" separator)
    all_arguments = sys.argv[1:]
    cmake_args_separator_index = all_arguments.index("--") if "--" in all_arguments else len(all_arguments)
    extra_config_options = all_arguments[cmake_args_separator_index+1:]
    args_for_script  = all_arguments[:cmake_args_separator_index]
    args = parser.parse_args(args_for_script)

    build_type = args.build_type
    external_build_dir = os.path.abspath(args.external_build_dir)
    build_dir = os.path.abspath(args.build_dir)
    dry_run = args.dry_run

    osx_target   = None
    enable_ASAN  = False
    enable_TSAN  = False

    if sys.platform in ['darwin']:
        osx_target   = args.osx_target
        enable_ASAN  = args.enable_ASAN
        enable_TSAN  = args.enable_TSAN


    if enable_ASAN and enable_TSAN:
        print("\n==== Error: ASAN and TSAN can not be set at the same time ====\n")
        sys.exit()

    if dry_run:
        print("#####################################################################################################")

    if dry_run:
        print(join_with_quotes(["mkdir",build_dir]))
        print(join_with_quotes(["mkdir",external_build_dir]))
    else:
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        if not os.path.exists(external_build_dir):
            os.makedirs(external_build_dir)

    build_external_lib(external_libs_dir, external_build_dir, build_type, osx_target, extra_config_options, enable_ASAN, enable_TSAN)
    build_source(source_dir,   build_dir, external_build_dir, build_type, osx_target, extra_config_options, enable_ASAN, enable_TSAN)

    if dry_run:
        print("#####################################################################################################")
    else:
        print('\n####################################################################################################')
        print('BUILD WAS SUCCESSFUL')
        print('external libraries installed in: ' + os.path.join(external_build_dir, 'install'))
        print('source built in: ' + build_dir)
        print('####################################################################################################\n')
