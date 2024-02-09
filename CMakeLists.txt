cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "23")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "5")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kmag VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.92.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMDeprecationSettings)
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

include(ECMInstallIcons)
include(FeatureSummary)
include(ECMSetupVersion)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    PrintSupport
)

find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
    DocTools
    I18n
    KIO
    XmlGui
)

if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(QAccessibilityClient6 0.4 CONFIG)
    set_package_properties(QAccessibilityClient6 PROPERTIES
        DESCRIPTION "KDE client-side accessibility library"
        URL "https://commits.kde.org/libqaccessibilityclient"
        TYPE OPTIONAL
        PURPOSE "Required to enable keyboard focus tracking."
    )
else()
    find_package(QAccessibilityClient 0.4 CONFIG)
    set_package_properties(QAccessibilityClient PROPERTIES
        DESCRIPTION "KDE client-side accessibility library"
        URL "https://commits.kde.org/libqaccessibilityclient"
        TYPE OPTIONAL
        PURPOSE "Required to enable keyboard focus tracking."
    )
endif()

ecm_set_disabled_deprecation_versions(QT 6.4  KF 5.105.0)

add_subdirectory( doc )
add_subdirectory( pics )
add_subdirectory( src )

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
