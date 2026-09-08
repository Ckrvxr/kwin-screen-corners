cmake_minimum_required(VERSION 3.28)

project(kwin-screen-corners VERSION 0.1.11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM 6.13 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Widgets DBus)
find_package(KF6Config 6.13 REQUIRED)
find_package(KF6CoreAddons 6.13 REQUIRED)
find_package(KF6I18n 6.13 REQUIRED)
find_package(KF6KCMUtils 6.13 REQUIRED)
find_package(KWin 6.3 REQUIRED)

set(kwin_screen_corners_effect_sources
    kwin_screen_corners_effect.cpp
    main.cpp
)
kconfig_add_kcfg_files(kwin_screen_corners_effect_sources
    kwin_screen_corners.kcfgc
)

set(kwin_screen_corners_config_sources
    kwin_screen_corners_config.cpp
)
kconfig_add_kcfg_files(kwin_screen_corners_config_sources
    kwin_screen_corners.kcfgc
)

kcoreaddons_add_plugin(kwin-screen-corners
    SOURCES ${kwin_screen_corners_effect_sources}
    INSTALL_NAMESPACE "kwin/effects/plugins"
)

target_link_libraries(kwin-screen-corners PRIVATE
    KWin::kwin
    KF6::ConfigGui
    Qt6::Core
)

kcoreaddons_add_plugin(kwin_screen_corners_config
    SOURCES ${kwin_screen_corners_config_sources}
    INSTALL_NAMESPACE "kwin/effects/configs"
)

target_link_libraries(kwin_screen_corners_config PRIVATE
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::ConfigGui
    KF6::I18n
    Qt6::Core
    Qt6::Widgets
    Qt6::DBus
)

install(FILES
    LICENSE
    DESTINATION ${KDE_INSTALL_DATADIR}/licenses/kwin-screen-corners
)
