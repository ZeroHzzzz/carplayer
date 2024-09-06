cmake_minimum_required(VERSION 3.5)

project(NewPlayer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(Qt5_DIR "D:/Qt/Qt5.13.2/5.13.2/mingw73_64/lib/cmake/Qt5")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "UI include directories: ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(NewPlayer
  headfile.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  datahandler.cpp
  datahandler.h
  imagehandler.cpp
  imagehandler.h
  ctrl.h
  ctrl.cpp
  stack.h
  stack.cpp
  speed.h
  speed.cpp
  motor.h
  motor.cpp
)

target_link_libraries(NewPlayer PRIVATE Qt5::Widgets ${OpenCV_LIBS})
