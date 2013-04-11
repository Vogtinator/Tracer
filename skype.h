#ifndef SKYPE_H
#define SKYPE_H

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusAbstractAdaptor>

class MainWindow;
class Skype;

class SkypeClient : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.Skype.API.Client")
public:
    SkypeClient(Skype *parent);
public slots:
    Q_NOREPLY void Notify(QString msg);
private:
    Skype *parent;
};

class Skype : public QObject
{
    friend class MainWindow;
    Q_OBJECT
public:
    Skype();
    bool connect();
    void disconnect();
    QString callSkype(QString cmd);
    bool callSkypeAsync(QString cmd);
signals:
    void receivedReply(QString);
    void receivedMessage(QString);
    void error(QString);
    void connectionStatusChanged(bool);
public slots:
    void serviceUnregistered(QString name);
    void _receivedReply(QDBusMessage msg);
    void _receivedMessage(QString msg);
private:
    bool connected;
    QDBusInterface *interface;
    QDBusConnection connection;
    SkypeClient *client;
};

#endif // SKYPE_H
