#################################################
# cmake file for building DQMViz package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# ----- project name -----
PROJECT( dCache-gui )

SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# ----- Generate DOXYGEN documentation (optionnal) -----
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# ----- Install doc -----
IF( INSTALL_DOC )

FIND_PACKAGE( Doxygen )

IF( DOXYGEN_FOUND )
ADD_SUBDIRECTORY( ./doc )
ELSE()
MESSAGE( SEND_ERROR "Could not find doxygen required to build documentation" )
MESSAGE( "Please install doxygen or set INSTALL_DOC to OFF" )
MESSAGE( "" )
ENDIF()

ENDIF()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# include directories
set( HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui )

INCLUDE_DIRECTORIES (${HEADER_DIR})

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

subdirs(src)
