#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "skype.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void callSkype();
    void connectToSkype();
    void received(QString res);
    void receivedMessage(QString res);
    void connectionStatusChanged(bool connected);
private:
    Skype *skype;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
