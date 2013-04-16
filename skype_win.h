#ifndef SKYPE_WIN_H
#define SKYPE_WIN_H

#include <QString>
#include <QAbstractNativeEventFilter>
#include <windows.h>

#include "skype.h"

class SkypeWin : public Skype, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    SkypeWin(QString name);
    ~SkypeWin();
    bool connect();
    void disconnect();
    QString callSkype(QString cmd);
    bool callSkypeAsync(QString cmd);
    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result);
private:
    unsigned int skypecontrolapiattach;
    unsigned int skypecontrolapidiscover;
    HHOOK msgHook;
    HWND skypewnd;
    static LRESULT CALLBACK _receivedMessage(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif // SKYPE_WIN_H
