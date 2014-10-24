cmake_minimum_required( VERSION 3.0 )

# Project name will be CAHAL
project( CAHALLibStatic )

if( DEFINED ENV{SOURCE_DIRECTORY} AND IS_DIRECTORY $ENV{SOURCE_DIRECTORY} )
  message( STATUS "Working with source directory: $ENV{SOURCE_DIRECTORY}" )
  message (
            STATUS
            "Working with include directory: $ENV{SOURCE_DIRECTORY}/include"
          )
else()
  message( FATAL_ERROR "No SOURCE_DIRECTORY environment variable set" )
endif()

if( DEFINED ENV{BIN_DIRECTORY} )
  message( STATUS "Working with bin directory: $ENV{BIN_DIRECTORY}" )
else()
  message( FATAL_ERROR "No BIN_DIRECTORY environment variable set" )
endif()

# Set the location of the source files
set( CMAKE_SOURCE_DIR "$ENV{SOURCE_DIRECTORY}" )
set( CMAKE_INCLUDE_DIR "$ENV{SOURCE_DIRECTORY}" )

# Set the location of the include files
include_directories( "$ENV{SOURCE_DIRECTORY}/include" )

# Group the files together so they show up this way in editors
source_group(
              "common"
              REGULAR_EXPRESSION
              "$ENV{SOURCE_DIRECTORY}/common/*"
            )
source_group(
              "include"
              REGULAR_EXPRESSION
              "$ENV{SOURCE_DIRECTORY}/include/*"
            )

if( DEFINED CMAKE_SYSTEM_NAME AND CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
  message( STATUS "Working with system: Darwin" )

  source_group(
                "darwin"
                REGULAR_EXPRESSION
                "$ENV{SOURCE_DIRECTORY}/darwin/*"
              )

elseif( DEFINED CMAKE_SYSTEM_NAME AND CMAKE_SYSTEM_NAME STREQUAL "Windows" )
  message( STATUS "Working with system: Windows" )

  source_group(
                "windows"
                REGULAR_EXPRESSION
                "$ENV{SOURCE_DIRECTORY}/windows/*"
              )

elseif( DEFINED CMAKE_SYSTEM_NAME AND CMAKE_SYSTEM_NAME STREQUAL "Generic" )
  message( STATUS "Working with system: Generic" )

  source_group(
                "generic"
                REGULAR_EXPRESSION
                "$ENV{SOURCE_DIRECTORY}/generic/*"
              )
endif()

# Set the output location
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "$ENV{BIN_DIRECTORY}" )

# Look for all .h/.c files in the source directory
file( GLOB_RECURSE TARGETS_SRC ${CMAKE_SOURCE_DIR}/*.c )
file( GLOB_RECURSE TARGETS_INC ${CMAKE_SOURCE_DIR}/*.h )

message( STATUS "C source files found: ${TARGETS_SRC}" )
message( STATUS "C header files found: ${TARGETS_INC}" )

add_library( CAHAL STATIC ${TARGETS_SRC} ${TARGETS_INC} )
