#include "mainwindow.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("NxpTestApp"));
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
    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(100,100,180,50);

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //非单次触发
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(RecvTimer()));
    m_timer->start();
}

void MainWindow::SetSignalAndSLot(void)
{
    connect(this->inputTxt1, SIGNAL(textEdited(QString)), this->displayTxt1, SLOT(setText(QString)));
    connect(this->inputTxt1,SIGNAL(textEdited(QString)),this,SLOT(PrintText(QString)));
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::RecvTimer(void)
{
    //获取当前时间
    QDateTime dt = QDateTime::currentDateTime();

    //构造时间字符串
    QString strTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    //设置 LCD 时钟
    this->lcdnumber->display(strTime);

    //设置进度条的显示： 秒数%10
    //ui->progressBar->setValue( dt.time().second() % 10 );
}

