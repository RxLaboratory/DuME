#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    stackedWidget->setCurrentIndex(1);

    //Load FFMpeg path
    //TODO in settings
    ffmpegPath = "E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe";
    ffmpeg = new QProcess(this);
    ffmpeg->setProgram(ffmpegPath);

    //connect ffmpeg
    connect(ffmpeg,SIGNAL(readyReadStandardError()),this,SLOT(ffmpeg_stdError()));
    connect(ffmpeg,SIGNAL(readyReadStandardOutput()),this,SLOT(ffmpeg_stdOutput()));
    connect(ffmpeg,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(ffmpeg_errorOccurred(QProcess::ProcessError)));

    //get codec list
    ffmpeg->setArguments(QStringList("-codecs"));
    ffmpeg->start(QIODevice::ReadOnly);

    stackedWidget->setCurrentIndex(0);
}

void MainWindow::ffmpeg_stdError()
{
    QString output = ffmpeg->readAllStandardError();
    ffmpeg_readyRead(output);
}

void MainWindow::ffmpeg_stdOutput()
{
    QString output = ffmpeg->readAllStandardOutput();
    ffmpeg_readyRead(output);
}

void MainWindow::ffmpeg_readyRead(QString ffmpegOutput)
{
    //read
    qDebug() << ffmpegOutput;

    //got codecs list
    if (ffmpegOutput.startsWith("Codecs:"))
    {
        videoCodecsBox->clear();
        audioCodecsBox->clear();
        //get codecs
        QStringList codecs = ffmpegOutput.split("\n");
        for (int i = 10 ; i < codecs.count() ; i++)
        {
            QString codec = codecs[i];
            //if video encoding
            QRegExp reVideo("[\\.D]EV[\\.I][\\.L][\\.S] ([\\S]+)\\s+([^\\(\\n]+)");
            if (reVideo.indexIn(codec) != -1)
            {
                QString codecName = reVideo.cap(1);
                QString codecPrettyName = reVideo.cap(2);
                videoCodecsBox->addItem(codecPrettyName.trimmed(),QVariant(codecName));
            }
            //if audio encoding
            QRegExp reAudio("[\\.D]EA[\\.I][\\.L][\\.S] ([\\S]+)\\s+([^\\(\\n]+)");
            if (reAudio.indexIn(codec) != -1)
            {
                QString codecName = reAudio.cap(1);
                QString codecPrettyName = reAudio.cap(2);
                audioCodecsBox->addItem(codecPrettyName.trimmed(),QVariant(codecName));
            }
        }
        ffmpeg->close();
    }
    //encoding
    else if (ffmpegOutput.startsWith("frame="))
    {
        console(ffmpegOutput);
    }
}

void MainWindow::ffmpeg_errorOccurred(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start FFMpeg.";
    }
    else if (e == QProcess::Crashed)
    {
        error = "FFMpeg just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "Operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "Write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read FFMpeg output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown error occured.";
    }
    console(error);
    qDebug() << error;
}

void MainWindow::console(QString log)
{
    consoleEdit->setText(consoleEdit->toPlainText() + "\n" + log);
    // put the slider at the bottom
    consoleEdit->verticalScrollBar()->setSliderPosition(consoleEdit->verticalScrollBar()->maximum());
}

void MainWindow::on_videoTranscodeButton_toggled(bool checked)
{
    videoTranscodeWidget->setEnabled(checked);
}

void MainWindow::on_audioTranscodeButton_toggled(bool checked)
{
    audioTranscodeWidget->setEnabled(checked);
}

void MainWindow::on_resizeButton_toggled(bool checked)
{
    videoWidthButton->setEnabled(checked);
    videoHeightButton->setEnabled(checked);
    widthLabel->setEnabled(checked);
    heightLabel->setEnabled(checked);
    aspectLabel->setEnabled(checked);
}

void MainWindow::on_frameRateButton_toggled(bool checked)
{
    frameRateBox->setEnabled(checked);
    frameRateEdit->setEnabled(checked);
}

void MainWindow::on_samplingButton_toggled(bool checked)
{
    samplingBox->setEnabled(checked);
}

void MainWindow::on_channelsButton_toggled(bool checked)
{
    channelsBox->setEnabled(checked);
}

void MainWindow::on_inputBrowseButton_clicked()
{
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode");
    if (inputPath == "") return;
    inputEdit->setText(inputPath);
    toolBox->setItemText(0,QFileInfo(inputPath).fileName());
    isReady();
}

void MainWindow::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file");
    if (outputPath == "") return;
    outputEdit->setText(outputPath);
    isReady();
}

void MainWindow::on_inputEdit_textChanged(const QString &arg1)
{
    isReady();
}

void MainWindow::on_outputEdit_textChanged(const QString &arg1)
{
    isReady();
}

bool MainWindow::isReady()
{
    if (!QFile(inputEdit->text()).exists())
    {
        actionGo->setEnabled(false);
        //TODO add warning icon
        return false;
    }
    if (outputEdit->text() == "")
    {
        actionGo->setEnabled(false);
        //TODO add warning icon
        return false;
    }
    actionGo->setEnabled(true);
    return true;
}

void MainWindow::on_actionGo_triggered()
{
    QStringList arguments("-stats");
    //inFile
    arguments << "-i" << inputEdit->text().replace("/","\\");
    //out options
    if (noVideoButton->isChecked())
    {
        arguments << "-vn";
    }
    else if (videoCopyButton->isChecked())
    {
        arguments << "-vcodec" << "copy";
    }
    else
    {
        //resize
        if (resizeButton->isChecked())
        {
            arguments << "-s" << QString::number(videoWidthButton->value()) + "x" + QString::number(videoHeightButton->value()) ;
        }
        if (frameRateButton->isChecked())
        {
            arguments << "-r" << QString::number(frameRateEdit->value());
        }
        arguments << "-vcodec" << videoCodecsBox->currentData().toString();
        arguments << "-b:v" << QString::number(videoBitRateEdit->value()*1000000);
        //TODO Passes
    }

    if (noAudioButton->isChecked())
    {
        arguments << "-an";
    }
    else if (audioCopyButton->isChecked())
    {
        arguments << "-acodec" << "copy";
    }
    else
    {
        if (samplingButton->isChecked())
        {
            //TODO set sampling rate
            arguments << "-ar" << "48000";
        }
        if (channelsButton->isChecked())
        {
            //TODO set channels
            arguments << "-ac" << "2";
        }
        arguments << "-acodec" << audioCodecsBox->currentData().toString();
        arguments << "-b:a" << QString::number(audioBitRateEdit->value()*1000);
    }

    //output file
    arguments << outputEdit->text().replace("/","\\");

    //Launch!

    //ffmpeg->setArguments(QStringList("-h"));

    //QProcess::execute(ffmpegPath,arguments);
    //TODO use ffmpeg start instead
    //try startdetached
    //try waitForStarted
    //try not changing arguments (codecs)
    //or try simple -h for help
    ffmpeg->setArguments(arguments);
    ffmpeg->start(QIODevice::ReadWrite);
    ffmpeg->waitForStarted();
    stackedWidget->setCurrentIndex(1);
}
