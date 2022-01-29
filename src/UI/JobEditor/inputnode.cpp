#include "inputnode.h"

InputNode::InputNode(QGraphicsItem *parent):
    DuMENode("Input Media", parent)
{
    m_mediaInfo = new MediaInfo();
    ui_inputBlock = new InputBlock(m_mediaInfo);
    setEditWidget(ui_inputBlock);
    connectEvents();
}

InputNode::InputNode(QString file, QGraphicsItem *parent):
    DuMENode("Input Media", parent)
{
    m_mediaInfo = new MediaInfo();
    ui_inputBlock = new InputBlock(m_mediaInfo);
    setEditWidget(ui_inputBlock);

    // We could just set the file to the mediainfo
    // But this way the path is stored in the settings too
    ui_inputBlock->openFile(file);
    mediaChanged();
    connectEvents();
}

MediaInfo *InputNode::mediaInfo() const
{
    return m_mediaInfo;
}

void InputNode::mediaChanged()
{
    //Update title with file name
    QFileInfo f(m_mediaInfo->fileName());
    this->setTitle( f.fileName() );

    QList<VideoInfo *> videoStreams = m_mediaInfo->videoStreams();
    QList<AudioInfo *> audioStreams = m_mediaInfo->audioStreams();

    // Don't change anything if the number of streams did not change
    // TODO we SHOULD change if the type of audios have changed
    if (m_numVideo == videoStreams.count() && m_numAudio == audioStreams.count()) return;

    m_numVideo = videoStreams.count();
    m_numAudio = audioStreams.count();

    // Remove all slots
    clearOutputs();

    // Add video
    for (int i = 0; i < videoStreams.count(); i++)
    {
        addVideoOutput();
    }

    // Add audio
    for (int i = 0; i < audioStreams.count(); i++)
    {
        QString channels = audioStreams.at(i)->channels();
        if (channels == "stereo") addStereoOutput();
        else if (channels == "5.1") add5Dot1Output();
        else addMonoOutput();
    }
}

void InputNode::connectEvents()
{
    connect(m_mediaInfo, &MediaInfo::changed, this, &InputNode::mediaChanged);
}
