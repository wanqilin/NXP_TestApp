#ifndef WIFIWORKTHREAD_H
#define WIFIWORKTHREAD_H

#include <QObject>

class WifiWorkThread : public QObject
{
    Q_OBJECT

public:
    WifiWorkThread();
    ~WifiWorkThread();

public slots:
    void process();

signals:
    void RefreshOSD(const QStringList& wifiList);

private:
    QStringList wifiList;
    QStringList getWifiList();
};
#endif // WIFIWORKTHREAD_H
