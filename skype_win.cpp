#include <QtWidgets>

#include "skype_win.h"

enum {
    ATTACH_SUCCESS=0,
    ATTACH_PENDING_AUTHORIZATION=1,
    ATTACH_REFUSED=2,
    ATTACH_NOT_AVAILABLE=3,
    ATTACH_API_AVAILABLE=0x8001
};

SkypeWin::SkypeWin(QString name)
{
    Q_UNUSED(name);

    int len;
    WCHAR skypeApi[255];
    len = QString("SkypeControlAPIDiscover").toWCharArray(skypeApi);
    skypeApi[len] = 0;
    this->skypecontrolapidiscover = RegisterWindowMessage(skypeApi);

    len = QString("SkypeControlAPIAttach").toWCharArray(skypeApi);
    skypeApi[len] = 0;
    this->skypecontrolapiattach = RegisterWindowMessage(skypeApi);

    QCoreApplication::instance()->installNativeEventFilter(this);

    connected = false;
}

SkypeWin::~SkypeWin()
{
    if(connected)
        disconnect();
}

bool SkypeWin::connect()
{
    return SendMessageTimeout(HWND_BROADCAST, skypecontrolapidiscover, (WPARAM)QApplication::activeWindow()->winId(), 0, SMTO_ABORTIFHUNG, 1000, NULL);
}

void SkypeWin::disconnect()
{
    emit connectionStatusChanged(false);
}


void SkypeWin::sendCmd(QString cmd)
{
    COPYDATASTRUCT copydata;
    copydata.dwData = 1;
    copydata.lpData = (PVOID)cmd.toStdString().c_str();
    copydata.cbData = strlen((char*)copydata.lpData)+1;

    SendMessage(skypewnd, WM_COPYDATA, (WPARAM)QApplication::activeWindow()->winId(), (LPARAM)&copydata);
}

QString SkypeWin::callSkype(QString cmd)
{
    int id = reserveID();

    cmd.prepend(QString("#%1 ").arg(queue.count()));

    QPair<QString, QEventLoop*> *value = new QPair();

    value->first = nullptr;
    value->second = new QEventLoop;

    queue.insert(id, value);

    sendCmd(cmd);

    value->second->exec();

    QString reply = value->first;
    queue.remove(id);
    delete value;

    freeID(id);

    return reply;
}

int SkypeWin::callSkypeAsync(QString cmd)
{
    int id = reserveID();

    cmd.prepend(QString("#%1 ").arg(id));

    sendCmd(cmd);

    return id;
}

bool SkypeWin::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    MSG *msg = (MSG*)message;
    UINT uiMessage = msg->message;
    LPARAM lParam = msg->lParam;
    WPARAM wParam = msg->wParam;

    if(uiMessage == WM_COPYDATA && (HWND)wParam == skypewnd)
    {
        PCOPYDATASTRUCT poCopyData=(PCOPYDATASTRUCT)lParam;
        QString msg = QString.fromUtf8(poCopyData->lpData);

        if(msg.at(0) != '#')
            emit receivedMessage(msg);

        else
        {
            QRegExp rx("#(\\d+) (.*)");
            rx.indexIn(msg);
            int id = rx.cap(1).toInt();

            if(!queue.contains(id))
            {
                emit receivedReply(rx.cap(2), id);
                freeID(id);
            }
            else
            {
                queue.value(id)->first = rx.cap(2);
                queue.value(id)->second->quit();
            }
        }

        *result = 1;
        return true;
    }
    else if((unsigned int)uiMessage == skypecontrolapiattach)
    {
        switch(lParam)
        {
        case ATTACH_SUCCESS:
            skypewnd = (HWND)wParam;
            emit connectionStatusChanged(true);
            connected = true;
            break;
        case ATTACH_REFUSED:
        case ATTACH_NOT_AVAILABLE:
            emit connectionStatusChanged(false);
            connected = false;
            break;
        default:
            return true;
        }
        *result = 1;
        return true;
    }

    return false;
}
