#ifndef MEDIA_HPP
#define MEDIA_HPP

#include <QObject>
#include <QStringList>
#include <QString>
#include <QUrl>
#include <Qt>
#include <unordered_map>

#ifndef MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
extern "C"
{
#include "ma_lib/miniaudio.h"
}
#endif // MINIAUDIO_IMPLEMENTATION

class Media : public QObject
{
    Q_OBJECT

    /********* For Interval Controls Begins *************/
    Q_PROPERTY(bool intervalStatus READ getIntervalStatus WRITE setIntervalStatus NOTIFY intervalStatusChanged)
    Q_PROPERTY(qint16 lapCount WRITE setLapCount)
    Q_PROPERTY(qint16 lapDuration WRITE setLapDuration)
    Q_PROPERTY(qint16 restDuration WRITE setRestDuration)
    /****************************************************/

    /* ******* For The Media Player Begins ***************/
    Q_PROPERTY(quint8 player READ playbackState WRITE setPlaybackState NOTIFY playbackStateChanged) // rename to getCurrAudio
    Q_PROPERTY(QString playSource WRITE playSource NOTIFY playbackStateChanged) // For when a link is clicked in audio listings
    Q_PROPERTY(bool change WRITE changePlay NOTIFY playbackStateChanged)        // When prevOrNext clicked by user
    Q_PROPERTY(QString title READ getTitle NOTIFY playbackStateChanged)         // Get the filename when new sound starts playing
    Q_PROPERTY(quint8 repeat READ getRepeat WRITE setRepeat NOTIFY repeatChanged)
    /******************************************************/

    /************ For Directory Management Begins *********/
    Q_PROPERTY(QString currDir /*READ getDir*/ WRITE addDir/* NOTIFY dirChanged*/)          // Returns the current directory
    Q_PROPERTY(QStringList audiopaths READ getAudioPaths NOTIFY audioPathsChanged)  // Returns the new list of files that should be indexed
    Q_PROPERTY(bool openDialog READ manageDocumentPickModal)
    /******************************************************/

public:
    explicit Media(QObject *parent = nullptr);
    virtual  ~Media() = 0;

    /******** Methods Implemented here in Media.cpp ********/
    void        setRepeat(quint8 val);
    quint8      getRepeat();
    /*******************************************************/

    /*****  Implemented in Interval.hpp Begns  *****/
    virtual void killIntervalThread() = 0;
    virtual void setIntervalStatus(bool flag) = 0;
    virtual bool getIntervalStatus() = 0;
    virtual void setLapCount(qint16 newValue) = 0;
    virtual void setLapDuration(qint16 newValue) = 0;
    virtual void setRestDuration(qint16 newValue) = 0;
    /***********************************************/

    /********** implemented in Player.cpp ***********/
    virtual quint8  playbackState() = 0;
    virtual void    setSource(const char * path) = 0;
    virtual void    playSource(QString path) = 0;
    virtual void    setPlaybackState(quint8) = 0;
    virtual void    stopAnyCurrentPlaying() = 0;
    virtual void    play() = 0;
    virtual void    pause() = 0;
    virtual void    suspendAudio() = 0;
    virtual void    unsuspendAudio() = 0;
    virtual void    playOrPause() = 0;
    virtual QString getTitle() = 0;
    /*************************************************/

    /******** implemented in Directory.cpp ***********/
    virtual void    addDir(QUrl path) = 0;
    // virtual QString getDir() const = 0;
    virtual QStringList getAudioPaths() = 0;
    virtual void loadSavedPaths() = 0;
    /*************************************************/

    /******** Implemented in ChangePlay.cpp **********/
    virtual void    changePlay(bool move) = 0;
    /*************************************************/

    /******* Implemented in Top.cpp ***************/
    virtual void killAllThreads() = 0;
    virtual void notifyJavaSeviceAboutPlaying(bool isplaying) = 0;
    virtual void updateAllAudioDetailsDisplayers() = 0;
    virtual bool seizeControlOfAudioSession() = 0;
    // virtual bool objc_StartAccessingSecuredLocation(const char * urlPath) = 0;
    virtual bool manageDocumentPickModal() = 0;
    /**********************************************/

public slots:
    virtual void checkForBackPress() = 0;

signals:
    /******* Mostly Used in Interval.hpp Begns  ********/
    // For starting the Blocking method in its sub thread
    void startTheIntervalLooping();
    // For Refreshing the UI onIntervalStatusChanged
    void intervalStatusChanged();
    /***************************************************/

    /********* Mostly used in Directory.hpp ************/
    // For getting the PWD dir Selected by the user
    // void dirChanged();
    // For getting the indexed filelists to frontend
    void audioPathsChanged();
    /***************************************************/

    /********* Mostly Used from Player.cpp *************/
    // Whenever a playing sound changes
    void playbackStateChanged(QString newPath);
    /***************************************************/

    /******** Mostly used from here in media.cpp *******/
    // For notifying frontend that repeat-X changed
    void repeatChanged();
    /***************************************************/

protected:
    // QStringList           videoPaths_;
    // QStringList::iterator       vpIt_; // iterator to videoPaths_
    QString                  currDir_; // Dir selected by the user, from where media files was last added
    quint8                    repeat_; // 0 == repeat none, 1 == repeat 1, 2 == repeat all

    QStringList           audioPaths_;  // holds all the mp3 files found in directory selected by the user for media search
    QStringList::iterator      audIt_;  // iterator to audioPaths_
    QString       currentPlayingPath_;  // stores the filepath of currently active audio
    QString      currentPlayingTitle_;  // stores the title of currently active audio
    QString     currentPlayingArtist_;  // stores the Artist of currently active audio
    std::unordered_map<QString, ma_sound *> soundsHash_; // used as a storage for Holding audioPaths_ keys and their associated values == their decoded ma_sound.
    bool                    suspended_; // if true, it means the current nowPlaying audio is suspended. In this case, calling unsuspendAudio() will play it. If false, calling unsuspendAudio will do nothing.
};

#endif // MEDIA_HPP
