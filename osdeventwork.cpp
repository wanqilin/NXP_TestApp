#include "osdeventwork.h"
#include <QPushButton>
#include <QtDebug>

OsdEventWork::OsdEventWork(QWidget *parent)
{
    qDebug() << "OsdEventThread is run!";
    //init class
    audioplayer = new QMediaPlayer(static_cast<QWidget*>(parent));
    audiorecorder = new QMediaRecorder(audioplayer,static_cast<QWidget*>(parent));
    m_pAudioRecorder = new QAudioRecorder (static_cast<QWidget*>(parent));

    connect(audioplayer, &QMediaPlayer::stateChanged, this, &OsdEventWork::onStateChanged);
    connect(audioplayer, &QMediaPlayer::mediaStatusChanged, this, &OsdEventWork::onMediaStateChanged);
    connect(m_pAudioRecorder, &QAudioRecorder::durationChanged, this, &OsdEventWork::onDurationChanged);
}

OsdEventWork::~OsdEventWork()
{
    qDebug() << "~OsdEventThread!";
}

void OsdEventWork::onStateChanged(void)
{
    qDebug() << "audioplayer onStateChange" << audioplayer->state();
    emit RefreshPlayStatus(audioplayer->state());
}

void OsdEventWork::onMediaStateChanged(void)
{
    qDebug() << "audioplayer onMediaStateChanged" << audioplayer->mediaStatus();
    emit RefreshMediaPlayStatus(audioplayer->mediaStatus());
}

void OsdEventWork::onDurationChanged(qint64 duration)
{
    emit RefreshdurationChanged(duration);
}

void OsdEventWork:: recordAudio(QString *sAudiofileName)
{
    m_sAudiofileName = *sAudiofileName;

    if (!m_sAudiofileName.isEmpty()) {
        m_pAudioRecorder->setOutputLocation(QUrl::fromLocalFile(m_sAudiofileName));
        m_pAudioRecorder->setAudioInput(m_pAudioRecorder->defaultAudioInput());

        QAudioEncoderSettings encoderSettings;
        encoderSettings.setCodec("audio/mpeg");
        encoderSettings.setSampleRate(44100);
        encoderSettings.setChannelCount(2);
        encoderSettings.setBitRate(128000);
        encoderSettings.setQuality(QMultimedia::HighQuality);
        m_pAudioRecorder->setAudioSettings(encoderSettings);

        if (m_pAudioRecorder->state() == QMediaRecorder::StoppedState){
            m_pAudioRecorder->record();
            qDebug() << "Recording started to" << m_sAudiofileName;
            qDebug() << "Current recorder state:" << m_pAudioRecorder->state();
        }
        else{
            qDebug() << "Recorder is already in use or an error occurred.";
        }
    }
    else {
        qDebug() << "Recording cancelled.";
    }
}

void OsdEventWork::playAudio(QString *sAudiofileName)
{

    if (!(*sAudiofileName).isEmpty()) {
        audioplayer->setMedia(QUrl::fromLocalFile((*sAudiofileName)));
        audioplayer->play();
        qDebug() << "Playing audio from" << (*sAudiofileName);
    }
}

void OsdEventWork::stopRecording(void)
{
    if (m_pAudioRecorder->state() == QMediaRecorder::RecordingState) {
        m_pAudioRecorder->stop();
        if (QFile::exists(m_sAudiofileName)) {
            qDebug() << "Recording successfully saved to:" << m_sAudiofileName;
        } else {
            qDebug() << "Recording failed. File not found at:" << m_sAudiofileName;
        }
    } else {
        qDebug() << "No active recording to stop.";
    }
}

