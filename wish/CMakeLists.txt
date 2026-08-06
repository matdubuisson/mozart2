cmake_minimum_required(VERSION 4.0)
project(MOZARTWISH)

find_package(TCL REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_INTERP_RESULT=1")
log("CMake CXX flags: ${CMAKE_CXX_FLAGS}")

include_directories("${TCL_INCLUDE_PATH}" "${TK_INCLUDE_PATH}")

if(WIN32)
  # Path and version for Inno Setup
  set(ISS_TCL_VERSION ${TCLSH_VERSION_STRING} CACHE STRING "Tcl version string")
  set(ISS_TCL_EXEC ${TCL_TCLSH} CACHE STRING "Tclsh executable path")

  set(MAIN_SRC winmain.cc)
else()
  set(MAIN_SRC unixmain.cc)
endif()

log("Main source: ${MAIN_SRC}")

add_executable(ozwish ${MAIN_SRC})
log("New executable: ozwish")

target_link_libraries(ozwish "${TCL_LIBRARY}" "${TK_LIBRARY}")
log("Links libraries: ${TCL_LIBRARY} ${TK_LIBRARY}")

if(NOT MINGW)
  target_link_libraries(ozwish pthread)
  log("Links library: pthread")
endif()

# Install
install(TARGETS ozwish DESTINATION bin)
