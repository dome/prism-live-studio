# CONFIG PRISM_DIR SRC_DIR BIN_DIR ARCH TARGET

if(${CONFIG} STREQUAL "RelWithDebInfo")
	set(CONFIG_SRC Release)
else()
	set(CONFIG_SRC ${CONFIG})
endif()

set(OUTPUT_DIR ${BIN_DIR}/${CONFIG_SRC}/${ARCH})

message(STATUS "Copy target file.")
message(STATUS "PRISM_DIR=${PRISM_DIR}")
message(STATUS "OUTPUT_DIR=${OUTPUT_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "TARGET=${TARGET}")

if(EXISTS "${TARGET}")
    message(STATUS "${TARGET} -> ${OUTPUT_DIR}")
    file(COPY "${TARGET}" DESTINATION "${OUTPUT_DIR}")
endif()

