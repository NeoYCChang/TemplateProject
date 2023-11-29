#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    port = 8010;
    connect(ui->Create_BTN,SIGNAL(clicked()),this,SLOT(slotCreateServer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotCreateServer()
{
    port = ui->Port_TXT->text().toInt();
    host = new server(this,port);
    connect(host,SIGNAL(updateServer(QString,int)),this,SLOT(updateServer(QString,int)));
    ui->Create_BTN->setEnabled(false);
}

void MainWindow::updateServer(QString msg,int length)
{
    ui->Chat_WG->addItem(msg.left(length));
}
