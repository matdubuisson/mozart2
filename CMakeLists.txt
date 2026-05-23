cmake_minimum_required(VERSION 4.0)

# Logger functions to make a nice prompt
set(__LOG_INDENT "")

function(log _message)
  message(STATUS "${__LOG_INDENT}| ${_message}")
endfunction()

function(debug_log _message)
  log("${_message}")
endfunction()

function(warn _message)
  message(WARNING "${__LOG_INDENT}${_message}")
endfunction()

function(log_section _section)
  log("\t[${_section}]")
endfunction()

macro(add_subdirectory_and_log _directory)
  set(_previous_log_indent "${__LOG_INDENT}")
  set(__LOG_INDENT "${__LOG_INDENT}  ")
  set(_title "________________[${_directory}]________________")
  string(LENGTH "${_title}" _title_length)

  message(STATUS "${_previous_log_indent} \\")
  log("${_title}")

  add_subdirectory("${_directory}")

  string(REPEAT "_" ${_title_length} _end_title)
  log("${_end_title}")
  message(STATUS "${_previous_log_indent} /")

  set(__LOG_INDENT "${_previous_log_indent}")
endmacro()

log_section("General properties")

# This needs to be before "project(..)".
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release")
endif()
log("Build type: ${CMAKE_BUILD_TYPE}")

project(MOZART CXX)

ENABLE_TESTING()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
log("CTest command: ${CMAKE_CTEST_COMMAND}")

# Custom CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_local ${CMAKE_MODULE_PATH})
log("CMake module path: ${CMAKE_MODULE_PATH}")

include(MozartProperties)

# Set build options
log_section("Build configuration")

# Set variables that the submodules will need to find me
set(MOZART_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MOZART_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

log("Mozart directory: ${MOZART_DIR}")
log("Mozart build directory: ${MOZART_BUILD_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DEFAULT_MOZART_GENERATOR_FLAGS
      "--std=c++17"
      "-Wno-invalid-noreturn"
      "-Wno-return-type"
      "-Wno-braced-scalar-init" # Silence warning for macro ATOMIC_FLAG_INIT in atomic_base.h
   )

log("CMake CXX standard: ${CMAKE_CXX_STANDARD}")
log("CMake CXX standard required: ${CMAKE_CXX_STANDARD_REQUIRED}")
log("Default Mozart generator flags: ${DEFAULT_MOZART_GENERATOR_FLAGS}")

# TO-DODO : Test new versions on windows
# # Some helpers when using MinGW
# if(MINGW)
#   get_filename_component(CMAKE_MAKE_PROGRAM_PATH "${CMAKE_MAKE_PROGRAM}" PATH)
#   get_filename_component(CMAKE_MAKE_PROGRAM_PATH_PARENT "${CMAKE_MAKE_PROGRAM_PATH}" PATH)

#   set(MINGW_ROOT "${CMAKE_MAKE_PROGRAM_PATH_PARENT}"
#       CACHE PATH "Path where MinGW is installed")
#   set(MINGW_COMPILER_VERSION "4.9.1"
#       CACHE STRING "Version of GCC in your MinGW installation")

#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format -Wno-format-extra-args")
#   # Linking mingw libs statically
#   set(CMAKE_CXX_STANDARD_LIBRARIES "-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive, -static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CSS_STANDARD_LIBRARIES}")

#   # Check for gcc target architecture
#   execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine OUTPUT_VARIABLE GCC_ARCH)
#   string(STRIP "${GCC_ARCH}" GCC_ARCH_TRIPLE)
#   message(STATUS "Target architecture : ${GCC_ARCH_TRIPLE}")

#   set(DEFAULT_MOZART_GENERATOR_FLAGS
#         "-I${MINGW_ROOT}/${GCC_ARCH_TRIPLE}/include/c++"
#         "-I${MINGW_ROOT}/${GCC_ARCH_TRIPLE}/include/c++/${GCC_ARCH_TRIPLE}"
#         "-I${MINGW_ROOT}/${GCC_ARCH_TRIPLE}/include"
#         ${DEFAULT_MOZART_GENERATOR_FLAGS}
#      )
  
#   set(BOOST_ROOT "${MINGW_ROOT}"
#       CACHE PATH "Path where Boost is installed")

#   # Configuration for resources files
#     set(CMAKE_RC_COMPILER_INIT windres)
#       enable_language(RC)
#         set(CMAKE_RC_COMPILE_OBJECT
# 	    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
# endif()

# # Some configuration

# if(WIN32)
#   add_definitions(-DMOZART_WINDOWS=1)
#   set(DEFAULT_MOZART_GENERATOR_FLAGS "-DMOZART_WINDOWS=1" ${DEFAULT_MOZART_GENERATOR_FLAGS})
# endif()

# CMake does not allow to add dependencies to "builtin" targets like package_source.
# We therefore wrap package_source in our own dist target.

if("${MOZART_BUILD_DIR}/" MATCHES "${MOZART_DIR}/.*")
    log("Building inside the source tree: 'dist' target disabled")
    set(DIST_TARGET_ENABLED FALSE)
else()
    log("Building outside the source tree: 'dist' target enabled")
    add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
    set(DIST_TARGET_ENABLED TRUE)
    configure_file(cmake_local/MozartConfigVersion.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake_local/MozartConfigVersion.cmake @ONLY)
    define_property(GLOBAL PROPERTY SRCS_INSTALLED_DIRS
        BRIEF_DOCS "List of source files"
        FULL_DOCS "List of source/dest pairs of path to be included in the source archive")
    set_property(GLOBAL APPEND PROPERTY SRCS_INSTALLED_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}" "/"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_local/" "/cmake_local/")
    log("SRCS installed dirs: ${SRCS_INSTALLED_DIRS}")
