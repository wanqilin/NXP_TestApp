#ifndef HOTPLUGWORKTHREAD_H
#define HOTPLUGWORKTHREAD_H

#include <QObject>
#include <QThread>
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
#endif

class HotPlugWorkThread : public QThread
{
    Q_OBJECT

public:
    HotPlugWorkThread();
    ~HotPlugWorkThread();
    void run() override;

signals:
    void RefreshUsbOSD(const int usbCnt);

private:
    int getUSBDeviceCount();
#ifdef OS_UNIX
    bool isUsbStorage(const std::string &devicePath);
#endif
    int usbCnt;
};
#endif // HOTPLUGWORKTHREAD_H
