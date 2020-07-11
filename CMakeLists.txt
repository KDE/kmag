cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "07")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kmag VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "5.9.0")
set(KF5_MIN_VERSION "5.46.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(FeatureSummary)
include(ECMSetupVersion)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    PrintSupport
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    DocTools
    I18n
    KIO
    XmlGui
)

find_package(QAccessibilityClient 0.4 CONFIG)
set_package_properties(QAccessibilityClient PROPERTIES
    DESCRIPTION "KDE client-side accessibility library"
    URL "https://commits.kde.org/libqaccessibilityclient"
    TYPE OPTIONAL
    PURPOSE "Required to enable keyboard focus tracking."
)

configure_file(
    "${PROJECT_SOURCE_DIR}/focustrackconfig.h.in"
    "${PROJECT_BINARY_DIR}/focustrackconfig.h")

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})


add_definitions(
    -DQT_DEPRECATED_WARNINGS
)

add_subdirectory( doc )
add_subdirectory( pics )
########### next target ###############

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KMAG VERSION_HEADER kmag_version.h)

set(kmag_SRCS
   kmagzoomview.cpp
   kmagselrect.cpp
   kmag.cpp
   colorsim.cpp
   main.cpp )

qt5_add_resources(kmag_SRCS
    kmag.qrc
)
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
install(FILES org.kde.kmag.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
ecm_install_icons( ICONS 16-apps-kmag.png 32-apps-kmag.png  DESTINATION ${ICON_INSTALL_DIR} )
