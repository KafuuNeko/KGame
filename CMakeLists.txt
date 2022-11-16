cmake_minimum_required(VERSION 3.9)
project(KGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O2")
set(CMAKE_CXX_FLAGS "-pthread -ldl ${CMAKE_CXX_FLAGS}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/lib)


if(WIN32)
    add_definitions(-DWIN32) 
    link_libraries(ws2_32 wsock32)

elseif(UNIX OR CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-D__linux__)
endif()

message("CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")

include_directories(src/core src/utils)
include_directories(third_party third_party/lepton/include third_party/cppcrc/include third_party/fmt/include)

# ---------------------------------------------------------------------------------------
# third_party
# ---------------------------------------------------------------------------------------
add_subdirectory(third_party)

# ---------------------------------------------------------------------------------------
# core
# ---------------------------------------------------------------------------------------
add_subdirectory(src/core)

# ---------------------------------------------------------------------------------------
# service
# ---------------------------------------------------------------------------------------
add_subdirectory(src/service)

# ---------------------------------------------------------------------------------------
# editor
# ---------------------------------------------------------------------------------------
add_subdirectory(src/editor)

# ---------------------------------------------------------------------------------------
# tools
# ---------------------------------------------------------------------------------------
add_subdirectory(tools/file_crc)

# ---------------------------------------------------------------------------------------
# tests
# ---------------------------------------------------------------------------------------
add_subdirectory(tests)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/model/game.sql DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/output/game/db/struct)
message("copy ${CMAKE_CURRENT_SOURCE_DIR}/model/game.sql to ${CMAKE_CURRENT_SOURCE_DIR}/output/game/db/struct")
