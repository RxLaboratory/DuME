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
    Q_OBJECT
public:
    explicit DuQFNodeWidget(QString title = "Node view", QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    DuQFNodeScene *m_nodeScene;
    DuQFNodeView *ui_nodeView;

    QMenu *ui_addMenu;

private slots:
    void setSnapEnabled(bool enabled);
    void setGridSize(int size);

private:
    void setupUi(QString title);
    void connectEvents();

    DuQFTitleBar *ui_titleBar;

    QAction *ui_actionReinitView;
    QAction *ui_actionViewAll;
    QAction *ui_actionSelectAll;
    QAction *ui_actionSelectChildren;
    QAction *ui_actionSelectParents;
    QAction *ui_actionLayoutAll;
    QAction *ui_actionLayoutSelected;
    QAction *ui_actionDeleteNode;
    QAction *ui_actionDeleteSelection;
    QAction *ui_actionDeleteConnections;

    QToolButton *ui_viewAllButton;
    QToolButton *ui_viewSelectedButton;
    QToolButton *ui_snapButton;
    DuQFSpinBox *ui_zoomBox;
    DuQFSpinBox *ui_gridSizeBox;
};

#endif // JOBEDITORWIDGET_H
