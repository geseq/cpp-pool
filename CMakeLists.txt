cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(pool LANGUAGES CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

include(cmake/CPM.cmake)
CPMUsePackageLock(package-lock.cmake)

CPMAddPackage( NAME Boost VERSION 1.80.0 GITHUB_REPOSITORY "boostorg/boost" GIT_TAG "boost-1.80.0")

add_library(pool INTERFACE)
target_sources(pool INTERFACE include/pool.hpp)
target_include_directories(pool INTERFACE include/)
target_link_libraries(pool INTERFACE Boost::pool)

option(ENABLE_TESTING "Enable test target generation" OFF)
option(BENCHMARK_ENABLE_TESTING "run benchmarks" OFF)

if (ENABLE_TESTING)
    CPMAddPackage( NAME googletest GITHUB_REPOSITORY google/googletest VERSION 1.14.0 )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    include_directories(googletest_SOURCE_DIR)
    enable_testing()

    FILE(GLOB tests CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/*)
    FOREACH (test ${tests})
        get_filename_component(test_name ${test} NAME)
        message("Adding test: " ${test_name})
        add_executable(${test_name} ${PROJECT_SOURCE_DIR}/test/${test_name})
        target_link_libraries(${test_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} pool gtest)
        add_test(${test_name} ${test_name})
        set_property(TEST ${test_name} PROPERTY LABELS "test")
    ENDFOREACH ()


    if (BENCHMARK_ENABLE_TESTING)

        CPMAddPackage( NAME benchmark GITHUB_REPOSITORY google/benchmark VERSION 1.5.2 OPTIONS "BENCHMARK_ENABLE_TESTING Off")

        if(TARGET benchmark::benchmark)
            # enable c++11 to avoid compilation errors
            set_target_properties(benchmark PROPERTIES CXX_STANDARD 17)
        endif()


        FILE(GLOB tests ${PROJECT_SOURCE_DIR}/bench/*)
        FOREACH (test ${tests})
            get_filename_component(test_name ${test} NAME)
            message("Adding bench: " ${test_name})
            add_executable(${test_name} ${PROJECT_SOURCE_DIR}/bench/${test_name})
            target_link_libraries(${test_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} benchmark::benchmark pool)
            add_test(${test_name} ${test_name})
            set_property(TEST ${test_name} PROPERTY LABELS "bench")
            list(APPEND ignore_tests ${test_name})
        ENDFOREACH ()

        set (CMAKE_CTEST_ARGUMENTS "-L;test")
        add_custom_target(bench COMMAND ctest -L bench -V)
    endif()
endif()

