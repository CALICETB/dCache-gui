#  CMake used to find if gfal-utils are installed
#  Once done this will define
#  GRID_TOOLS_FOUND - System has GFAL_UTILS and VOMS

find_program (GFAL_LS NAMES gfal-ls 
					  PATHS /usr/bin )

find_program (GFAL_COPY NAMES gfal-copy 
					  PATHS /usr/bin )
					  
find_program (VOMS_INIT NAMES voms-proxy-init 
					  PATHS /usr/bin )
					  				  
find_program (VOMS_INFO NAMES voms-proxy-info
					  PATHS /usr/bin )
					  
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GFAL_UTILS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GRID_TOOLS  DEFAULT_MSG
                                  GFAL_LS GFAL_COPY VOMS_INIT VOMS_INFO)
                                  
                                  
MESSAGE( STATUS "" )
MESSAGE( STATUS "---------------------------------------" )
MESSAGE( STATUS "GFAL_LS                    = ${GFAL_LS}" )
MESSAGE( STATUS "GFAL_COPY                  = ${GFAL_COPY}" )
MESSAGE( STATUS "VOMS_INIT                  = ${VOMS_INIT}" )
MESSAGE( STATUS "VOMS_INFO                  = ${VOMS_INFO}" )
MESSAGE( STATUS "---------------------------------------" )
MESSAGE( STATUS "" )