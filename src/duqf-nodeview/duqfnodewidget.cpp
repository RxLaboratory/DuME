#include "duqfnodewidget.h"

DuQFNodeWidget::DuQFNodeWidget(QString title, QWidget *parent)
    : QWidget{parent}
{
    setupUi(title);
    connectEvents();
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

void DuQFNodeWidget::setSnapEnabled(bool enabled)
{
    QSignalBlocker b(ui_snapButton);

    ui_snapButton->setChecked(enabled);

    ui_nodeView->grid()->setSnapEnabled(enabled);

    QSettings settings;
    settings.setValue("nodeView/snapToGrid", enabled);
}

void DuQFNodeWidget::setGridSize(int size)
{
    QSignalBlocker b(ui_gridSizeBox);

    ui_gridSizeBox->setValue(size);
    ui_nodeView->grid()->setSize(size);
    ui_nodeView->update();

    QSettings settings;
    settings.setValue("nodeView/gridSize", size);
}

void DuQFNodeWidget::setupUi(QString title)
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

    ui_titleBar = new DuQFTitleBar(title,false, mw);
    ui_titleBar->setObjectName("jobNodesToolBar");
    ui_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,ui_titleBar);
    ui_titleBar->setFloatable(false);
    ui_titleBar->showCloseButton(false);
    ui_titleBar->hide();

    // View menu
    QMenu *viewMenu = new QMenu(this);

    ui_actionReinitView = new QAction("Center view", this);
    ui_actionReinitView->setShortcut(QKeySequence("Home"));
    viewMenu->addAction(ui_actionReinitView);

    ui_actionViewAll = new QAction("Center selection", this);
    ui_actionViewAll->setShortcut(QKeySequence("F"));
    viewMenu->addAction(ui_actionViewAll);

    QToolButton *viewButton = new QToolButton(this);
    viewButton->setText("View");
    viewButton->setIcon(QIcon(":/icons/show"));
    viewButton->setIconSize(QSize(16,16));
    viewButton->setObjectName("menuButton");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    viewButton->setMenu(viewMenu);

    ui_titleBar->insertLeft(viewButton);

    // Select menu
    QMenu *selectMenu = new QMenu(this);

    ui_actionSelectAll = new QAction("Select all nodes", this);
    ui_actionSelectAll->setShortcut(QKeySequence("A"));
    selectMenu->addAction(ui_actionSelectAll);

    ui_actionSelectChildren = new QAction("Select children nodes", this);
    ui_actionSelectChildren->setShortcut(QKeySequence("Ctrl+A"));
    selectMenu->addAction(ui_actionSelectChildren);

    ui_actionSelectParents = new QAction("Select parent nodes", this);
    ui_actionSelectParents->setShortcut(QKeySequence("Alt+A"));
    selectMenu->addAction(ui_actionSelectParents);

    QToolButton *selectButton = new QToolButton(this);
    selectButton->setText("Select");
    selectButton->setIcon(QIcon(":/icons/select"));
    selectButton->setIconSize(QSize(16,16));
    selectButton->setObjectName("menuButton");
    selectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    selectButton->setPopupMode(QToolButton::InstantPopup);
    selectButton->setMenu(selectMenu);

    ui_titleBar->insertLeft(selectButton);

    // Layout menu
    QMenu *layMenu = new QMenu(this);

    ui_actionLayoutAll = new QAction("Layout all nodes", this);
    ui_actionLayoutAll->setShortcut(QKeySequence("Shift+L"));
    layMenu->addAction(ui_actionLayoutAll);

    ui_actionLayoutSelected = new QAction("Layout selected nodes", this);
    ui_actionLayoutSelected->setShortcut(QKeySequence("Alt+L"));
    layMenu->addAction(ui_actionLayoutSelected);

    QToolButton *layButton = new QToolButton(this);
    layButton->setText("Layout");
    layButton->setIcon(QIcon(":/icons/nodes"));
    layButton->setIconSize(QSize(16,16));
    layButton->setObjectName("menuButton");
    layButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    layButton->setPopupMode(QToolButton::InstantPopup);
    layButton->setMenu(layMenu);

    ui_titleBar->insertLeft(layButton);

    // Node menu
    QMenu *ui_nodeMenu = new QMenu(this);

    ui_addMenu = new QMenu("Add",this);

    ui_nodeMenu->addMenu(ui_addMenu);
    ui_nodeMenu->addSeparator();

    ui_actionDeleteNode = new QAction("Remove selected nodes", this);
    ui_actionDeleteNode->setShortcut(QKeySequence("Shift+X"));
    ui_nodeMenu->addAction(ui_actionDeleteNode);

    ui_actionDeleteSelection = new QAction("Delete selection", this);
    ui_actionDeleteSelection->setShortcut(QKeySequence("Delete"));
    ui_nodeMenu->addAction(ui_actionDeleteSelection);

    QToolButton *nodeButton = new QToolButton(this);
    nodeButton->setText("Node");
    nodeButton->setIcon(QIcon(":/icons/node"));
    nodeButton->setIconSize(QSize(16,16));
    nodeButton->setObjectName("menuButton");
    nodeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    nodeButton->setPopupMode(QToolButton::InstantPopup);
    nodeButton->setMenu(ui_nodeMenu);

    ui_titleBar->insertLeft(nodeButton);

    // Connections menu
    QMenu *coMenu = new QMenu(this);

    ui_actionDeleteConnections = new QAction("Remove selected connections", this);
    ui_actionDeleteConnections->setShortcut(QKeySequence("Alt+X"));
    coMenu->addAction(ui_actionDeleteConnections);

    QToolButton *coButton = new QToolButton(this);
    coButton->setText("Connection");
    coButton->setIcon(QIcon(":/icons/node-connection"));
    coButton->setIconSize(QSize(16,16));
    coButton->setObjectName("menuButton");
    coButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    coButton->setPopupMode(QToolButton::InstantPopup);
    coButton->setMenu(coMenu);

    ui_titleBar->insertLeft(coButton);

    // Right buttons

    ui_viewAllButton = new QToolButton(this);
    ui_viewAllButton->setIcon(QIcon(":/icons/frame"));
    ui_titleBar->insertRight(ui_viewAllButton);
    ui_viewSelectedButton = new QToolButton(this);
    ui_viewSelectedButton->setIcon(QIcon(":/icons/frame-nodes"));
    ui_titleBar->insertRight(ui_viewSelectedButton);

    ui_zoomBox = new DuQFSpinBox(this);
    ui_zoomBox->setMinimum(25);
    ui_zoomBox->setMaximum(400);
    ui_zoomBox->setSuffix("%");
    ui_zoomBox->setPrefix("Zoom: ");
    ui_zoomBox->setMaximumWidth(100);
    ui_zoomBox->setValue(100);
    ui_titleBar->insertRight(ui_zoomBox);

    ui_gridSizeBox = new DuQFSpinBox(this);
    ui_gridSizeBox->setMinimum(10);
    ui_gridSizeBox->setMaximum(100);
    ui_gridSizeBox->setMaximumWidth(100);
    ui_gridSizeBox->setValue(20);
    ui_gridSizeBox->setPrefix("Grid size: ");
    ui_titleBar->insertRight(ui_gridSizeBox);

    ui_snapButton = new QToolButton();
    ui_snapButton->setCheckable(true);
    ui_snapButton->setIcon(QIcon(":/icons/snap"));
    ui_titleBar->insertRight(ui_snapButton);
}

