#include "mainwindow.h"
#include "OpenCVWindow.h"
#include "opencvfacerecognition.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>
#include <QVBoxLayout>

#include <QProcess>
#include <QGroupBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , pOpenCVWindow(nullptr)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);



    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerHandle()));
    m_timer->start();

    SetSignalAndSLot();
    DrawOSDInterface();
}

MainWindow::~MainWindow()
{
    processor->requestInterruption();
    processor->wait();
}

void MainWindow::DrawOSDInterface(void)
{
    this->displayTitle = new QLabel("TestApp",this);
    this->displayTitle->setGeometry(560,0,200,50);
    this->displayTitle->setFont(QFont("Arial",14,QFont::Bold));

    //Draw Clock
    DrawClockPage();

    //draw wifi
    DrawWifiPage();

    //draw camera
    DrawCameraPage();
}

void MainWindow::SetSignalAndSLot(void)
{
   connect(this,&MainWindow::StartOpenCVfaceRecognition,this,&MainWindow::OpenCVfaceRecognitionHandle);
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::TimerHandle(void)
{    
    ClockUpdate();
    wifiListUpdate();
}

void MainWindow::DrawClockPage(void)
{
    //draw lcdnumber
    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(550,60,180,50);
}

void MainWindow::ClockUpdate(void)
{
    //Get Current Time
    QDateTime dt = QDateTime::currentDateTime();
    QString strTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    this->lcdnumber->display(strTime);
}
void MainWindow::DrawCameraPage(void)
{
    QGroupBox *CameraGroupBox = new QGroupBox("Camera",this);
    CameraGroupBox->setGeometry(940,0,350,600);
    QVBoxLayout *Cameralayout = new QVBoxLayout(this);

    QPalette palette;
    palette.setColor(QPalette::Window,QColor(50, 50, 50));
    palette.setColor(QPalette::WindowText, Qt::white);
    //Camera View
    CameraView = new QLabel("No Data!",this);
    CameraView->setPalette(palette);
    CameraView->setFixedSize(320,240);
    CameraView->setAutoFillBackground(true);
    CameraView->setPalette(palette);
    CameraView->setAlignment(Qt::AlignCenter);

    //MatchImage
    MatchImage = new QLabel("No Data!",this);
    MatchImage->setPalette(palette);
    MatchImage->setFixedSize(320,240);
    MatchImage->setAutoFillBackground(true);
    MatchImage->setPalette(palette);
    MatchImage->setAlignment(Qt::AlignCenter);

    //CaptureButton
    captureButton= new QPushButton("Capture", this);
    captureButton->resize(180,50);

   //OpenCVButton
    //OpenCVButton = new QPushButton("GotoOpenCV",this);
    //OpenCVButton->resize(180,50);
    //connect(OpenCVButton,&QPushButton::clicked,this,&MainWindow::GotoOpenCVWindow);

    Cameralayout->addWidget(CameraView);
    Cameralayout->addWidget(MatchImage);
    Cameralayout->addWidget(captureButton);
    //Cameralayout->addWidget(OpenCVButton);

    CameraGroupBox->setLayout(Cameralayout);

    //startOpenCVfaceRecognition
    //CameraInit();
    emit StartOpenCVfaceRecognition();
}

void MainWindow::CameraInit(void)
{
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (!cameras.isEmpty())
    {
        qDebug()<<"Camera deviceName:";
        qDebug()<<cameras.first().deviceName();
        //Creat CameraObject
        camera = new QCamera(cameras.first(),this);

        QPalette palette;
        palette.setColor(QPalette::Window,QColor(50, 50, 50));
        //Creat Viewfinder
        viewfinder = new QCameraViewfinder(this);
        viewfinder->setFixedSize(320,240);
        viewfinder->setAutoFillBackground(true);
        viewfinder->setPalette(palette);
        //Cameralayout->addWidget(viewfinder);
        //CameraGroupBox->setLayout(Cameralayout);
        
        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(640,480);
        viewfinderSettings.setMaximumFrameRate(15.0);
        viewfinderSettings.setMaximumFrameRate(30.0);
        camera->setViewfinderSettings(viewfinderSettings);

        //camer connect Viewfinder
        camera->setViewfinder(viewfinder);

        //Creat ImageCapture Object
        imageCapture = new QCameraImageCapture(camera, this);
        connect(captureButton,&QPushButton::clicked,this,&MainWindow::captureImage);
        connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);
        camera->start();
    }
    else
    {
        qDebug()<<"No camera available.";
        CameraView->setText("No camera available.");
    }
}

void MainWindow::captureImage() {
    // CaptureImage
    imageCapture->capture();
}

void MainWindow::displayImage(int id, const QImage &preview)
{
    Q_UNUSED(id);
    CameraView->clear();
    CameraView->setScaledContents(true);
    CameraView->setPixmap(QPixmap::fromImage(preview));
}

void MainWindow::OpenCVfaceRecognitionHandle(void)
{
    qDebug()<<"OpenCVfaceRecognitionHandle!";
    processor = new OpenCVfaceRecognition();
    connect(processor,&OpenCVfaceRecognition::frameProcessed,this,&MainWindow::displayImage);
    processor->start();
}


void MainWindow::GotoOpenCVWindow()
{
    if(!pOpenCVWindow)
    {
        qDebug()<<"CreateOpenCVWindow!";
        pOpenCVWindow = new OpenCVWindow(this);
        connect(pOpenCVWindow, &QWidget::destroyed, this, [this]() { pOpenCVWindow = nullptr; });
    }
    qDebug()<<"OpenCVWindowShow!";
    pOpenCVWindow->show();
    pOpenCVWindow->raise();    //front desk
    pOpenCVWindow->activateWindow();
}

void MainWindow::DrawWifiPage(void)
{
    QGroupBox *wifiGroupBox = new QGroupBox("Wifi List",this);
    wifiGroupBox->setGeometry(10,20,220,280);
    QVBoxLayout *layout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    listWidget->setFixedSize(150, 200);
    listWidget->setMinimumSize(150, 200);
    listWidget->setMaximumSize(200, 220);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(listWidget);
    wifiGroupBox->setLayout(layout);
}

void MainWindow::wifiListUpdate(void)
{
    this->listWidget->clear();
    // get wifi list
    QStringList wifiList = getWifiList();
    for (const QString &wifi : wifiList) {
        this->listWidget->addItem(wifi);
    }
}

QStringList  MainWindow::getWifiList(void)
{
    QStringList wifiList;
    QProcess process;
#if(_WORK_OS_ENV_ == _WORK_OS_WINDOWS_)
    process.start("netsh", QStringList() << "wlan" << "show" << "network");
#else
#endif
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');

    for (const QString &line : lines) {
        if (line.contains("SSID")) {
            QStringList parts = line.split(':');
            if (parts.size() > 1) {
                wifiList.append(parts[1].trimmed());
            }
        }
    }
    return wifiList;
}
