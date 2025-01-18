cmake_minimum_required(VERSION 3.19)

project(Dinkplay VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Core
    QuickControls2
    Qml
    Widgets
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appDinkplay
    closed_sources/fingerprints.cpp
)

target_link_options(appDinkplay BEFORE PUBLIC
    -L./closed_sources
)

target_link_libraries(appDinkplay PRIVATE
    Qt6::Quick                  # LGPLv3 Compatible. See https://doc.qt.io/qt-6/qtquick-index.html#licenses-and-attributions
    Qt6::Core                   # LGPLv3 Compatible. See https://doc.qt.io/qt-6/qtcore-index.html#licenses-and-attributions
    Qt6::QuickControls2         # LGPLv3 Compatible. See https://doc.qt.io/qt-6/qtquickcontrols-index.html#license-and-attributions
    Qt6::Qml                    # LGPLv3 Compatible. See https://doc.qt.io/qt-6/qtqml-index.html#licenses-and-attributions
    Qt6::Widgets                # LGPLv3 Compatible. See https://doc.qt.io/qt-6/qtwidgets-index.html#licenses
    all_iOS
    "-framework MediaPlayer"
    "-framework AVFoundation"
)

set(asset_catalog_path "${CMAKE_SOURCE_DIR}/iOS/Assets.xcassets")

target_sources(appDinkplay PRIVATE ${asset_catalog_path})

set_source_files_properties(
    ${asset_catalog_path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
)

set_target_properties(appDinkplay PROPERTIES
    QT_IOS_LAUNCH_SCREEN "${CMAKE_SOURCE_DIR}/iOS/LaunchScr.storyboard"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/iOS/Info.plist
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
    XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_SOURCE_DIR}/iOS/appDinkplay.entitlements
)

include(GNUInstallDirs)
install(TARGETS appDinkplay
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
