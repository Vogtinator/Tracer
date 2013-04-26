#include <QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "skype.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectionStatusChanged(false);

    skype = Skype::getSkype("Tracer", this);

    this->connect(this->skype, SIGNAL(connectionStatusChanged(bool)), SLOT(connectionStatusChanged(bool)), Qt::QueuedConnection);
    this->connect(this->skype, SIGNAL(receivedReply(QString, int)), SLOT(received(QString, int)), Qt::QueuedConnection);
    this->connect(this->skype, SIGNAL(receivedMessage(QString)), SLOT(receivedMessage(QString)), Qt::QueuedConnection);
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

void MainWindow::received(QString res, int)
{
    ui->plainTextEdit->appendPlainText(QString("> %1").arg(ui->lineEdit->text()));
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
    {
        if(!skype->connect())
           ui->plainTextEdit->appendPlainText("Failed to connect");
    }
}

void MainWindow::connectionStatusChanged(bool connected)
{
    if(connected)
    {
        ui->actionVerbinden->setText("Disconnect");
        ui->statusBar->showMessage("Connected");
        ui->lineEdit->setReadOnly(false);
    }
    else
    {
        ui->actionVerbinden->setText("Connect");
        ui->statusBar->showMessage("Not connected");
        ui->lineEdit->setReadOnly(true);
    }
}
