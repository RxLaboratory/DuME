#include "jobeditor.h"

JobEditor::JobEditor(QWidget *parent):
    DuQFNodeWidget("Job editor", parent)
{
    setupUi();
    connectEvents();

    // Add default nodes
    addInputNode();
    addOutputNode();
}

void JobEditor::addInputNode()
{
    InputNode *node = new InputNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addOutputNode()
{
    OutputNode *node = new OutputNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addSeparate5Dot1Node()
{
    Separate5Dot1Node *node = new Separate5Dot1Node();
    m_nodeScene->addNode(node);
}

void JobEditor::addSeparateStereoNode()
{
    SeparateStereoNode *node = new SeparateStereoNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addMix5Dot1Node()
{
    Mix5Dot1Node *node = new Mix5Dot1Node();
    m_nodeScene->addNode(node);
}

void JobEditor::addMixStereoNode()
{
    MixStereoNode *node = new MixStereoNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addDownMix5Dot1toStereoNode()
{
    DownMix5Dot1ToStereoNode *node = new DownMix5Dot1ToStereoNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addDownMix5Dot1toMonoNode()
{
    DownMix5Dot1ToMonoNode *node = new DownMix5Dot1ToMonoNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addDownMixStereotoMonoNode()
{
    DownMixStereoToMonoNode *node = new DownMixStereoToMonoNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addUnpremultiplyNode()
{
    UnPremultiplyNode *node = new UnPremultiplyNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addDeinterlaceNode()
{
    DeinterlaceNode *node = new DeinterlaceNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addMotionInterpolationNode()
{
    MotionInterpolationNode *node = new MotionInterpolationNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addCropNode()
{
    CropNode *node = new CropNode();
    m_nodeScene->addNode(node);
}

void JobEditor::addVideoSpeedNode()
{
    VideoSpeedNode *node = new VideoSpeedNode();
    m_nodeScene->addNode(node);
}

void JobEditor::setupUi()
{
    // Add create actions
    ui_addInputNode = new QAction("Input Media");
    ui_addMenu->addAction(ui_addInputNode);

    ui_addOutputNode = new QAction("Output Media");
    ui_addMenu->addAction(ui_addOutputNode);

    QMenu *videoFiltersMenu = new QMenu("Video filters");

    ui_addUnpremultiplyNode = new QAction("Unpremultiply");
    videoFiltersMenu->addAction(ui_addUnpremultiplyNode);

    ui_addDeinterlaceNode = new QAction("Deinterlace");
    videoFiltersMenu->addAction(ui_addDeinterlaceNode);

    ui_addMotionInterpolationNode = new QAction("Motion interpolation");
    videoFiltersMenu->addAction(ui_addMotionInterpolationNode);

    ui_addCropNode = new QAction("Crop");
    videoFiltersMenu->addAction(ui_addCropNode);

    ui_addVideoSpeedNode = new QAction("Change speed");
    videoFiltersMenu->addAction(ui_addVideoSpeedNode);

    ui_addMenu->addMenu(videoFiltersMenu);

    QMenu *audioChannelMenu = new QMenu("Audio channels");

    ui_addSeparate5Dot1Node = new QAction("Separate 5.1");
    audioChannelMenu->addAction(ui_addSeparate5Dot1Node);

    ui_addSeparateStereoNode = new QAction("Separate Stereo");
    audioChannelMenu->addAction(ui_addSeparateStereoNode);

    ui_addMix5Dot1Node = new QAction("5.1 Mix");
    audioChannelMenu->addAction(ui_addMix5Dot1Node);

    ui_addMixStereoNode = new QAction("Stereo Mix");
    audioChannelMenu->addAction(ui_addMixStereoNode);

    ui_addDownMix5Dot1toStereoNode = new QAction("5.1 to Stereo downmix");
    audioChannelMenu->addAction(ui_addDownMix5Dot1toStereoNode);

    ui_addDownMix5Dot1toMonoNode = new QAction("5.1 to Mono downmix");
    audioChannelMenu->addAction(ui_addDownMix5Dot1toMonoNode);

    ui_addDownMixStereotoMonoNode = new QAction("Stereo to Mono downmix");
    audioChannelMenu->addAction(ui_addDownMixStereotoMonoNode);

    ui_addMenu->addMenu(audioChannelMenu);

}

void JobEditor::connectEvents()
{
    connect(ui_addInputNode, SIGNAL(triggered(bool)), this, SLOT(addInputNode()));

    connect(ui_addSeparate5Dot1Node, SIGNAL(triggered(bool)), this, SLOT(addSeparate5Dot1Node()));
    connect(ui_addSeparateStereoNode, SIGNAL(triggered(bool)), this, SLOT(addSeparateStereoNode()));
    connect(ui_addMix5Dot1Node, SIGNAL(triggered(bool)), this, SLOT(addMix5Dot1Node()));
    connect(ui_addMixStereoNode, SIGNAL(triggered(bool)), this, SLOT(addMixStereoNode()));
    connect(ui_addDownMix5Dot1toStereoNode, SIGNAL(triggered(bool)), this, SLOT(addDownMix5Dot1toStereoNode()));
    connect(ui_addDownMix5Dot1toMonoNode, SIGNAL(triggered(bool)), this, SLOT(addDownMix5Dot1toMonoNode()));
    connect(ui_addDownMixStereotoMonoNode, SIGNAL(triggered(bool)), this, SLOT(addDownMixStereotoMonoNode()));

    connect(ui_addUnpremultiplyNode, SIGNAL(triggered(bool)), this, SLOT(addUnpremultiplyNode()));
    connect(ui_addDeinterlaceNode, SIGNAL(triggered(bool)), this, SLOT(addDeinterlaceNode()));
    connect(ui_addMotionInterpolationNode, SIGNAL(triggered(bool)), this, SLOT(addMotionInterpolationNode()));
    connect(ui_addCropNode, SIGNAL(triggered(bool)), this, SLOT(addCropNode()));
    connect(ui_addVideoSpeedNode, SIGNAL(triggered(bool)), this, SLOT(addVideoSpeedNode()));
}
