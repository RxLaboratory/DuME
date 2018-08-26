#include "queuewidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

QueueWidget::QueueWidget(FFmpeg *ff,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;

    InputWidget *inputWidget = new InputWidget(ffmpeg,this);
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

FFMediaInfo *QueueWidget::getInputMedia()
{
    //TODO build list
    return inputWidgets.at(0)->getMediaInfo();
}

QList<FFMediaInfo *> QueueWidget::getOutputMedia()
{
    QList<FFMediaInfo *> output;
    foreach(OutputWidget *ow,outputWidgets)
    {
        output << ow->getMediaInfo();
    }
    return output;
}

void QueueWidget::presetsPathChanged(QString path)
{
    foreach(OutputWidget *ow,outputWidgets)
    {
        ow->loadPresets(path);
    }
}

void QueueWidget::on_outputTab_tabCloseRequested(int index)
{
    int numTabs = outputTab->count();
    if (index == numTabs-1) return;
    if (numTabs <= 2) return;

    //remove tab
    outputTab->removeTab(index);
    //delete corresponding widget
    OutputWidget *w = outputWidgets.takeAt(index);
    delete w;
    //update current index
    if (outputTab->currentIndex() == outputTab->count()-1)
    {
        outputTab->setCurrentIndex(outputTab->count()-2);
    }
}

void QueueWidget::on_outputTab_tabBarClicked(int index)
{
    if (index != outputTab->count()-1) return;
    //add new
    addOutput();
}

void QueueWidget::addOutput()
{
    //number of the output
    int num = outputWidgets.count()+1;
    //create widget
    OutputWidget *ow = new OutputWidget(ffmpeg,num,this);
    outputWidgets << ow;
    outputTab->insertTab(outputTab->count()-1,ow,"Output " + QString::number(num));
    //connect first input widget
    InputWidget *iw = inputWidgets.at(0);
    connect(iw,SIGNAL(newMediaLoaded(FFMediaInfo*)),ow,SLOT(newInputMedia(FFMediaInfo*)));
    //set tab index
    outputTab->setCurrentIndex(outputTab->count()-2);
    //get current media info
    FFMediaInfo *iwInfo = iw->getMediaInfo();
    if (iwInfo->fileName() != "") ow->newInputMedia(iwInfo);
}

