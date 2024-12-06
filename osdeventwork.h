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
    QMediaPlayer *audioplayer;
    QMediaRecorder *audiorecorder;
    QAudioRecorder  *m_pAudioRecorder;
    QString m_sAudiofileName;

public slots:
    void recordAudio(QString *sAudiofileName);
    void playAudio(QString *sAudiofileName);
    void stopRecording(void);

signals:
    void RefreshdurationChanged(const qint64 duration);
    void RefreshPlayStatus(const QMediaPlayer::State newState);
    void RefreshMediaPlayStatus(const QMediaPlayer::MediaStatus newState);

private slots:
    void onDurationChanged(qint64 duration);
    void onStateChanged();
    void onMediaStateChanged();
};
#endif // OSDEVENTWORK_H
