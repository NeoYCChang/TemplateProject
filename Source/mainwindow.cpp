#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    show_flag=false;
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(Show_Hello()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Show_Hello(void)
{
    if(show_flag)
    {
        ui->pushButton->setText(tr("OK"));
        ui->label->setText(tr(""));
        show_flag = false;
    }
    else
    {
        ui->pushButton->setText(tr("Bye"));
        ui->label->setText(tr("Hello World"));
        show_flag = true;
    }
}

