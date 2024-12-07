#ifndef WIRELESSDEVICEWORKTHREAD_H
#define WIRELESSDEVICEWORKTHREAD_H

#include <QObject>
#include <QThread>
#include <QRegularExpression>

class WirelessDeviceWorkThread : public QThread
{
    Q_OBJECT

public:
    WirelessDeviceWorkThread();
    ~WirelessDeviceWorkThread();
    void run() override;

signals:
    void RefreshWifiOSD(const QStringList& wifiList);

private:
    QStringList wifiList;
    QStringList getWifiList();
};
#endif // WIRELESSDEVICEWORKTHREAD_H
