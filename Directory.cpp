#include "Directory.hpp"

#ifdef Q_OS_ANDROID
bool checkPermission() {
    QList<bool> permissions;

    /* PERMISSION REQUIRED TO ACCESS AUDIO FILE TYPES IN DEVICE FROM OUR APP */
    // Allow read of audio files (Granular Media Permissions) (for API 33 and above)
    // https://developer.android.com/about/versions/13/behavior-changes-13#granular-media-permissions
    auto r = QtAndroidPrivate::checkPermission("android.permission.READ_MEDIA_AUDIO").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_MEDIA_AUDIO").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    // Allow read of all file types (for API 32 and below)
    r = QtAndroidPrivate::checkPermission("android.permission.READ_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }

    // Allow display of notifications / foreground service notifications (for API 33 and above)
    r = QtAndroidPrivate::checkPermission("android.permission.POST_NOTIFICATIONS").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.POST_NOTIFICATIONS").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }

    // r = QtAndroidPrivate::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    // r = QtAndroidPrivate::checkPermission("android.permission.MANAGE_EXTERNAL_STORAGE").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.MANAGE_EXTERNAL_STORAGE").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    // r = QtAndroidPrivate::checkPermission("android.permission.READ_MEDIA_IMAGES").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.READ_MEDIA_IMAGES").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    // r = QtAndroidPrivate::checkPermission("android.permission.WRITE_MEDIA_IMAGES").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.WRITE_MEDIA_IMAGES").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    return (permissions.count() != 5);
}
#endif


Directory::Directory(QObject *parent) :
    Media{parent},
    backups_{"Adinkra-Ideas", "Dinkplay"}
{
    // //testing to see what got bundled into the app (ios)
    // QString currDir_ = ".";
    // qDebug() << "checking base:" << currDir_;
    // QDir dir(currDir_);
    // QStringList all = dir.entryList(QStringList()/*, QDir::Dirs*/);
    // for (QString &one: all) {
    //     qDebug() << "checking:" << one;
    // }
}

Directory::~Directory(){}

/**
  * This method loads the QSettings persisted
  * paths stored with key 'soundPaths' into
  * audioPaths_ and then calls preparePathsForPlay();
  * to prepare the app accordingly.
  * NOTE that before loading the persisted data,
  * the App must be at a point where startup is
  * completed. That is why I had to call it from the
  * bottom of main.cpp right before the return;
  * @returns void
  */
void Directory::loadSavedPaths() {
    audioPaths_ = backups_.value("soundPaths").value<QList<QString>>();

    for (QString &aPath: audioPaths_) {
        soundsHash_[aPath] = nullptr;
    }

    preparePathsForPlay();
}


void    Directory::addDir(QUrl path) {
    // Permission request for android users
    #ifdef Q_OS_ANDROID
    checkPermission();
    #endif

    // if there is an active, we stop the ma_sound.
    // THIS MUST BE DONE BEFORE TAMPERING WITH
    // audIt_ or *audIt_ or audioPaths_
    stopAnyCurrentPlaying();

    // Remove the rubbish prefixes that Qt does randomly.
    // Sometimes it sends file:///C:/Users/
    // Other Times it sends C:/Users/
    while (path.isLocalFile()) {
        path = QUrl(path).toLocalFile();
    }
    // Now change path to a schemed path AKA file:///
    if (! path.isLocalFile()) {
        path.setScheme(QString());
        path = QUrl::fromLocalFile(path.toString());
    }

    // Arranging directory path according to android uncertainties and specifications
    #ifdef Q_OS_ANDROID
    // decode twice from %253A to %3A then to :
    path.setUrl(QUrl::fromPercentEncoding(path.toString().toLatin1()));
    path.setUrl(QUrl::fromPercentEncoding(path.toString().toLatin1()));

    qsizetype pos = QString(path.toString()).indexOf("/tree/"); // /tree/primary:
    qsizetype subPathPos = QString(path.toString()).indexOf(":", pos);
    QString subPath;
    if (subPathPos != -1) {
        subPath = QString(path.toString()).sliced(subPathPos + 1);
        if (subPath.size() > 0 && subPath.at(0) != '/')
            subPath.prepend("/");
    }

    if (QString(path.toString()).sliced(pos + 6).startsWith("primary")) // inbuilt memory contains */tree/primary*
        path.setUrl(subPath.prepend("/storage/emulated/0"));
    else
        path.setUrl(QString(path.toString()).sliced(pos + 6, subPathPos - (pos + 6)).prepend("/storage/").append(subPath)); // 6 == len("/tree/"), 9 == len("xxxx-xxxx")
    path = QUrl::fromLocalFile(path.toString());
    #endif

    // If IOS, it means we received a big string with each selected filepath separated by a comma.
    // here we break the string and assign them directly.
    // #ifdef Q_OS_IOS
    // QStringList allPaths = path.toString().split(',', Qt::SkipEmptyParts);
    // for (QString &onePath: allPaths) {
    //     // If path begins with "file:///", we have to change it to "/"
    //     // before assigning to audioPaths_
    //     if (QUrl(onePath).isLocalFile()) {
    //         // we will use the path to generate a temp file here

    //         audioPaths_.push_back(QUrl(onePath).toLocalFile());
    //         // Add the new path to our soundHash_ with its sound ptr = nullptr
    //         soundsHash_[QUrl(onePath).toLocalFile()] = nullptr;
    //         qDebug() << "the received path in if: " << path.toString();
    //     } else {
    //         audioPaths_.push_back(onePath);
    //         // Add the new path to our soundHash_ with its sound ptr = nullptr
    //         soundsHash_[onePath] = nullptr;
    //         qDebug() << "the received path in else: " << onePath;
    //     }
    // }
    // #endif


    //////
    // Extracting the unique App_ID of this app
    // QString thisAppID = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    // qsizetype pos = thisAppID.lastIndexOf("/");
    // currDir_ = thisAppID.sliced(0, pos + 1);
    // thisAppID = thisAppID.sliced(pos + 1);
    // currDir_.append(thisAppID + "/");
    // now you can use dir.entryList to print out sandox content of *this app

    currDir_ = path.toString();
    if (! currDir_.endsWith('/')) {
        currDir_.append('/');
    }

    doAddDir();
}

QStringList Directory::getAudioPaths() {
    return audioPaths_;
}

// INVALID COMMENT
// On iOS, this method will be called from objective-c
// after the user-selected audios have been stored to
// sandbox/tmp directory
void Directory::doAddDir() {
    // INVALID COMMENT
    // Since IOS audios will only be picked from sandbox/tmp,
    // no need to enter this block for iOS users.
    // #ifndef Q_OS_IOS
    // // we can clean this below code to if we can test with android to know there is no problem
    if (QUrl(currDir_).isLocalFile()) {
        currDir_ = QUrl(currDir_).toLocalFile();
    }
    QDir dir(currDir_);

    // Fetch all filepaths that ends with .mp3 from the directory
    QStringList mp3 = dir.entryList(QStringList() << "*.mp3", QDir::Files);

    for (QString &aMp3: mp3) {
        if (! audioPaths_.contains(currDir_ + aMp3)) {  // no repeat
            // Add the filepath to our sound stringlist
            audioPaths_.push_back(currDir_ + aMp3);
            // use the filepath as key in our soundHash_
            // dict, with its decodedSound ptr = nullptr
            soundsHash_[currDir_ + aMp3] = nullptr;
        }
    }

    #ifndef Q_OS_IOS
    // preparing for backup to localStorage onExit.
    // This was also placed inside this ifdef coz iOS
    // wont backup onExit coz the logic we used is to
    // load only the audio files from its sandbox/tmp
    // directory during each launch.
    backups_.setValue("soundPaths", QVariant::fromValue(audioPaths_));
    #endif

    // // IOS audios will simply be picked from sandbox/tmp.
    // #ifdef Q_OS_IOS
    // // We check whether sounds exists in our
    // // sandbox/tmp directory, so we add them too.
    // pickIosAudiosFromSandboxTmpDir();
    // #endif

    preparePathsForPlay();
}

void Directory::addStartupAudiosOnEmptyStartupAudioListings() {
    // For Android, simply add the single factory audio file.
    // We had to copy it first into a temp directory created
    // by qt coz c cannot open(""assets:/Dinkplay_Tone.mp3")
    // for miniaudio to play the file.
    #ifdef Q_OS_ANDROID
    QTemporaryDir tempDir;
    QString tempFile;
    if (tempDir.isValid()) {
        tempFile = tempDir.path() + "/DinkplayTone.mp3";
        if (QFile::copy("assets:/Dinkplay_Tone.mp3", tempFile)) {
            tempDir.setAutoRemove(false);
        } else {
            tempFile.clear();
        }
    }
    if (! tempFile.isEmpty()) {
        // Add the filepath to our sound stringlist
        audioPaths_.push_back(tempFile);
        // use the filepath as key in our soundHash_
        // dict, with its decodedSound ptr = nullptr
        soundsHash_[tempFile] = nullptr;
    }
    #endif

    // For iOS, first we add the single factory sound,
    // Then we check whether sounds exists in our
    // sandbox/tmp directory. if yes, we add them too.
    #ifdef Q_OS_IOS
    // we will first add factory default sound here
    // Add the filepath to our sound stringlist
    audioPaths_.push_back("audios/Dinkplay_Tone.mp3");
    // use the filepath as key in our soundHash_
    // dict, with its decodedSound ptr = nullptr
    soundsHash_["audios/Dinkplay_Tone.mp3"] = nullptr;

    // Now we check whether sounds exists in our
    // sandbox/tmp directory, so we add them too.
    pickIosAudiosFromSandboxTmpDir();
    #endif
}

// THIS METHOD CAN BE ADAPTED TO REPLACE THE for() LOOP IN doAddDir()
/**
  * This method is for iOS only.
  * It simply scans the app's sandbox/tmp directory
  * and load all .mp3 files into audioPaths_ and
  * soundsHash_, making them ready for play.
  * @returns void
  */
void Directory::pickIosAudiosFromSandboxTmpDir() {
    QString sandboxTmpDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/tmp/";
    QDir tmpDir(sandboxTmpDir);
    QStringList all = tmpDir.entryList(QStringList() << "*.mp3", QDir::Files);
    for (QString &one: all) {
        if (! audioPaths_.contains(sandboxTmpDir + one)) { // no repeat
            // Add the filepath to our sound stringlist
            audioPaths_.push_back(sandboxTmpDir + one);
            // use the filepath as key in our soundHash_
            // dict, with its decodedSound ptr = nullptr
            soundsHash_[sandboxTmpDir + one] = nullptr;
        }
    }
}

/**
  * By the time this method is called,
  * audioPaths_ should already have its
  * list of filepaths that we need to
  * use to populate the audio page directory.
  * This method simply sets the path at the
  * index of audiopaths_ as the 'play now' AKA
  * When the user clicks on play, it starts
  * Playing.
  * And then this method emits the audioPathsChanged()
  * signal which tells frontend to reindex the
  * audio lists.
  * @returns void
  */
void Directory::preparePathsForPlay() {
    if (audioPaths_.isEmpty()) {
        addStartupAudiosOnEmptyStartupAudioListings();
    }

    // set iterator back to beginning
    audIt_ = audioPaths_.begin();

    // so that after the user selects a directory
    // If they now sets interval timer, the interval
    // timer can trigger the play() from the mp3 list
    if (audioPaths_.size()) {
        // Set the path at *audit_ as active, ready for play
        setSource((*audIt_).toLocal8Bit().constData());
    }

    // After update, now refresh the qml view displaying audioPaths_ as list
    emit audioPathsChanged();
}

void Directory::openDialogFromCpp() {

}

// void Directory::openDialogFromCpp() {
//     QFileDialog dialog(0);
//     dialog.setFileMode(QFileDialog::ExistingFile);
//     dialog.setDirectory("assets-library://");

//     QStringList fileN = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
//     for (QString &one: fileN) {
//         qDebug() << "debugger" << one;
//     }

//     QStringList fileNames;
//     if (dialog.exec())
//         fileNames = dialog.selectedFiles();

//     for (QString &one: fileNames) {
//         qDebug() << "checking:" << one;
//     }

//     // QUrl url = QFileDialog::getOpenFileUrl(0,
//     //                                        tr("Open File"),
//     //                                        tr("/data/Containers/Bundle/Application/6935D002-44A6-4328-86A2-1CEAC71DC409/")
//     //                                     );
//     // qDebug() << "Received URL: " << url;

// }

//qDebug() << "path: " << path.toString();
//qsizetype pos = path.toString().lastIndexOf("/");
//currDir_ = path.toString();
// currDir_.truncate(pos + 1);
// path = path.toString().sliced(pos + 1);
// qDebug() << "currDir: " << currDir_ << "pos: " << pos;
// qDebug() << "path: " << path.toString();
// audioPaths_.push_back(currDir_ + path.toString());
// soundsHash_[currDir_ + path.toString()] = nullptr;


// home dir in simulation == /data/Containers/Data/Application/FC24ED6A-E8AA-44FC-B38C-55B7993BE416/
// file explorere         == /data/Containers/Shared/AppGroup/FC24ED6A-E8AA-44FC-B38C-55B7993BE416/File Provider Storage/
