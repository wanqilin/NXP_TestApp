#include "wifiworkthread.h"
#include <QtDebug>
#include <QThread>
#include <QProcess>

WifiWorkThread::WifiWorkThread() {}

WifiWorkThread::~WifiWorkThread() {}

void WifiWorkThread::process(void)
{
    qDebug()<<"wifiworkThread is run!";
    while(1)
    {
        wifiList = getWifiList();
        if(!wifiList.isEmpty())
            emit RefreshOSD(wifiList);

        QThread::sleep(1);
    }
}

QStringList WifiWorkThread::getWifiList(void)
{
    QStringList wifiList;
    QProcess process;
#ifdef OS_WINDOWS
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
#else
    process.start("nmcli", QStringList() << "d" << "wifi" << "list");
    process.waitForFinished();
    QTextStream stream(process.readAllStandardOutput());

    while (!stream.atEnd()) {
        QString line = stream.readLine();

        if (line.startsWith("SSID")) continue; // skip table

        QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        //qDebug()<<fields;
        if (fields.size() > 0) {
            wifiList.append(fields[1]); // add SSID
        }
    }
#endif

    return wifiList;
}
