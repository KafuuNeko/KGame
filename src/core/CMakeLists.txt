cmake_minimum_required(VERSION 3.9)


file(GLOB_RECURSE SourceFiles *.cc)
add_library(core STATIC ${SourceFiles})
target_link_libraries(core PRIVATE sqlite cppcrc aes md5 fmt)

