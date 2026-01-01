message(STATUS "[${CORE_NAME}]: Configuring...")

file(GLOB_RECURSE CORE_ALL_CXX_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

file(GLOB_RECURSE CORE_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.test.cpp"
)

set(CORE_MODULE_SOURCES ${CORE_ALL_CXX_SOURCES})
list(REMOVE_ITEM CORE_MODULE_SOURCES ${CORE_TEST_SOURCES})

column_print_list("[${CORE_NAME}]: All Sources:" CORE_ALL_CXX_SOURCES)
column_print_list("[${CORE_NAME}]: Module Sources:" CORE_MODULE_SOURCES)
column_print_list("[${CORE_NAME}]: Unit Test Sources:" CORE_TEST_SOURCES)

add_library(${CORE_NAME} STATIC
    ${CORE_MODULE_SOURCES}
)

target_include_directories(${CORE_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

if(BUILD_TESTING AND CORE_TEST_SOURCES)
    message(STATUS "[${CORE_NAME}]: Configuring unit tests...")
    
    foreach(test_file ${CORE_TEST_SOURCES})

        get_filename_component(test_name_raw ${test_file} NAME)

        string(REPLACE ".test.cpp" "_Test" test_name ${test_name_raw})
        
        message(STATUS "[${CORE_NAME}]: Adding test target ${test_name} from ${test_file}.")

        add_executable(${test_name}
            ${test_file}
        )
        
        target_link_libraries(${test_name}
            PRIVATE
            ${CORE_NAME}
        )

        target_include_directories(${test_name}
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/source
            )
        
        add_test(
            NAME ${test_name}
            COMMAND ${test_name}
        )

    endforeach()
endif()
