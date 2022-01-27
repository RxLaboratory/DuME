#ifndef JOBEDITOR_H
#define JOBEDITOR_H

#include "duqf-nodeview/duqfnodewidget.h"

#include "UI/JobEditor/inputnode.h"
#include "UI/JobEditor/outputnode.h"
#include "UI/JobEditor/separate5dot1node.h"
#include "UI/JobEditor/separatestereonode.h"
#include "UI/JobEditor/mix5dot1node.h"
#include "UI/JobEditor/mixstereonode.h"
#include "UI/JobEditor/downmix5dot1tomononode.h"
#include "UI/JobEditor/downmix5dot1tostereonode.h"
#include "UI/JobEditor/downmixstereotomononode.h"
#include "UI/JobEditor/unpremultiplynode.h"
#include "UI/JobEditor/deinterlacenode.h"
#include "UI/JobEditor/motioninterpolationnode.h"
#include "UI/JobEditor/cropnode.h"
#include "UI/JobEditor/videospeednode.h"

class JobEditor : public DuQFNodeWidget
{
    Q_OBJECT
public:
    JobEditor(QWidget *parent = nullptr);

private slots:
    void addInputNode();
    void addOutputNode();

    void addSeparate5Dot1Node();
    void addSeparateStereoNode();
    void addMix5Dot1Node();
    void addMixStereoNode();
    void addDownMix5Dot1toStereoNode();
    void addDownMix5Dot1toMonoNode();
    void addDownMixStereotoMonoNode();

    void addUnpremultiplyNode();
    void addDeinterlaceNode();
    void addMotionInterpolationNode();
    void addCropNode();
    void addVideoSpeedNode();

private:
    void setupUi();
    void connectEvents();

    QAction *ui_addInputNode;
    QAction *ui_addOutputNode;

    QAction *ui_addSeparate5Dot1Node;
    QAction *ui_addSeparateStereoNode;
    QAction *ui_addMix5Dot1Node;
    QAction *ui_addMixStereoNode;
    QAction *ui_addDownMix5Dot1toStereoNode;
    QAction *ui_addDownMix5Dot1toMonoNode;
    QAction *ui_addDownMixStereotoMonoNode;

    QAction *ui_addUnpremultiplyNode;
    QAction *ui_addDeinterlaceNode;
    QAction *ui_addMotionInterpolationNode;
    QAction *ui_addCropNode;
    QAction *ui_addVideoSpeedNode;
};

#endif // JOBEDITOR_H
