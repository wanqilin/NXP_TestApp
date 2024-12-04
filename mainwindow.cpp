#include "mainwindow.h"
#include "OpenCVWindow.h"
#include "osdupdatethread.h"
#include "opencvfacerecognition.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>
#include <QVBoxLayout>

#include <QGroupBox>

using namespace std;

#ifdef OS_WINDOWS
// define GUID
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
            0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

// define GUID_DEVINTERFACE_DISK check usb disk
DEFINE_GUID(GUID_DEVINTERFACE_DISK,
            0x53f56307,0xb6bf,0x11d0,0x94,0xf2,0x00,0xa0,0xc9,0x1e,0xfb,0x8b);
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, pOpenCVWindow(nullptr)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    InitVariable();

    qthread = new QThread(this);
    pWifiWorkThread = new WifiWorkThread();
    pWifiWorkThread->moveToThread(qthread);

    DrawOSDInterface();
    SetSignalAndSLot();

    qthread->start();
}

MainWindow::~MainWindow()
{    
    //processor->requestInterruption();
    //processor->wait();
    //qthread->requestInterruption();
    //qthread->->wait();
}

void MainWindow::InitVariable(void)
{
    blanstatus = false;
}

void MainWindow::DrawOSDInterface(void)
{
    appbox = new QGroupBox(this);
    appbox->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    applayout = new QBoxLayout(QBoxLayout::TopToBottom,this);

    this->displayTitle = new QLabel("TestApp",this);
    this->displayTitle->setGeometry(530,50,200,50);
    this->displayTitle->setFont(QFont("Arial",16,QFont::Bold));
    //applayout->addWidget(displayTitle);
    //appbox->setLayout(applayout);

    //Draw Clock
    DrawClockPage();

    //draw wifi
    DrawWifiPage();

    //draw BT
    DrawBtPage();

    //Draw Listen event
    DrawListenEventPage();

    //Draw Audio
    DrawAudioPage();

    //draw camera
    DrawCameraPage();
}

