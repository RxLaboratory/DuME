#include "queuewidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

QueueWidget::QueueWidget(QWidget *parent) :
    QWidget(parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Queue Widget";
#endif
    setupUi(this);

    _job = new QueueItem();

    _inputMedias = _job->inputMedias();
    _outputMedias = _job->outputMedias();

    addOutput();

    //hide close buttons on add buttons
    QTabBar *outputTabBar = outputTab->tabBar();
    QWidget *closeButton = outputTabBar->tabButton(1, QTabBar::RightSide);
    if (closeButton == nullptr) closeButton = outputTabBar->tabButton(1, QTabBar::LeftSide);
    if (closeButton != nullptr) closeButton->hide();

    QTabBar *inputTabBar = inputTab->tabBar();
    closeButton = inputTabBar->tabButton(0, QTabBar::RightSide);
    if (closeButton == nullptr) closeButton = inputTabBar->tabButton(1, QTabBar::LeftSide);
    if (closeButton != nullptr) closeButton->hide();

    // splitter sizes
    QSettings settings;
    settings.beginGroup("queueSplitter");
    QList<int>sizes;
    sizes << settings.value("inputSize",50).toInt();
    sizes << settings.value("outputSize",50).toInt();
    splitter->setSizes(sizes);
    settings.endGroup();
    qDebug() << "Queue Widget created";
}

QueueItem *QueueWidget::job()
{
    return _job;
}

QList<MediaInfo *> QueueWidget::getInputMedia()
{
    return _inputMedias->medias();
}

MediaList *QueueWidget::inputMedias()
{
    return _inputMedias;
}

QList<MediaInfo *> QueueWidget::getOutputMedia()
{
    return _outputMedias->medias();
}

MediaList *QueueWidget::outputMedias()
{
    return _outputMedias;
}

void QueueWidget::openInputFile(QString file)
{
    if (inputWidgets.count() == 0)
    {
        addInput();
    }
    inputWidgets[ inputTab->currentIndex() ]->openFile(file);
}

void QueueWidget::openInputFile(QUrl file)
{
    if (inputWidgets.count() == 0)
    {
        addInput();
    }
    inputWidgets[ inputTab->currentIndex() ]->openFile(file);
}

MediaInfo *QueueWidget::addInputFile(QString file)
{
    addInput();
    InputWidget *iw = inputWidgets[ inputTab->currentIndex() ];
    iw->openFile(file);
    return iw->mediaInfo();
}

void QueueWidget::presetsPathChanged()
{
    foreach(OutputWidget *ow,outputWidgets)
    {
        ow->loadPresets();
    }
}

void QueueWidget::setOutputPath(QString outputPath, int outputIndex)
{
    if (outputIndex >= 0 && outputIndex < outputWidgets.count())
    {
        outputWidgets[outputIndex]->setOutputPath( outputPath );
    }
}

void QueueWidget::setOutputPreset(QString preset, int outputIndex)
{
    if (outputIndex >= 0 && outputIndex < outputWidgets.count())
    {
        outputWidgets[outputIndex]->setOutputPreset( preset );
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
    w->deleteLater();
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

void QueueWidget::on_inputTab_tabCloseRequested(int index)
{
    int numTabs = inputTab->count();
    if (index == numTabs-1) return;
    if (numTabs <= 2) return;

    //remove tab
    inputTab->removeTab(index);
    //delete corresponding widget and media
    InputWidget *w = inputWidgets.takeAt(index);
    _inputMedias->removeMedia( w->mediaInfo() );
    w->deleteLater();
    //update current index
    if (inputTab->currentIndex() == inputTab->count()-1)
    {
        inputTab->setCurrentIndex(inputTab->count()-2);
    }
}

void QueueWidget::on_inputTab_tabBarClicked(int index)
{
    if (index != inputTab->count()-1) return;
    //add new
    addInput();
}

void QueueWidget::log(QString log, DuQFLog::LogType lt )
{
    emit newLog(log, lt);
}

void QueueWidget::addInput()
{
    if ( !FFmpeg::instance()->isValid() ) return;

    int num = inputWidgets.count();

    //create widget
    InputWidget *inputWidget = new InputWidget(num, this);
    inputWidgets << inputWidget;
    inputTab->insertTab( inputTab->count()-1, inputWidget, "Input " + QString::number(num + 1));

    //add media
    _inputMedias->addMedia( inputWidget->mediaInfo() );

    //set tab index
    inputTab->setCurrentIndex(inputTab->count()-2);

    //re-set toolbutton style
    int styleIndex = QSettings().value("appearance/toolButtonStyle", 2).toInt();
    DuUI::setToolButtonStyle(styleIndex);
}

void QueueWidget::addOutput()
{
    qDebug() << "Add output widget";
    if ( !FFmpeg::instance()->isValid() ) return;

    //number of the output
    int num = outputWidgets.count()+1;

    //create widget
    OutputWidget *ow = new OutputWidget(num, _inputMedias, this);
    outputWidgets << ow;
    outputTab->insertTab(outputTab->count()-1,ow,"Output " + QString::number(num));

    //add media
    _outputMedias->addMedia(ow->mediaInfo());

    //connect console
    connect(ow,SIGNAL(newLog(QString,DuQFLog::LogType)),this,SLOT(log(QString,DuQFLog::LogType)));
    //set tab index
    outputTab->setCurrentIndex(outputTab->count()-2);
    qDebug() << "Output widget added";

    //re-set toolbutton style
    int styleIndex = QSettings().value("appearance/toolButtonStyle", 2).toInt();
    DuUI::setToolButtonStyle(styleIndex);
}

void QueueWidget::on_splitter_splitterMoved()
{
    QSettings settings;
    settings.beginGroup("queueSplitter");
    settings.setValue("inputSize", splitter->sizes()[0]);
    settings.setValue("outputSize", splitter->sizes()[1]);
    settings.endGroup();
}
