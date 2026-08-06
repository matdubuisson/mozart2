log_section("Generator config")

find_package(LLVM REQUIRED CONFIG)
log("Found LLVM ${LLVM_PACKAGE_VERSION}")
log("Using LLVMConfig.cmake in: ${LLVM_DIR}")
log("LLVM include directories: ${LLVM_INCLUDE_DIRS}")
log("LLVM definitions: ${LLVM_DEFINITIONS}")

find_package(Clang REQUIRED CONFIG)
log("Found Clang using ClangConfig.cmake in: ${Clang_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(generator EXCLUDE_FROM_ALL generator.cc interfaces.cc implementations.cc builtins.cc utils.cc)
target_link_libraries(generator clangFrontend clangAST)

log("Generated executable: generator")