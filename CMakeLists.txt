if(Q_WS_X11)
project(kmag)

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)
########### next target ###############

set(kmag_SRCS 
   kmagzoomview.cpp 
   kmagselrect.cpp 
   kmag.cpp 
   colorsim.cpp
   main.cpp )

kde4_add_executable(kmag ${kmag_SRCS})

target_link_libraries(kmag ${KDE4_KIO_LIBS} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS kmag  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kmag.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmagui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmag )

kde4_install_icons( ${ICON_INSTALL_DIR} )

endif(Q_WS_X11)
