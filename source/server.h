#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include "tcpclientsocket.h"

class server : public QTcpServer
{
    Q_OBJECT
public:
    server(QObject *parent = 0,int port = 0);
    QList<tcpclientsocket*> tcpclientssocket_list;

signals:
    void updateServer(QString,int);
public slots:
    void updateClients(QString,int);
    void slotDisconnected(int);
protected:
    void incomingConnection(qintptr socketDescriptor); //incomingConnectiond() is from <QTcpServer> (virtual function)

};

#endif // SERVER_H
