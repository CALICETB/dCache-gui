#################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# ----- project name -----
PROJECT( dCache-gui )

SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11")
ELSE()
MESSAGE( SEND_ERROR "dCache-gui requires C++11 support. Please upgrade your compiler !" )
ENDIF()

## Prefix path cmake
set (CMAKE_PREFIX_PATH "/home/calice/Qt/5.6/gcc_64")

## Modules path cmake
set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
 
find_package(Git)
if(GIT_FOUND)
    message("git found: ${GIT_EXECUTABLE}")
    execute_process(
      COMMAND git pull
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# ----- Generate DOXYGEN documentation (optionnal) -----
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# ----- Install doc -----
if( INSTALL_DOC )

    find_package( Doxygen )

    if( DOXYGEN_FOUND )
        add_subdirectory( ./doc )
    else()
        message( SEND_ERROR "Could not find doxygen required to build documentation" )
        message( "Please install doxygen or set INSTALL_DOC to OFF" )
    endif()

endif()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

SET(CMAKE_AUTOMOC OFF)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# include directories
set( HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

set(Boost_NO_BOOST_CMAKE         ON)
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.36.0 COMPONENTS system filesystem REQUIRED)
set(LIBS ${LIBS} ${Boost_LIBRARIES})

include_directories (${HEADER_DIR} ${Boost_INCLUDE_DIRS})

subdirs(src)
