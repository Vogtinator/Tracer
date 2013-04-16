#include <QDBusConnectionInterface>

#include "skype.h"

SkypeLinux::SkypeLinux(QString name) : connection(name)
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

    this->client = new SkypeClient(this);
    this->interface = new QDBusInterface("com.Skype.API", "/com/Skype", "com.Skype.API", connection);
    connected = interface->isValid();
    if(!connected)
        goto exit;

    connected = this->connection.registerObject("/com/Skype/Client", this);

    exit:
    emit connectionStatusChanged(connected);

    return connected;
}

QString SkypeLinux::callSkype(QString cmd)
{
    if(!connected)
        return NULL;

    QDBusMessage reply = interface->call("Invoke", cmd);
    if(reply.type() == QDBusMessage::ErrorMessage)
        return NULL;
    else
        return reply.arguments().at(0).toString();
}

bool SkypeLinux::callSkypeAsync(QString cmd)
{
    if(!connected)
        return false;

    QList<QVariant> args;
    args.append(cmd);
    return interface->callWithCallback("Invoke", args, this, SLOT(_receivedReply(QDBusMessage)));
}

void SkypeLinux::serviceUnregistered(QString name)
{
    if (name != "com.Skype.API")
        return;

    if(connected)
            emit connectionStatusChanged(false);

    connected = false;
}

void SkypeLinux::disconnect()
{
    connection.unregisterObject("/com/Skype/Client");
    if(connected)
            emit connectionStatusChanged(false);

    connected = false;
}

void SkypeLinux::_receivedReply(QDBusMessage msg)
{
    if(msg.type() != QDBusMessage::ReplyMessage)
        emit error(msg.errorMessage());
    else
        emit receivedReply(msg.arguments().at(0).toString());
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
