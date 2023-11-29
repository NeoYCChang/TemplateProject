#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostInfo>
#include <iostream>
using namespace std;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

public slots:
    void slotConnected();
    void slotDisconnected();
    void dataReceived();

public:
    int slotEnter();
    int slotSend(char* msg);


private:
    bool status;
    bool connect_before;
    int port;
    QHostAddress* serverIP;
    char username[256];
    QTcpSocket* client_socket;

};

#endif // CLIENT_H
