separate_arguments(SPLIT_CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
set(MOZART_GENERATOR_FLAGS "${MOZART_GENERATOR_FLAGS}" "${SPLIT_CXX_FLAGS}")

log("Mozart generator flags: ${MOZART_GENERATOR_FLAGS}")

# properties-config.cc

include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/properties-config.cc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/properties-config.cc"
  ESCAPE_QUOTES @ONLY)

# Generated sources

log_section("Generated sources")

set(GENERATED_SOURCES_CACHED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cached/)
set(GENERATED_SOURCES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/)

log("Generated sources cached directory: ${GENERATED_SOURCES_CACHED_DIR}")
log("Generated sources binary directory: ${GENERATED_SOURCES_BINARY_DIR}")

find_program(clang_bin clang++)

add_custom_command(
  OUTPUT ${GENERATED_SOURCES_BINARY_DIR}/mozart.gen

  # First pass: generate data types and interfaces
  COMMAND ${clang_bin} -emit-ast
    -o ${GENERATED_SOURCES_BINARY_DIR}/mozart.ast
    -DMOZART_GENERATOR
    ${MOZART_GENERATOR_FLAGS}
    ${CMAKE_CURRENT_SOURCE_DIR}/mozart.hh
  COMMAND generator intfimpl
    ${GENERATED_SOURCES_BINARY_DIR}/mozart.ast
    ${GENERATED_SOURCES_BINARY_DIR}/

  # Second pass: generate builtin information
  COMMAND ${clang_bin} -emit-ast
    -o ${GENERATED_SOURCES_BINARY_DIR}/coremodules.astbi
    -I ${GENERATED_SOURCES_BINARY_DIR}
    -DMOZART_BUILTIN_GENERATOR
    ${MOZART_GENERATOR_FLAGS}
    ${CMAKE_CURRENT_SOURCE_DIR}/coremodules.hh
  COMMAND generator builtins
    ${GENERATED_SOURCES_BINARY_DIR}/coremodules.astbi
    ${GENERATED_SOURCES_BINARY_DIR}/
    mozartbuiltins

  COMMAND ${CMAKE_COMMAND} -E remove
    ${GENERATED_SOURCES_BINARY_DIR}/mozart.ast
    ${GENERATED_SOURCES_BINARY_DIR}/coremodules.astbi

  # Tag
  COMMAND ${CMAKE_COMMAND} -E touch ${GENERATED_SOURCES_BINARY_DIR}/mozart.gen

  DEPENDS generator
  IMPLICIT_DEPENDS CXX "${CMAKE_CURRENT_SOURCE_DIR}/coremodules.hh"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generate sources for mozart"
  VERBATIM)

log("Generated commands in ${GENERATED_SOURCES_BINARY_DIR}/mozart.gen")

# Use the cached files if they exist.

if(EXISTS ${GENERATED_SOURCES_CACHED_DIR}/mozart.gen)
    log("Pre-generated files found in ${GENERATED_SOURCES_CACHED_DIR}")
    
    set(GENERATED_SOURCES_DIR ${GENERATED_SOURCES_CACHED_DIR})
else()
    log("No pre-generated files found in ${GENERATED_SOURCES_CACHED_DIR}")

    set(GENERATED_SOURCES_DIR ${GENERATED_SOURCES_BINARY_DIR})
    file(MAKE_DIRECTORY ${GENERATED_SOURCES_BINARY_DIR})
endif()
set_property(GLOBAL PROPERTY GENERATED_SOURCES_DIR "${GENERATED_SOURCES_DIR}")

log("Generated sources directory: ${GENERATED_SOURCES_DIR}")

add_custom_target(gensources
  DEPENDS ${GENERATED_SOURCES_DIR}/mozart.gen
  VERBATIM)

# Include the generated files in the source archive.

set_property(GLOBAL APPEND PROPERTY SRCS_INSTALLED_DIRS
    "${GENERATED_SOURCES_DIR}" "/vm/vm/main/cached/")

if (DIST_TARGET_ENABLED)
    add_dependencies(dist gensources)
    log("Dist target enabled")
endif()

# Build the library
include_directories(${GENERATED_SOURCES_DIR})
add_library(mozartvm runnable.cc variables.cc emulate.cc memmanager.cc gcollect.cc
  unify.cc sclone.cc vm.cc coredatatypes.cc coders.cc properties.cc
  coremodules.cc unpickler.cc serializer.cc pickler.cc introspection.cc)
if(NOT MOZART_CACHED_BUILD)
    add_dependencies(mozartvm gensources)
endif()
