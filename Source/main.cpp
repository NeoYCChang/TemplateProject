#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client* client = new Client();
    if(client->slotEnter() == 0)
    {
		cout << "Bye!" << endl;
        return a.exec();
    }
    else
    {
        char msg[1024];
        while(true)
        {
			cout << "Please enter some message: ";
			cin.getline(msg,1024);
            //scanf("%s",msg);
            client->slotSend(msg);
            memset(msg,'\0',1024);
        }

    }


    return a.exec();
}
