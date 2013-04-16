#include "skype.h"

Skype *Skype::getSkype(QString name)
{
#ifdef Q_OS_WIN
    return new SkypeWin(name);
#elif defined(Q_OS_UNIX)
    return new SkypeLinux(name);
#endif
}