void DuQFNodeWidget::connectEvents()
{
    connect(ui_viewAllButton, SIGNAL(clicked()), ui_nodeView, SLOT(reinitTransform()));
    connect(ui_actionReinitView, SIGNAL(triggered()), ui_nodeView, SLOT(reinitTransform()));
    connect(ui_viewSelectedButton, SIGNAL(clicked()), ui_nodeView, SLOT(frameSelected()));
    connect(ui_actionViewAll, SIGNAL(triggered()), ui_nodeView, SLOT(frameSelected()));
    connect(ui_zoomBox, SIGNAL(valueChanged(int)), ui_nodeView, SLOT(setZoom(int)));
    connect(ui_nodeView, SIGNAL(zoomed(int)), ui_zoomBox, SLOT(setValue(int)));
    connect(ui_actionDeleteNode, SIGNAL(triggered()), m_nodeScene, SLOT(removeSelectedNodes()));
    connect(ui_actionDeleteConnections, SIGNAL(triggered()), m_nodeScene, SLOT(removeSelectedConnections()));
    connect(ui_actionDeleteSelection, SIGNAL(triggered()), m_nodeScene, SLOT(removeSelection()));
    connect(ui_snapButton, SIGNAL(clicked(bool)), this, SLOT(setSnapEnabled(bool)));
    connect(ui_gridSizeBox, SIGNAL(valueChanged(int)), this, SLOT(setGridSize(int)));
    connect(ui_actionLayoutAll, SIGNAL(triggered()), m_nodeScene, SLOT(autoLayoutAll()));
    connect(ui_actionLayoutAll, SIGNAL(triggered()), ui_nodeView, SLOT(frameSelected()));
    connect(ui_actionLayoutSelected, SIGNAL(triggered()), m_nodeScene, SLOT(autoLayoutSelectedNodes()));
    connect(ui_actionSelectAll, SIGNAL(triggered()), m_nodeScene, SLOT(selectAllNodes()));
    connect(ui_actionSelectChildren, SIGNAL(triggered()), m_nodeScene, SLOT(selectChildNodes()));
    connect(ui_actionSelectParents, SIGNAL(triggered()), m_nodeScene, SLOT(selectParentNodes()));
}
