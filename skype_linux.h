#ifndef SKYPE_LINUX_H
#define SKYPE_LINUX_H

#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QDBusMessage>

#include "skype.h"

class SkypeLinux;

class SkypeClient : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.Skype.API.Client")
public:
    SkypeClient(SkypeLinux *parent);
public slots:
    Q_NOREPLY void Notify(QString msg);
private:
    SkypeLinux *parent;
};

class SkypeLinux : public Skype
{
    Q_OBJECT
public:
    SkypeLinux(QString name);
    ~SkypeLinux();
    bool connect();
    void disconnect();
    QString callSkype(QString cmd);
    int callSkypeAsync(QString cmd);
public slots:
    void serviceUnregistered(QString name);
    void _receivedReply(QDBusMessage msg);
    void _receivedMessage(QString msg);
private:
    QDBusConnection connection;
    const QString name;
    QDBusInterface *interface;
    const SkypeClient client;
};

#endif // SKYPE_LINUX_H
