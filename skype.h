#ifndef SKYPE_H
#define SKYPE_H

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QMutex>

class Skype : public QObject
{
    Q_OBJECT
public:
    /** Get a Skype instance depending on OS */
    static Skype* getSkype(QString name);
    Skype() : connected(false) {}
    virtual ~Skype(){}
    /** Connect, fires connectionStatusChanged.
     *  @return true on success, false on failure */
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    /** Executed command cmd. cmd mustn't contain an identifier.
     *  Returns Result */
    virtual QString callSkype(QString cmd) = 0;
    /** Same as above, but returns immediately.
     *  @return id, which will be given to receivedReply */
    virtual int callSkypeAsync(QString cmd) = 0;
    bool isConnected() { return connected; }
signals:
    void receivedReply(QString, int);
    void receivedMessage(QString);
    void error(QString);
    void connectionStatusChanged(bool);
protected:
    int reserveID();
    void freeID(int id);
    QList<int> cmd_ids;
    bool connected;
private:
    QMutex id_lock;
};

#ifdef Q_OS_WIN
#include "skype_win.h"
#elif defined(Q_OS_UNIX)
#include "skype_linux.h"
#else
#error "Platform not supported!"
#endif

#endif // SKYPE_H
