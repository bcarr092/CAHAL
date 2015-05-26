cmake_minimum_required( VERSION 3.0 )

set( PROJECT_NAME "cahal" )

project( ${PROJECT_NAME} )

message( STATUS "Project: ${PROJECT_NAME}" )
message( STATUS "Project directory: ${PROJECT_SOURCE_DIR}" )

set( SOURCE_DIR "${PROJECT_SOURCE_DIR}/src" )
set( INCLUDE_DIR "${SOURCE_DIR}/include" )
set( INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" )

message( STATUS "Source directory: ${SOURCE_DIR}" )
message( STATUS "Include directory: ${INCLUDE_DIR}" )
message( STATUS "Install directory: ${INSTALL_DIR}" )

set( SOURCES "${SOURCE_DIR}/common/cahal.c" )
list( APPEND SOURCES "${SOURCE_DIR}/common/cahal_audio_format_flags.c" )
list( APPEND SOURCES "${SOURCE_DIR}/common/cahal_audio_format_description.c" )
list( APPEND SOURCES "${SOURCE_DIR}/common/cahal_device.c" )
list( APPEND SOURCES "${SOURCE_DIR}/common/cahal_device_stream.c" )

set( HEADERS "${INCLUDE_DIR}/cahal.h" )
list( APPEND HEADERS "${INCLUDE_DIR}/cahal_audio_format_flags.h" )
list( APPEND HEADERS "${INCLUDE_DIR}/cahal_audio_format_description.h" )
list( APPEND HEADERS "${INCLUDE_DIR}/cahal_device.h" )
list( APPEND HEADERS "${INCLUDE_DIR}/cahal_device_stream.h" )

if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin" )
  find_library( FOUNDATION_FRAMEWORK Foundation )
  find_library( CORE_FOUNDATION_FRAMEWORK CoreFoundation )
  find_library( CORE_AUDIO_FRAMEWORK CoreAudio )
  find_library( AUDIO_TOOLBOX_FRAMEWORK AudioToolbox )
  find_library( AVFOUNDATION_FRAMEWORK AVFoundation )
  find_library( COREDATA_FRAMEWORK CoreData )
  find_library( OBJC_FRAMEWORK objc )

  set (
        EXTRA_LIBS
        ${CORE_FOUNDATION_FRAMEWORK}
        ${CORE_AUDIO_FRAMEWORK}
        ${AUDIO_TOOLBOX_FRAMEWORK}
        ${AVFOUNDATION_FRAMEWORK}
        ${COREDATA_FRAMEWORK}
        ${OBJC_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
      )

  message( STATUS "Framework libraries: ${EXTRA_LIBS}" )

  find_path( AVFOUNDATION_INCLUDES   "AVFoundation/AVFoundation.h" )
  find_path( AUDIO_TOOLBOX_INCLUDES   "AudioToolbox/AudioToolbox.h" )
  find_path( CORE_FOUNDATION_INCLUDES "CoreFoundation/CoreFoundation.h" )

  set ( EXTRA_INCLUDES
        ${CORE_FOUNDATION_INCLUDES} ${AVFOUNDATION_INCLUDES}
        ${AUDIO_TOOLBOX_INCLUDES}
      )
  
  message( STATUS "Framework include directories: ${EXTRA_INCLUDES}" )

  include_directories( ${EXTRA_INCLUDES} )

  set( DARWINHELPER_INCLUDE_DIR "${darwinhelper_SOURCE_DIR}/src/include" )

  message (
    STATUS
    "Working with darwinhelper source directory: ${DARWINHELPER_INCLUDE_DIR}"
          )

  include_directories( ${DARWINHELPER_INCLUDE_DIR} )

  if( ${PLATFORM} STREQUAL "iPhoneOS" )
    list( APPEND SOURCES "${SOURCE_DIR}/darwin/ios/ios_cahal.c" )
    list( APPEND SOURCES "${SOURCE_DIR}/darwin/ios/ios_cahal_device.c" )
    list( APPEND SOURCES "${SOURCE_DIR}/darwin/ios/ios_cahal_initialize.m" )

    list( APPEND HEADERS "${INCLUDE_DIR}/darwin/ios/ios_cahal.h" )
    list( APPEND HEADERS "${INCLUDE_DIR}/darwin/ios/ios_cahal_device.h" )
    list( APPEND HEADERS "${INCLUDE_DIR}/darwin/ios/ios_cahal_initialize.h" )
  elseif( ${PLATFORM} STREQUAL "MacOSX" )
    list( APPEND SOURCES "${SOURCE_DIR}/darwin/osx/osx_cahal.c" )
    list(
          APPEND
          SOURCES
          "${SOURCE_DIR}/darwin/osx/osx_cahal_audio_format_description.c"
        )
    list( APPEND SOURCES "${SOURCE_DIR}/darwin/osx/osx_cahal_device.c" )
    list( APPEND SOURCES "${SOURCE_DIR}/darwin/osx/osx_cahal_device_stream.c" )

    list( APPEND HEADERS "${INCLUDE_DIR}/darwin/osx/osx_cahal.h" )
    list(
          APPEND
          HEADERS
          "${INCLUDE_DIR}/darwin/osx/osx_cahal_audio_format_description.h"
        )
    list  (
            APPEND
            HEADERS
            "${INCLUDE_DIR}/darwin/osx/osx_cahal_device.h"
          )
    list( APPEND HEADERS "${INCLUDE_DIR}/darwin/osx/osx_cahal_device_stream.h" )
  endif()

  list  (
          APPEND
          SOURCES
          "${SOURCE_DIR}/darwin/darwin_cahal_audio_format_flags.c"
        )
  list(
        APPEND
        SOURCES
        "${SOURCE_DIR}/darwin/darwin_cahal_audio_format_description.c"
      )
  list( APPEND SOURCES "${SOURCE_DIR}/darwin/darwin_cahal_device.c" )

  list  (
          APPEND
          HEADERS
          "${INCLUDE_DIR}/darwin/darwin_cahal_audio_format_flags.h"
        )
  list(
        APPEND
        HEADERS
        "${INCLUDE_DIR}/darwin/darwin_cahal_audio_format_description.h"
      )
  list( APPEND HEADERS "${INCLUDE_DIR}/darwin/darwin_cahal_device.h" )
elseif( ${PLATFORM} STREQUAL "Generic" AND ${TARGET} MATCHES "^android" )
  list( APPEND SOURCES "${SOURCE_DIR}/android/android_cahal.c" )
  list( APPEND SOURCES "${SOURCE_DIR}/android/android_cahal_device.c" )
  list(
        APPEND
        SOURCES
        "${SOURCE_DIR}/android/android_cahal_audio_format_description.c"
      )

  list( APPEND HEADERS "${INCLUDE_DIR}/android/android_cahal.h" )
  list( APPEND HEADERS "${INCLUDE_DIR}/android/android_cahal_device.h" )
  list(
        APPEND
        HEADERS
        "${INCLUDE_DIR}/android/android_cahal_audio_format_description.h"
      )

  find_library  (
                  OPENSLES_LIB
                  OpenSLES
                  HINTS "${CMAKE_FIND_ROOT_PATH}/usr/lib"
                )
  find_library  (
                  MATH_LIB
                  m
                  HINTS "${CMAKE_FIND_ROOT_PATH}/usr/lib"
                )

  set (
        EXTRA_LIBS
        ${OPENSLES_LIB}
        ${MATH_LIB}
      )

  message( STATUS "Android libraries: ${EXTRA_LIBS}" )

  find_path (
              OPENSLES_INCLUDES
              "SLES/OpenSLES.h"
              HINTS "${CMAKE_FIND_ROOT_PATH}/usr/include"
            )

  set (
        EXTRA_INCLUDES
        ${OPENSLES_INCLUDES}
      )
  
  message( STATUS "Android include directories: ${EXTRA_INCLUDES}" )

  include_directories( ${EXTRA_INCLUDES} )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  find_library  (
                  PROPSYS_LIB
                  Propsys 
                )

  set (
        EXTRA_LIBS
        ${PROPSYS_LIB}
      )

  list( APPEND SOURCES "${SOURCE_DIR}/windows/windows_cahal.c" )
  list( APPEND SOURCES "${SOURCE_DIR}/windows/windows_cahal_device.cpp" )

  list( APPEND HEADERS "${INCLUDE_DIR}/windows/windows_cahal.h" )
  list( APPEND HEADERS "${INCLUDE_DIR}/windows/windows_cahal_device.hpp" )
else()
  message( FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}" )
endif()

set( CPCOMMON_INCLUDE_DIR "${cpcommon_SOURCE_DIR}/src/include" )

message (
  STATUS
  "Working with cpcommon source directory: ${CPCOMMON_INCLUDE_DIR}"
        )

include_directories( ${INCLUDE_DIR} )
include_directories( ${CPCOMMON_INCLUDE_DIR} )

message( STATUS "C source files found: ${SOURCES}" )
message( STATUS "C header files found: ${HEADERS}" )

add_library( ${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( ${PROJECT_NAME} cpcommon )

if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin" )
  target_link_libraries( ${PROJECT_NAME} ${EXTRA_LIBS} )
  target_link_libraries( ${PROJECT_NAME} darwinhelper )
elseif( ${PLATFORM} STREQUAL "Generic" AND ${TARGET} MATCHES "^android" )
  #target_link_libraries( ${PROJECT_NAME} ${EXTRA_LIBS} )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  target_link_libraries( ${PROJECT_NAME} ${EXTRA_LIBS} )
endif()

install (
          TARGETS ${PROJECT_NAME}
          ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
          LIBRARY DESTINATION "${INSTALL_DIR}/lib"
        )
install( FILES ${HEADERS} DESTINATION "${INSTALL_DIR}/include/${PROJECT_NAME}" )

# Build test suite as well
add_subdirectory( test )

enable_testing()

find_package( Doxygen )

if( DOXYGEN_FOUND )
  set( DOXYGEN_OUTPUT "${INSTALL_DIR}/doc/${PROJECT_NAME}" )

  configure_file  (
    ${PROJECT_SOURCE_DIR}/Doxyfile.in
    ${PROJECT_BINARY_DIR}/Doxyfile
    @ONLY
                  )

  add_custom_target (
    doc_cahal
    ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
                    )

  file( MAKE_DIRECTORY ${DOXYGEN_OUTPUT} )

endif(DOXYGEN_FOUND)