endif()

# The subdirectories which contain the real thing

add_subdirectory_and_log(vm)
add_subdirectory_and_log(bootcompiler)
add_subdirectory_and_log(boosthost)
add_subdirectory_and_log(lib)
add_subdirectory_and_log(opi)
add_subdirectory_and_log(wish)
add_subdirectory_and_log(stdlib)
# add_subdirectory_and_log(platform-test EXCLUDE_FROM_ALL) # FIX LATER

# Add launcher and icons
if(UNIX)
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/distrib/share/" DESTINATION share)
endif()

# General CPack configuration

log_section("CPack package configuration")

string(TOLOWER "${CMAKE_SYSTEM_NAME}" MOZART_SYSTEM_NAME)
log("Mozart system name: ${MOZART_SYSTEM_NAME}")

set(CPACK_PACKAGE_FILE_NAME
    "mozart2-${MOZART_PROP_OZ_VERSION}-${MOZART_PROP_PLATFORM_ARCH}-${MOZART_SYSTEM_NAME}")
set(CPACK_PACKAGE_NAME "mozart2")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Mozart Programming System v2")
set(CPACK_PACKAGE_VERSION "${MOZART_PROP_OZ_VERSION}")
set(CPACK_PACKAGE_VENDOR "Université catholique de Louvain")
set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_CONTACT "Sébastien Doeraene <sjrdoeraene@gmail.com>")

