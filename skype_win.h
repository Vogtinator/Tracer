#ifndef SKYPE_WIN_H
#define SKYPE_WIN_H

#include <QString>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <QMap>
#include <QPair>
#include <QEventLoop>

#include "skype.h"

class SkypeWin : public Skype, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    SkypeWin(QString name, int winID);
    ~SkypeWin();
    bool connect();
    void disconnect();
    QString callSkype(QString cmd);
    int callSkypeAsync(QString cmd);
    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result);

signals:
    void receivedInternalReply();

private:
    int sendCmd(QString cmd);
    unsigned int skypecontrolapiattach;
    unsigned int skypecontrolapidiscover;
    QMap<int, QString*> queue;
    HHOOK msgHook;
    HWND skypewnd;
    HWND mewnd;
    static LRESULT CALLBACK _receivedMessage(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif // SKYPE_WIN_H
