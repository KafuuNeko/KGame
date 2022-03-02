SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/tools)

# ---------------------------------------------------------------------------------------
# fcrc
# ---------------------------------------------------------------------------------------
add_executable(fcrc file_crc/main.cc)
target_link_libraries(fcrc PRIVATE crc)

