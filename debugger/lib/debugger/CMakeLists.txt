log_section("Debugger files")

set(DEBUGGER_FUNCTOR
    "${CMAKE_CURRENT_SOURCE_DIR}/Debugger.oz")
set(DEBUGGER_OZF "${CMAKE_CURRENT_BINARY_DIR}/Debugger.ozf")

log("Debugger functor: ${DEBUGGER_FUNCTOR}")
log("Debugger ozf: ${DEBUGGER_OZF}")

add_custom_command(
  OUTPUT "${DEBUGGER_OZF}"
  COMMAND ozemulator
    --home "${MOZART_BUILD_DIR}"
    x-oz://system/Compile.ozf
    -c "${DEBUGGER_FUNCTOR}" -o "${DEBUGGER_OZF}"
  DEPENDS library "${DEBUGGER_FUNCTOR}"
  COMMENT "(compiling) ${DEBUGGER_OZF}"
  VERBATIM)

add_custom_target(
  debugger ALL
  DEPENDS "${DEBUGGER_OZF}"
)

install(FILES "${DEBUGGER_OZF}"
        DESTINATION share/mozart/cache/x-oz/system)
