cmake_minimum_required(VERSION 3.9)


# ---------------------------------------------------------------------------------------
# aes
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE SourceFiles aes/*.c)
add_library(aes STATIC ${SourceFiles})

# ---------------------------------------------------------------------------------------
# sqlite
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE SourceFiles sqlite/*.c)
add_library(sqlite STATIC ${SourceFiles})

# ---------------------------------------------------------------------------------------
# md5
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE SourceFiles md5/*.cpp)
add_library(md5 STATIC ${SourceFiles})


# ---------------------------------------------------------------------------------------
# lepton
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE SourceFiles lepton/*.cpp)
add_library(lepton STATIC ${SourceFiles})

# ---------------------------------------------------------------------------------------
# cppcrc
# ---------------------------------------------------------------------------------------
add_subdirectory(cppcrc)

# ---------------------------------------------------------------------------------------
# fmt
# ---------------------------------------------------------------------------------------
add_subdirectory(fmt)