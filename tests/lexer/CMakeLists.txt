#----------------------------------------------------------------------------------------------------------------------
# Testing Setup
#----------------------------------------------------------------------------------------------------------------------

add_executable(lexer_tests
	  "test_lexer.cpp"
)

target_link_libraries(
	  lexer_tests
	  PRIVATE
	  aera::aera
	  GTest::gtest_main 
)

target_include_directories(lexer_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
)

gtest_discover_tests(lexer_tests)