#############################################################################
#
# This CMake macro allows to autmatically generate Doxygen ducumentation.
#
# It expects a file doc/Doxyfile.in relative to the project base directory.
# The file may contain cmake variables in both ${XXX} or @XXX@ notation,
# which will be substituted with the actual value before Doxygen is executed.
# The building of the documentation takes part in the build tree.
#
# The target doc is added and included in the all target.
# The source tree is searched for .C .cc .h .hh files and doc will depend
# on the files found. It will also depend on doc/Doxyfile.in
#
# The content of the directories html and latex will be installed to
# <install-directory>/doc/${PROJECT_NAME}
#
# author:  Benjamin.Lutz@desy.de
# version: 1.2
# date:    28.01.2009
#
#############################################################################


FIND_PACKAGE( Doxygen )
IF( DOXYGEN_FOUND )

    SET (${PROJECT_NAME}_DOC_INSTALL_DIR "doc/${PROJECT_NAME}" CACHE STRING "Installation directory for the documentaion" )

    SET (${PROJECT_NAME}_DOC_AUTOMAKE ON CACHE BOOL "If set to ON the documentation will be generated with make all" )
    MARK_AS_ADVANCED(${PROJECT_NAME}_DOC_AUTOMAKE)

    SET (${PROJECT_NAME}_DOC_INSTALL ON CACHE BOOL "If set to ON the documentation will be generated with make all" )
    MARK_AS_ADVANCED(${PROJECT_NAME}_DOC_INSTALL)
    IF (${PROJECT_NAME}_DOC_INSTALL AND NOT ${PROJECT_NAME}_DOC_AUTOMAKE)
        MESSAGE ( WARNING " ${PROJECT_NAME}_DOC_INSTALL forces ${PROJECT_NAME}_DOC_AUTOMAKE=ON")
        SET(${PROJECT_NAME}_DOC_AUTOMAKE ON CACHE BOOL "documentation auto-building forced by ${PROJECT_NAME}_DOC_INSTALL" FORCE)
    ENDIF (${PROJECT_NAME}_DOC_INSTALL AND NOT ${PROJECT_NAME}_DOC_AUTOMAKE)

    SET (${PROJECT_NAME}_DOC_VERBOSE OFF CACHE BOOL "If set to ON full Doxygen output is enabled" )
    MARK_AS_ADVANCED(${PROJECT_NAME}_DOC_VERBOSE)

    SET (${PROJECT_NAME}_DOC_WORKDIR ${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME} )

    FILE(MAKE_DIRECTORY ${${PROJECT_NAME}_DOC_WORKDIR} )
    FILE(MAKE_DIRECTORY ${${PROJECT_NAME}_DOC_WORKDIR}/html )
    FILE(MAKE_DIRECTORY ${${PROJECT_NAME}_DOC_WORKDIR}/latex )

    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile )

    IF (${PROJECT_NAME}_DOC_VERBOSE)

        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nQUIET = NO\n")
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nWARNINGS = YES\n")
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nWARN_IF_UNDOCUMENTED = YES\n")
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nWARN_IF_DOC_ERROR = YES\n" )

    ELSE (${PROJECT_NAME}_DOC_VERBOSE)
 
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nQUIET = YES\n")
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nWARNINGS = NO\n")
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nWARN_IF_UNDOCUMENTED = NO\n")
        FILE(APPEND ${${PROJECT_NAME}_DOC_WORKDIR}/Doxyfile "\nWARN_IF_DOC_ERROR = YES\n" )

    ENDIF (${PROJECT_NAME}_DOC_VERBOSE)

    FILE(GLOB_RECURSE ${PROJECT_NAME}_DOC_HEADERS
         RELATIVE ${PROJECT_SOURCE_DIR}
         *.h *.hh )

    FILE(GLOB_RECURSE ${PROJECT_NAME}_DOC_SOURCES
         RELATIVE ${PROJECT_SOURCE_DIR}
         *.cc *.C )

    FOREACH( CODE_FILE ${${PROJECT_NAME}_DOC_HEADERS} ${${PROJECT_NAME}_DOC_SOURCES} )
        LIST ( APPEND ${PROJECT_NAME}_DOC_DEPENDENCIES  "${PROJECT_SOURCE_DIR}/${CODE_FILE}" )
    ENDFOREACH( CODE_FILE )


    IF ( NOT global_doc_exists )
      ADD_CUSTOM_TARGET( doc )
      SET( global_doc_exists ON PARENT_SCOPE)
    ENDIF ( NOT global_doc_exists )

    IF (${PROJECT_NAME}_DOC_AUTOMAKE)
      ADD_CUSTOM_TARGET( doc_${PROJECT_NAME} ALL DEPENDS ${${PROJECT_NAME}_DOC_WORKDIR}/html/index.html )
    ELSE (${PROJECT_NAME}_DOC_AUTOMAKE)
      ADD_CUSTOM_TARGET( doc_${PROJECT_NAME} DEPENDS ${${PROJECT_NAME}_DOC_WORKDIR}/html/index.html )
    ENDIF (${PROJECT_NAME}_DOC_AUTOMAKE)

    ADD_DEPENDENCIES( doc doc_${PROJECT_NAME} )




    ADD_CUSTOM_COMMAND( OUTPUT ${${PROJECT_NAME}_DOC_WORKDIR}/html/index.html
                        COMMAND ${DOXYGEN_EXECUTABLE}
                        WORKING_DIRECTORY ${${PROJECT_NAME}_DOC_WORKDIR}
                        DEPENDS ${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${${PROJECT_NAME}_DOC_DEPENDENCIES}
                        COMMENT "Building API Documentation for ${PROJECT_NAME}..."
                        )
                       
    IF ( ${PROJECT_NAME}_DOC_INSTALL )
      INSTALL( DIRECTORY ${${PROJECT_NAME}_DOC_WORKDIR}/html ${${PROJECT_NAME}_DOC_WORKDIR}/latex
               DESTINATION ${${PROJECT_NAME}_DOC_INSTALL_DIR}
               PATTERN "CVS" EXCLUDE
               )
    ENDIF ( ${PROJECT_NAME}_DOC_INSTALL )


    MESSAGE( STATUS "")
    MESSAGE( STATUS "---------------------------------------" )
    MESSAGE( STATUS " Documentation for ${PROJECT_NAME}     " )
    MESSAGE( STATUS " ${PROJECT_NAME}_DOC_AUTOMAKE      = ${${PROJECT_NAME}_DOC_AUTOMAKE}" )
    MESSAGE( STATUS " ${PROJECT_NAME}_DOC_INSTALL       = ${${PROJECT_NAME}_DOC_INSTALL}" )
    MESSAGE( STATUS " ${PROJECT_NAME}_DOC_WORK_DIR      = ${${PROJECT_NAME}_DOC_WORKDIR}" )
    MESSAGE( STATUS " ${PROJECT_NAME}_DOC_INSTALL_DIR   = ${${PROJECT_NAME}_DOC_INSTALL_DIR}" )
    MESSAGE( STATUS " ${PROJECT_NAME}_DOC_VERBOSE       = ${${PROJECT_NAME}_DOC_VERBOSE}    " )
    MESSAGE( STATUS "---------------------------------------" )
    MESSAGE( STATUS "")

ELSE ( DOXYGEN_FOUND)        

    MESSAGE( WARNING "************************************************************")
    MESSAGE( WARNING "* Doxygen was not found! No documenation can be generated. *")
    MESSAGE( WARNING "************************************************************")

ENDIF( DOXYGEN_FOUND )
