project(pfs-uri-geo_EXAMPLES)

set(EXAMPLE_NAMES
    parser
    custom_parser
    composer)

find_package(Qt5Core QUIET)

foreach (name ${EXAMPLE_NAMES})
    if (${name}_SOURCES)
        add_executable(example_${name} ${name}.cpp ${${name}_SOURCES})
    else ()
        add_executable(example_${name} ${name}.cpp)
    endif()

    if (Qt5Core_FOUND)
        target_compile_definitions(example_${name} PRIVATE HAVE_QT5_CORE)
        target_link_libraries(example_${name} PRIVATE pfs::uri::geo Qt5::Core)
    else()
        target_link_libraries(example_${name} PRIVATE pfs::uri::geo)
    endif()
endforeach()
