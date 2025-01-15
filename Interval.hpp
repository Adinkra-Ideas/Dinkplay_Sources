// CLEANED
#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <QThread>

#include "Media.hpp"
#include "IntervalWorker.hpp"

class Interval : virtual public Media
{
    QThread theIntervalThread;

public:
    explicit Interval(QObject *parent = nullptr);
    virtual ~Interval();

    void killIntervalThread() override;

    bool getIntervalStatus() override;
    void setLapCount(qint16 newValue) override;
    void setLapDuration(qint16 newValue) override;
    void setRestDuration(qint16 newValue) override;
    void setIntervalStatus(bool flag) override;

private:
    qint16          lapCount_;
    qint16          lapDuration_;
    qint16          restDuration_;
    bool            intervalState_;  // false stopped, true running
    IntervalWorker  intervalWorker_; // Thread for timer
};

#endif // INTERVAL_HPP
