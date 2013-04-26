#include <QDBusConnectionInterface>

#include "skype.h"

SkypeLinux::SkypeLinux(QString name) : connection(name), interface(0), client(0)
{
    this->connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus, name);
    QObject::connect(this->connection.interface(), SIGNAL(serviceUnregistered(QString)), this, SLOT(serviceUnregistered(QString)));
}

SkypeLinux::~SkypeLinux()
{
    if(connected)
        disconnect();
}

bool SkypeLinux::connect()
{
    if(connected)
        return true;

    bool c;

    this->client = new SkypeClient(this);
    this->interface = new QDBusInterface("com.Skype.API", "/com/Skype", "com.Skype.API", connection);
    c = interface->isValid();
    if(!c)
        return false;

    c = this->connection.registerObject("/com/Skype/Client", this);

    if(!c)
    {
        delete this->client;
        delete this->interface;
        return false;
    }

    if(callSkype(QString("NAME %1").arg(name)) != "OK")
    {
        connection.unregisterObject("/com/Skype/Client");
        delete this->client;
        delete this->interface;
        return false;
    }
    callSkype("PROTOCOL 8");

    connected = c;
    emit connectionStatusChanged(true);
    return true;
}

QString SkypeLinux::callSkype(QString cmd)
{
    if(!connected)
    {
        emit error("Not connected!");
        return NULL;
    }

    QDBusMessage reply = interface->call("Invoke", cmd);
    if(reply.type() == QDBusMessage::ErrorMessage)
        return NULL;
    else
        return reply.arguments().at(0).toString();
}

int SkypeLinux::callSkypeAsync(QString cmd)
{
    if(!connected)
    {
        emit error("Not connected!");
        return -1;
    }

    int id = reserveID();

    cmd.prepend(QString("#%1 ").arg(id));

    QList<QVariant> args;
    args.append(cmd);

    interface->callWithCallback("Invoke", args, this, SLOT(_receivedReply(QDBusMessage)));

    return id;
}

void SkypeLinux::serviceUnregistered(QString name)
{
    if (name != "com.Skype.API")
        return;

    disconnect();
}

void SkypeLinux::disconnect()
{
    if(!connected)
        return;

    clearIDs();
    connection.unregisterObject("/com/Skype/Client");
    if(connected)
        emit connectionStatusChanged(false);

    if(this->client)
        delete this->client;
    if(this->interface)
        delete this->interface;

    connected = false;
}

void SkypeLinux::_receivedReply(QDBusMessage msg)
{
    if(msg.type() != QDBusMessage::ReplyMessage)
    {
        emit error(msg.errorMessage());
        return;
    }

    QString reply = msg.arguments().at(0).toString();
    int id = 0;
    QRegExp rx("#(\\d+) (.*)");
    rx.indexIn(reply);
    id = rx.cap(1).toInt();

    freeID(id);

    emit receivedReply(rx.cap(2), id);
}

void SkypeLinux::_receivedMessage(QString msg)
{
    emit receivedMessage(msg);
}

SkypeClient::SkypeClient(SkypeLinux *nparent) : QDBusAbstractAdaptor(nparent), parent(nparent)
{
}

void SkypeClient::Notify(QString msg)
{
    parent->_receivedMessage(msg);
}
