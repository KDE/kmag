project(kmag)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.2.0")

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    PrintSupport
)

find_package(KF5 REQUIRED COMPONENTS
    DocTools
    I18n
    KIO
    XmlGui
)

find_package(QAccessibilityClient)
set_package_properties(QAccessibilityClient PROPERTIES
    DESCRIPTION "KDE client-side accessibility library"
    URL "https://projects.kde.org/projects/playground/accessibility/libkdeaccessibilityclient"
    TYPE OPTIONAL
    PURPOSE "Required to enable keyboard focus tracking."
)

configure_file(
    "${PROJECT_SOURCE_DIR}/focustrackconfig.h.in"
    "${PROJECT_BINARY_DIR}/focustrackconfig.h")

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
if(QAccessibilityClient_FOUND)
    include_directories(${QACCESSIBILITYCLIENT_INCLUDE_DIR})
endif(QAccessibilityClient_FOUND)

add_subdirectory( doc )
add_subdirectory( pics )
########### next target ###############

set(kmag_SRCS 
   kmagzoomview.cpp 
   kmagselrect.cpp 
   kmag.cpp 
   colorsim.cpp
   main.cpp )

add_executable(kmag ${kmag_SRCS})

target_link_libraries(kmag
    Qt5::Core
    Qt5::Widgets
    Qt5::PrintSupport
    KF5::KIOCore
    KF5::I18n
    KF5::XmlGui
)
if(QAccessibilityClient_FOUND)
    target_link_libraries(kmag ${QACCESSIBILITYCLIENT_LIBRARY})
endif(QAccessibilityClient_FOUND)

install(TARGETS kmag  ${INSTALL_TARGETS_DEFAULT_ARGS} )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)


########### install files ###############

install( PROGRAMS org.kde.kmag.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kmagui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/kmag )

ecm_install_icons( ICONS 16-apps-kmag.png 32-apps-kmag.png  DESTINATION ${ICON_INSTALL_DIR} )
