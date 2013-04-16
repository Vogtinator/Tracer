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

QString SkypeWin::callSkype(QString cmd)
{
    COPYDATASTRUCT oCopyData;

    oCopyData.dwData = 1;
    oCopyData.lpData = (PVOID)cmd.toStdString().c_str();
    oCopyData.cbData = strlen((char*)oCopyData.lpData)+1;

    SendMessage(skypewnd, WM_COPYDATA, (WPARAM)QApplication::activeWindow()->winId(), (LPARAM)&oCopyData);

    return QString("");
}

bool SkypeWin::callSkypeAsync(QString cmd)
{
    callSkype(cmd);
    return false;
}
#include <iostream>
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
        const char* content = (const char*)poCopyData->lpData;

        /*if(strstr(msg, "#CMdx") == msg)*/
            emit receivedReply(QString::fromUtf8(content));
        /*else
            emit receivedMessage(QString::fromUtf8(msg));*/

        std::cout << content << " length: " << msg->hwnd << wParam << std::endl;

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