log("CPack package file name: ${CPACK_PACKAGE_FILE_NAME}")
log("CPack package name: ${CPACK_PACKAGE_NAME}")
log("CPack package description summary: ${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
log("CPack package version: ${CPACK_PACKAGE_VERSION}")
log("CPack package vendor: ${CPACK_PACKAGE_VENDOR}")
log("CPack package file license: ${CPACK_RESOURCE_FILE_LICENSE}")
log("CPack package contact: ${CPACK_PACKAGE_CONTACT}")

# Configuration of the Bundle generator

log_section("CPack bundle configuration")

set(MACOS_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/macos")
log("MACOS resources directory: ${MACOS_RESOURCES_DIR}")

set(CPACK_BUNDLE_NAME "Mozart2")
set(CPACK_BUNDLE_PLIST "${MACOS_RESOURCES_DIR}/Info.plist")
set(CPACK_BUNDLE_STARTUP_COMMAND "${MACOS_RESOURCES_DIR}/startup")
set(CPACK_BUNDLE_ICON "${MACOS_RESOURCES_DIR}/mozart.icns")

log("CPack bundle name: ${CPACK_BUNDLE_NAME}")
log("CPack bundle plist: ${CPACK_BUNDLE_PLIST}")
log("CPack bundle startup command: ${CPACK_BUNDLE_STARTUP_COMMAND}")
log("CPack bundle icon: ${CPACK_BUNDLE_ICON}")

# Configuration of the DEB generator

log_section("CPack debian generator configuration")

if(${MOZART_PROP_PLATFORM_ARCH} MATCHES "x86_64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
else()
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${MOZART_PROP_PLATFORM_ARCH}")
endif()
log("CPack debian package architecture: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

string(REGEX REPLACE "-(alpha|beta|rc)\\." "~\\1"
       CPACK_DEBIAN_PACKAGE_VERSION "${MOZART_PROP_OZ_VERSION}")
log("CPack debian package version: ${CPACK_DEBIAN_PACKAGE_VERSION}")

set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "emacs")

log("CPack debian package section: ${CPACK_DEBIAN_PACKAGE_SECTION}")
log("CPack debian package priority: ${CPACK_DEBIAN_PACKAGE_PRIORITY}")
log("CPack debian package shlibdeps: ${CPACK_DEBIAN_PACKAGE_SHLIBDEPS}")
log("CPack debian package depends: ${CPACK_DEBIAN_PACKAGE_DEPENDS}")

# Configuration of the RPM generator

log_section("CPack rpm generator configuration")

set(CPACK_RPM_PACKAGE_ARCHITECTURE "${MOZART_PROP_PLATFORM_ARCH}")
set(CPACK_RPM_PACKAGE_AUTOREQ yes)
set(CPACK_RPM_PACKAGE_REQUIRES "emacs")
set(CPACK_RPM_PACKAGE_GROUP "Development/Languages")

log("CPack rpm package architecture: ${CPACK_RPM_PACKAGE_ARCHITECTURE}")
log("CPack rpm package autoreq: ${CPACK_RPM_PACKAGE_AUTOREQ}")
log("CPack rpm package requires: ${CPACK_RPM_PACKAGE_REQUIRES}")
log("CPack rpm package group: ${CPACK_RPM_PACKAGE_GROUP}")

# Configuration of the Source generator

log_section("CPack source generator")

set(CPACK_SOURCE_GENERATOR "ZIP")
get_property(CPACK_SOURCE_INSTALLED_DIRECTORIES GLOBAL PROPERTY SRCS_INSTALLED_DIRS)
set(CPACK_SOURCE_IGNORE_FILES
    "/\\\\.git"
    "\\\\.astbi$"
    "\\\\.ast$"
    "/bootcompiler/project/project/"
    "/bootcompiler/project/target/"
    "/bootcompiler/target/"
)

log("CPack source generator: ${CPACK_SOURCE_GENERATOR}")
log("CPack source ignore files: ${CPACK_SOURCE_IGNORE_FILES}")

# TODO: Test on Windows new configuration
# # Configuration of Inno Setup files

# if(WIN32)
#   find_program(ISS_COMPILER NAMES iscc ISCC
#                HINTS "C:/Program Files (x86)/Inno Setup 5" "C:/Program Files/Inno Setup 5")

#   if(NOT ISS_COMPILER)
#     message(WARNING "Inno Setup Compiler not found. You won't be able to build setup files.")
#   else()
#     message(STATUS "Using Inno Setup Compiler from: ${ISS_COMPILER}")
#     file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/distrib/windows"
#          DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distrib")

#     # Compute Tcl/Tk install path
#     get_filename_component(ISS_TCL_EXEC_PARENT "${ISS_TCL_EXEC}" DIRECTORY)
#     get_filename_component(ISS_TCL_PATH "${ISS_TCL_EXEC_PARENT}" DIRECTORY)

#     # Compute emacs install path
#     get_filename_component(ISS_EMACS_EXEC_PARENT "${ISS_EMACS_EXEC}" DIRECTORY)
#     get_filename_component(ISS_EMACS_PATH "${ISS_EMACS_EXEC_PARENT}" DIRECTORY)

#     # Parse Inno Setup config file
#     configure_file("${CMAKE_CURRENT_SOURCE_DIR}/distrib/windows/MozartConfig.iss"
#                    "${CMAKE_CURRENT_BINARY_DIR}/distrib/windows/MozartConfig.iss")

#     # Add installer target
#     add_custom_target(installer
#                       COMMAND ${CMAKE_MAKE_PROGRAM} install
#                       COMMAND ${ISS_COMPILER} "${CMAKE_CURRENT_BINARY_DIR}/distrib/windows/MozartSetup.iss"
#                       VERBATIM)
#   endif()
# endif()

# Finally include CPack
log_section("CPack")
log("Include CPack")
include(CPack)
