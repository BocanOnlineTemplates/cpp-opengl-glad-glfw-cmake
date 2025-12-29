message(STATUS "[${APP_NAME}]: Configuring...")

cmake_path(SET PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

file(GLOB_RECURSE APP_ALL_CXX_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

column_print_list("[${APP_NAME}]: All Sources:" APP_ALL_CXX_SOURCES)

add_executable(${APP_NAME}
    ${APP_ALL_CXX_SOURCES}
)

target_include_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(${APP_NAME}
    PRIVATE
        ${CORE_NAME}
)


