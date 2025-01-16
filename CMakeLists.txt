cmake_minimum_required(VERSION 3.19)

project(Dinkplay VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(UI ui)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Core
    QuickControls2
    Qml
    Widgets
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appDinkplay
)

qt_add_qml_module(appDinkplay
    URI Dinkplay
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES ${UI}/TopControls.qml
        QML_FILES ${UI}/Playing.qml
        QML_FILES ${UI}/Audio.qml
        QML_FILES ${UI}/Settings.qml
        QML_FILES ${UI}/MediaControls.qml
        QML_FILES ${UI}/About.qml
    RESOURCES
        ${UI}/images/musicSpin.gif
        ${UI}/images/musicIco/artist.png
        ${UI}/images/musicIco/playing.png
        ${UI}/images/musicIco/playing_white.png
        ${UI}/images/musicIco/audio.png
        ${UI}/images/musicIco/audio_white.png
        ${UI}/images/musicIco/add.png
        ${UI}/images/musicIco/settings.png
        ${UI}/images/musicIco/settings_white.png
        ${UI}/images/musicIco/about.png
        ${UI}/images/musicIco/about_white.png
        ${UI}/images/musicIco/rand.png
        ${UI}/images/musicIco/rand_white.png
        ${UI}/images/musicIco/next.png
        ${UI}/images/musicIco/pause.png
        ${UI}/images/musicIco/play.png
        ${UI}/images/musicIco/prev.png
        ${UI}/images/musicIco/loop.png
        ${UI}/images/musicIco/loop1.png
        ${UI}/images/musicIco/loop2.png
        ${UI}/images/musicIco/exercise.gif
    SOURCES
        Media.hpp #Media.cpp
        Top.hpp #Top.cpp
        Directory.hpp #Directory.cpp
        ChangePlay.hpp #ChangePlay.cpp
        Interval.hpp #Interval.cpp
        Player.hpp #Player.cpp
        IntervalWorker.hpp #IntervalWorker.cpp
        #CompileC /Users/daniel/Documents/Dinkplay/build/Qt_6_8_1_for_iOS/build/appDinkplay.build/Release-iphoneos/Objects-normal/arm64/Player.o
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDinkplay PROPERTIES
##    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDinkplay
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


target_link_options(appDinkplay BEFORE PUBLIC
    -L./ma_lib
    ./iOS_Binaries/Media.o
    ./iOS_Binaries/Top.o
    ./iOS_Binaries/Directory.o
    ./iOS_Binaries/ChangePlay.o
    ./iOS_Binaries/Interval.o
    ./iOS_Binaries/Player.o
    ./iOS_Binaries/IntervalWorker.o
    ./iOS_Binaries/main.o
    # ./iOS_Binaries/About_qml.o
    # ./iOS_Binaries/MediaControls_qml.o
    # ./iOS_Binaries/Settings_qml.o
    # ./iOS_Binaries/Audio_qml.o
    # ./iOS_Binaries/Playing_qml.o
    # ./iOS_Binaries/TopControls_qml.o
    # ./iOS_Binaries/appDinkplay_Main_qml.o
    ./iOS_Binaries/MyMPRemoteCC.o
    # Till here found in CompileC /Users/daniel/Documents/Dinkplay/build/Qt_6_8_1_for_iOS/build/appDinkplay.build/Release-iphoneos/Objects-normal/arm64/Player.o
)


#########################################
#       ADD LINKED LIBRARIES            #
#########################################

target_link_libraries(appDinkplay PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::QuickControls2
    Qt6::Qml
    Qt6::Widgets
)

    set(default_audio "${CMAKE_SOURCE_DIR}/android/assets/Dinkplay_Tone.mp3")
    set(default_artist_icon "${UI}/images/musicIco/artist.png")
    set(asset_catalog_path "iOS/Assets.xcassets") #naming the folder ".whatever" makes it pickable as a single file into bundle I believe
    target_sources(appDinkplay PRIVATE
        ${default_audio}
        ${default_artist_icon}
        ${asset_catalog_path}
        # ma_lib/miniaudio.m # added for simulator
        #iOS/src/MyMPRemoteCC.h #iOS/src/MyMPRemoteCC.mm########
    )
    set_source_files_properties(
        ${default_audio} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/audios"
    )
    set_source_files_properties(
        ${default_artist_icon} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/images
    )
    set_source_files_properties(
        ${asset_catalog_path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
    )

    # set_target_properties() is used to initialize qt macros
    # for buld time.
    set_target_properties(appDinkplay PROPERTIES
        # Replaye Qt's default white splashscreen
        QT_IOS_LAUNCH_SCREEN "${CMAKE_SOURCE_DIR}/iOS/LaunchScr.storyboard"

        MACOSX_BUNDLE TRUE #####

        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/iOS/Info.plist

        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon

        # ADD CUSTOM ENTITLEMENTS FOR ADDING ENTITLEMENTS THAT ARE POSSIBLE TO BE ADDED IN CUSTOM .entitlements file
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_SOURCE_DIR}/iOS/appDinkplay.entitlements

        # ADD XCODE SIGNING FROM CMAKE (THESE CAN BE MANUALLY ENTERED IN XCODE DIRECTLY)
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM 9W37AU7273 #big improvement to prevent the manual sign sign sign

        # APPSTORE DEPLOYMENT SETTINGS TO PREVENT REJECTION
        #XCODE_ATTRIBUTE_ENABLE_APP_SANDBOX "YES" #This only enters bunndle if added from here(cmake) or clicked from xcode. Adding it from Entitlements file using "<key>com.apple.security.app-sandbox</key><true/>" wont make it into the app
        #XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
        XCODE_ATTRIBUTE_ENABLE_USER_SELECTED_FILES "readwrite"
        #XCODE_ATTRIBUTE_ALLOW_DYLD_ENVIRONMENT_VARIABLES "YES"
    )


    target_link_libraries(appDinkplay PRIVATE
        miniaudio_iOSLgplv3 #removed for simulator
        "-framework MediaPlayer"
        "-framework AVFoundation"
    )



include(GNUInstallDirs)
install(TARGETS appDinkplay
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
