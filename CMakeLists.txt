project(kmag)



########### next target ###############

set(kmag_SRCS 
   kmagzoomview.cpp 
   kmagselrect.cpp 
   kmag.cpp 
   main.cpp )

kde4_automoc(${kmag_SRCS})

kde4_add_executable(kmag ${kmag_SRCS})

target_link_libraries(kmag  ${KDE4_KDEPRINT_LIBS} )

install(TARGETS kmag  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kmag.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kmagui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmag )

kde4_install_icons( ${DATA_INSTALL_DIR}/kmag/icons   )
