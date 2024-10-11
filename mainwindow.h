#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTimer>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QCameraInfo>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class OpenCVWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    #define APP_WIDTH   1280
    #define APP_HEIGH   800

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void CameraInit();
public slots:
    void RecvTimer();

private slots:
    void PrintText(const QString &text);

    void captureImage();
    void displayImage(int id, const QImage &preview);
    void GotoOpenCVWindow();
private:
    QPushButton *captureButton;
    QPushButton *OpenCVButton;
    QLCDNumber *lcdnumber;
    QLabel *displayTitle;
    QTimer *m_timer;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QLabel *CameraImage;
    OpenCVWindow *pOpenCVWindow;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
    void WifiListInit();
    QStringList getWifiList();
    void CameraHandle();
    QImage Mat2QImage(Mat cvImg);
    int OpenCVfaceRecognition();
};
#endif // MAINWINDOW_H
