#include "Top.hpp"

Top * Top::myInstance = nullptr;

#ifdef Q_OS_ANDROID
static void logJavaResponse(JNIEnv *env, jobject /*thiz*/, jstring value)
{
    QString javaMsg(env->GetStringUTFChars(value, nullptr));
    if (javaMsg == QString("((((Prev))))")) {
        Top::myInstance->changePlay(false);
    } else if (javaMsg == QString("((((Play))))")) {
        Top::myInstance->play();
    } else if (javaMsg == QString("((((Pause))))")) {
        Top::myInstance->pause();
    } else if (javaMsg == QString("((((Next))))")) {
        Top::myInstance->changePlay(true);
    } else if (javaMsg == QString("((((Suspend))))")) {
        Top::myInstance->suspendAudio();
    } else if (javaMsg == QString("((((Unsuspend))))")) {
        Top::myInstance->unsuspendAudio();
    }

    // qDebug() << "the message from java" << env->GetStringUTFChars(value, nullptr);
}
#endif

Top::Top(QObject * parent) :
    Directory{parent},
    ChangePlay{parent},
    Interval{parent},
    Player{parent}
    #ifdef Q_OS_ANDROID
    , javaObject{"com.denkplay.states.DenkService"}
    #endif
{
    #ifdef Q_OS_ANDROID
    JNINativeMethod methods[] {{"sendToQt", "(Ljava/lang/String;)V", reinterpret_cast<void *>(logJavaResponse)}};

    QJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaObject.object<jobject>()); // this is simply peeling the class definition from the running object. Can be assigned to "jclass clazz"
    env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);


    auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    QAndroidIntent serviceIntent(activity.object(),
                                 "com.denkplay.states.DenkService");

    QJniObject theStartedService = activity.callObjectMethod(
        "startForegroundService",
        "(Landroid/content/Intent;)Landroid/content/ComponentName;",
        serviceIntent.handle().object() );

    // preserve the Activity to javaObject for future use
    javaObject.swap(activity);
    #endif

    // initialize objective-c objects
    #ifdef Q_OS_IOS
    initMyMPRemoteCC(this);
    #endif

    myInstance = this;
}

Top::~Top(){
    // Cleanup objective-c if iOS
    #ifdef Q_OS_IOS
    destroyMyMPRemoteCC();
    #endif

    // qDebug() << "App is getting Killedffffffff";

    // //
    // checkForBackPress();
}

void Top::killAllThreads() {
    killIntervalThread();

}

// will comment
void Top::checkForBackPress() {
    #ifdef Q_OS_ANDROID
    // auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    javaObject.callObjectMethod(
        "myBackPressManager",
        "()V" );
    #endif
}

// will comment
// will also rename to notifyAllPlatformsAboutPlaying
void Top::notifyJavaSeviceAboutPlaying(bool isplaying) {
    #ifdef Q_OS_ANDROID
    javaObject.callObjectMethod(
        "setPlayPauseIconInActivity",
        "(Z)V",
        isplaying );
    #endif
}

bool Top::seizeControlOfAudioSession() {
    #ifdef Q_OS_ANDROID
    auto result = javaObject.callMethod<bool>(
        "seizeControlOfAudioManager",
        "()Z");
    return result;
    #endif

    #ifdef Q_OS_IOS
    seizeControlOfInfoCenter();
    #endif
    return true;
}

// will comment
void Top::updateAllAudioDetailsDisplayers() {
    #ifdef Q_OS_IOS
    // seizeControlOfInfoCenter();
    updateInfoCenter(currentPlayingTitle_.toLocal8Bit().constData(),
                     currentPlayingArtist_.toLocal8Bit().constData());
    #endif
}


// bool Top::objc_StartAccessingSecuredLocation(const char * urlPath) {
//     return startAccessingSecuredLocation(urlPath);
// }

/**
  * This method is called when the user clicks the
  * folder icon from app interface, for selecting
  * files/folder for play.
  * It simply checks the platform and redirects
  * to where the document picker dialog for that
  * platform is implemented. objective-c for iOS,
  * Cpp for android.
  * @returns bool but simply coz void cant be used
  * with Q_PROPERTY
  */
bool Top::manageDocumentPickModal() {
    // We will change android/windows picker from qml to cpp when I got the time
    #ifndef Q_OS_IOS
    // Directory::openDialogFromCpp();
    // return false;
    #endif

    // iOS uses its uidocument picker to open dialogues
    // that returns security-scoped url data types, which contains
    // special unseen infos. Using Qt's filedialog will simply return
    // bare strings that wont work with startAccessingSecurityScopedResource
    #ifdef Q_OS_IOS
    openIosFileDialog();
    #endif

    return 0;
}
