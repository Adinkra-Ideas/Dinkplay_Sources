// #include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>

#ifdef Q_OS_ANDROID
#include <QJniEnvironment>
#include <QJniObject>
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#endif

#include "Top.hpp"

int main(int argc, char *argv[])
{
    #ifdef Q_OS_ANDROID
    // Allow display of notifications / foreground service notifications (for API 33 and above)
    auto r = QtAndroidPrivate::checkPermission("android.permission.POST_NOTIFICATIONS").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.POST_NOTIFICATIONS").result();
    }
    /* [[1]] Needed for detecting when bluetooth connect/disconnect from something */
    r = QtAndroidPrivate::checkPermission("android.permission.BLUETOOTH_CONNECT").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.BLUETOOTH_CONNECT").result();
    }
    #endif

    // QGuiApplication app(argc, argv);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Top media(&app);
    QQmlContext* rootContext = engine.rootContext();
    rootContext->setContextProperty("Media", &media);

    // For making the threads shut down gracefully
    Media & base = media;
    QObject::connect(&app, &QGuiApplication::aboutToQuit, &base, &Media::killAllThreads);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Dinkplay", "Main");

    #ifdef Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([]() {
        QJniObject activity = QNativeInterface::QAndroidApplication::context();

        QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        window.callMethod<void>("addFlags", "(I)V", 0x80000000);
        window.callMethod<void>("clearFlags", "(I)V", 0x04000000);
        window.callMethod<void>("setStatusBarColor", "(I)V", 0xCC1777B7);
        window.callMethod<void>("setNavigationBarColor", "(I)V", 0xCC1777B7);

        QJniObject decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
        decorView.callMethod<void>("setSystemUiVisibility", "(I)V", 0x00002000);
    }).waitForFinished();
    #endif

    // Calling the method to load the persisted
    // filepaths (if any) on startup.
    media.loadSavedPaths(); // UNCOMMENT THIS. ONLY COMMENTED BECAUSE OF IOS TESTING

    #ifdef Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::hideSplashScreen();
    #endif

    return app.exec();
}
