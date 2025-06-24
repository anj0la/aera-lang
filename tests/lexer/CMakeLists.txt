#----------------------------------------------------------------------------------------------------------------------
# Testing Setup
#----------------------------------------------------------------------------------------------------------------------

add_executable(lexer_tests
	  "test_lexer.cpp"
)

target_link_libraries(
	  lexer_tests
	  aera::aera
	  gtest_main
)

gtest_discover_tests(lexer_tests)