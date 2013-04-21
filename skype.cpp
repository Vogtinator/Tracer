#include "skype.h"

Skype *Skype::getSkype(QString name)
{
#ifdef Q_OS_WIN
    return new SkypeWin(name);
#elif defined(Q_OS_UNIX)
    return new SkypeLinux(name);
#endif
}

int Skype::reserveID()
{
    id_lock.lock();

    int id = 0;
    while(1)
    {
       if(!cmd_ids.contains(id))
           break;

       id++;
    }

    cmd_ids.append(id);

    id_lock.unlock();

    return id;
}

void Skype::freeID(int id)
{
    id_lock.lock();

    cmd_ids.removeOne(id);

    id_lock.unlock();
}
