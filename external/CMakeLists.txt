cmake_minimum_required(VERSION 3.12)

if(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "\nERROR: Building in source directory is not allowed. Make sure to remove the generated CMakeCache.txt and CMakeFiles directory and rebuild.")
endif()

project(tcutils_contrib)
include(ExternalProject)
include(ProcessorCount)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
find_package (Python3 COMPONENTS Interpreter REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(PARALLEL_JOBS 1)
ProcessorCount(PARALLEL_JOBS)

if(NOT CMAKE_MODULE_PATH)
    get_filename_component(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules" ABSOLUTE)
endif()


if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(UNIX)
    if(APPLE)
        execute_process(COMMAND /usr/bin/xcrun --show-sdk-path
                         OUTPUT_VARIABLE XCODE_SDK
                         OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(COMMAND /usr/bin/xcrun --show-sdk-version
                         OUTPUT_VARIABLE SDK_VERSION
                         OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
            set(CMAKE_OSX_DEPLOYMENT_TARGET ${SDK_VERSION})
        endif()
        add_compile_options("--system-header-prefix=gmock/")
        add_compile_options("--system-header-prefix=gtest/")
    endif()
    set(CMAKE_GENERATOR "Unix Makefiles")
    set(CMAKE_GENERATOR_TOOLSET "")
    set(BUILD_CMD ${MAKE}) # will inherit number of jobs given in toplevel
    set(INSTALL_CMD make install) # will inherit number of jobs given in toplevel
    set(CMAKE_SKIP_INSTALL_RPATH ON)
    set(CMAKE_SKIP_RPATH ON)
    add_compile_options($<$<CXX_COMPILER_ID:GNU>:-msse3>)
    # skip warnings from external libraries. Warnings can appear when devs use more modern compiler vefrsions, which will break compilation
    list(APPEND CMAKE_CXX_FLAGS "-w")
elseif(WIN32)
    set(BUILD_CMD cmake --build . --config ${CMAKE_BUILD_TYPE} -- /m)
    set(INSTALL_CMD cmake --build . --config ${CMAKE_BUILD_TYPE} --target install -- /m)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
endif(UNIX)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
set(CMAKE_STATIC_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

include_directories(SYSTEM ${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

message(STATUS "+-------------------------------------------------------------------------------")
message(STATUS "| System info")
message(STATUS "+-------------------------------------------------------------------------------")
message(STATUS "| System:            ${CMAKE_SYSTEM}")
message(STATUS "| Processor:         ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "| Processor Count:   ${PARALLEL_JOBS}")
message(STATUS "| CXX Compiler:      ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "| C Compiler:        ${CMAKE_C_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "| Generator:         ${CMAKE_GENERATOR}")
message(STATUS "| Toolset:           ${CMAKE_GENERATOR_TOOLSET}")
if(APPLE)
        message(STATUS "| XCODE SDK:         ${XCODE_SDK}")
        if(CMAKE_OSX_DEPLOYMENT_TARGET)
            message(STATUS "| OSX TARGET:        ${CMAKE_OSX_DEPLOYMENT_TARGET}")
        else()
            message(STATUS "| OSX TARGET:        NOT SET")
        endif()
endif()
message(STATUS "|")
message(STATUS "| CMAKE_BUILD_TYPE:  ${CMAKE_BUILD_TYPE}")
message(STATUS "| CXX_FLAGS:         ${CMAKE_CXX_FLAGS}")
message(STATUS "| CXX_FLAGS_DEBUG:   ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "| CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "| C_FLAGS:           ${CMAKE_C_FLAGS}")
message(STATUS "| C_FLAGS_DEBUG:     ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "| C_FLAGS_RELEASE:   ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "| Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "+-------------------------------------------------------------------------------")

set(DEFAULT_CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                       -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                       -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
                       -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
                       -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
                       -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                       -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                       -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS})

set(using_MD OFF)
if(MSVC)
    foreach(flag
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
           )
        string( FIND "${${flag}}" "/MD" return_str)
        if (NOT (return_str STREQUAL "-1"))
           set (using_MD ON)
        endif()
    endforeach()
endif()

################################################################################################

ExternalProject_Add(
    gmock
    GIT_REPOSITORY          https://github.com/google/googletest.git
    GIT_TAG                 release-1.12.1
    GIT_SHALLOW             TRUE
    PREFIX                  ${CMAKE_CURRENT_BINARY_DIR}/gmock
    INSTALL_DIR             ${CMAKE_INSTALL_PREFIX}
    BUILD_COMMAND           ${BUILD_CMD}
    INSTALL_COMMAND         ${INSTALL_CMD}
    CMAKE_GENERATOR         ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS              ${DEFAULT_CMAKE_ARGS}
                            -DBUILD_GMOCK=OFF
                            -DBUILD_SHARED_LIBS=OFF
                            -Dgtest_force_shared_crt=${using_MD}
    CMAKE_CACHE_ARGS        -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
)
