#ifndef OSDEVENTWORK_H
#define OSDEVENTWORK_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QAudioRecorder>
#include <QAudioEncoderSettings>
#include <QFileDialog>

class OsdEventWork : public QObject
{
    Q_OBJECT

public:
    OsdEventWork(QWidget *parent = nullptr);
    ~OsdEventWork();

private:
    QString AudiofileName;
    QMediaPlayer *audioplayer;
    QMediaRecorder *audiorecorder;
    QAudioRecorder  *m_pAudioRecorder;

public slots:
    void recordAudio(QWidget *parent = nullptr);
    void playAudio(QWidget *parent = nullptr);
    void stopRecording(void);
    void TestSlot();

signals:
    void RefreshdurationChanged(const qint64 duration);

};
#endif // OSDEVENTWORK_H
