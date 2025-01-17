import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
// import QtMultimedia

Rectangle {
    id: topControls

    property bool playingTab
    signal playSignal;
    required property bool audioTab
    signal audioSignal;
    required property bool settingsTab
    signal settingsSignal;
    required property bool aboutTab
    // signal aboutSignal;

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Item {
            id: playing
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            visible: Media.player !== 0
            // un-commenting this will cause the page
            // to jump into nowPlaying page whenever
            // a new sound starts playing
            // Connections {
            //     target: Media
            //     function onPlaybackStateChanged() {
            //         if (Media.player === 1) {
            //             topControls.playSignal()
            //         }
            //     }
            // }
            Image {
                source: topControls.playingTab ? "qrc:/ui/images/musicIco/playing_white.png" : "qrc:/ui/images/musicIco/playing.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    topControls.playSignal()
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: topControls.audioTab ? "qrc:/ui/images/musicIco/audio_white.png" : "qrc:/ui/images/musicIco/audio.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    topControls.audioSignal()
                }
            }
        }
        Item {
            id: add
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: "qrc:/ui/images/musicIco/add.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // If iPhone, we want to select individual files.
                    // on other platforms, we select full folder.
                    if (Qt.platform.os == "ios") {
                        // fileDialog.open()
                        Media.openDialog
                    } else {
                        folderDialog.open()
                    }
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            // When running Interval stops, emit what will change the view to settings
            Connections {
                target: Media
                function onIntervalStatusChanged() {
                    if (!Media.intervalStatus) {
                        topControls.settingsSignal()
                    }
                }
            }
            Image {
                source: topControls.settingsTab ? "qrc:/ui/images/musicIco/settings_white.png" : "qrc:/ui/images/musicIco/settings.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    topControls.settingsSignal()
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            visible: topControls.aboutTab
            Image {
                source: "qrc:/ui/images/musicIco/about_white.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                // onClicked: {
                //     topControls.aboutSignal()
                //     // we will change image here to active
                // }
            }
        }
    }

    // called when add is clicked
    FolderDialog {
        id: folderDialog
        options: FolderDialog.ReadOnly
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]

        // selectedFolder is the folder that the user just selected last.
        // currentFolder is the folder that is first opened by FolderDialog.
        onAccepted: {
            folderDialog.currentFolder = folderDialog.selectedFolder
            Media.currDir = folderDialog.selectedFolder
        }

        // currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        // selectedFolder: viewer.folder
    }
    FileDialog {
        id: fileDialog
        fileMode: FileDialog.OpenFiles
        options: FileDialog.ReadOnly
        nameFilters: ["Text files (*.mp3)"]
        // currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

        // selectedFile is the file that the user just selected last.
        // currentFolder is the folder that is first opened by FileDialog.
        onAccepted: {
            // fileDialog.currentFolde?r = fileDialog.currentFolder
            // concatenat the array of selectedFiles into one string
            var hold = ""
            for (var i in fileDialog.selectedFiles) {
                hold += fileDialog.selectedFiles[i]
                hold += ","
            }
            Media.currDir = hold
        }
    }

}
