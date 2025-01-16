#include "IntervalWorker.hpp"

IntervalWorker::IntervalWorker(Media * media, qint16 & lapCount, qint16 & lapDuration, qint16 & restDuration, bool & intervalState) :
    media_{media},
    lapCount_{lapCount},
    lapDuration_{lapDuration},
    restDuration_{restDuration},
    intervalState_{intervalState}
{}

IntervalWorker::~IntervalWorker() {}

void IntervalWorker::startTheIntervalLoop() {
    if (lapCount_ > 0
        && lapDuration_ > 0
        && restDuration_ > 0
        ) {
        intervalState_ = true;
        // call the UI (through this thread's parent) to refresh interval screen
        emit intervalStatusChanged();
    }

    while (intervalState_) {
        media_->play();
        // Set the Breakpoint to end of one lapDuration + now()
        gettimeofday(&clock_, 0);
        clkBreakPoint_ = clock_.tv_sec + lapDuration_;
        // Wait here until either clkBreakPoint_ time
        // elapses or intervalState_ becomes false
        while (intervalState_ && clock_.tv_sec < clkBreakPoint_) {
            gettimeofday(&clock_, 0);
        }

        --lapCount_;
        media_->pause();

        if (lapCount_ > 0) {
            // Set the Breakpoint to end of one restDuration + now()
            gettimeofday(&clock_, 0);
            clkBreakPoint_ = clock_.tv_sec + restDuration_;
            // Wait here until either clkBreakPoint_ time
            // elapses or intervalState_ becomes false
            while (intervalState_ && clock_.tv_sec < clkBreakPoint_) {
                gettimeofday(&clock_, 0);
            }
        } else {
            intervalState_ = false;
            // call the UI (through this thread's parent) to refresh interval screen
            emit intervalStatusChanged();
            break ;
        }

        if (!intervalState_) {
            // Entering here means intervalState_
            // was used to manually terminate the loop.
            // We will just let the UI know
            emit intervalStatusChanged();
        }
    }
}

/**
  * Simply sets the intervalState_ to false
  * so that any blocking loop will be released.
  * @returns void
  */
void IntervalWorker::StopTheIntervalLoop() {
    // Break any running loop
    // If intervalState_ was true, set it to false and emit shange
    if (intervalState_ == true) {
        intervalState_ = false;
        // call the UI (through this thread's parent) to refresh interval screen
        emit intervalStatusChanged();
    }
}

