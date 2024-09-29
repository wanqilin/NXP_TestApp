#include "mainwindow.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>
#include <QVBoxLayout>
#include <QListWidget>
#include <QProcess>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    DrawOSDInterface();
    SetSignalAndSLot();
    CameraHandle();
    WifiListInit();
}

MainWindow::~MainWindow() {}

void MainWindow::DrawOSDInterface(void)
{
    this->displayTitle = new QLabel("i.Mx8MP Test App",this);
    this->displayTitle->setGeometry(560,0,200,50);
    this->displayTitle->setFont(QFont("Arial",14,QFont::Bold));

    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(550,60,180,50);

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
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
    //Get Current Time
    QDateTime dt = QDateTime::currentDateTime();
    QString strTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    this->lcdnumber->display(strTime);

}

void MainWindow::CameraHandle(void)
{
    QGroupBox *CameraGroupBox = new QGroupBox("Camera",this);
    CameraGroupBox->setGeometry(940,0,350,550);
    QVBoxLayout *Cameralayout = new QVBoxLayout(this);

    QPalette palette;
    palette.setColor(QPalette::Window,QColor(50, 50, 50));

    //Creat Viewfinder
    viewfinder = new QCameraViewfinder(this);
    viewfinder->setFixedSize(320,240);
    viewfinder->setAutoFillBackground(true);
    viewfinder->setPalette(palette);

    //Creat CaptureImage object
    QPalette CameraImageTextpalette;
    CameraImageTextpalette.setColor(QPalette::WindowText,Qt::red);
    CameraImage = new QLabel("ImageDisplay",this);
    CameraImage->setPalette(CameraImageTextpalette);
    CameraImage->setFixedSize(320,240);
    CameraImage->setAutoFillBackground(true);
    CameraImage->setPalette(palette);

   // CameraImage->setScaledContents(true);
    CameraImage->setAlignment(Qt::AlignCenter);


    //CaptureButton
    captureButton= new QPushButton("Capture", this);
    //captureButton->setGeometry(1000,500,180,50);
    captureButton->resize(180,50);

    Cameralayout->addWidget(viewfinder);
    Cameralayout->addWidget(CameraImage);
    Cameralayout->addWidget(captureButton);

    CameraGroupBox->setLayout(Cameralayout);
    CameraInit();
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

        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(640,480);
        viewfinderSettings.setMaximumFrameRate(15.0);
        viewfinderSettings.setMaximumFrameRate(30.0);
        camera->setViewfinderSettings(viewfinderSettings);

        //camer connect Viewfinder
        camera->setViewfinder(viewfinder);

        CameraImage->clear();
        CameraImage->setScaledContents(true);

        //Creat ImageCapture Object
        imageCapture = new QCameraImageCapture(camera, this);
        connect(captureButton,&QPushButton::clicked,this,&MainWindow::captureImage);
        connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);
        camera->start();
    }
    else
    {
        qDebug()<<"No camera available.";
        CameraImage->setText("No camera available.");
    }
}

void MainWindow::captureImage() {
    // CaptureImage
    imageCapture->capture();
}

void MainWindow::displayImage(int id, const QImage &preview)
{
    Q_UNUSED(id);
    CameraImage->setPixmap(QPixmap::fromImage(preview));
}

void MainWindow::WifiListInit(void)
{
    QGroupBox *wifiGroupBox = new QGroupBox("Wifi List",this);
    wifiGroupBox->setGeometry(10,20,220,280);
    QPushButton *wifiRefresh = new QPushButton("Refresh",this);
    wifiRefresh->resize(140,20);
    QVBoxLayout *layout = new QVBoxLayout(this);
    QListWidget *listWidget = new QListWidget(this);
    listWidget->setFixedSize(150, 200);
    listWidget->setMinimumSize(150, 200);
    listWidget->setMaximumSize(200, 220);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(wifiRefresh);
    layout->addWidget(listWidget);
    wifiGroupBox->setLayout(layout);

    // get wifi list
    QStringList wifiList = getWifiList();
    for (const QString &wifi : wifiList) {
        listWidget->addItem(wifi);
    }
}

QStringList  MainWindow::getWifiList(void)
{
    QStringList wifiList;
    QProcess process;
    process.start("netsh", QStringList() << "wlan" << "show" << "network");
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
