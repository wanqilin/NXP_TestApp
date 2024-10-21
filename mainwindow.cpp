#include "mainwindow.h"
#include "OpenCVWindow.h"
#include "osdupdatethread.h"
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
    InitVariable();

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerHandle()));
    m_timer->start();


    osdupdatethread = new OSDUpdateThread();
    qthread = new QThread(this);
    osdupdatethread->moveToThread(qthread);
    qthread->start();

    SetSignalAndSLot();
    DrawOSDInterface();
}

MainWindow::~MainWindow()
{
    processor->requestInterruption();
    processor->wait();
}

void MainWindow::InitVariable(void)
{
    blanstatus = false;
    busb1status = false;
    busb2status = false;
    busb3status = false;
    btypecstatus = false;
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

    //Draw Listen event
    DrawListenEventPage();

    //draw camera
    DrawCameraPage();

    emit StartOSDThread();
}

void MainWindow::SetSignalAndSLot(void)
{
    connect(this,&MainWindow::StartOpenCVfaceRecognition,this,&MainWindow::OpenCVfaceRecognitionHandle);
    connect(qthread, &QThread::finished, qthread, &QThread::deleteLater);
    connect(this, &MainWindow::StartOSDThread, osdupdatethread, &OSDUpdateThread::working);
    connect(osdupdatethread, &OSDUpdateThread::ReDrawOSD,this,&MainWindow::OSDUpdate);

    //listen network connect status
    networkManager = new QNetworkConfigurationManager(this);
    connect(networkManager, &QNetworkConfigurationManager::onlineStateChanged, this, &MainWindow::DrawlanStatusUpdate);
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::TimerHandle(void)
{    
    ClockUpdate();
}

void MainWindow::OSDUpdate(void)
{
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
    //qDebug()<<"wifi list update!";
    // get wifi list
    QStringList wifiList = getWifiList();
    for (const QString &wifi : wifiList) {
        //qDebug()<<wifi;
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

void MainWindow::DrawListenEventPage(void)
{
    QGroupBox *ListenEventBox = new QGroupBox("Event",this);
    ListenEventBox->setGeometry(10,320,120,240);
    QVBoxLayout *Eventlayout = new QVBoxLayout(this);

    //Lan
    lanbox = new QGroupBox(this);
    QLabel *lantxt = new QLabel("LAN",this);
    lanstatus = new QLabel(this);
    lanstatus->setFixedSize(20, 20);
    lanstatus->setStyleSheet("background-color: gray; border-radius: 10px;");
    QHBoxLayout *lanlayout = new QHBoxLayout(this);
    lanlayout->addWidget(lantxt);
    lanlayout->addWidget(lanstatus);
    //lanlayout->addStretch();
    lanbox->setLayout(lanlayout);

    //USB-1
    usb1box = new QGroupBox(this);
    QLabel *usb1txt = new QLabel("USB-1",this);
    QLabel *usb1status = new QLabel(this);
    usb1status->setFixedSize(20, 20);
    usb1status->setStyleSheet("background-color: gray; border-radius: 10px;");
    QHBoxLayout *usb1layout = new QHBoxLayout(this);
    usb1layout->addWidget(usb1txt);
    usb1layout->addWidget(usb1status);
    //usb1layout->addStretch();
    usb1box->setLayout(usb1layout);

    //USB-2
    usb2box = new QGroupBox(this);
    QLabel *usb2txt = new QLabel("USB-2",this);
    QLabel *usb2status = new QLabel(this);
    usb2status->setFixedSize(20, 20);
    usb2status->setStyleSheet("background-color: gray; border-radius: 10px;");
    QHBoxLayout *usb2layout = new QHBoxLayout(this);
    usb2layout->addWidget(usb2txt);
    usb2layout->addWidget(usb2status);
    //usb2layout->addStretch();
    usb2box->setLayout(usb2layout);

    //USB-3
    usb3box = new QGroupBox(this);
    QLabel *usb3txt = new QLabel("USB-3",this);
    QLabel *usb3status = new QLabel(this);
    usb3status->setFixedSize(20, 20);
    usb3status->setStyleSheet("background-color: gray; border-radius: 10px;");
    QHBoxLayout *usb3layout = new QHBoxLayout(this);
    usb3layout->addWidget(usb3txt);
    usb3layout->addWidget(usb3status);
    //usb3layout->addStretch();
    usb3box->setLayout(usb3layout);

    //Type-C
    typecbox = new QGroupBox(this);
    QLabel *typectxt = new QLabel("Type-C",this);
    QLabel *typecstatus = new QLabel(this);
    typecstatus->setFixedSize(20, 20);
    typecstatus->setStyleSheet("background-color: gray; border-radius: 10px;");
    QHBoxLayout *typeclayout = new QHBoxLayout(this);
    typeclayout->addWidget(typectxt);
    typeclayout->addWidget(typecstatus);
    //typeclayout->addStretch();
    typecbox->setLayout(typeclayout);

    Eventlayout->addWidget(lanbox);
    Eventlayout->addWidget(usb1box);
    Eventlayout->addWidget(usb2box);
    Eventlayout->addWidget(usb3box);
    Eventlayout->addWidget(typecbox);
    Eventlayout->addStretch();
    ListenEventBox->setLayout(Eventlayout);

    blanstatus = networkManager->isOnline();
    DrawlanStatusUpdate(blanstatus);

}

void MainWindow::DrawlanStatusUpdate(bool isOnline)
{
    if (isOnline) {
        qDebug() << "Network connected!";
        lanstatus->setStyleSheet("background-color: green; border-radius: 10px;");
    } else {
        qDebug() << "Network disconnected!";
        lanstatus->setStyleSheet("background-color: gray; border-radius: 10px;");
    }
}
