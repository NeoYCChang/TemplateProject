#include "server.h"

server::server(QObject *parent,int port)
    : QTcpServer(parent)
{
    listen(QHostAddress::Any,port); //listen() is from <QTcpServer>
	printf("Host listening...\n");
}

void server::incomingConnection(qintptr socketDescriptor)
{
    tcpclientsocket* client_socket = new tcpclientsocket(this);
    connect(client_socket,SIGNAL(updateClients(QString,int,qintptr)),this,SLOT(updateClients(QString,int,qintptr)));
    connect(client_socket,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));
    client_socket->setSocketDescriptor(socketDescriptor);
    tcpclientssocket_list.append(client_socket);
}

void server::updateClients(QString msg,int length,qintptr socketDescriptor)
{
    emit updateServer(msg,length);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeStr = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    QByteArray ba = currentDateTimeStr.toLocal8Bit();
	ba.append("->");
    int ba_size = ba.size();
    ba.append(msg.toLocal8Bit());
    const char *c_str2 = ba.data();
    for(int i = 0;i < ba_size+length;i++)
	{
        printf("%c",*(c_str2+i));
	}
	printf("\n");
    QByteArray buffer = msg.toLatin1();
    buffer.remove(length,buffer.size()-length);
    buffer.append('\0');
    for(int i = 0;i< tcpclientssocket_list.count();i++)
    {
        QTcpSocket* item = tcpclientssocket_list.at(i);
        if(socketDescriptor == item->socketDescriptor() && tcpclientssocket_list.at(i)->name.isEmpty())
        {
            tcpclientssocket_list.at(i)->name = msg.left(msg.indexOf(':'));
        }
        if(item->write(buffer,length+1)!=length+1)
        {
            continue;
        }
    }
}

void server::slotDisconnected(int descirptor)
{
    QString name;
    for(int i = 0;i< tcpclientssocket_list.count();i++)
    {
        QTcpSocket* item = tcpclientssocket_list.at(i);
        if(item->socketDescriptor() == descirptor)
        {
            name = tcpclientssocket_list.at(i)->name;
            tcpclientssocket_list.removeAt(i);
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString currentDateTimeStr = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
            QByteArray ba = currentDateTimeStr.toLocal8Bit();
            ba.append("->");
            ba.append(name.toLocal8Bit());
            ba.append(":disconnected from server");
            const char *c_str2 = ba.data();
            printf("%s",c_str2);
            printf("\n");
            break;
        }
    }
    if(!name.isEmpty())
    {
        QString msg = name+":disconnected from server";
        QByteArray buffer = msg.toLatin1();
        buffer.append('\0');
        for(int i = 0;i< tcpclientssocket_list.count();i++)
        {
            QTcpSocket* item = tcpclientssocket_list.at(i);
            if(item->write(buffer,buffer.size())!=buffer.size())
            {
                continue;
            }
        }
    }
}
