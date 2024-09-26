#include "mainwindow.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    DrawOSDInterface();
    SetSignalAndSLot();
    CameraInit();
}

MainWindow::~MainWindow() {}

void MainWindow::CameraInit(void)
{

}

void MainWindow::DrawOSDInterface(void)
{
    this->displayTitle = new QLabel("i.Mx8MP Test App",this);
    this->displayTitle->setGeometry(560,0,200,50);
    this->displayTitle->setFont(QFont("Arial",16,QFont::Bold));

    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(550,60,180,50);

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //非单次触发
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(RecvTimer()));
    m_timer->start();
}

void MainWindow::SetSignalAndSLot(void)
{
    ;
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

