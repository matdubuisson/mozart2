cmake_minimum_required(VERSION 4.0)
project(MOZARTVM)

include(ExternalProject)

log_section("EP BASE")

set(ep_base "${CMAKE_BINARY_DIR}/externals")
set_property(DIRECTORY PROPERTY "EP_BASE" ${ep_base})
log("ep_base: ${ep_base}")

log_section("LLVM paths")

set(DEFAULT_LLVM_SRC_DIR "${ep_base}/Source/llvm")
set(DEFAULT_LLVM_BUILD_DIR "${ep_base}/Build/llvm")
set(LLVM_SRC_DIR ${DEFAULT_LLVM_SRC_DIR} CACHE PATH "Path to LLVM sources (with Clang)")
set(LLVM_BUILD_DIR ${DEFAULT_LLVM_BUILD_DIR} CACHE PATH "Path to LLVM build (with Clang)")

log("Default LLVM source directory: ${DEFAULT_LLVM_SRC_DIR}")
log("Default LLVM build directory: ${DEFAULT_LLVM_BUILD_DIR}")
log("LLVM source directory: ${LLVM_SRC_DIR}")
log("LLVM build directory: ${LLVM_BUILD_DIR}")

log_section("Mozart generator options")

set(MOZART_GENERATOR_FLAGS "${DEFAULT_MOZART_GENERATOR_FLAGS}" CACHE STRING
    "Additional flags for the generator parser (clang)")
set(MOZART_CACHED_BUILD ON CACHE STRING "Use pre-generated sources")

log("Mozart generator flags: ${MOZART_GENERATOR_FLAGS}")
log("Mozart cached build: ${MOZART_CACHED_BUILD}")

add_compile_options(-std=c++17)

add_subdirectory_and_log(generator)

add_compile_options(-Wall)

add_subdirectory_and_log(vm)
add_subdirectory_and_log(boostenv)
