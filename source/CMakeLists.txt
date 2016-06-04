
SET ( SRC_FILES 
	  src/dCacheMainWindow.cpp
      src/dCacheTools.cpp
      src/Logger.cpp
      src/ProxyTools.cpp)

SET ( INC_FILES
      include/dCacheMainWindow.h
      include/dCacheTools.h
      include/Logger.h
      include/ProxyTools.h)

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

IF ( WITH_QT5 )

SET(CMAKE_AUTOMOC OFF)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

INCLUDE_DIRECTORIES( BEFORE include ${Qt5Widgets_INCLUDE_DIRS})
SET(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES})
  
QT5_WRAP_CPP(MOC_SRC_FILES ${INC_FILES} )

QT5_WRAP_UI(UI_FILES
			../ui/dCacheMainWindow.ui)

ADD_LIBRARY( ${PROJECT_NAME} SHARED ${SRC_FILES} ${MOC_SRC_FILES} ${UI_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} )

ADD_EXECUTABLE( dCache_start_gui main/dCache_gui.cpp )
TARGET_LINK_LIBRARIES( dCache_start_gui ${PROJECT_NAME} ${PROJECT_NAME} )

QT5_USE_MODULES(dCache_start_gui Core Gui Widgets)

ELSE( WITH_QT5 )

INCLUDE_DIRECTORIES( BEFORE include ${QT_INCLUDE_DIRS})

QT4_WRAP_CPP( MOC_SRC_FILES ${INC_FILES} )
QT5_WRAP_UI( UI_FILES
			../ui/dCacheMainWindow.ui)
			
ADD_LIBRARY( ${PROJECT_NAME} SHARED ${SRC_FILES} ${MOC_SRC_FILES} ${UI_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} )

ADD_EXECUTABLE( dCache_start_gui main/dCache_gui.cpp )
TARGET_LINK_LIBRARIES( dCache_start_gui ${PROJECT_NAME} ${PROJECT_NAME} )

ENDIF ( WITH_QT5 )

# ----- Install headers -----
INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )
