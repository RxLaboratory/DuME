#include "rqueuewidget.h"

RQueueWidget::RQueueWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    // Add a few items
    /*queueListItem = new QListWidgetItem(QIcon(":/icons/audio-video"), "Job #1", rQueueList);
    // RQUEUE DEMO MODE / DEV TESTS! NOT FOR PUBLIC USE YET
#ifdef QT_DEBUG
    new QListWidgetItem(QIcon(":/icons/audio"), "Job #2", rQueueList);
    rQueueStack->addWidget(new QueueWidget(this));
    new QListWidgetItem(QIcon(":/icons/video"), "Job #3", rQueueList);
    rQueueStack->addWidget(new QueueWidget(this));
    rQueueList->setCurrentRow(0);
    new QListWidgetItem(QIcon(":/icons/ok"), "Job #4", rQueueList);
    rQueueStack->addWidget(new QueueWidget(this));
    new QListWidgetItem(QIcon(":/icons/error"), "Job #5", rQueueList);
    rQueueStack->addWidget(new QueueWidget(this));
    new QListWidgetItem(QIcon(":/icons/rendering"), "Job #6", rQueueList);
    rQueueStack->addWidget(new QueueWidget(this));
    connect(rQueueList, &QListWidget::currentRowChanged, rQueueStack, &QStackedWidget::setCurrentIndex);
    //TODO : drag and drop on the queuewidget; drop on the list adds a new queuewidget
#else // DISABLE QUEUE FOR NOW IN PUBLIC RELEASE
    actionLaunchJob->setEnabled(false);
    addJobButton->setEnabled(false);
    removeJobButton->setEnabled(false);
    launchJobButton->setEnabled(false);
    new QListWidgetItem("Render queue in development\nAvailable soon!", rQueueList);
    rQueueList->setCurrentRow(0);
    rQueueList->setEnabled(false);
#endif//*/
}

void RQueueWidget::setupUi()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(3);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(3);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    ui_addJobButton = new QToolButton(this);
    ui_addJobButton->setText("Add job");
    ui_addJobButton->setIcon(QIcon(":/icons/add-job"));

    buttonLayout->addWidget(ui_addJobButton);

    ui_removeJobButton = new QToolButton(this);
    ui_removeJobButton->setText("Remove job");
    ui_removeJobButton->setIcon(QIcon(":/icons/remove-job"));

    buttonLayout->addWidget(ui_removeJobButton);

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    ui_launchJobButton = new QToolButton(this);
    ui_launchJobButton->setText("Launch job");
    ui_launchJobButton->setIcon(QIcon(":/icons/encode-job"));

    buttonLayout->addWidget(ui_launchJobButton);

    ui_launchQueueButton = new QToolButton(this);
    ui_launchQueueButton->setText("Launch queue");
    ui_launchQueueButton->setIcon(QIcon(":/icons/encode"));

    buttonLayout->addWidget(ui_launchQueueButton);

    verticalLayout->addLayout(buttonLayout);

    ui_rQueueList = new QListWidget(this);
    ui_rQueueList->setSortingEnabled(true);

    verticalLayout->addWidget(ui_rQueueList);
}
