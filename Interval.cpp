#include "Interval.hpp"

Interval::Interval(QObject *parent) :
    Media{parent},
    lapCount_{0},
    lapDuration_{0},
    restDuration_{0},
    intervalState_{false},
    intervalWorker_{this, lapCount_, lapDuration_, restDuration_, intervalState_}
{
    connect(this, &Media::startTheIntervalLooping, &intervalWorker_, &IntervalWorker::startTheIntervalLoop);
    connect(&intervalWorker_, &IntervalWorker::intervalStatusChanged, this, &Media::intervalStatusChanged);
    intervalWorker_.moveToThread(&theIntervalThread);
    theIntervalThread.start();
}

Interval::~Interval() {
    theIntervalThread.quit();
    theIntervalThread.wait();
}

void Interval::killIntervalThread() {
    // Stop any loop in thread and
    // destroy the thread
    intervalWorker_.StopTheIntervalLoop();
}

bool Interval::getIntervalStatus() {
    return intervalState_;
}

void Interval::setLapCount(qint16 newValue) {
    lapCount_ = newValue;
}

void Interval::setLapDuration(qint16 newValue) {
    lapDuration_ = newValue;
}

void Interval::setRestDuration(qint16 newValue) {
    restDuration_ = newValue;
}

/**
  * Called from frontend when user clicks the
  * 'Start Interval' button.
  * @returns void
  */
void Interval::setIntervalStatus(bool flag) {
    // whether flag is true or false, pause play if any first
    pause();

    // lapCount_ = 5;
    // lapDuration_ = 5;
    // restDuration_ = 2;

    if (flag && lapCount_ && lapDuration_ && restDuration_) {
        // Entering here means we need to start a new interval.
        // Kill any previous Interval if exist.
        /* Thread safe method in intervalWorker_ */
        // if (intervalState_) {
        //     intervalWorker_.StopTheIntervalLoop();
        // }
        // Start new thread
        // intervalWorker_.moveToThread(&theIntervalThread);
        // theIntervalThread.start();
        // Call blocking loop in Interval thread
        emit startTheIntervalLooping();

    } else if (!flag) {
        // flag==false means request to terminate Interval from user
        /* simply checks if running timer, and kills it. */
        if (intervalState_) {
            intervalWorker_.StopTheIntervalLoop();

            lapCount_ = 0;
            lapDuration_ = 0;
            restDuration_ = 0;
        }
        emit intervalStatusChanged();
    }
}
