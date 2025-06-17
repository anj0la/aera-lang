#----------------------------------------------------------------------------------------------------------------------
# Project Settings
#----------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15...3.31)
project(aera VERSION 0.1.0 LANGUAGES CXX)

#----------------------------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#----------------------------------------------------------------------------------------------------------------------
# Build Options
#----------------------------------------------------------------------------------------------------------------------

option(AERA_BUILD_TESTS "Build Aera tests" OFF)
option(AERA_BUILD_EXAMPLES "Build Aera examples" OFF)
option(AERA_BUILD_DOCS "Build Aera documentation" OFF)

#----------------------------------------------------------------------------------------------------------------------
# Build Settings
#----------------------------------------------------------------------------------------------------------------------

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#----------------------------------------------------------------------------------------------------------------------
# Library Definition
#----------------------------------------------------------------------------------------------------------------------

message(STATUS "Defining aera...")

add_library(aera STATIC

)
add_library(aera::aera ALIAS aera)

# CLI executable
add_executable(aera_cli src/main.cpp)
target_link_libraries(aera_cli aera::aera)

message(STATUS "Finished defining aera.")

#----------------------------------------------------------------------------------------------------------------------
# Target Properties & Linking
#----------------------------------------------------------------------------------------------------------------------
target_include_directories(aera
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(aera PUBLIC cxx_std_20)

#----------------------------------------------------------------------------------------------------------------------
# Other Targets
#----------------------------------------------------------------------------------------------------------------------
if(AERA_BUILD_TESTS)
    message(STATUS "Building aera tests...")
    enable_testing()
    #add_subdirectory(tests) 
endif()

if(AERA_BUILD_EXAMPLES)
    message(STATUS "Building aera examples...")
    #add_subdirectory(examples) 
endif()

if(AERA_BUILD_DOCS)
    message(STATUS "Building aera documentation...")
    #add_subdirectory(docs)
endif()