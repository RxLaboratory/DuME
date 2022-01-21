#ifndef JOBEDITORWIDGET_H
#define JOBEDITORWIDGET_H

#include <QWidget>

#include "duqf-utils/guiutils.h"

#include "duqf-widgets/duqftitlebar.h"
#include "duqf-widgets/duqfspinbox.h"

#include "duqf-nodeview/duqfnodeview.h"
#include "duqf-nodeview/duqfnodescene.h"

class DuQFNodeWidget : public QWidget
{
public:
    explicit DuQFNodeWidget(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    void setupUi();

    DuQFTitleBar *ui_titleBar;

    DuQFNodeScene *m_nodeScene;
    DuQFNodeView *ui_nodeView;

};

#endif // JOBEDITORWIDGET_H
