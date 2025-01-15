#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QObject>

#include "Media.hpp"

#ifndef MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
extern "C"
{
#include "ma_lib/miniaudio.h"
}
#endif // MINIAUDIO_IMPLEMENTATION

class Player : virtual public Media
{
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    quint8  playbackState() override;
    void    setSource(const char * path) override;
    void    playSource(QString) override;
    void    setPlaybackState(quint8 state) override;
    void    stopAnyCurrentPlaying() override;
    QString getTitle() override;

    void    play() override;
    void    pause() override;
    void    suspendAudio() override;
    void    unsuspendAudio() override;
    void    playOrPause() override;
    void    endOfCurrentAudio(bool shouldStopCompletely = false); // 3 GREAT CHANGE CHNAGED THIS TO having a param
    void    readyAudioForNewPlay();

protected:
    ma_sound            sound_;
    ma_result           result_;
    ma_engine           engine_;
    ma_device         * device_;  // After the ma_engine_init() ints our engine, we backup the device it created for us here so we can use it when necessary
    ma_device_config    config_;

    bool        engineInit_;    // True if mini audio engine was successfully init-ed, else false
    quint8      state_;         // Holds the current media playback state at any given time. 0 == stopped, 1 == playing, 2 == paused
};

#endif // PLAYER_HPP
