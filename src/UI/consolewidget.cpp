#include "consolewidget.h"
#include "ui_consolewidget.h"

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
    connectEvents();
    ffmpegValid(FFmpeg::instance()->isValid());
}

void ConsoleWidget::setupUi()
{
    QVBoxLayout *verticalLayout= new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(3,3,3,3);

    QTabWidget *consoleTabs = new QTabWidget(this);
    consoleTabs->setTabPosition(QTabWidget::East);
    consoleTabs->setCurrentIndex(0);

    verticalLayout->addWidget(consoleTabs);

    ui_debugEdit = new QTextEdit(this);
    ui_debugEdit->setReadOnly(true);
    ui_debugEdit->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

    consoleTabs->addTab(ui_debugEdit, "Log");

    QWidget *ffmpegTab = new QWidget();
    QVBoxLayout *ffmpegLayout = new QVBoxLayout(ffmpegTab);
    ffmpegLayout->setSpacing(3);
    ffmpegLayout->setContentsMargins(0, 0, 0, 3);

    ui_ffmpegEdit = new QTextBrowser(ffmpegTab);
    ffmpegLayout->addWidget(ui_ffmpegEdit);

    QHBoxLayout *ffmpegCmdsLayout = new QHBoxLayout();
    ffmpegCmdsLayout->setSpacing(3);
    ffmpegCmdsLayout->setContentsMargins(3, -1, 3, -1);
    ffmpegCmdsLayout->addWidget(new QLabel("ffmpeg", ffmpegTab));

    ui_ffmpegCommandsEdit = new QLineEdit(ffmpegTab);
    ui_ffmpegCommandsEdit->setClearButtonEnabled(true);
    ui_ffmpegCommandsEdit->setPlaceholderText("Input FFmpeg commands here... -h will print the basics");
    ffmpegCmdsLayout->addWidget(ui_ffmpegCommandsEdit);

    ui_ffmpegCommandsButton = new QPushButton(ffmpegTab);
    ui_ffmpegCommandsButton->setText("Execute!");
    ffmpegCmdsLayout->addWidget(ui_ffmpegCommandsButton);

    ffmpegLayout->addLayout(ffmpegCmdsLayout);

    consoleTabs->addTab(ffmpegTab, "FFmpeg console");

    ui_helpEdit = new QTextEdit(this);

    consoleTabs->addTab(ui_helpEdit, "FFmpeg help");

    QWidget *aeTab = new QWidget();
    QVBoxLayout *aeLayout = new QVBoxLayout(aeTab);
    aeLayout->setSpacing(3);
    aeLayout->setContentsMargins(0, 0, 0, 3);

    ui_aeConsoleEdit = new QTextBrowser(aeTab);
    ui_aeConsoleEdit->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt;\">Ready!</span></p></body></html>");

    aeLayout->addWidget(ui_aeConsoleEdit);

    QHBoxLayout *aeCmdsLayout = new QHBoxLayout();
    aeCmdsLayout->setSpacing(3);
    aeCmdsLayout->setContentsMargins(3, -1, 3, -1);

    QLabel *aeLabel = new QLabel(aeTab);
    aeLabel->setText("aerender");

    aeCmdsLayout->addWidget(aeLabel);

    ui_aeCommandsEdit = new QLineEdit(aeTab);
    ui_aeCommandsEdit->setClearButtonEnabled(true);
    ui_aeCommandsEdit->setPlaceholderText("Input After Effects commands here... -h will print the basics");

    aeCmdsLayout->addWidget(ui_aeCommandsEdit);

    ui_aeCommandsButton = new QPushButton(aeTab);
    ui_aeCommandsButton->setText("Execute!");

    aeCmdsLayout->addWidget(ui_aeCommandsButton);

    aeLayout->addLayout(aeCmdsLayout);

    consoleTabs->addTab(aeTab, "After Effects Console");//*/
}

void ConsoleWidget::connectEvents()
{
    connect(FFmpegRenderer::instance(), &AbstractRenderer::console, this, &ConsoleWidget::ffmpegConsole );
    connect(FFmpegRenderer::instance(), &AbstractRenderer::newLog, this, &ConsoleWidget::ffmpegLog );

    connect(AERenderer::instance(), &AbstractRenderer::console, this, &ConsoleWidget::aeConsole );
    connect(AERenderer::instance(), &AbstractRenderer::newLog, this, &ConsoleWidget::aeLog );

    connect( FFmpeg::instance(), SIGNAL( newLog(QString,DuQFLog::LogType) ),this,SLOT( ffmpegLog(QString,DuQFLog::LogType)) );
    connect( FFmpeg::instance(), SIGNAL( console(QString)), this, SLOT( ffmpegConsole(QString)) );
    connect(FFmpeg::instance(), SIGNAL(valid(bool)), this, SLOT(ffmpegValid(bool)));

    connect( AfterEffects::instance(), SIGNAL( newLog(QString,DuQFLog::LogType) ), this, SLOT( aeLog(QString,DuQFLog::LogType )) );
    connect( AfterEffects::instance(), SIGNAL( console(QString)), this, SLOT( aeConsole(QString)) );

    connect(ui_ffmpegCommandsButton, SIGNAL(clicked()), this, SLOT(runFFmpegCommand()));
    connect(ui_ffmpegCommandsEdit, SIGNAL(returnPressed()), this, SLOT(runFFmpegCommand()));

    connect(ui_aeCommandsButton, SIGNAL(clicked()), this, SLOT(runAeCommand()));
    connect(ui_aeCommandsEdit, SIGNAL(returnPressed()), this, SLOT(runAeCommand()));
}

