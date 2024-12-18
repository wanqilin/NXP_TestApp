#include "osdeventwork.h"
#include <QPushButton>
#include <QtDebug>

OsdEventWork::OsdEventWork(QWidget *parent)
{
    qDebug() << "OsdEventThread is run!";
    #ifdef OS_UNIX
    // Init Gstreamer lib
    gst_init(nullptr, nullptr);
    Recordpipeline = nullptr;
    playingPipeline = nullptr;
    #else
    //init class
    audioplayer = new QMediaPlayer(static_cast<QWidget*>(parent));
    audiorecorder = new QMediaRecorder(audioplayer,static_cast<QWidget*>(parent));
    m_pAudioRecorder = new QAudioRecorder (static_cast<QWidget*>(parent));
    
    for ( QString &device : m_pAudioRecorder->audioInputs())
    {
        qDebug() << "Audiodevice:" << device;
    }
    

    for ( QString &codecName : m_pAudioRecorder->supportedAudioCodecs () )
    {
        qDebug() << "AudiocodecName:" << codecName;
    }

    connect(audioplayer, &QMediaPlayer::stateChanged, this, &OsdEventWork::onStateChanged);
    connect(audioplayer, &QMediaPlayer::mediaStatusChanged, this, &OsdEventWork::onMediaStateChanged);
    connect(m_pAudioRecorder, &QAudioRecorder::durationChanged, this, &OsdEventWork::onDurationChanged);
    #endif
}

OsdEventWork::~OsdEventWork()
{
    qDebug() << "~OsdEventThread!";
    stopRecording();
    #if OS_UNIX
    if (Recordpipeline) {
        gst_object_unref(Recordpipeline);
    }
    if (playingPipeline) {
        gst_object_unref(playingPipeline);
    }
    #endif
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

#if OS_UNIX
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_DURATION_CHANGED: {
            GstElement *pipeline = GST_ELEMENT(data);
            gint64 duration = 0;

            if (gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration)) {
                qDebug() << "Duration changed:" << GST_TIME_AS_SECONDS(duration) << "seconds";
            }
            break;
        }
        default:
            break;
    }
    return TRUE;
}
#endif

void OsdEventWork:: recordAudio(QString *sAudiofileName)
{
    m_sAudiofileName = *sAudiofileName;
    #if OS_UNIX
    stopRecording();

    QString pipelineDescription =
        "autoaudiosrc ! audioconvert ! audioresample ! "
        "wavenc ! filesink location=" + m_sAudiofileName;

    Recordpipeline = gst_parse_launch(pipelineDescription.toUtf8().constData(), nullptr);
    if (!Recordpipeline) {
         qDebug() << "Failed to create recording pipeline";
        return;
    }
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(Recordpipeline));
    gst_bus_add_watch(bus, bus_call, Recordpipeline);

    gst_element_set_state(Recordpipeline, GST_STATE_PLAYING);
    qDebug() << "Recording Audio started..." << m_sAudiofileName;

    #else
    if (!m_sAudiofileName.isEmpty()) {
        if (!m_sAudiofileName.endsWith(".wav", Qt::CaseInsensitive))
        {
            m_sAudiofileName += ".wav";
        }

        m_pAudioRecorder->setOutputLocation(QUrl::fromLocalFile(m_sAudiofileName));
        m_pAudioRecorder->setAudioInput(m_pAudioRecorder->defaultAudioInput());

        QAudioEncoderSettings AudioSettings;
        AudioSettings.setCodec("audio/pcm");
        AudioSettings.setSampleRate(44100);
        AudioSettings.setChannelCount(2);
        AudioSettings.setBitRate(128000);
        AudioSettings.setQuality(QMultimedia::HighQuality);
        m_pAudioRecorder->setAudioSettings(AudioSettings);

        QAudioEncoderSettings Settings;
        Settings= m_pAudioRecorder->audioSettings();

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
    #endif
}

void OsdEventWork::playAudio(QString *sAudiofileName)
{
    m_sAudiofileName = *sAudiofileName;
#if OS_UNIX
    QString pipelineDescription =
            "filesrc location=" + m_sAudiofileName + " ! wavparse ! audioconvert ! autoaudiosink";

    playingPipeline = gst_parse_launch(pipelineDescription.toUtf8().constData(), nullptr);
    if (!playingPipeline) {
        qDebug() << "Failed to create playback pipeline";
        return;
    }

    gst_element_set_state(playingPipeline, GST_STATE_PLAYING);
    qDebug() << "Playing audio...";

    GstBus *bus = gst_element_get_bus(playingPipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                                    (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));
    if (msg) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    gst_element_set_state(playingPipeline, GST_STATE_NULL);
    qDebug() << "Playback finished.";
#else
    if (!m_sAudiofileName.isEmpty()) {
        audioplayer->setMedia(QUrl::fromLocalFile(m_sAudiofileName));
        audioplayer->play();
        qDebug() << "Playing audio from" << m_sAudiofileName;
    }
#endif    
}

void OsdEventWork::stopRecording(void)
{
    #if OS_UNIX
    if (Recordpipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(Recordpipeline, GST_STATE_NULL);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qDebug() << "Failed to stop the recording pipeline.";
        }
        qDebug() << "Stop Recording..." << m_sAudiofileName;
    }
    if (playingPipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(playingPipeline, GST_STATE_NULL);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qDebug() << "Failed to stop the recording pipeline.";
        }
        qDebug() << "Stop Playing..." << m_sAudiofileName;
    }

    #else
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
    #endif
}

