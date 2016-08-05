
file(GLOB GLOB_SRC
       src/*.cpp)

SET ( INC_FILES
      include/dCacheMainWindow.h
      include/dCacheTools.h
      include/Logger.h
      include/ProxyTools.h)
      
file(GLOB GLOB_UI
        ${PROJECT_SOURCE_DIR}/ui/*.ui )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

INCLUDE_DIRECTORIES( BEFORE include ${QT_INCLUDE_DIRS})

SET(CMAKE_AUTOMOC OFF)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

QT4_WRAP_CPP( MOC_SRC_FILES ${INC_FILES} )
QT4_WRAP_UI( UI_FILES ${GLOB_UI} )
			
ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${GLOB_SRC} ${MOC_SRC_FILES} ${UI_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} )

INSTALL(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
)

ADD_EXECUTABLE( dCache_start_gui main/dCache_gui.cpp )
TARGET_LINK_LIBRARIES( dCache_start_gui ${PROJECT_NAME} ${PROJECT_NAME} )

INSTALL(
    TARGETS dCache_start_gui
    RUNTIME DESTINATION bin
)

# ----- Install headers -----
INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )
