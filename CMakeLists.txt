project(kmag)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

macro_optional_find_package(QAccessibilityClient)
macro_log_feature(QAccessibilityClient_FOUND "qaccessibilityclient" "KDE client-side accessibility library" "https://projects.kde.org/projects/playground/accessibility/libkdeaccessibilityclient" FALSE "" "Required to enable keyboard focus tracking.")

configure_file(
    "${PROJECT_SOURCE_DIR}/focustrackconfig.h.in"
    "${PROJECT_BINARY_DIR}/focustrackconfig.h")

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
if(QAccessibilityClient_FOUND)
    include_directories(${QACCESSIBILITYCLIENT_INCLUDE_DIR})
endif(QAccessibilityClient_FOUND)

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
if(QAccessibilityClient_FOUND)
    target_link_libraries(kmag ${QACCESSIBILITYCLIENT_LIBRARY})
endif(QAccessibilityClient_FOUND)

install(TARGETS kmag  ${INSTALL_TARGETS_DEFAULT_ARGS} )

macro_display_feature_log()


########### install files ###############

install( PROGRAMS kmag.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmagui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmag )

kde4_install_icons( ${ICON_INSTALL_DIR} )
