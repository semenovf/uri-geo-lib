project(pfs-uri-geo_TESTS)

set(TEST_NAMES
    parser
    composer)

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${name}.cpp ${${name}_SOURCES})
    else ()
        add_executable(${name} ${name}.cpp)
    endif()

    target_link_libraries(${name} PRIVATE pfs::uri::geo)
    add_test(NAME ${name} COMMAND ${name})
endforeach()
