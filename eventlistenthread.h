#ifndef EVENTLISTENTHREAD_H
#define EVENTLISTENTHREAD_H

#include <QObject>
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

class EventListenThread : public QObject
{
    Q_OBJECT

public:
    EventListenThread();
    ~EventListenThread();

public slots:
    void process();

signals:
    void RefreshOSD(const int usbCnt);

private:
    int getUSBDeviceCount();
#ifdef OS_UNIX
    bool isUsbStorage(const std::string &devicePath);
#endif
    int usbCnt;
};
#endif // EVENTLISTENTHREAD_H
