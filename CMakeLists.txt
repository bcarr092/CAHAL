cmake_minimum_required( VERSION 3.0 )

set( PROJECT_NAME "cahal" )

project( ${PROJECT_NAME} )

message( STATUS "Project: ${PROJECT_NAME}" )
message( STATUS "Project directory: ${PROJECT_SOURCE_DIR}" )

set( SOURCE_DIR "${PROJECT_SOURCE_DIR}/src" )
set( INCLUDE_DIR "${SOURCE_DIR}/include" )
set( INSTALL_DIR "${CMAKE_BINARY_DIR}/install" )

message( STATUS "Source directory: ${SOURCE_DIR}" )
message( STATUS "Include directory: ${INCLUDE_DIR}" )
message( STATUS "Install directory: ${INSTALL_DIR}" )

set( SOURCES "${SOURCE_DIR}/common/cahal.c" )
list( APPEND SOURCES "${SOURCE_DIR}/darwin/osx_cahal.c" )
list( APPEND SOURCES "${SOURCE_DIR}/darwin/osx_log.c" )

set( HEADERS "${INCLUDE_DIR}/cahal.h" )
list( APPEND HEADERS "${INCLUDE_DIR}/darwin/osx_cahal.h" )
list( APPEND HEADERS "${INCLUDE_DIR}/darwin/osx_log.h" )

set( CPCOMMON_INCLUDE_DIR "${cpcommon_SOURCE_DIR}/src/include" )

message (
  STATUS
  "Working with cpcommon source directory: ${CPCOMMON_INCLUDE_DIR}"
        )

include_directories( ${INCLUDE_DIR} )
include_directories( ${CPCOMMON_INCLUDE_DIR} )

message( STATUS "C source files found: ${SOURCES}" )
message( STATUS "C header files found: ${HEADERS}" )

add_library( ${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} )

target_link_libraries( ${PROJECT_NAME} cpcommon )

install (
          TARGETS ${PROJECT_NAME}
          ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
          LIBRARY DESTINATION "${INSTALL_DIR}/lib"
        )
install( FILES ${HEADERS} DESTINATION "${INSTALL_DIR}/include/${PROJECT_NAME}" )

# Build test suite as well
add_subdirectory( test )

enable_testing()

# These are platform specific build instructions, this needs to be cleaned up
find_path( CORE_FOUNDATION_INCLUDES "CoreFoundation/CoreFoundation.h" )
find_path( CORE_AUDIO_INCLUDES "CoreAudio/CoreAudio.h" )

set( EXTRA_INCLUDES ${CORE_FOUNDATION_INCLUDES} ${CORE_AUDIO_INCLUDES} )

message( STATUS "Framework include directories: ${EXTRA_INCLUDES}" )

include_directories( ${EXTRA_INCLUDES} )
