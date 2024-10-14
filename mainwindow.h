#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sw_app_config.h"
#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QTimer>
#include <QCamera>
#include <QMessageBox>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QCameraInfo>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class OpenCVWindow;
class OpenCVfaceRecognition;

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

private slots:
    void PrintText(const QString &text);

    void captureImage();
    void displayImage(int id, const QImage &preview);
    void GotoOpenCVWindow();
    void OpenCVfaceRecognitionHandle();
    void TimerHandle();
private:
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
    OpenCVWindow *pOpenCVWindow;
    class OpenCVfaceRecognition *processor;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
    QStringList getWifiList();
    void wifiListUpdate();
    void ClockUpdate();
    void DrawClockPage();
    void DrawWifiPage();
    void DrawCameraPage();
};
#endif // MAINWINDOW_H
