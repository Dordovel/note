cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -lsqlite3")

file(GLOB SOURCE_FILE ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(test
                ${SOURCE_FILE}
                main.cpp)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})

