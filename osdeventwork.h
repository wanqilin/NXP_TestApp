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
    OsdEventWork();
    ~OsdEventWork();

private:
    QString AudiofileName;
    QMediaPlayer *audioplayer;
    QMediaRecorder *audiorecorder;
    QAudioRecorder  *m_pAudioRecorder;

public slots:
    void recordAudio();
    void playAudio();
    void stopRecording();

signals:
    void RefreshdurationChanged(const qint64 duration);


};
#endif // OSDEVENTWORK_H
