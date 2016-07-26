#################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

#As of CMake 2.6.0 the ELSE() and ENDIF() constructs can be empty
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

# ----- project name -----
PROJECT( DCACHE_GUI )

SET( ${PROJECT_NAME}_MAJOR_VERSION 1 )
SET( ${PROJECT_NAME}_MINOR_VERSION 0 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

# library *nix style versioning
SET( ${PROJECT_NAME}_SOVERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
SET( ${PROJECT_NAME}_VERSION   "${${PROJECT_NAME}_SOVERSION}.${${PROJECT_NAME}_VERSION_PATCH}" )
    
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

IF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )

#################################################################
# cmake modules                                                 #
#################################################################

SET ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

INCLUDE ( MacroInstallDirectory )    
INCLUDE( FULL_INSTALL_PATH )

#################################################################
# installation options                                          #
#################################################################

# change default install prefix
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )
ENDIF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )

# default destination for header files: ${CMAKE_INSTALL_PREFIX}/install
SET( INCLUDE_INSTALL_DIR "include" CACHE PATH "Directory to install the header files" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for executables: ${CMAKE_INSTALL_PREFIX}/install
SET( BIN_INSTALL_DIR "bin" CACHE PATH "Directory to install the executables" )
MARK_AS_ADVANCED( INCLUDE_INSTALL_DIR )

# default destination for library files: ${CMAKE_INSTALL_PREFIX}/install
SET( LIB_INSTALL_DIR "lib" CACHE PATH "Directory to install the libraries" )
MARK_AS_ADVANCED( LIB_INSTALL_DIR )

# default destination for .cmake files: ${CMAKE_INSTALL_PREFIX}/install
SET( CONFIG_INSTALL_DIR "cmake" CACHE PATH "Directory to install the XXXConfig.cmake files" )
MARK_AS_ADVANCED( CONFIG_INSTALL_DIR )

# provide nicer directory layout in build directory
SET( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH
    "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH
    "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )

##################################################################
# install configuration                                          #
##################################################################
#compute install path for bin, lib and include
FULL_INSTALL_PATH( LIB     FULL_LIB_INSTALL_DIR )
FULL_INSTALL_PATH( INCLUDE FULL_INCLUDE_INSTALL_DIR )

CONFIGURE_FILE( ${DCACHE_GUI_SOURCE_DIR}/cmake/DCACHE_GUIConfig.cmake.in
                ${DCACHE_GUI_BINARY_DIR}/DCACHE_GUIConfig.cmake  @ONLY   )

INSTALL( FILES ${DCACHE_GUI_BINARY_DIR}/DCACHE_GUIConfig.cmake
         DESTINATION ${CONFIG_INSTALL_DIR}                               )

# add library install path to the rpath list
SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH )

# add install path to the rpath list (apple)
IF( APPLE )
    SET( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
    MARK_AS_ADVANCED( CMAKE_INSTALL_NAME_DIR )
ENDIF()

# append link pathes to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

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
INCLUDE( Documentation )
ENDIF()

################################################################
# print status report                                          #
################################################################
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX                    = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "ENABLE C++11                            = ${ENABLE_C++11}" )
MESSAGE( STATUS "IS_DEVEL                                = ${IS_DEVEL}" )
MESSAGE( STATUS "GRID_TOOLS_FOUND                        = ${GRID_TOOLS_FOUND}" )
MESSAGE( STATUS "BUILD_DOCUMENTATION                     = ${BUILD_DOCUMENTATION}" )
MESSAGE( STATUS "QT_QMAKE_EXECUTABLE                     = ${QT_QMAKE_EXECUTABLE}" )
MESSAGE( STATUS "FULL_LIB_INSTALL_DIR                    = ${FULL_LIB_INSTALL_DIR}" )
MESSAGE( STATUS "FULL_INCLUDE_INSTALL_DIR                = ${FULL_INCLUDE_INSTALL_DIR}" )

MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
