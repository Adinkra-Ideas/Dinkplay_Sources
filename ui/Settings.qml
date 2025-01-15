import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: settingsPage

    implicitHeight: columnLayout.anchors.topMargin + intervalRunningAnimation.height + lapCountComponent.height + lapDurationComponent.height + restDurationComponent.height + startStopSwitch.height + aboutButton.height + last.height

    signal aboutPageSignal

    // Component.onCompleted: {
    //     console.log(settingsPage.height, " the height")
    // }

    ColumnLayout {
        id: columnLayout
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 55       // TopControls height == 40
            leftMargin: 10
            rightMargin: 10
            // bottomMargin: 900    // MediaControls height == 55
        }
        spacing: 10

        Item {
            id: intervalRunningAnimation
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 42 * 3
            visible: Media.intervalStatus
            enabled: Media.intervalStatus
            AnimatedImage {
                width: parent.width
                height: parent.height
                source: "images/musicIco/exercise.gif"
                fillMode: AnimatedImage.PreserveAspectFit
                paused: Media.player !== 1 /* pause when music player is paused */
            }
        }

        Item {
            id: lapCountComponent
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 42
            visible: !Media.intervalStatus
            enabled: !Media.intervalStatus
            Text {
                id: lapCountText
                text: "How Many Laps?"
                color: "white"
                font.pointSize: 13
            }
            Slider {
                id: lapCount
                implicitWidth: parent.width * 0.7;
                implicitHeight: 20
                anchors {
                    top: lapCountText.bottom
                    topMargin: 3
                }
                from: 1
                value: 1
                to: 99
                stepSize: 1

                handle: Rectangle {
                    x: lapCount.leftPadding + lapCount.visualPosition * (lapCount.availableWidth - width)
                    y: lapCount.topPadding + lapCount.availableHeight / 2 - height / 2
                    implicitWidth: (Qt.platform.os == "ios") ? 20 : 20
                    implicitHeight: implicitWidth
                    radius: implicitWidth * 0.5
                }
            }
            TextField {
                validator: IntValidator {bottom: lapCount.from; top: lapCount.to}
                anchors {
                    top: lapCountText.top
                    bottom: lapCount.bottom
                    left: lapCount.right
                    margins: 5
                }
                color: "white"
                text: lapCount.value
                font.pointSize: 15
                background : Rectangle {
                    color: "black"
                    radius: 2
                    border {
                        color: "#1777B7"
                        width: 0.5
                    }
                }
                onEditingFinished: {
                    lapCount.value = text
                }
            }
        }

        Item {
            id: lapDurationComponent
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 42
            visible: !Media.intervalStatus
            enabled: !Media.intervalStatus
            Text {
                id: lapDurText
                text: "Duration For Each Lap: (Secs)"
                color: "white"
                font.pointSize: 13
            }
            Slider {
                id: lapDuration
                implicitWidth: parent.width * 0.7;
                implicitHeight: 20
                anchors {
                    top: lapDurText.bottom
                    topMargin: 3
                }
                from: 0
                value: 0
                to: 10800
                stepSize: 1

                handle: Rectangle {
                    x: lapDuration.leftPadding + lapDuration.visualPosition * (lapDuration.availableWidth - width)
                    y: lapDuration.topPadding + lapDuration.availableHeight / 2 - height / 2
                    implicitWidth: (Qt.platform.os == "ios") ? 20 : 20
                    implicitHeight: implicitWidth
                    radius: implicitWidth * 0.5
                }
            }
            TextField {
                validator: IntValidator {bottom: lapDuration.from; top: lapDuration.to}
                anchors {
                    top: lapDurText.top
                    bottom: lapDuration.bottom
                    left: lapDuration.right
                    margins: 5
                }
                color: "white"
                text: lapDuration.value
                font.pointSize: 15
                background : Rectangle {
                    color: "black"
                    radius: 2
                    border {
                        color: "#1777B7"
                        width: 0.5
                    }
                }
                onEditingFinished: {
                    lapDuration.value = text
                }
            }
        }

        Item {
            id: restDurationComponent
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 42
            visible: !Media.intervalStatus
            enabled: !Media.intervalStatus
            Text {
                id: restDurText
                text: "Duration for Resting: (Secs)"
                color: "white"
                font.pointSize: 13
            }
            Slider {
                id: restDuration
                implicitWidth: parent.width * 0.7;
                implicitHeight: 20
                anchors {
                    top: restDurText.bottom
                    topMargin: 3
                }
                from: 1
                value: 1
                to: 1800
                stepSize: 1

                handle: Rectangle {
                    x: restDuration.leftPadding + restDuration.visualPosition * (restDuration.availableWidth - width)
                    y: restDuration.topPadding + restDuration.availableHeight / 2 - height / 2
                    implicitWidth: (Qt.platform.os == "ios") ? 20 : 20
                    implicitHeight: implicitWidth
                    radius: implicitWidth * 0.5
                }
            }
            TextField {
                validator: IntValidator {bottom: restDuration.from; top: restDuration.to}
                anchors {
                    top: restDurText.top
                    bottom: restDuration.bottom
                    left: restDuration.right
                    margins: 5
                }
                color: "white"
                text: restDuration.value
                font.pointSize: 15
                background : Rectangle {
                    color: "black"
                    radius: 2
                    border {
                        color: "#1777B7"
                        width: 0.5
                    }
                }
                onEditingFinished: {
                    restDuration.value = text
                }
            }
        }
        Item {
            id: startStopSwitch
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 42
            Switch {
                id: startStop
                font.pointSize: 12
                y: 10
                checked: Media.intervalStatus
                onClicked: {
                    if (startStop.checked) {
                        // Return and do nothing if the user didnt input correct lap duration
                        if (lapDuration.value == 0) {
                            startStop.checked = false;
                            return ;
                        }

                        // pass values of all three to CPP
                        Media.lapCount = lapCount.value
                        Media.lapDuration = lapDuration.value
                        Media.restDuration = restDuration.value

                        Media.intervalStatus = true;
                    } else {
                        Media.intervalStatus = false;
                    }
                }

                indicator: Rectangle {
                    implicitWidth: 48
                    implicitHeight: 20
                    radius: 10
                    color: startStop.checked ? "#1777B7" : "grey"

                    Rectangle {
                        x: startStop.checked ? parent.width - width : 0
                        Behavior on x { NumberAnimation { duration: 200 } }
                        width: 20
                        height: 20
                        radius: 13
                        color: startStop.down ? "#cccccc" : "#ffffff"
                        border.color: startStop.checked ? (startStop.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
            }
        }

        Item {
            id: aboutButton
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 84

            Text {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                }
                text: "> About Dinkplay"
                color: "grey"
                font.pointSize: 13
                font.underline: true
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        settingsPage.aboutPageSignal()
                    }
                }
            }

            // Button {
            //     anchors {
            //         bottom: parent.bottom
            //         horizontalCenter: parent.horizontalCenter
            //     }
            //     text: "About Dinkplay"
            //     background: Rectangle {
            //         implicitWidth: 100
            //         implicitHeight: 30
            //         color: "grey"
            //         radius: 2
            //     }
            //     onClicked: {
            //         settingsPage.aboutPageSignal()
            //     }
            // }
        }



        Rectangle {
            id: last
            color: "transparent"
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 60 // MediaControls height == 55
        }

    }

}