void ConsoleWidget::log(QString log, DuQFLog::LogType type)
{
    //type
    QString typeString = "";
    if ( type == DuQFLog::Debug )
    {
        qDebug().noquote() << log;
    }
    else if ( type == DuQFLog::Information )
    {
        qInfo().noquote() << log;
    }
    else if (type == DuQFLog::Warning)
    {
        qWarning().noquote() << log;
        typeString = "/!\\ Warning: ";
    }
    else if (type == DuQFLog::Critical)
    {
        qCritical().noquote() << log;
        typeString = " --- !!! Critical: ";
    }
    else if (type == DuQFLog::Fatal)
    {
        qFatal("%s", qUtf8Printable(log));
        typeString = " === Fatal === ";
    }

    //log
#ifndef QT_DEBUG
    if ( type != DuQFLog::Debug )
    {
#endif
        //add date
        QTime currentTime = QTime::currentTime();
        ui_debugEdit->setTextColor(QColor(109,109,109));
        ui_debugEdit->setFontWeight(300);
        ui_debugEdit->append(currentTime.toString("[hh:mm:ss.zzz]: "));
        ui_debugEdit->moveCursor(QTextCursor::End);
        if (type == DuQFLog::Information) ui_debugEdit->setTextColor(QColor(227,227,227));
        else if (type == DuQFLog::Warning) ui_debugEdit->setTextColor(QColor(236,215,24));
        else if (type == DuQFLog::Critical) ui_debugEdit->setTextColor(QColor(249,105,105));
        ui_debugEdit->setFontWeight(800);
        ui_debugEdit->setFontItalic(true);
        ui_debugEdit->insertPlainText(typeString);
        ui_debugEdit->setFontWeight(400);
        ui_debugEdit->setFontItalic(false);
        ui_debugEdit->insertPlainText(log);
        //ui_debugEdit->verticalScrollBar()->setSliderPosition(ui_debugEdit->verticalScrollBar()->maximum());
#ifndef QT_DEBUG
    }
#endif
}

void ConsoleWidget::ffmpegConsole(QString c)
{
    //add date
    QTime currentTime = QTime::currentTime();

    //log
    ui_ffmpegEdit->setText(ui_ffmpegEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + c );
    ui_ffmpegEdit->verticalScrollBar()->setSliderPosition(ui_ffmpegEdit->verticalScrollBar()->maximum());
}

void ConsoleWidget::ffmpegLog(QString l, DuQFLog::LogType lt)
{
    log( "FFmpeg | " + l, lt);
}

void ConsoleWidget::aeLog(QString l, DuQFLog::LogType lt)
{
    AfterEffectsVersion *aev = AfterEffects::instance()->currentVersion();
    if (aev)
    {
        l = "After Effects " + AfterEffects::instance()->currentVersion()->name() + " | " + l;
    }
    else
    {
        l = "After Effects | " + l;
    }
    log(l, lt);
}

void ConsoleWidget::aeConsole(QString c)
{
    //add date
    QTime currentTime = QTime::currentTime();

    //log
    ui_aeConsoleEdit->setText(ui_aeConsoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + c );
    ui_aeConsoleEdit->verticalScrollBar()->setSliderPosition(ui_aeConsoleEdit->verticalScrollBar()->maximum());
}

void ConsoleWidget::ffmpegValid(bool valid)
{
    if (valid) ui_helpEdit->setText(FFmpeg::instance()->longHelp());
    else ui_helpEdit->setText("FFmpeg is not working correctly, sorry.");
}

void ConsoleWidget::runFFmpegCommand()
{
    QString commands = ui_ffmpegCommandsEdit->text();
    if (commands == "") commands = "-h";
    FFmpeg::instance()->runCommand(commands);
}

void ConsoleWidget::runAeCommand()
{
    QString commands = ui_aeCommandsEdit->text();
    if (commands == "") commands = "-h";
    AfterEffects::instance()->runCommand(commands);
}
