#include "duqffolderselectorwidget.h"

DuQFFolderSelectorWidget::DuQFFolderSelectorWidget(SelectorType type, QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    m_type = type;
    m_dialogTitle = "";

    //if (type == File) ui_exploreButton->hide();

    connect(ui_browseButton, &QToolButton::clicked, this, &DuQFFolderSelectorWidget::browseButton_clicked);
    connect(ui_exploreButton, &QToolButton::clicked, this, &DuQFFolderSelectorWidget::exploreButton_clicked);
    connect(ui_deleteButton, &QToolButton::clicked, this, &DuQFFolderSelectorWidget::deleteButton_clicked);
    connect(ui_folderEdit, &QLineEdit::textChanged, this, &DuQFFolderSelectorWidget::pathChanging);
    connect(ui_folderEdit, &QLineEdit::editingFinished, this, &DuQFFolderSelectorWidget::folderEdit_editingFinished);
}

QString DuQFFolderSelectorWidget::path()
{
    return ui_folderEdit->text();
}

void DuQFFolderSelectorWidget::setPath(QString p)
{
    p = QDir::toNativeSeparators(p);
    ui_folderEdit->setText(p);
    if (p == "" ) p = ui_folderEdit->placeholderText();
    ui_folderEdit->setToolTip(p);
    ui_exploreButton->setToolTip("Reveal folder at " + p);
}

void DuQFFolderSelectorWidget::setPlaceHolderText(QString t)
{
    ui_folderEdit->setPlaceholderText(t);
    setPath(ui_folderEdit->text());
}

void DuQFFolderSelectorWidget::setDialogTitle(QString t)
{
    m_dialogTitle = t;
}

void DuQFFolderSelectorWidget::showDeleteButton(QString trashFolder, bool show)
{
    m_trashFolder = trashFolder;
    ui_deleteButton->setVisible(show);
}

void DuQFFolderSelectorWidget::browseButton_clicked()
{
    QString p = "";
    QString d = ui_folderEdit->text();
    if (d == "") d = ui_folderEdit->toolTip();
    if (d == "") d = ui_folderEdit->placeholderText();
    if (m_type == Folder) p = QFileDialog::getExistingDirectory(this, m_dialogTitle, ui_folderEdit->text());
    else p = QFileDialog::getOpenFileName(this, m_dialogTitle, ui_folderEdit->text());
    if (p != "")
    {
        setPath(p);
        emit pathChanged(p);
    }
}

void DuQFFolderSelectorWidget::exploreButton_clicked()
{
    QString path = ui_folderEdit->text();
    if (path == "" ) path = ui_folderEdit->placeholderText();
    if (m_type == Folder) FileUtils::openInExplorer( path, true );
    else FileUtils::openInExplorer( path, false );
}

void DuQFFolderSelectorWidget::folderEdit_editingFinished()
{
    setPath(ui_folderEdit->text());
    emit pathChanged(ui_folderEdit->text());
}

void DuQFFolderSelectorWidget::deleteButton_clicked()
{
    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm deletion",
        "Are you sure you want to move this file to the trash?" );

    if ( confirm != QMessageBox::Yes) return;

    QString origin = ui_folderEdit->text();
    if (origin == "") origin = ui_folderEdit->placeholderText();
    QFileInfo originInfo(origin);
    if(!originInfo.exists()) return;

    // First, try to move to trash
    if( m_trashFolder != "" )
    {
        // Move any already existing file in trash
        QDir originDir = originInfo.dir();
        QString destination = originDir.absolutePath() + "/" + m_trashFolder + "/" + originInfo.fileName();
        originDir.mkdir(m_trashFolder);
        FileUtils::move(origin, destination, true);
    }
    else
    {
        FileUtils::moveToTrash(origin);
    }
    emit fileRemoved();
}

void DuQFFolderSelectorWidget::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->setObjectName(QStringLiteral("DuQFFolderSelectorWidget"));
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(3);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    ui_folderEdit = new QLineEdit(this);

    horizontalLayout->addWidget(ui_folderEdit);

    ui_deleteButton = new QToolButton(this);
    ui_deleteButton->setIcon(QIcon(":/icons/remove"));
    ui_deleteButton->setVisible(false);

    horizontalLayout->addWidget(ui_deleteButton);

    ui_browseButton = new QToolButton(this);
    if (m_type == Folder) ui_browseButton->setIcon(QIcon(":/icons/folder"));
    else ui_browseButton->setIcon(QIcon(":/icons/file"));

    horizontalLayout->addWidget(ui_browseButton);

    ui_exploreButton = new QToolButton(this);
    ui_exploreButton->setObjectName(QStringLiteral("exploreButton"));
    ui_exploreButton->setIcon(QIcon(":/icons/browse-folder"));

    horizontalLayout->addWidget(ui_exploreButton);
}

