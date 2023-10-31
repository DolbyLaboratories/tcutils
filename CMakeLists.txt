cmake_minimum_required(VERSION 3.19.2)
project(tcutils VERSION 1.3.0 LANGUAGES "CXX")

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif ()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_MODULE_PATH)
    get_filename_component(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" ABSOLUTE)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    add_compile_options(-Wall -Werror -Wextra -pedantic -Wreturn-type -Wreorder -Wdeprecated-declarations -Wunused -Wswitch)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    add_compile_options(-Wall -Werror -Wextra -pedantic -Wreturn-type -Wreorder -Wdeprecated-declarations -Wunused -Wswitch -Wmost)
elseif (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_USE_MATH_DEFINES)
elseif (XCODE)
    # by default xcode has many warning flags disabled. Enable them!
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_ABOUT_RETURN_TYPE "YES")
    #set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_NEWLINE           "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VALUE "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VARIABLE "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_SIGN_COMPARE "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNUSED_FUNCTION "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNINITIALIZED_AUTOS "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_NON_VIRTUAL_DESTRUCTOR "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_SHADOW "YES")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_PEDANTIC "YES")
    set(GCC_WARN_EFFECTIVE_CPLUSPLUS_VIOLATIONS "YES")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES")
endif ()

if (NOT TARGET GTest::gtest)
    find_package(GTest NO_SYSTEM_ENVIRONMENT_PATH)
endif ()

set(public_headers
    include/${PROJECT_NAME}/Convert.h
    include/${PROJECT_NAME}/FrameBoundary.h
    include/${PROJECT_NAME}/Framerate.h
    include/${PROJECT_NAME}/Timecode.h
    include/${PROJECT_NAME}/TimecodeSamples.h
    include/${PROJECT_NAME}/TimecodeSubframes.h
    include/${PROJECT_NAME}/Types.h
    include/${PROJECT_NAME}/Version.h
    )

set(private_headers
    src/DropFrameHelpers.h
    )

set(sources
        src/Convert.cpp
        src/FrameBoundary.cpp
        src/Framerate.cpp
        src/Timecode.cpp
        src/TimecodeSubframes.cpp
        src/TimecodeSamples.cpp
        src/Version.cpp
        )

add_library(
    ${PROJECT_NAME}
    ${public_headers}
    ${private_headers}
    ${sources}
    )

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
        ${PROJECT_NAME}
        # add dependencies here
)

if (NOT BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif ()

add_library(
        ${PROJECT_NAME}::${PROJECT_NAME}
        ALIAS
        ${PROJECT_NAME}
)

if (GTest_FOUND)
    include(CTest)
    add_subdirectory(test)
endif ()

add_subdirectory(utility)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
    INCLUDES DESTINATION include
)

include(CMakePackageConfigHelpers)
file(
    WRITE
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config.cmake
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-targets.cmake\")"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(
    EXPORT ${PROJECT_NAME}Targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake"
    NAMESPACE ${PROJECT_NAME}::
)

configure_package_config_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION cmake
)

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)
