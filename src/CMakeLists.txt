configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/focustrackconfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/focustrackconfig.h"
)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX KMAG
    VERSION_HEADER kmag_version.h
)

add_executable(kmag)

target_sources(kmag PRIVATE
    kmagzoomview.cpp
    kmagselrect.cpp
    kmag.cpp
    colorsim.cpp
    main.cpp

    kmag.qrc
)

target_link_libraries(kmag
    KF5::KIOCore
    KF5::I18n
    KF5::XmlGui
    Qt::Core
    Qt::Widgets
    Qt::PrintSupport
)

if(QAccessibilityClient_FOUND)
    target_link_libraries(kmag ${QACCESSIBILITYCLIENT_LIBRARY})
endif()

install(TARGETS kmag ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.kmag.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kmag.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
