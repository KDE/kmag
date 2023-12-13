if(QAccessibilityClient6_FOUND)
    set(HAVE_QACCESSIBILITYCLIENT TRUE)
elseif(QAccessibilityClient_FOUND)
    set(HAVE_QACCESSIBILITYCLIENT TRUE)
else()
    set(HAVE_QACCESSIBILITYCLIENT FALSE)
endif()

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
    kmagzoomview.cpp kmagzoomview.h
    kmagselrect.cpp kmagselrect.h
    kmag.cpp kmag.h
    colorsim.cpp colorsim.h
    main.cpp

    kmag.qrc
)

target_link_libraries(kmag
    KF6::KIOCore
    KF6::I18n
    KF6::XmlGui
    Qt6::Core
    Qt6::Widgets
    Qt6::PrintSupport
)

if(QAccessibilityClient6_FOUND)
    target_link_libraries(kmag QAccessibilityClient6)
elseif(QAccessibilityClient_FOUND)
    target_link_libraries(kmag ${QACCESSIBILITYCLIENT_LIBRARY})
endif()

install(TARGETS kmag ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.kmag.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kmag.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
