 ################################
# Provides FULL_INSTALL_PATH
# FULL_INSTALL_PATH ( <component> <destination variable> )
# <component>            : LIB,BIN,INCLUDE or CMAKE,...
# <destination variable> : name of varible to hold complete path
#
# computes the full install path for given component
# result will be ${CMAKE_INSALL_PREFIX}/${<component>}_INSTALL_DIR, if
# ${<component>}_INSTALL_DIR is a relative PATH, will be
# ${<component>}_INSTALL_DIR if ${<component>}_INSTALL_DIR is an absolute
# path


MACRO( FULL_INSTALL_PATH _fip_component _fip_destvarname )
  IF ( ${ARGC} GREATER 2 )
    MESSAGE( FATAL_ERROR "FULL_INSTALL_PATH invoked with incorrect number of arguments." )
  ENDIF ( ${ARGC} GREATER 2 )

  IF ( ${_fip_component}_INSTALL_DIR MATCHES "^/.*" )
    SET( ${_fip_destvarname} "${${_fip_component}_INSTALL_DIR}" )
  ELSE( ${_fip_component}_INSTALL_DIR MATCHES "^/.*" )
    SET( ${_fip_destvarname} "${CMAKE_INSTALL_PREFIX}/${${_fip_component}_INSTALL_DIR}" )
  ENDIF( ${_fip_component}_INSTALL_DIR MATCHES "^/.*" )

ENDMACRO( FULL_INSTALL_PATH ) 
