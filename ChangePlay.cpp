#include "ChangePlay.hpp"

ChangePlay::ChangePlay(QObject *parent) :
    Media{parent}
{}

ChangePlay::~ChangePlay(){}

void ChangePlay::changePlay(bool move) {
    // if there is an active, we stop it first
    stopAnyCurrentPlaying();

    // if last, stop
    // if not last, next
    if (move && audioPaths_.size()) { // fwd
        if (*audIt_ == audioPaths_.last()
            && repeat_ == 2) {
            audIt_ = audioPaths_.begin();
        } else if (*audIt_ != audioPaths_.last()) {
            ++audIt_;
        }
        playSource(*audIt_);
    } else if (!move && audioPaths_.size()) { // bkwd
        if (*audIt_ != audioPaths_.first()) {
            qsizetype pos = audioPaths_.indexOf(*audIt_);
            if (pos != -1 && pos != 0) {
                --audIt_;
            }
        }
        playSource(*audIt_);
    }

}
