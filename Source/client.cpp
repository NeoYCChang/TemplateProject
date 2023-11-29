#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    status = false;
    connect_before = false;
    serverIP = new QHostAddress();
    port = 8010;
}

int Client::slotEnter()
{
    if(!status)
    {
        char ip[16];
        while(true)
        {
			cout << "Please enter server ip: ";
            cin.getline(ip,16);
            //scanf("%s",ip);
            if(!serverIP->setAddress(ip))
            {
				cout << "[Error] Server ip address error!" << endl;
            }
            else
            {
                break;
            }
        }
        while(true)
        {
			cout << "Please enter user name: ";
            cin.getline(username,256);
            //scanf("%s",username);
            if(username[0] == NULL)
            {
				cout << "[Error] User name error!" << endl;
            }
            else
            {
                break;
            }
        }

        client_socket = new QTcpSocket(this);
        connect(client_socket,SIGNAL(connected()),this,SLOT(slotConnected())); //connected() is from <QTcpSocket>
        connect(client_socket,SIGNAL(disconnected()),this,SLOT(slotDisconnected())); //disconnected() is from <QTcpSocket>
        connect(client_socket,SIGNAL(readyRead()),this,SLOT(dataReceived())); //readyRead() is from <QTcpSocket>
        while(true)
        {
            client_socket->connectToHost(*serverIP,port);
            if (!client_socket->waitForConnected(1000))
            {
                char judge[4];
				cout << "Connect server error!" << endl;
				cout << "Try again? yes/no: ";
                cin.getline(judge,4);
                //scanf("%s",judge);
                if((judge[0]=='n' || judge[0]=='N') && (judge[1]=='o' || judge[1]=='O'))
                {
                    return 0;
                }
            }
            else
            {
                connect_before = true;
                break;
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
            return 0;
        }
        client_socket->disconnectFromHost();
        status = false;
    }
    return 1;
}

void Client::slotConnected()
{
	cout << "Connected to server" << endl;
    int length = 0;
    QString msg = username + tr(":Enter Chat Room");
    if((length = client_socket->write(msg.toLatin1(),msg.length())) != msg.length())
    {
        client_socket->waitForBytesWritten();
        return;
    }
    client_socket->waitForBytesWritten();
}

void Client::slotDisconnected()
{
	cout << "Disconnected from server" << endl;
    status = false;
}

void Client::dataReceived()
{
    while(client_socket->bytesAvailable() > 0)
    {
        QByteArray datagram;
        datagram.resize(client_socket->bytesAvailable());
        client_socket->read(datagram.data(),datagram.size());
        char* msg = datagram.data();
		cout << "[From server] ";
        for(int i = 0;i < datagram.size();i++)
        {
            if(*(msg+i) == '\0')
            {
                if(i< datagram.size()-1)
                {
					cout << "\n[From server] ";
                }
            }
            else
            {
				cout << *(msg+i);
            }
        }
		cout << endl;
    }
}

int Client::slotSend(char* message)
{
    if(message == NULL)
    {
        return 0;
    }
    int length = 0;
    QString msg = username + tr(":") + message;
	QByteArray ba = msg.toLatin1();
	ba.append("\0");
    if((length = client_socket->write(ba,ba.size())) != ba.size())
    {
        client_socket->waitForBytesWritten();
        return 0;
    }
    else
    {
        client_socket->waitForBytesWritten();
        return 1;
    }
}
