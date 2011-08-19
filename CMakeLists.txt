project(kmag)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

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
