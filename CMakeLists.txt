cmake_minimum_required(VERSION 3.23)
project(corefw LANGUAGES C)

#Necessary for shared library on Windows
if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-std=c17")
include_directories("${CMAKE_SOURCE_DIR}/include/corefw")

add_library(corefw SHARED)
target_include_directories(corefw PRIVATE "${CMAKE_SOURCE_DIR}/include")
add_subdirectory("src")

add_executable(app)
target_sources(app PRIVATE "tests/tests.c")
target_link_libraries(app PRIVATE corefw)

install(TARGETS corefw 
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/corefw"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
