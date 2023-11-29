#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>

class tcpclientsocket : public QTcpSocket
{
    Q_OBJECT
public:
    tcpclientsocket(QObject *parent = 0);
    QString name;
signals:
    void updateClients(QString,int,qintptr socketDescriptor);
    void disconnected(int);
protected slots:
    void dataReceived();
    void slotDisconnected();
};

#endif // TCPCLIENTSOCKET_H
