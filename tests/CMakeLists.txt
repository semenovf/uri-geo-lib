project(${_project_basename}_TESTS)

option(${_project_basename}_ENABLE_COVERAGE "Build tests with coverage support" OFF)

if (${_project_basename}_ENABLE_COVERAGE)
    # Helpfull resources:
    # 1. https://github.com/TheLartians/ModernCppStarter
    # 2. https://github.com/bsamseth/cpp-project
    # 3. https://github.com/eddyxu/cpp-coveralls
    # 4. https://github.com/bilke/cmake-modules
    # 5. https://github.com/JoakimSoderberg/coveralls-cmake
    # 6. https://github.com/okkez/coveralls-lcov

    message(STATUS "Coverage feature enabled")

    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
            message(FATAL_ERROR "Coverage feature requires GCC or Clang")
        endif()
    endif()

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "Code coverage results with an optimised (non-Debug) build may be misleading")
    endif()

#     find_program(GCOV_PATH gcov)
#     find_program(LCOV_PATH lcov)
#     find_program(GENHTML_PATH genhtml)
#     find_program(GCOVR_PATH gcovr PATHS ${CMAKE_SOURCE_DIR}/tests)
#     lcov --directory . --zerocounters
#     ctest
#     lcov --directory tests --capture --output-file COVERAGE.info
#     lcov --remove COVERAGE.info '/usr/*' '*/doctest.h' --output-file COVERAGE.info.cleaned
#     genhtml -o COVERAGE COVERAGE.info.cleaned
#     lcov --list COVERAGE.info.cleaned
endif()

set(TEST_NAMES
    parser
    composer)

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${name}.cpp ${${name}_SOURCES})
    else ()
        add_executable(${name} ${name}.cpp)
    endif()

    # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#Instrumentation-Options
    #
    # --coverage
    #       This option is used to compile and link code instrumented for
    #       coverage analysis. The option is a synonym for -fprofile-arcs
    #       -ftest-coverage (when compiling) and -lgcov (when linking).
    if (${_project_basename}_ENABLE_COVERAGE)
        target_compile_options(${name} PRIVATE -g -O0 --coverage)
        target_link_libraries(${name} PRIVATE -g -O0 --coverage)
    endif()

    target_link_libraries(${name} PRIVATE pfs::uri::geo)
    add_test(NAME ${name} COMMAND ${name})
endforeach()
