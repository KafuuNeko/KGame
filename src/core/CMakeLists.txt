file(GLOB_RECURSE SourceFiles *.cc)
add_library(core STATIC ${SourceFiles})
target_link_libraries(core sqlite crc aes md5)

