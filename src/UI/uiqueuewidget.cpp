#include "uiqueuewidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

UIQueueWidget::UIQueueWidget(FFmpeg *ffmpeg, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _ffmpeg = ffmpeg;

    UIInputWidget *inputWidget = new UIInputWidget(_ffmpeg, this);
    inputWidgets << inputWidget;

    inputTab1->layout()->addWidget(inputWidget);

    addOutput();

    //hide close buttons on add buttons
    QTabBar *outputTabBar = outputTab->tabBar();

    QWidget *closeButton = outputTabBar->tabButton(1, QTabBar::RightSide);
    if (closeButton == nullptr) closeButton = outputTabBar->tabButton(1, QTabBar::LeftSide);
    if (closeButton != nullptr) closeButton->hide();

    //No add tab for now on input
    //QTabBar *inputTabBar = inputTab->tabBar();
    //inputTabBar->tabButton(1,QTabBar::RightSide)->hide();

    qDebug() << "splitter sizes";

    // splitter sizes
    QSettings settings;
    settings.beginGroup("queueSplitter");
    QList<int>sizes;
    sizes << settings.value("inputSize",50).toInt();
    sizes << settings.value("outputSize",50).toInt();
    splitter->setSizes(sizes);
    settings.endGroup();

    qDebug() << "queue created";
}

MediaInfo *UIQueueWidget::getInputMedia()
{
    //TODO build list
    return inputWidgets.at(0)->getMediaInfo();
}

QList<MediaInfo *> UIQueueWidget::getOutputMedia()
{
    QList<MediaInfo *> output;
    foreach(UIOutputWidget *ow,outputWidgets)
    {
        output << ow->getMediaInfo();
    }
    return output;
}

void UIQueueWidget::addInputFile(QString file)
{
    inputWidgets[0]->openFile(file);
}

void UIQueueWidget::addInputFile(QUrl file)
{
    inputWidgets[0]->openFile(file);
}

void UIQueueWidget::presetsPathChanged()
{
    foreach(UIOutputWidget *ow,outputWidgets)
    {
        ow->loadPresets();
    }
}

void UIQueueWidget::on_outputTab_tabCloseRequested(int index)
{
    int numTabs = outputTab->count();
    if (index == numTabs-1) return;
    if (numTabs <= 2) return;

    //remove tab
    outputTab->removeTab(index);
    //delete corresponding widget
    UIOutputWidget *w = outputWidgets.takeAt(index);
    w->deleteLater();
    //update current index
    if (outputTab->currentIndex() == outputTab->count()-1)
    {
        outputTab->setCurrentIndex(outputTab->count()-2);
    }
}

void UIQueueWidget::on_outputTab_tabBarClicked(int index)
{
    if (index != outputTab->count()-1) return;
    //add new
    addOutput();
}

void UIQueueWidget::log(QString log, LogUtils::LogType lt )
{
    emit newLog(log, lt);
}

void UIQueueWidget::addOutput()
{
    //number of the output
    int num = outputWidgets.count()+1;
    //create widget
    UIOutputWidget *ow = new UIOutputWidget(_ffmpeg, num, this);
    outputWidgets << ow;
    outputTab->insertTab(outputTab->count()-1,ow,"Output " + QString::number(num));
    //connect first input widget
    UIInputWidget *iw = inputWidgets.at(0);
    connect(iw,SIGNAL(newMediaLoaded(MediaInfo*)),ow,SLOT(newInputMedia(MediaInfo*)));
    //set tab index
    outputTab->setCurrentIndex(outputTab->count()-2);
    //get current media info
    MediaInfo *iwInfo = iw->getMediaInfo();
    if (iwInfo->fileName() != "") ow->newInputMedia(iwInfo);
    //connect console
    connect(ow,SIGNAL(newLog(QString,LogUtils::LogType)),this,SLOT(log(QString,LogUtils::LogType)));
}

void UIQueueWidget::on_splitter_splitterMoved()
{
    QSettings settings;
    settings.beginGroup("queueSplitter");
    settings.setValue("inputSize", splitter->sizes()[0]);
    settings.setValue("outputSize", splitter->sizes()[1]);
    settings.endGroup();
}
