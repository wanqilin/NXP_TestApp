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
}

OsdEventWork::~OsdEventWork() {}

void OsdEventWork::TestSlot(void)
{
    qDebug()<< "TestSlot Slot";
}
void OsdEventWork:: recordAudio(QWidget *parent)
{
    qDebug()<< "recordAudio Slot";
#ifdef OS_WINDOWS
    QString outputFile = "audio_output.mp3";
    AudiofileName = QFileDialog::getSaveFileName(static_cast<QWidget*>(parent), "Save Audio File", outputFile, "*.mp3");
#else
    AudiofileName = "/usr/bin/audio_output.mp3"
#endif
    if (!AudiofileName.isEmpty()) {
        m_pAudioRecorder->setOutputLocation(QUrl::fromLocalFile(AudiofileName));
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
            qDebug() << "Recording started to" << AudiofileName;
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

void OsdEventWork::playAudio(QWidget *parent)
{
    qDebug()<< "playAudio Slot";
    QString fileName = QFileDialog::getOpenFileName(static_cast<QWidget*>(parent), "Open Audio File", "", "*.mp3 *.wav");
    if (!fileName.isEmpty()) {
        audioplayer->setMedia(QUrl::fromLocalFile(fileName));
        audioplayer->play();
        qDebug() << "Playing audio from" << fileName;
    }
}

void OsdEventWork::stopRecording(void)
{
    qDebug()<< "stopRecording Slot";
    if (m_pAudioRecorder->state() == QMediaRecorder::RecordingState) {
        m_pAudioRecorder->stop();
        if (QFile::exists(AudiofileName)) {
            qDebug() << "Recording successfully saved to:" << AudiofileName;
        } else {
            qDebug() << "Recording failed. File not found at:" << AudiofileName;
        }
    } else {
        qDebug() << "No active recording to stop.";
    }
}

