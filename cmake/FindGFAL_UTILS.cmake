#  CMake used to find if gfal-utils are installed
#  Once done this will define
#  GFAL_UTILS_FOUND - System has GFAL_UTILS

find_program (GFAL_LS NAMES gfal-ls 
					  PATHS /usr/bin )

find_program (GFAL_COPY NAMES gfal-copy 
					  PATHS /usr/bin )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GFAL_UTILS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GFAL_UTILS  DEFAULT_MSG
                                  GFAL_LS GFAL_COPY)
                                  
                                  
MESSAGE( STATUS "" )
MESSAGE( STATUS "---------------------------------------" )
MESSAGE( STATUS "GFAL_LS                    = ${GFAL_LS}" )
MESSAGE( STATUS "GFAL_COPY                  = ${GFAL_COPY}" )
MESSAGE( STATUS "---------------------------------------" )
MESSAGE( STATUS "" )