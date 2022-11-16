cmake_minimum_required(VERSION 3.9)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/tests)

# ---------------------------------------------------------------------------------------
# sqlite
# ---------------------------------------------------------------------------------------
add_executable(test_sqlite sqlite.cc)
add_dependencies(test_sqlite sqlite)
target_link_libraries(test_sqlite PRIVATE sqlite)

# ---------------------------------------------------------------------------------------
# core
# ---------------------------------------------------------------------------------------
add_executable(test_core core.cc)
target_link_libraries(test_core PRIVATE core)

# ---------------------------------------------------------------------------------------
# lepton
# ---------------------------------------------------------------------------------------
add_executable(test_lepton lepton.cc)
target_link_libraries(test_lepton PRIVATE lepton)

# ---------------------------------------------------------------------------------------
# cppcrc
# ---------------------------------------------------------------------------------------
add_executable(test_cppcrc cppcrc.cc) 
target_link_libraries(test_cppcrc PRIVATE cppcrc)


# ---------------------------------------------------------------------------------------
# simple_udp
# ---------------------------------------------------------------------------------------
add_executable(test_udp udp.cc)



