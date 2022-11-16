cmake_minimum_required(VERSION 3.9)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/game)

file(GLOB_RECURSE SourceFiles *.cc)
add_executable(service ${SourceFiles})
target_link_libraries(service core)

