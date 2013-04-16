#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "skype.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectionStatusChanged(false);

    skype = Skype::getSkype("Tracer");

    connect(this->skype, SIGNAL(connectionStatusChanged(bool)), this, SLOT(connectionStatusChanged(bool)));
    connect(this->skype, SIGNAL(receivedReply(QString)), this, SLOT(received(QString)));
    connect(this->skype, SIGNAL(receivedMessage(QString)), this, SLOT(receivedMessage(QString)));
}

void MainWindow::callSkype()
{
    skype->callSkypeAsync(ui->lineEdit->text());
    ui->lineEdit->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete skype;
    delete ui;
}

void MainWindow::received(QString res)
{
    ui->plainTextEdit->appendPlainText(QString::fromStdString("> %1").arg(ui->lineEdit->text()));
    ui->plainTextEdit->appendPlainText(res);
    if(!res.startsWith("ERROR"))
        ui->lineEdit->clear();
    ui->lineEdit->setReadOnly(false);
}

void MainWindow::receivedMessage(QString res)
{
    ui->plainTextEdit->appendPlainText(res);
}

void MainWindow::connectToSkype()
{
    if(ui->actionVerbinden->text() == "Disconnect")
        skype->disconnect();
    else
        skype->connect();
}

void MainWindow::connectionStatusChanged(bool connected)
{
    if(connected)
    {
        ui->actionVerbinden->setText("Disconnect");
        ui->statusBar->showMessage("Connected");

        /*if(skype->callSkype("NAME Tracer") != "OK")
            skype->disconnect();

        skype->callSkype("PROTOCOL 8");*/

        ui->lineEdit->setReadOnly(false);
    }
    else
    {
        ui->actionVerbinden->setText("Connect");
        ui->statusBar->showMessage("Not connected");
        ui->lineEdit->setReadOnly(true);
    }
}
