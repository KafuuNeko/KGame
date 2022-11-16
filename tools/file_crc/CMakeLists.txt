cmake_minimum_required(VERSION 3.9)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/tools)

# ---------------------------------------------------------------------------------------
# fcrc
# ---------------------------------------------------------------------------------------
add_executable(fcrc main.cc)
target_link_libraries(fcrc PRIVATE cppcrc)

