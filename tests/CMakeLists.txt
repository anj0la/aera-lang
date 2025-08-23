#----------------------------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------------------------

# Add the tests/cmake directory to the module path
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") 

set(TEST_BINARY_DIR ${CMAKE_BINARY_DIR}/tests/bin)
file(MAKE_DIRECTORY ${TEST_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TEST_BINARY_DIR})

#----------------------------------------------------------------------------------------------------------------------
# Testing Framework
#----------------------------------------------------------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding the parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)
include(GoogleTest) 

if(TARGET GTest::gtest)
    get_target_property(GTEST_INCLUDES GTest::gtest INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "GTest::gtest INTERFACE_INCLUDE_DIRECTORIES reported: ${GTEST_INCLUDES}")
else()
    message(WARNING "GTest::gtest target was NOT found after FetchContent_MakeAvailable!")
endif()

#----------------------------------------------------------------------------------------------------------------------
# Top-Level Tests (Aera)
#----------------------------------------------------------------------------------------------------------------------

add_executable(aera_top_level_tests
	  "test_source_context.cpp"
      "test_diagnostics.cpp"
)

target_link_libraries(
	  aera_top_level_tests
	  PRIVATE
	  aera::aera
	  GTest::gtest_main 
)

target_include_directories(aera_top_level_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
)

gtest_discover_tests(aera_top_level_tests)

#----------------------------------------------------------------------------------------------------------------------
# Testing Subdirectories
#----------------------------------------------------------------------------------------------------------------------

add_subdirectory(lexer)