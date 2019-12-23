#include "uiqueuewidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

UIQueueWidget::UIQueueWidget(FFmpeg *ff,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;

    UIInputWidget *inputWidget = new UIInputWidget(ffmpeg,this);
    inputWidgets << inputWidget;

    inputTab1->layout()->addWidget(inputWidget);

    addOutput();

    //hide close buttons on add buttons
    QTabBar *outputTabBar = outputTab->tabBar();
    outputTabBar->tabButton(1,QTabBar::RightSide)->hide();
    //No add tab for now on input
    //QTabBar *inputTabBar = inputTab->tabBar();
    //inputTabBar->tabButton(1,QTabBar::RightSide)->hide();

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

void UIQueueWidget::presetsPathChanged(QString path)
{
    foreach(UIOutputWidget *ow,outputWidgets)
    {
        ow->loadPresets(path);
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
    delete w;
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

void UIQueueWidget::consoleReceive(QString log)
{
    emit consoleEmit(log);
}

void UIQueueWidget::addOutput()
{
    //number of the output
    int num = outputWidgets.count()+1;
    //create widget
    UIOutputWidget *ow = new UIOutputWidget(ffmpeg,num,this);
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
    connect(ow,SIGNAL(consoleEmit(QString)),this,SLOT(consoleReceive(QString)));
}

