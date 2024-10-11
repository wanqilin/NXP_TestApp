#include "mainwindow.h"
#include "OpenCVWindow.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>
#include <QVBoxLayout>
#include <QListWidget>
#include <QProcess>
#include <QGroupBox>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2\imgproc\types_c.h>
#include <QImage>

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , pOpenCVWindow(nullptr)
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
    this->displayTitle = new QLabel("i.Mx8MP Test ",this);
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
    CameraGroupBox->setGeometry(940,0,350,600);
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
    captureButton->resize(180,50);

   //OpenCVButton
    OpenCVButton = new QPushButton("GotoOpenCV",this);
    OpenCVButton->resize(180,50);
    connect(OpenCVButton,&QPushButton::clicked,this,&MainWindow::GotoOpenCVWindow);

    Cameralayout->addWidget(viewfinder);
    Cameralayout->addWidget(CameraImage);
    Cameralayout->addWidget(captureButton);
    Cameralayout->addWidget(OpenCVButton);

    CameraGroupBox->setLayout(Cameralayout);
    OpenCVfaceRecognition(); //CameraInit();
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

int MainWindow::OpenCVfaceRecognition(void)
{
    Mat src;
    Mat gray;
    Mat dst;

    CascadeClassifier c;

    if(!c.load("D:/wql/openCV/4.5.1/QT-opencv-4.5.1/source/data/haarcascades/haarcascade_frontalface_alt2.xml"))
    {
        qDebug()<<"Load haarcascade_frontalface_alt2 fail!";
        return -1;
    }

    vector<Rect> faces;

    VideoCapture v(0);

    while(v.read(src))
    {
        flip(src, src, 1);
        cvtColor(src, gray, CV_BGR2GRAY);
        equalizeHist(gray, dst);
        c.detectMultiScale(dst, faces);

        for(std::vector<cv::Rect>::size_type i=0; i<faces.size(); i++)
        {
            rectangle(src, faces[i], Scalar(0,0,255), 2);
        }

        //CameraImage->clear();
        //CameraImage->setScaledContents(true);
        //QImage qimg = Mat2QImage(src);
        //CameraImage->setPixmap(QPixmap::fromImage(qimg));

        //展示图像
        imshow("Test1", src);
        //imshow("Test2", gray);
        //imshow("Test3", dst);


        //加延时函数
        //函数原型：int waitKey(int delay = 0);
        //参数：等待时间
        //返回值：在等待期间用户按下的键盘的ascii值    ESC键对应的值为27
        if(waitKey(20)==27)
        {
            break;
        }
    }
    return 0;
}

QImage MainWindow::Mat2QImage(Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)     //3 channels color image
    {

        cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols, cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols,cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols,cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_RGB888);
    }
    return qImg;
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
