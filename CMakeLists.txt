project(kmag)

add_subdirectory( doc )
########### next target ###############

set(kmag_SRCS 
   kmagzoomview.cpp 
   kmagselrect.cpp 
   kmag.cpp 
   colorsim.cpp
   main.cpp )

kde4_add_executable(kmag ${kmag_SRCS})

target_link_libraries(kmag ${KDE4_KIO_LIBS})

install(TARGETS kmag  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS kmag.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmagui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmag )

kde4_install_icons( ${ICON_INSTALL_DIR} )
