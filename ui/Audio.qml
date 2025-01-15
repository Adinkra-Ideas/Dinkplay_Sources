import QtQuick

// This only scrolls because of ListView default state
// is to scroll. Else, implicitHeight == Window.height
// meant the parent scrollview will always detect this
// child's height to be same as Window.height meaning
// it would have never scrolled.
Item {
    implicitHeight: Window.height
    implicitWidth: Window.width

    ListView {
        id: theListView
        anchors{
            fill: parent
            topMargin: 55       // TopControls height == 40
            leftMargin: 10
            rightMargin: 10
            bottomMargin: 55    // MediaControls height == 55
        }
        clip: true

        property int delegateHeight: 40

        model: Media.audiopaths
        delegate: Item {
            id: theDelegate
            width: theListView.width
            height: theListView.delegateHeight
            Text {
                id: audioName
                text: modelData.slice(modelData.lastIndexOf('/') + 1)
                // color: index % 2 ? "grey" : "yellow"
                color: theListView.currentIndex === index ? "white" : "grey"
                font.pointSize: 14
                anchors.verticalCenter: parent.verticalCenter
                // anchors.left: parent.left
                // anchors.leftMargin: 5
                x: 5
            }
            MouseArea {
                anchors.fill : parent
                onClicked: {
                    Media.playSource = modelData
                }
            }
            /* Set this delegate to currentIndex whenever the PlaybackStateChanged path === path in its modeldata */
            Connections {
                target: Media
                function onPlaybackStateChanged(newPath) {
                    if (newPath === modelData) {
                        theListView.currentIndex = index
                    }
                }
            }
            /* Set this delegate to currentIndex whenever the PlaybackStateChanged path === path in its modeldata */

            SequentialAnimation {
                id: audioNameAnim
                loops: Animation.Infinite
                running: (audioName.width > theDelegate.width) && theListView.currentIndex === index
                NumberAnimation { target: audioName; property: "x"; to: -audioName.width; duration: (audioName.width / 25) * 1000 }
                NumberAnimation { target: audioName; property: "x"; to: 5; duration: 5000 }
                onStopped: {
                    audioName.x = 5
                }
            }
        }
        /* highlights the delegate which is currently used as the currentIndex of this listModel */
        highlight: Rectangle { color: "#1777B7"; radius: 2;}
        focus: true
        /* highlights the delegate which is currently used as the currentIndex of this listModel */
    }
}
