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
#include <QBoxLayout>
#include <QNetworkConfigurationManager>
#include <opencv2/core/core.hpp>
#ifdef OS_UNIX
#include <dirent.h>
#endif
#ifdef OS_WINDOWS
#include <windows.h>
#include <QDir>
#include <QFileInfo>
#include <dbt.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#include <QAudioRecorder>
#endif
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QAudioEncoderSettings>
#include <QFileDialog>

#include "sw_app_config.h"
#include "wifiworkthread.h"


using namespace cv;
using namespace std;

//class OpenCVWindow;
class OpenCVfaceRecognition;
class WifiWorkThread;

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
    void StartOSDThread(void);

protected:
#ifdef OS_WINDOWS
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif

private slots:
    void PrintText(const QString &text);
    void captureImage();
    void displayImage(int id, const QImage &preview);
    //void GotoOpenCVWindow();
    void ClockUpdate(void);
    void OpenCVfaceRecognitionHandle();
    void DrawlanStatusUpdate(bool isOnline);
    void recordAudio();
    void playAudio();
    void stopRecording();
    void wifiListUpdate(const QStringList &wifiList);
private:
    bool blanstatus;
    QString AudiofileName;
    QPushButton *captureButton;
    QPushButton *OpenCVButton;
    QPushButton *AudioPlayButton;
    QPushButton *AudioRecordButton;
    QPushButton *stopRecordButton;
    QLCDNumber *lcdnumber;
    QListWidget *WifilistWidget;
    QListWidget *BtlistWidget;
    QLabel *displayTitle;
    QTimer *m_timer;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QLabel *CameraView;
    QLabel *MatchImage;
    QLabel *lanstatus;
    QLabel *usbtxt;
    QLabel *usbstatus;

    QGroupBox *appbox;
    QGroupBox *lanbox;
    QGroupBox *usbbox;
    QBoxLayout *applayout;
    QHBoxLayout *usblayout;
    QVBoxLayout *Audiolayout;
    //OpenCVWindow *pOpenCVWindow;
    OpenCVfaceRecognition *processor;
    WifiWorkThread *pWifiWorkThread;
    QThread* qthread;
    QNetworkConfigurationManager *networkManager;
    QMediaPlayer *audioplayer;
    QMediaRecorder *audiorecorder;
    QAudioRecorder  * m_pAudioRecorder;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
    QStringList getWifiList();
    void DrawClockPage();
    void DrawWifiPage();
    void DrawCameraPage();
    void DrawListenEventPage();
    void InitVariable();
    int getUSBDeviceCount();
#ifdef OS_UNIX
    bool isUsbStorage(const std::string &devicePath);
    void USBDeviceUpdate();
#endif
    void DrawAudioPage();
    void DrawBtPage();
    void onStateChanged(QMediaRecorder::State state);
    void onDurationChanged(qint64 duration);
    void OSDUpdate();
};
#endif // MAINWINDOW_H
