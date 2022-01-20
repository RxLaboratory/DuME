#ifndef RQUEUEWIDGET_H
#define RQUEUEWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

#include "duqf-utils/utils.h"

namespace Ui {
class RQueueWidget;
}

class RQueueWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RQueueWidget(QWidget *parent = nullptr);

signals:
    void launchQueue();

private slots:
    void queueItemStatusChanged(MediaUtils::RenderStatus status);
private:
    void setupUi();
    void connectEvents();

    QToolButton *ui_addJobButton;
    QToolButton *ui_removeJobButton;
    QToolButton *ui_launchJobButton;
    QToolButton *ui_launchQueueButton;
    QListWidget *ui_rQueueList;


    // For testing purposes
    QListWidgetItem *queueListItem;
};

#endif // RQUEUEWIDGET_H
