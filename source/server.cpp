#include "server.h"

server::server(QObject *parent,int port)
    : QTcpServer(parent)
{
    listen(QHostAddress::Any,port); //listen() is from <QTcpServer>
}

void server::incomingConnection(qintptr socketDescriptor)
{
    tcpclientsocket* client_socket = new tcpclientsocket(this);
    connect(client_socket,SIGNAL(updateClients(QString,int)),this,SLOT(updateClients(QString,int)));
    connect(client_socket,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));
    client_socket->setSocketDescriptor(socketDescriptor);
    tcpclientssocket_list.append(client_socket);
}

void server::updateClients(QString msg,int length)
{
    emit updateServer(msg,length);
    for(int i = 0;i< tcpclientssocket_list.count();i++)
    {
        QTcpSocket* item = tcpclientssocket_list.at(i);
        if(item->write(msg.toLatin1(),length)!=length)
        {
            continue;
        }
    }
}

void server::slotDisconnected(int descirptor)
{
    for(int i = 0;i< tcpclientssocket_list.count();i++)
    {
        QTcpSocket* item = tcpclientssocket_list.at(i);
        if(item->socketDescriptor() == descirptor)
        {
            tcpclientssocket_list.removeAt(i);
            return;
        }
    }
}
