project(pfs-rfc5870_TESTS)

set(TEST_NAMES
    parser)

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${name}.cpp ${${name}_SOURCES})
    else ()
        add_executable(${name} ${name}.cpp)
    endif()

    target_link_libraries(${name} PRIVATE pfs::rfc5870)
    add_test(NAME ${name} COMMAND ${name})
endforeach()
