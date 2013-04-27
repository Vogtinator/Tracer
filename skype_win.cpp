#include <QtWidgets>
#include <QTimer>

#include "skype_win.h"

enum {
    ATTACH_SUCCESS=0,
    ATTACH_PENDING_AUTHORIZATION=1,
    ATTACH_REFUSED=2,
    ATTACH_NOT_AVAILABLE=3,
    ATTACH_API_AVAILABLE=0x8001
};

SkypeWin::SkypeWin(QString name, int winID) : mewnd((HWND)winID)
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
    if(connected)
        return;

    QEventLoop *loop = new QEventLoop();

    loop->connect(this, SIGNAL(connectionStatusChanged(bool)), SLOT(quit()));

    int c = SendMessageTimeout(HWND_BROADCAST, skypecontrolapidiscover, (WPARAM)mewnd, 0, SMTO_ABORTIFHUNG, 1000, NULL);
    if(!c)
        return false;

    loop->exec();

    delete loop;

    return connected;
}

void SkypeWin::disconnect()
{
    if(!connected)
        return;

    clearIDs();
    emit connectionStatusChanged(false);
}

int SkypeWin::sendCmd(QString cmd)
{
    COPYDATASTRUCT copydata;
    copydata.dwData = 1;
    copydata.lpData = (PVOID)cmd.toStdString().c_str();
    copydata.cbData = strlen((char*)copydata.lpData)+1;

    SendMessageTimeout(skypewnd, WM_COPYDATA, (WPARAM)mewnd, (LPARAM)&copydata, SMTO_ABORTIFHUNG, 1000, NULL);
    return GetLastError();
}

QString SkypeWin::callSkype(QString cmd)
{
    if(!connected)
        return 0;

    int id = reserveID();

    cmd.prepend(QString("#%1 ").arg(id));

    QEventLoop *loop = new QEventLoop();
    QTimer *timer = new QTimer(this);

    loop->connect(this, SIGNAL(receivedInternalReply()), SLOT(quit()));
    loop->connect(timer, SIGNAL(timeout()), SLOT(quit()));

    queue.insert(id, 0);

    if(sendCmd(cmd))
        return 0;

    timer->start(2000);

    while(timer->isActive() && queue.value(id, 0) == 0)
        loop->exec();

    timer->stop();
    delete timer;
    delete loop;

    QString reply("");

    if(queue[id])
    {
        QString reply(*queue[id]);
        delete queue[id];
        queue.remove(id);
    }

    freeID(id);

    return reply;
}

int SkypeWin::callSkypeAsync(QString cmd)
{
    if(!connected)
        return -1;

    int id = reserveID();

    cmd.prepend(QString("#%1 ").arg(id));

    if(sendCmd(cmd))
        return -1;

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
        QString msg = QString::fromUtf8((const char*)poCopyData->lpData);

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
                QString *ret = new QString(rx.cap(2));
                queue[id] = ret;
                emit receivedInternalReply();
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
            connected = true;
            if(result)
                *result = 1;
            emit connectionStatusChanged(true);
            break;
        case ATTACH_REFUSED:
        case ATTACH_NOT_AVAILABLE:
            connected = false;
            emit connectionStatusChanged(false);
            break;
        }
        return true;
    }

    return false;
}
