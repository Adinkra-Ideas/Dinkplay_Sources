// CLEANED
#ifndef INTERVALWORKER_HPP
#define INTERVALWORKER_HPP

#include <QObject>
#include <sys/time.h>

#include "Media.hpp"

class IntervalWorker : public QObject
{
    Q_OBJECT

public:
    IntervalWorker(Media * media, qint16 & lapCount, qint16 & lapDuration, qint16 & restDuration, bool & intervalState);
    ~IntervalWorker();

    void StopTheIntervalLoop();
    void startTheIntervalLoop();

signals:
    void intervalStatusChanged();

private:
    IntervalWorker();

    Media    * media_;         // Parent instance that controls this thread
    qint16   & lapCount_;
    qint16   & lapDuration_;
    qint16   & restDuration_;
    bool     & intervalState_;

    timeval    clock_;
    qsizetype  clkBreakPoint_;
};

#endif // INTERVALWORKER_HPP
