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
    m_timer->setSingleShot(false);  //�ǵ��δ���
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
    //��ȡ��ǰʱ��
    QDateTime dt = QDateTime::currentDateTime();

    //����ʱ���ַ���
    QString strTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    //���� LCD ʱ��
    this->lcdnumber->display(strTime);

    //���ý���������ʾ�� ����%10
    //ui->progressBar->setValue( dt.time().second() % 10 );
}

