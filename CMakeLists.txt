project (ataes)
cmake_minimum_required(VERSION 3.11.4)

set(TARGET_GROUP main CACHE STRING "Group to build")

if(STATIC_ANALYSIS)
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable"
    )
    if(NOT CLANG_TIDY_EXE)
        message(FATAL_ERROR "clang-tidy not found.")
    else()
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE}" -checks=*,-clang-analyzer-alpha.* -warnings-as-errors=* -fix)
    endif()

    find_program(
        CMAKE_C_CPPCHECK
        NAMES "cppcheck"
        DOC "Path to cppcheck executable"
    )
    if(NOT CMAKE_C_CPPCHECK)
        message(FATAL_ERROR "cppcheck not found.")
    else()
        message(STATUS "cppcheck found: ${CMAKE_C_CPPCHECK}")
        set(CMAKE_C_CPPCHECK "${CMAKE_C_CPPCHECK}" --enable=warning,performance,portability,style --error-exitcode=1 --force --quiet)
    endif()
endif()


add_subdirectory(src)

if(TARGET_GROUP STREQUAL main)
    message(SKIP "Nothing to do...")
elseif(TARGET_GROUP STREQUAL test)
    include(CTest)
    add_compile_options(-std=c99 -Wall -Wextra -pedantic -g -O1 --coverage -pg)
    SET(CMAKE_EXE_LINKER_FLAGS "-std=c99 -Wall -Wextra -pedantic -g -O1 --coverage -pg ${CMAKE_EXE_LINKER_FLAGS}")
    add_subdirectory(tests)
endif()
