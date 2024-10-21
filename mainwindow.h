#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QCamera>
#include <QMessageBox>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QCameraInfo>
#include <QGroupBox>
#include <QNetworkConfigurationManager>
#include <opencv2/core/core.hpp>
#include "sw_app_config.h"
#include "osdupdatethread.h"

using namespace cv;
using namespace std;

class OpenCVWindow;
class OpenCVfaceRecognition;
class OSDUpdateThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    #define APP_WIDTH   1280
    #define APP_HEIGH   800

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void CameraInit();

signals:
    void StartOpenCVfaceRecognition(void);
    void StartOSDThread(void);

private slots:
    void PrintText(const QString &text);
    void captureImage();
    void displayImage(int id, const QImage &preview);
    void GotoOpenCVWindow();
    void OpenCVfaceRecognitionHandle();
    void TimerHandle();
    void OSDUpdate();
    void DrawlanStatusUpdate(bool isOnline);
private:
    bool blanstatus;
    bool busb1status;
    bool busb2status;
    bool busb3status;
    bool btypecstatus;
    QPushButton *captureButton;
    QPushButton *OpenCVButton;
    QLCDNumber *lcdnumber;
    QListWidget *listWidget;
    QLabel *displayTitle;
    QTimer *m_timer;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QLabel *CameraView;
    QLabel *MatchImage;
    QLabel *lanstatus;
    QLabel *usb1status;
    QLabel *usb2status;
    QLabel *usb3status;
    QLabel *typecstatus;
    QGroupBox *lanbox;
    QGroupBox *usb1box;
    QGroupBox *usb2box;
    QGroupBox *usb3box;
    QGroupBox *typecbox;
    OpenCVWindow *pOpenCVWindow;
    OpenCVfaceRecognition *processor;
    OSDUpdateThread* osdupdatethread;
    QThread* qthread;
    QNetworkConfigurationManager *networkManager;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
    QStringList getWifiList();
    void wifiListUpdate();
    void ClockUpdate();
    void DrawClockPage();
    void DrawWifiPage();
    void DrawCameraPage();
    void DrawListenEventPage();
    void InitVariable();
};
#endif // MAINWINDOW_H
