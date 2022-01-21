#include "duqfnodewidget.h"

DuQFNodeWidget::DuQFNodeWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
}

void DuQFNodeWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
    {
        ui_titleBar->show();
    }
    QWidget::showEvent(event);
}

void DuQFNodeWidget::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
    {
        m_nodeScene->clearSelection();
        m_nodeScene->clearFocus();
        ui_titleBar->hide();
    }
    QWidget::hideEvent(event);
}

void DuQFNodeWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(3);

    ui_nodeView = new DuQFNodeView(this);
    m_nodeScene = ui_nodeView->nodeScene();
    mainLayout->addWidget(ui_nodeView);

    // Title bar
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new DuQFTitleBar("Job Editor",false, mw);
    ui_titleBar->setObjectName("jobNodesToolBar");
    ui_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,ui_titleBar);
    ui_titleBar->setFloatable(false);
    ui_titleBar->hide();

    // View menu
    QMenu *viewMenu = new QMenu(this);

    QAction *actionReinitView = new QAction("Center view", this);
    actionReinitView->setShortcut(QKeySequence("Home"));
    viewMenu->addAction(actionReinitView);

    QAction *actionViewAll = new QAction("Center selection", this);
    actionViewAll->setShortcut(QKeySequence("F"));
    viewMenu->addAction(actionViewAll);

    QToolButton *viewButton = new QToolButton(this);
    viewButton->setText("View");
    viewButton->setIcon(QIcon(":/icons/view"));
    viewButton->setIconSize(QSize(16,16));
    viewButton->setObjectName("menuButton");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    viewButton->setMenu(viewMenu);

    ui_titleBar->insertLeft(viewButton);

    // Select menu
    QMenu *selectMenu = new QMenu(this);

    QAction *actionSelectAll = new QAction("Select all nodes", this);
    actionSelectAll->setShortcut(QKeySequence("A"));
    selectMenu->addAction(actionSelectAll);

    QAction *actionSelectChildren = new QAction("Select children nodes", this);
    actionSelectChildren->setShortcut(QKeySequence("Ctrl+A"));
    selectMenu->addAction(actionSelectChildren);

    QAction *actionSelectParents = new QAction("Select parent nodes", this);
    actionSelectParents->setShortcut(QKeySequence("Alt+A"));
    selectMenu->addAction(actionSelectParents);

    QToolButton *selectButton = new QToolButton(this);
    selectButton->setText("Select");
    selectButton->setIcon(QIcon(":/icons/select-menu"));
    selectButton->setIconSize(QSize(16,16));
    selectButton->setObjectName("menuButton");
    selectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    selectButton->setPopupMode(QToolButton::InstantPopup);
    selectButton->setMenu(selectMenu);

    ui_titleBar->insertLeft(selectButton);

    // Layout menu
    QMenu *layMenu = new QMenu(this);

    QAction *actionLayoutAll = new QAction("Layout all nodes", this);
    actionLayoutAll->setShortcut(QKeySequence("Shift+L"));
    layMenu->addAction(actionLayoutAll);

    QAction *actionLayoutSelected = new QAction("Layout selected nodes", this);
    actionLayoutSelected->setShortcut(QKeySequence("Alt+L"));
    layMenu->addAction(actionLayoutSelected);

    QToolButton *layButton = new QToolButton(this);
    layButton->setText("Layout");
    layButton->setIcon(QIcon(":/icons/steps-menu"));
    layButton->setIconSize(QSize(16,16));
    layButton->setObjectName("menuButton");
    layButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    layButton->setPopupMode(QToolButton::InstantPopup);
    layButton->setMenu(layMenu);

    ui_titleBar->insertLeft(layButton);

    // Node menu
    QMenu *ui_nodeMenu = new QMenu(this);

    QAction *actionDeleteNode = new QAction("Remove selected nodes", this);
    actionDeleteNode->setShortcut(QKeySequence("Shift+X"));
    ui_nodeMenu->addAction(actionDeleteNode);

    QAction *actionDeleteSelection = new QAction("Delete selection", this);
    actionDeleteSelection->setShortcut(QKeySequence("Delete"));
    ui_nodeMenu->addAction(actionDeleteSelection);

    QToolButton *nodeButton = new QToolButton(this);
    nodeButton->setText("Node");
    nodeButton->setIcon(QIcon(":/icons/step"));
    nodeButton->setIconSize(QSize(16,16));
    nodeButton->setObjectName("menuButton");
    nodeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    nodeButton->setPopupMode(QToolButton::InstantPopup);
    nodeButton->setMenu(ui_nodeMenu);

    ui_titleBar->insertLeft(nodeButton);

    // Connections menu
    QMenu *coMenu = new QMenu(this);

    QAction *actionDeleteConnections = new QAction("Remove selected connections", this);
    actionDeleteConnections->setShortcut(QKeySequence("Alt+X"));
    coMenu->addAction(actionDeleteConnections);

    QToolButton *coButton = new QToolButton(this);
    coButton->setText("Pipe");
    coButton->setIcon(QIcon(":/icons/connection"));
    coButton->setIconSize(QSize(16,16));
    coButton->setObjectName("menuButton");
    coButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    coButton->setPopupMode(QToolButton::InstantPopup);
    coButton->setMenu(coMenu);

    ui_titleBar->insertLeft(coButton);

    // Right buttons

    QToolButton *viewAllButton = new QToolButton(this);
    viewAllButton->setIcon(QIcon(":/icons/view-all"));
    ui_titleBar->insertRight(viewAllButton);
    QToolButton *viewSelectedButton = new QToolButton(this);
    viewSelectedButton->setIcon(QIcon(":/icons/view-selected"));
    ui_titleBar->insertRight(viewSelectedButton);

    DuQFSpinBox *zoomBox = new DuQFSpinBox(this);
    zoomBox->setMinimum(25);
    zoomBox->setMaximum(400);
    zoomBox->setSuffix("%");
    zoomBox->setPrefix("Zoom: ");
    zoomBox->setMaximumWidth(100);
    zoomBox->setValue(100);
    ui_titleBar->insertRight(zoomBox);

    DuQFSpinBox *ui_gridSizeBox = new DuQFSpinBox(this);
    ui_gridSizeBox->setMinimum(10);
    ui_gridSizeBox->setMaximum(100);
    ui_gridSizeBox->setMaximumWidth(100);
    ui_gridSizeBox->setValue(20);
    ui_gridSizeBox->setPrefix("Grid size: ");
    ui_titleBar->insertRight(ui_gridSizeBox);

    QToolButton *ui_snapButton = new QToolButton();
    ui_snapButton->setCheckable(true);
    ui_snapButton->setIcon(QIcon(":/icons/snap"));
    ui_titleBar->insertRight(ui_snapButton);
}
