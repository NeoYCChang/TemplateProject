#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    server* host = new server(NULL,8010);
    return a.exec();
}
