#  CMake used to find if gfal-utils are installed
#  Once done this will define
#  GRID_TOOLS_FOUND - System has GFAL_UTILS and VOMS

SET( GFAL_LS_EXECUTABLE GFAL_LS_EXECUTABLE-NOTFOUND )
SET( GFAL_COPY_EXECUTABLE GFAL_COPY_EXECUTABLE-NOTFOUND )
SET( VOMS_INIT_EXECUTABLE VOMS_INIT_EXECUTABLE-NOTFOUND )
SET( VOMS_INFO_EXECUTABLE VOMS_INFO_EXECUTABLE-NOTFOUND )

find_program (GFAL_LS_EXECUTABLE NAMES gfal-ls 
					  PATHS /usr/bin /usr/local/bin /usr/sbin /usr/local/sbin )

find_program (GFAL_COPY_EXECUTABLE NAMES gfal-copy 
					  PATHS /usr/bin /usr/local/bin /usr/sbin /usr/local/sbin )
					  
find_program (VOMS_INIT_EXECUTABLE NAMES voms-proxy-init 
					  PATHS /usr/bin /usr/local/bin /usr/sbin /usr/local/sbin )
					  				  
find_program (VOMS_INFO_EXECUTABLE NAMES voms-proxy-info
					  PATHS /usr/bin /usr/local/bin /usr/sbin /usr/local/sbin )
					  
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GFAL_UTILS_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRID_TOOLS  DEFAULT_MSG
                                  GFAL_LS_EXECUTABLE GFAL_COPY_EXECUTABLE VOMS_INIT_EXECUTABLE VOMS_INFO_EXECUTABLE)
                                  
                                  
MESSAGE( STATUS "" )
MESSAGE( STATUS "---------------------------------------" )
MESSAGE( STATUS "GFAL_LS_EXECUTABLE                    = ${GFAL_LS_EXECUTABLE}" )
MESSAGE( STATUS "GFAL_COPY_EXECUTABLE                  = ${GFAL_COPY_EXECUTABLE}" )
MESSAGE( STATUS "VOMS_INIT_EXECUTABLE                  = ${VOMS_INIT_EXECUTABLE}" )
MESSAGE( STATUS "VOMS_INFO_EXECUTABLE                  = ${VOMS_INFO_EXECUTABLE}" )
MESSAGE( STATUS "---------------------------------------" )
MESSAGE( STATUS "" )

SET( GRID_TOOLS_FOUND ${GRID_TOOLS_FOUND} )