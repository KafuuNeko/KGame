set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/game)

add_executable(editor
  main.cpp

  # Windows
  windows/window_main.cpp
  windows/window_main.h
  windows/window_main.ui

  # Dialogs Setting
  dialog/dialog_setting.cpp
  dialog/dialog_setting.h
  dialog/dialog_setting.ui

  # Dialog About
  dialog/dialog_about.cpp
  dialog/dialog_about.h
  dialog/dialog_about.ui
)

target_link_libraries(editor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(editor PRIVATE core)

target_include_directories(editor PRIVATE dialog windows)
target_include_directories(editor PRIVATE "${CMAKE_BINARY_DIR}/src/editor/editor_autogen/include")

message("Editor include: ${CMAKE_BINARY_DIR}/src/editor/editor_autogen/include")

