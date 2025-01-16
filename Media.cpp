#include "Media.hpp"

Media::Media(QObject *parent) :
    QObject{parent},
    repeat_{2},
    suspended_{false} {
    // set iterator accordingly
    audIt_ = audioPaths_.begin();
    // vpIt_ = videoPaths_.begin();
}

Media::~Media() {}

// ******************************************
//       GETTERS AND SETTERS BEGINS         *
// ******************************************
/* 0 == repeat none,
 * 1 == repeat 1,
 * 2 == repeat all
 */
void    Media::setRepeat(quint8 val) {
    repeat_ = val;
    emit repeatChanged();
}
quint8  Media::getRepeat() {
    return repeat_;
}

// ******************************************
//       GETTERS AND SETTERS ENDS           *
// ******************************************
