log_section("Browser files")

set(BROWSER_FUNCTOR
    "${CMAKE_CURRENT_SOURCE_DIR}/Browser.oz")
set(BROWSER_OZF "${CMAKE_CURRENT_BINARY_DIR}/Browser.ozf")

log("Browser functor: ${BROWSER_FUNCTOR}")
log("Browser ozf: ${BROWSER_OZF}")

add_custom_command(
  OUTPUT "${BROWSER_OZF}"
  COMMAND ozemulator
    --home "${MOZART_BUILD_DIR}"
    x-oz://system/Compile.ozf
    -c "${BROWSER_FUNCTOR}" -o "${BROWSER_OZF}"
  DEPENDS library "${BROWSER_FUNCTOR}"
  COMMENT "(compiling) ${BROWSER_OZF}"
  VERBATIM)

add_custom_target(
  browser ALL
  DEPENDS "${BROWSER_OZF}")

# Supplementary files

log_section("Supplementary files")

set(SRC_IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/images")
set(BIN_IMAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/images")

log("Source image directory: ${SRC_IMAGE_DIR}")
log("Binary image directory: ${BIN_IMAGE_DIR}")

file(GLOB SUPPLEMENTARY_FILES RELATIVE "${SRC_IMAGE_DIR}"
  "${SRC_IMAGE_DIR}/*.xbm"
  "${SRC_IMAGE_DIR}/*.jpg")

file(MAKE_DIRECTORY "${BIN_IMAGE_DIR}")

set(TARGET_SUPPLEMENTARY_FILES "")

foreach(SUPP_FILE ${SUPPLEMENTARY_FILES})
  set(SRC_FILE "${SRC_IMAGE_DIR}/${SUPP_FILE}")
  set(TARGET_FILE "${BIN_IMAGE_DIR}/${SUPP_FILE}")
  set(TARGET_SUPPLEMENTARY_FILES ${TARGET_SUPPLEMENTARY_FILES} "${TARGET_FILE}")

  add_custom_command(
    TARGET browser POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${SRC_FILE}" "${TARGET_FILE}"
    VERBATIM)
endforeach()

log("Target supplementary files: ${TARGET_SUPPLEMENTARY_FILES}")

# Install
install(FILES "${BROWSER_OZF}"
        DESTINATION share/mozart/cache/x-oz/system)

install(FILES ${TARGET_SUPPLEMENTARY_FILES}
        DESTINATION share/mozart/cache/x-oz/system/images)
