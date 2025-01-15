// CLEANED
#ifndef TOP_HPP
#define TOP_HPP

#include <QObject>

#ifdef Q_OS_ANDROID
#include <QJniEnvironment>
#include <QJniObject>
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#endif

#ifdef Q_OS_IOS
#include "iOS/src/MyMPRemoteCC.h"
#endif

#include "Directory.hpp"
#include "ChangePlay.hpp"
#include "Interval.hpp"
#include "Player.hpp"

class Top : public Directory,
            public ChangePlay,
            public Interval,
            public Player
{
public:
    explicit Top(QObject * parent = nullptr);
    ~Top();

    void killAllThreads() override;
    void checkForBackPress() override;
    void notifyJavaSeviceAboutPlaying(bool isplaying) override;
    void updateAllAudioDetailsDisplayers() override;
    bool seizeControlOfAudioSession() override;
    // bool objc_StartAccessingSecuredLocation(const char * urlPath) override;
    bool manageDocumentPickModal() override;

    static Top * myInstance;

private:
    #ifdef Q_OS_ANDROID
    QJniObject javaObject;
    #endif
};

#endif // TOP_HPP
