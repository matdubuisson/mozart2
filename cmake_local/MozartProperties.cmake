find_package(Git)

include(TargetArch)

log("CMake current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# platform.os

if(WIN32)
  set(MOZART_PROP_PLATFORM_OS "win32")
else()
  string(TOLOWER "${CMAKE_SYSTEM_NAME}" MOZART_PROP_PLATFORM_OS)
endif()
log("Mozart properties platform os: ${MOZART_PROP_PLATFORM_OS}")

# platform.arch

target_architecture(MOZART_PROP_PLATFORM_ARCH)
log("Mozart property platform architecture: ${MOZART_PROP_PLATFORM_ARCH}")

# For source tarballs, oz.version and os.date
# are exported id MozartConfigVersion.cmake
include(MozartConfigVersion
	RESULT_VARIABLE MOZART_SOURCE_TARBALL_BUILD OPTIONAL)
log("Try to include optional MozartConfigVersion: ${MOZART_SOURCE_TARBALL_BUILD} (Mozart source tarball build)")

# oz.version

if(NOT MOZART_SOURCE_TARBALL_BUILD)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --dirty
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE git_describe_output
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  log("Execute: ${GIT_EXECUTABLE} describe --dirty")

  if("${git_describe_output}" MATCHES "^v[0-9].+-[0-9]+-g[0-9a-f]+(-dirty)?$")
    string(REGEX REPLACE "^v(.+)-([0-9]+)-g([0-9a-f]+)((-dirty)?)$" "\\1+build.\\2.\\3\\4"
           MOZART_PROP_OZ_VERSION "${git_describe_output}")
  elseif("${git_describe_output}" MATCHES "^v[0-9].+[0-9](-dirty)?$")
    string(REGEX REPLACE "^v(.+)$" "\\1"
           MOZART_PROP_OZ_VERSION "${git_describe_output}")
  else()
    message(WARNING "'git describe --dirty' did not return something sensible")
    set(MOZART_PROP_OZ_VERSION "unknown")
  endif()

  unset(git_describe_output)
endif()

log("Mozart property OZ version: ${MOZART_PROP_OZ_VERSION}")

# oz.date

if(NOT MOZART_SOURCE_TARBALL_BUILD)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log "--format=%aD" -1
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE git_date_output
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(MOZART_PROP_OZ_DATE "${git_date_output}")

  unset(git_date_output)
endif()

log("Mozart property OZ date: ${MOZART_PROP_OZ_DATE}")

# Display which version we found

log("Mozart ${MOZART_PROP_OZ_VERSION} (${MOZART_PROP_OZ_DATE})")
