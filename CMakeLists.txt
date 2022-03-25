cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(camera_calibrator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)


message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")





include_directories( ${OpenCV_INCLUDE_DIRS})


find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)


include_directories( ${OpenCV_INCLUDE_DIRS})
add_library(calibrator
			src/calibrationprocess.h
			src/calibrationprocess.cpp
)

add_executable(camera_calibrator 
				mainwindow.ui
				mainwindow.cpp
				main.cpp
			)

include_directories(camera_calibrator src/calibrationprocess.h)

target_link_libraries(calibrator ${OpenCV_LIBS} Qt6::Core Qt6::Xml)
target_link_libraries(camera_calibrator PRIVATE Qt6::Widgets)
target_link_libraries(camera_calibrator PRIVATE Qt6::Xml)
target_link_libraries(camera_calibrator PRIVATE calibrator)
