import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

Item {
    property bool isLandscape: Window.width > Window.height

    // implicitHeight: Window.height
    implicitHeight: child1.height + child2.height

    GridLayout {
        id: thePlaying
        anchors.fill: parent
        anchors.margins: 20
        rowSpacing: 20
        columnSpacing: 20
        flow: parent.isLandscape ? GridLayout.LeftToRight : GridLayout.TopToBottom
        Item {
            id: child1
            Layout.fillWidth: true
            Layout.fillHeight: true
            AnimatedImage {
                id: musicSpin
                width: parent.width / 2
                height: parent.height / 2
                anchors.centerIn: parent
                source: "qrc:/ui/images/musicSpin.gif"
                fillMode: AnimatedImage.PreserveAspectFit
                paused: Media.player !== 1
            }
        }

        Item {
            id: child2
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            Text {
                id: songTitle
                x: 0
                color: "#FFFFFF"
                text: Media.title // "All I have to do is dream"
                font.pointSize: (parent.width / 14.2857142857) > 0 ? parent.width / 14.285714285 : 14 // should be 14 at 200 which is estimated smallest possible display of this grid item
                anchors.top: child2.top;
                anchors.topMargin: 5
                SequentialAnimation {
                    id: songTitleAnim
                    loops: Animation.Infinite
                    running: (songTitle.width > child2.width) // && (playMusic.playbackState === MediaPlayer.PlayingState)
                    NumberAnimation { target: songTitle; property: "x"; to: -songTitle.width; duration: (songTitle.width / 40) * 1000 }
                    NumberAnimation { target: songTitle; property: "x"; to: 0; duration: 5000 }
                }
                // prevent the animation from hanging when windows is resized
                onWidthChanged: {
                    songTitle.x = 0
                }
                onHeightChanged: {
                    songTitle.x = 0
                }
            }

            Text {
                id: artistName
                x: 0
                color: "#1777B7";
                text: "Unknown" //"The Everly Brothers"
                font.pointSize: (parent.width / 20) > 0 ? parent.width / 20 : 10 // should be 10 at 200 which is estimated smallest possible display of this grid item
                anchors.top: songTitle.bottom;
                anchors.topMargin: 5
            }
            SequentialAnimation {
                id: artistNameAnim
                running: (artistName.width > child2.width) // && (playMusic.playbackState === MediaPlayer.PlayingState)
                NumberAnimation { target: artistName; property: "x"; to: -artistName.width; duration: (artistName.width / 50) * 1000 }
                NumberAnimation { target: artistName; property: "x"; to: 0; duration: 5000 }
            }
            // prevent the animation from hanging when windows is resized
            onWidthChanged: {
                artistName.x = 0
            }
            onHeightChanged: {
                artistName.x = 0
            }

            Image {
                id: artistImage
                width: (parent.width < parent.height) ? parent.width / 4 : parent.height / 4
                height: width
                source: "qrc:/ui/images/musicIco/artist.png"
                fillMode: AnimatedImage.PreserveAspectFit
                visible: false // required for // rounded edge effect for image
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: artistName.bottom;
                anchors.topMargin: 25
            }
            // rounded edge effect for image above
            MultiEffect {
                source: artistImage
                anchors.fill: artistImage
                maskEnabled: true
                maskSource: mask
            }
            Item {
                id: mask
                width: artistImage.width
                height: artistImage.height
                layer.enabled: true
                visible: false
                Rectangle {
                    width: artistImage.width
                    height: artistImage.height
                    radius: width/2
                    color: "blue"
                }
            }
        }
    }

}