void MainWindow::SetSignalAndSLot(void)
{
    connect(qthread, &QThread::finished, qthread, &QThread::deleteLater);
    //opencv slot
    connect(qthread, &QThread::started,this,&MainWindow::OpenCVfaceRecognitionHandle);
    //wifi slot
    connect(qthread, &QThread::started, pWifiWorkThread, &WifiWorkThread::process);
    connect(pWifiWorkThread, &WifiWorkThread::RefreshOSD, this, &MainWindow::wifiListUpdate);
    //network slot
    connect(networkManager, &QNetworkConfigurationManager::onlineStateChanged, this, &MainWindow::DrawlanStatusUpdate);
    //Audio slot
    connect(m_pAudioRecorder, &QAudioRecorder::stateChanged, this, &MainWindow::onStateChanged);
    connect (m_pAudioRecorder, &QAudioRecorder::durationChanged, this, &MainWindow::onDurationChanged);
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::OSDUpdate(void)
{
#ifdef OS_UNIX
    USBDeviceUpdate();
#endif
}

void MainWindow::DrawClockPage(void)
{
    //draw lcdnumber
    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(500,160,180,50);
    //applayout->addWidget(lcdnumber);
    //appbox->setLayout(applayout);

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(ClockUpdate()));
    m_timer->start();
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
    CameraGroupBox->setGeometry(720,20,560,445);
    QVBoxLayout *Cameralayout = new QVBoxLayout(this);

    QPalette palette;
    palette.setColor(QPalette::Window,QColor(50, 50, 50));
    palette.setColor(QPalette::WindowText, Qt::white);
    //Camera View
    CameraView = new QLabel("No Data!",this);
    CameraView->setPalette(palette);
    CameraView->setFixedSize(540,405);
    CameraView->setAutoFillBackground(true);
    CameraView->setPalette(palette);
    CameraView->setAlignment(Qt::AlignCenter);

    //MatchImage
    /*
    MatchImage = new QLabel("No Data!",this);
    MatchImage->setPalette(palette);
    MatchImage->setFixedSize(320,240);
    MatchImage->setAutoFillBackground(true);
    MatchImage->setPalette(palette);
    MatchImage->setAlignment(Qt::AlignCenter);
    */
    //CaptureButton
    /*
    captureButton= new QPushButton("Capture", this);
    captureButton->resize(180,50);
    */
   //OpenCVButton
    //OpenCVButton = new QPushButton("GotoOpenCV",this);
    //OpenCVButton->resize(180,50);
    //connect(OpenCVButton,&QPushButton::clicked,this,&MainWindow::GotoOpenCVWindow);

    Cameralayout->addWidget(CameraView);
    //Cameralayout->addWidget(MatchImage);
    //Cameralayout->addWidget(captureButton);
    //Cameralayout->addWidget(OpenCVButton);

    CameraGroupBox->setLayout(Cameralayout);

    //applayout->addWidget(CameraGroupBox);
    //appbox->setLayout(applayout);

    //startOpenCVfaceRecognition
    //CameraInit();
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
/*
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
*/

void MainWindow::DrawWifiPage(void)
{
    QGroupBox *wifiGroupBox = new QGroupBox("Wifi List",this);
    wifiGroupBox->setGeometry(10,20,220,280);
    QVBoxLayout *Wifilayout = new QVBoxLayout(this);
    WifilistWidget = new QListWidget(this);
    WifilistWidget->setFixedSize(150, 200);
    WifilistWidget->setMinimumSize(150, 200);
    WifilistWidget->setMaximumSize(200, 220);
    WifilistWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    WifilistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Wifilayout->addWidget(WifilistWidget);
    wifiGroupBox->setLayout(Wifilayout);
    //applayout->addWidget(wifiGroupBox);
    //appbox->setLayout(applayout);

}

void MainWindow::wifiListUpdate(const QStringList &wifiList)
{
    //qDebug()<<"wifi list update!";
    this->WifilistWidget->clear();

    // get wifi list
    for (const QString &wifi : wifiList) {
        //qDebug()<<wifi;
        this->WifilistWidget->addItem(wifi);
    }
}

void MainWindow::DrawBtPage(void)
{
    QGroupBox *BtGroupBox = new QGroupBox("BT List",this);
    BtGroupBox->setGeometry(240,20,220,280);
    QVBoxLayout *Btlayout = new QVBoxLayout(this);
    BtlistWidget = new QListWidget(this);
    BtlistWidget->setFixedSize(150, 200);
    BtlistWidget->setMinimumSize(150, 200);
    BtlistWidget->setMaximumSize(200, 220);
    BtlistWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    BtlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Btlayout->addWidget(BtlistWidget);
    BtGroupBox->setLayout(Btlayout);
    //applayout->addWidget(wifiGroupBox);
    //appbox->setLayout(applayout);

}

void MainWindow::DrawListenEventPage(void)
{
    networkManager = new QNetworkConfigurationManager(this);

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

    //USB
    usbbox = new QGroupBox(this);
    usbtxt = new QLabel("USB-1",this);
    usbstatus = new QLabel("0",this);
    usbstatus->setFixedSize(20, 20);
    usbstatus->setAlignment(Qt::AlignCenter);
    usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    QHBoxLayout *usblayout = new QHBoxLayout(this);
    usblayout->addWidget(usbtxt);
    usblayout->addWidget(usbstatus);
    //usb1layout->addStretch();
    usbbox->setLayout(usblayout);

    Eventlayout->addWidget(lanbox);
    Eventlayout->addWidget(usbbox);
    Eventlayout->addStretch();
    ListenEventBox->setLayout(Eventlayout);

    //applayout->addWidget(ListenEventBox);
    //appbox->setLayout(applayout);

    qDebug()<<"Usb Init Cnt:"<<getUSBDeviceCount();
    if(getUSBDeviceCount()>0)
        usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
    else
        usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    usbstatus->setNum(getUSBDeviceCount());
    //blanstatus = networkManager->isOnline();
    //DrawlanStatusUpdate(blanstatus);

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

#ifdef OS_WINDOWS
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result){
    int usbCnt=0;

    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_DEVICECHANGE) {
            if (msg->wParam == DBT_DEVICEARRIVAL) {
                qDebug() << "USB plug-in";
            } else if (msg->wParam == DBT_DEVICEREMOVECOMPLETE) {    
                qDebug() << "USB plug-out";
            }

            usbCnt=getUSBDeviceCount();
            qDebug()<<"Usb Cnt:"<<usbCnt;
            if(usbCnt>0)
                usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
            else
                usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
            usbstatus->setNum(usbCnt);

        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

int MainWindow::getUSBDeviceCount() {
    // Get usb device info 
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
         &GUID_DEVINTERFACE_DISK, // &GUID_DEVINTERFACE_USB_DEVICE,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        qWarning() << "GetDeviceInfo fail";
        return -1;
    }

    // loop device info
    int deviceCount = 0;
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (int i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        deviceCount++;
    }

    // clean device info
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return deviceCount;
}
#endif 
#ifdef OS_UNIX
bool MainWindow::isUsbStorage(const std::string& devicePath) {
    std::string usbPath = "/sys/class/block/" + devicePath + "/device";
    DIR* dir = opendir(usbPath.c_str());
    if (dir) {
        // If the device directory exists, it is a USB device
        closedir(dir);
        return true;
    }
    return false;
}

int MainWindow::getUSBDeviceCount() {
    int usbCount = 0;

    DIR* dir = opendir("/sys/class/block");
    if (!dir) {
        qDebug() << "Failed to open /sys/class/block";
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // exclude "." and ".."
        if (entry->d_name[0] == '.')
            continue;

        std::string deviceName(entry->d_name);

        if (isUsbStorage(deviceName)) {
            usbCount++;
            //qDebug() << "Found USB storage: " << deviceName.c_str();
        }
    }

    closedir(dir);
    return usbCount;
}

void MainWindow::USBDeviceUpdate(void)
{
    int usbCnt=0;

    usbCnt=getUSBDeviceCount();
    //qDebug()<<"Usb Cnt:"<<usbCnt;
    if(usbCnt>0)
        usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
    else
        usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    usbstatus->setNum(usbCnt);
}
#endif

void MainWindow:: recordAudio(void)
{

#ifdef OS_WINDOWS
    QString outputFile = "audio_output.mp3";
    AudiofileName = QFileDialog::getSaveFileName(this, "Save Audio File", outputFile, "*.mp3");
#else
    AudiofileName = "/usr/bin/audio_output.mp3"
#endif
    if (!AudiofileName.isEmpty()) {
        m_pAudioRecorder->setOutputLocation(QUrl::fromLocalFile(AudiofileName));
        m_pAudioRecorder->setAudioInput(m_pAudioRecorder->defaultAudioInput());

        QAudioEncoderSettings encoderSettings;
        encoderSettings.setCodec("audio/mpeg");
        encoderSettings.setSampleRate(44100);
        encoderSettings.setChannelCount(2);
        encoderSettings.setBitRate(128000);
        encoderSettings.setQuality(QMultimedia::HighQuality);
        m_pAudioRecorder->setAudioSettings(encoderSettings);

        if (m_pAudioRecorder->state() == QMediaRecorder::StoppedState){
            m_pAudioRecorder->record();
            qDebug() << "Recording started to" << AudiofileName;
            qDebug() << "Current recorder state:" << m_pAudioRecorder->state();
        }
        else{
            qDebug() << "Recorder is already in use or an error occurred.";
        }
    }
else {
        qDebug() << "Recording cancelled.";
    }
}

void MainWindow::playAudio(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Audio File", "", "*.mp3 *.wav");
    if (!fileName.isEmpty()) {
        audioplayer->setMedia(QUrl::fromLocalFile(fileName));
        audioplayer->play();
        qDebug() << "Playing audio from" << fileName;
    }
}

void MainWindow::stopRecording()
{
    if (m_pAudioRecorder->state() == QMediaRecorder::RecordingState) {
        m_pAudioRecorder->stop();
        if (QFile::exists(AudiofileName)) {
            qDebug() << "Recording successfully saved to:" << AudiofileName;
        } else {
            qDebug() << "Recording failed. File not found at:" << AudiofileName;
        }
    } else {
        qDebug() << "No active recording to stop.";
    }
}
void MainWindow::onStateChanged(QMediaRecorder::State state)
{
    qDebug() << "Recording state:"<< state;
}

void MainWindow::onDurationChanged(qint64 duration)
{
    //record time
    qDebug() << "Record:"<< duration;
    AudioRecordButton->setText(QString("Recorded %1 seconds").arg(duration / 1000) ) ;
}
void MainWindow::DrawAudioPage(void)
{
    //init class
    audioplayer = new QMediaPlayer(this);
    audiorecorder = new QMediaRecorder(audioplayer,this);

    m_pAudioRecorder = new QAudioRecorder (this) ;

   /*
    for ( QString &device : m_pAudioRecorder->audioInputs())
    {
        qDebug() << "device:" << device;
        //ui->comboxDevices->addItem (device);
    }

    for ( QString &codecName : m_pAudioRecorder->supportedAudioCodecs () )
    {
        qDebug() << "codecName:" << codecName;
       // ui->comboxCodec->addItem (codecName) ;
    }
  */
    QGroupBox *AudioGroupBox = new QGroupBox("Audio",this);
    AudioGroupBox->setGeometry(1030,500,200,200);

    Audiolayout = new QVBoxLayout(this);

    //AddRecord
    AudioRecordButton = new QPushButton("Record Audio", this);
    connect(AudioRecordButton, &QPushButton::clicked, this, &MainWindow::recordAudio);
    Audiolayout->addWidget(AudioRecordButton);

    //add stopRecord
    stopRecordButton = new QPushButton("Stop Recording", this);
    connect(stopRecordButton, &QPushButton::clicked, this, &MainWindow::stopRecording);
    Audiolayout->addWidget(stopRecordButton);

    //AddPlayButton
    AudioPlayButton = new QPushButton("Play Audio", this);
    connect(AudioPlayButton, &QPushButton::clicked, this, &MainWindow::playAudio);
    Audiolayout->addWidget(AudioPlayButton);

    AudioGroupBox->setLayout(Audiolayout);
}


