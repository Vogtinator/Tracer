#include <QMainWindow>

#include "skype.h"

Skype *Skype::getSkype(QString name, QMainWindow* mw)
{
    Q_UNUSED(mw);
#ifdef Q_OS_WIN
    return new SkypeWin(name, mw->winId());
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

void Skype::clearIDs()
{
    id_lock.lock();

    cmd_ids.clear();

    id_lock.unlock();
}
