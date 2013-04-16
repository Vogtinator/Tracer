#ifndef SKYPE_H
#define SKYPE_H

#include <QtGlobal>
#include <QObject>
#include <QString>

class MainWindow;

class Skype : public QObject
{
    Q_OBJECT
public:
    static Skype* getSkype(QString name);
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual QString callSkype(QString cmd) = 0;
    virtual bool callSkypeAsync(QString cmd) = 0;
signals:
    void receivedReply(QString);
    void receivedMessage(QString);
    void error(QString);
    void connectionStatusChanged(bool);
protected:
    bool connected;
};

#ifdef Q_OS_WIN
#include "skype_win.h"
#elif defined(Q_OS_UNIX)
#include "skype_linux.h"
#else
#error "Platform not supported!"
#endif

#endif // SKYPE_H
