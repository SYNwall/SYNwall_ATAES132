file(GLOB TEST_LIB_SOURCES "*.c")

add_executable(test_ataes_app
    ${TEST_LIB_SOURCES}
)

target_include_directories(test_ataes_app PUBLIC .
)

add_subdirectory(Unity)
 
target_link_libraries(test_ataes_app
    ataes
    Unity
    gcov
)

target_compile_features(test_ataes_app PRIVATE
    c_std_99
)

add_custom_target(check
    COMMENT "Creating Coverage HTML and XML summary"
    COMMAND ${CMAKE_BINARY_DIR}/tests/test_ataes_app
    COMMAND gcovr -k  --branches  -r  ${CMAKE_SOURCE_DIR}
              --object-directory=${CMAKE_BINARY_DIR}/tests/CMakeFiles/test_ataes_app.dir/
              --xml-pretty -o ${CMAKE_BINARY_DIR}/coverage.xml
    COMMAND gcovr -k  --branches  -r  ${CMAKE_SOURCE_DIR}
              --object-directory=${CMAKE_BINARY_DIR}/tests/CMakeFiles/test_ataes_app.dir/
              --html --html-details -o ${CMAKE_BINARY_DIR}/coverage.html
    COMMENT "Creating Gprof results from testing."              
    COMMAND cd ${CMAKE_BINARY_DIR}/tests && gprof test_ataes_app gmon.out > analysis_test_ataes_app)

add_test(test_ataes test_ataes_app)