
qt5_wrap_cpp(dCache_gui_SRC
             ${HEADER_DIR}/dCacheMainWindow.h
             ${HEADER_DIR}/dCacheTools.h
             ${HEADER_DIR}/Logger.h)

qt5_wrap_ui(dCache_gui_UI ${UI_DIR}/dCacheMainWindow.ui)

add_executable(dCache_gui dCacheMainWindow.cpp dCache-gui.cpp Logger.cpp dCacheTools.cpp
               ${dCache_gui_UI} ${dCache_gui_SRC})

qt5_use_modules(dCache_gui Widgets)
