#include "rqueuewidget.h"

RQueueWidget::RQueueWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
    connectEvents();

    // Add a few items
    queueListItem = new QListWidgetItem(QIcon(":/icons/audio-video"), "Job #1", ui_rQueueList);
    // RQUEUE DEMO MODE / DEV TESTS! NOT FOR PUBLIC USE YET
#ifdef QT_DEBUG
    new QListWidgetItem(QIcon(":/icons/audio"), "Job #2", ui_rQueueList);
    new QListWidgetItem(QIcon(":/icons/video"), "Job #3", ui_rQueueList);
    new QListWidgetItem(QIcon(":/icons/ok"), "Job #4", ui_rQueueList);
    new QListWidgetItem(QIcon(":/icons/error"), "Job #5", ui_rQueueList);
    new QListWidgetItem(QIcon(":/icons/rendering"), "Job #6", ui_rQueueList);
    //TODO : drag and drop on the queuewidget; drop on the list adds a new queuewidget
#else // DISABLE QUEUE FOR NOW IN PUBLIC RELEASE
    ui_addJobButton->setEnabled(false);
    ui_removeJobButton->setEnabled(false);
    ui_launchJobButton->setEnabled(false);
    new QListWidgetItem("Render queue in development\nAvailable soon!", ui_rQueueList);
    ui_rQueueList->setCurrentRow(0);
    ui_rQueueList->setEnabled(false);
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

void RQueueWidget::connectEvents()
{
    // TODO connect job to its item
    //connect(queueWidget->job(), &QueueItem::statusChanged, this, &MainWindow::queueItemStatusChanged);
    // TODO connect list row to Job UI
    //connect(ui_rQueueList, &QListWidget::currentRowChanged, rQueueStack, &QStackedWidget::setCurrentIndex);

}

void RQueueWidget::queueItemStatusChanged(MediaUtils::RenderStatus status)
{
    QString itemText = queueListItem->text().split("\n")[0];
    if (MediaUtils::isBusy(status)) queueListItem->setIcon(QIcon(":/icons/rendering"));
    else if (status == MediaUtils::Finished || status == MediaUtils::Stopped) queueListItem->setIcon(QIcon(":/icons/ok"));
    else if (status == MediaUtils::Error) queueListItem->setIcon(QIcon(":/icons/error"));
    else queueListItem->setIcon(QIcon(":/icons/audio-video"));
    itemText += "\n{ " + MediaUtils::RenderStatusToHumanString(status) + " }";
    queueListItem->setText(itemText);
}
