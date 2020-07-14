project(pfs-rfc5870_EXAMPLES)

set(EXAMPLE_NAMES
    parser
    custom_parser
    composer)

foreach (name ${EXAMPLE_NAMES})
    if (${name}_SOURCES)
        add_executable(example_${name} ${name}.cpp ${${name}_SOURCES})
    else ()
        add_executable(example_${name} ${name}.cpp)
    endif()

    target_link_libraries(example_${name} PRIVATE pfs::rfc5870)
endforeach()
