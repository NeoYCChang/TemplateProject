#include "tcpclientsocket.h"

tcpclientsocket::tcpclientsocket(QObject *parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(dataReceived())); //readyRead() is from <QTcpSocket>
    connect(this,SIGNAL(disconnected()),this,SLOT(slotDisconnected())); //disconnected() is from <QTcpSocket>
}

//when data arrives
void tcpclientsocket::dataReceived()
{
    //bytesAvailable() is from <QTcpSocket>
    //Returns the number of bytes that are available for reading
    while(bytesAvailable() > 0)
    {
        int length = bytesAvailable();
        char buf[length];
        read(buf,length);
        QString msg(buf);
        emit updateClients(msg,length,this->socketDescriptor());
    }
}

void tcpclientsocket::slotDisconnected()
{
    emit disconnected(this->socketDescriptor());
}
