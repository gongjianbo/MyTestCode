#include "TalkListDefine.h"

QString TalkDataAudio::durationStr() const
{
    if(duration<1)
        return QString("0\"");
    const qint64 time=qRound64(duration/1000.0);
    QString duration_str=QString("%1\"").arg(time%60);
    if(time>=60){
        duration_str=QString("%1\'%2").arg(time/60%60).arg(duration_str);
    }
    if(time>=3600){
        duration_str=QString("%1:%2").arg(time/3600).arg(duration_str);
    }
    return duration_str;
}
