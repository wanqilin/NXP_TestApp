#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(100,100,500,600);
    DrawOSDInterface();
    SetSignalAndSLot();
}

MainWindow::~MainWindow() {}

void MainWindow::DrawOSDInterface(void)
{
    this->inputTxt1 = new QLineEdit("lineEdit",this);
    this->inputTxt1->setGeometry(40,40,100,30);
    this->displayTxt1 = new QLabel("label",this);
    this->displayTxt1->setGeometry(40,100,100,30);
}

void MainWindow::SetSignalAndSLot(void)
{
    connect(this->inputTxt1, SIGNAL(textEdited(QString)), this->displayTxt1, SLOT(setText(QString)));
}
