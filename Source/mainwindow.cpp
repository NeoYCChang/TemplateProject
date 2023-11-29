#include "mainwindow.h"
#include <QMessageBox>
#include <QHostInfo>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    status = false;
    connect_before = false;
    serverIP = new QHostAddress();
    port = 8010;
    connect(ui->Connect_BTN,SIGNAL(clicked()),this,SLOT(slotEnter()));
    connect(ui->Send_BTN,SIGNAL(clicked()),this,SLOT(slotSend()));
    ui->Send_BTN->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotEnter()
{
    if(!status)
    {
        QString ip = ui->IP_TXT->text();
        if(!serverIP->setAddress(ip))
        {
            QMessageBox::information(this,tr("error"),tr("server ip address error!"));
            return;
        }
        if(ui->Name_TXT->text() == "")
        {
            QMessageBox::information(this,tr("error"),tr("User name error!"));
            return;
        }
        username = ui->Name_TXT->text();
        if(!connect_before)
        {
            client_socket = new QTcpSocket(this);
            connect(client_socket,SIGNAL(connected()),this,SLOT(slotConnected())); //connected() is from <QTcpSocket>
            connect(client_socket,SIGNAL(disconnected()),this,SLOT(slotDisconnected())); //disconnected() is from <QTcpSocket>
            connect(client_socket,SIGNAL(readyRead()),this,SLOT(dataReceived())); //readyRead() is from <QTcpSocket>
            client_socket->connectToHost(*serverIP,port);
            if (!client_socket->waitForConnected(1000))
            {
                QMessageBox::information(this,tr("error"),tr("Connect server error!"));
                return;
            }
            connect_before = true;
        }
        else
        {
            client_socket->connectToHost(*serverIP,port);
            if (!client_socket->waitForConnected(1000))
            {
                QMessageBox::information(this,tr("error"),tr("Connect server error!"));
                return;
            }
        }
        status = true;
    }
    else
    {
        int length = 0;
        QString msg = username + tr(":Leave Chat Room");
        if((length = client_socket->write(msg.toLatin1(),msg.length())) != msg.length())
        {
            return;
        }
        client_socket->disconnectFromHost();
        status = false;

    }
}

void MainWindow::slotConnected()
{
    ui->Send_BTN->setEnabled(true);
    ui->Connect_BTN->setText(tr("disconnect"));
    int length = 0;
    QString msg = username + tr(":Enter Chat Room");
    if((length = client_socket->write(msg.toLatin1(),msg.length())) != msg.length())
    {
        return;
    }
}

void MainWindow::slotDisconnected()
{
    ui->Send_BTN->setEnabled(false);
    ui->Connect_BTN->setText(tr("Connect"));
    status = false;
}

void MainWindow::dataReceived()
{
    while(client_socket->bytesAvailable() > 0)
    {
        QByteArray datagram;
        datagram.resize(client_socket->bytesAvailable());
        client_socket->read(datagram.data(),datagram.size());
        QString msg = datagram.data();
        ui->Chat_WG->addItem(msg.left(datagram.size()));
    }
}

void MainWindow::slotSend()
{
    if(ui->Input_TXT->text() == "")
    {
        return;
    }
    int length = 0;
    QString msg = username + ":" + ui->Input_TXT->text();
    int len = msg.length()%8-5;
    if((length = client_socket->write(msg.toLatin1(),msg.length()-len)) != msg.length()-len)
    {
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(status)
    {
        int length = 0;
        QString msg = username + tr(":Leave Chat Room");
        if((length = client_socket->write(msg.toLatin1(),msg.length())) != msg.length())
        {
            return;
        }
        client_socket->disconnectFromHost();
        status = false;
    }
}
