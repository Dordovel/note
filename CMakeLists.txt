cmake_minimum_required(VERSION 3.15)
project(Note)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

file(GLOB HEADER_FILE ${CMAKE_SOURCE_DIR}/header/*.h)
file(GLOB INTERFACE_FILE ${CMAKE_SOURCE_DIR}/interface/*.h)
file(GLOB SOURCE_FILE ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}
                ${HEADER_FILE}
                ${SOURCE_FILE}
                main.cpp)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

pkg_check_modules(ATKMM atkmm-1.6)
link_directories(${ATKMM_LIBRARY_DIRS})
include_directories(${ATKMM_INCLUDE_DIRS})

find_package (SQLite3)
if (SQLITE3_FOUND)
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${SQLite3_LIBRARIES})
endif (SQLITE3_FOUND)

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})

