#ifndef CHANGEPLAY_HPP
#define CHANGEPLAY_HPP

#include <QObject>

#include "Media.hpp"

class ChangePlay : virtual public Media
{
public:
    explicit ChangePlay(QObject *parent = nullptr);
    virtual ~ChangePlay();

    void changePlay(bool move) override;

};

#endif // CHANGEPLAY_HPP
