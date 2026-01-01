cmake_minimum_required(VERSION 4.0)

project(OpenGLTemplate CXX)

set(APP_NAME ${PROJECT_NAME}-App)
set(CORE_NAME ${PROJECT_NAME}-Core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(column_print_list TITLE LIST_VAR)
    message(STATUS "${TITLE}")
    foreach(item IN LISTS ${LIST_VAR})
        message(STATUS "${item}")
    endforeach()
endfunction()

option(BUILD_TESTING "Build Tests" ON)

if(BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(source/${APP_NAME})
add_subdirectory(vendor)
