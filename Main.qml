import QtQuick
import QtQuick.Controls
import QtQml
// import QtMultimedia
import "ui"

Window {
    id: mainWindow
    minimumWidth: 320
    minimumHeight: 480
    maximumWidth: 1920  /* max we can test due to our laptop's resolution */
    maximumHeight: 1080 /* max we can test due to our laptop's resolution */
    color: "black"
    visible: true
    title: qsTr("DinkPlay")
    // Let Activity determine whether to close or minimize
    onClosing: {
        if (Qt.platform.os == "android" /*ios*/) {
            Media.checkForBackPress()
            close.accepted = false
        }
    }

    Item {
        id: root
        width: Window.width
        height: Window.height

        // Things inside here will be scrollable if overflow
        ScrollView {
            id: view
            anchors.fill: parent
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            clip: true
            contentWidth: root.width
            contentHeight: audioPage.height

            Playing {
                id: playingPage
                width: root.width
                visible: false
            }
            Audio {
                id: audioPage
                width: root.width
                visible: true
            }
            Settings {
                id: settingsPage
                width: root.width
                visible: false

                onAboutPageSignal: {
                    topControls.onAboutSignal()
                }
            }
            About {
                id: aboutPage
                width: root.width
                visible: false
            }
        }

        // an activity listener to hide the MediaControls when idle
        MouseArea {
            id: activityListener
            anchors.fill: parent
            z: 1
            propagateComposedEvents: true
            hoverEnabled: true
            property bool inactiveMouse: false
            // set inactiveMouse to inactive every 3 seconds
            Timer {
                id: timer
                interval: 7000
                onTriggered: {
                    activityListener.inactiveMouse = true
                }
            }
            function mouseEventHandler(mouse) {
                if (activityListener.inactiveMouse)
                    activityListener.inactiveMouse = false
                timer.restart()
                timer.start()
                mouse.accepted = false
            }
            onPressed: mouse => mouseEventHandler(mouse)
            onPositionChanged: mouse => mouseEventHandler(mouse)
            onWheel: mouse => mouseEventHandler(mouse)
            onDoubleClicked: mouse => mouse.accepted = false // setting mouse.accepted to false prevents the cursor from grabbing
        }

        TopControls {
            id: topControls
            color: mainWindow.color
            property bool showTopControls: !activityListener.inactiveMouse
            opacity: showTopControls
            Behavior on opacity { NumberAnimation { duration: 500 } }
            anchors.top: root.top
            anchors.left: root.left
            anchors.right: root.right
            height: 40
            playingTab: playingPage.visible
            audioTab: audioPage.visible
            settingsTab: settingsPage.visible
            aboutTab: aboutPage.visible

            // onClickSet the play tab active and bring put the playing window into view
            onPlaySignal: {
                hideAllTabs()
                view.contentHeight = playingPage.height
                playingPage.visible = true
            }
            onAudioSignal: {
                hideAllTabs()
                view.contentHeight = audioPage.height
                audioPage.visible = true
            }
            onSettingsSignal: {
                hideAllTabs()
                view.contentHeight = settingsPage.height
                settingsPage.visible = true
            }
            // this should enable the onAboutSignal
            // to be called from external components
            function onAboutSignal() {
                hideAllTabs()
                view.contentHeight = aboutPage.height
                aboutPage.visible = true
            }
            function hideAllTabs() {
                playingPage.visible = false
                audioPage.visible = false
                settingsPage.visible = false
                aboutPage.visible = false
            }
        }

        MediaControls {
            id: mediaControls
            color: mainWindow.color
            visible:  Media.audio !== 0
            property bool showMediaControls: !activityListener.inactiveMouse
            // onOpacityChanged can't be used as it is animated and therefore not immediate
            onShowMediaControlsChanged: {
                activityListener.cursorShape = showMediaControls ? Qt.ArrowCursor : Qt.BlankCursor
            }
            opacity: showMediaControls
            Behavior on opacity { NumberAnimation { duration: 500 } }
            anchors.bottom: root.bottom
            // anchors.bottomMargin: 10
            anchors.left: root.left
            anchors.right: root.right
            height: 55
        }
    }
}
