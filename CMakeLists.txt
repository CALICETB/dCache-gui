#################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# ----- project name -----
PROJECT( dCache-gui )

SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 0 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )
    
#################################################################
# Require C++11                                                 #
#################################################################

OPTION( ENABLE_C++11 "Set to ON to enable c++11" OFF )

IF(ENABLE_C++11)
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
MESSAGE( SEND_ERROR "dCache-gui requires C++11 support. Please upgrade your compiler !" )
ENDIF()
ENDIF()

###############################
# Dependencies
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
INCLUDE( default_settings )

################################################################
# GFAL Utils                                                   #
################################################################

OPTION( IS_DEVEL "Set to ON to skip gfal-utils check" ON )

IF( NOT IS_DEVEL )
FIND_PACKAGE( GRID_TOOLS REQUIRED )
ELSE( NOT IS_DEVEL )
SET( GRID_TOOLS_FOUND FALSE )
ENDIF( NOT IS_DEVEL )

################################################################
# Qt                                                           #
################################################################


FIND_PACKAGE( Qt4 COMPONENTS QtCore QtGui REQUIRED )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIRS} )
ADD_DEFINITIONS ( ${QT_DEFINITIONS} )
INCLUDE( ${QT_USE_FILE} )

# ----- Add source directory -----
ADD_SUBDIRECTORY( ./source )

################################################################
# documentation                                                #
################################################################

OPTION( BUILD_DOCUMENTATION  "Set to OFF to skip build/install Documentation" ON )

IF( BUILD_DOCUMENTATION )
    FIND_PACKAGE( Doxygen )

    IF( DOXYGEN_FOUND )
        ADD_SUBDIRECTORY( ./doc )
    ELSE()
        MESSAGE( SEND_ERROR "Could not find doxygen required to build documentation" )
        MESSAGE( "Please install doxygen or set BUILD_DOCUMENTATION to OFF" )
        MESSAGE( "" )
    ENDIF()
ENDIF()

# ----- display some variables and write them to cache -----
DISPLAY_STD_VARIABLES()

# ----- generate and install following configuration files -----
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake )
