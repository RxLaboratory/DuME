#include "aesettingswidget.h"

AESettingsWidget::AESettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _ae = AfterEffects::instance();

    updateAe();

    connect(_ae, &AfterEffects::binaryChanged, this, &AESettingsWidget::updateAe);
    connect(aeVersionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(aeVersionBox_currentIndexChanged(int)));
    connect(aerenderPathEdit, SIGNAL(textChanged(QString)), this, SLOT(aerenderPathEdit_textChanged(QString)));
    connect(aerenderBrowseButton, SIGNAL(clicked()), this, SLOT(aerenderBrowseButton_clicked()));
}

void AESettingsWidget::aeVersionBox_currentIndexChanged(int index)
{
    QString name = aeVersionBox->itemText(index);
    _ae->setBinary( name );
}

void AESettingsWidget::aerenderPathEdit_textChanged(const QString &arg1)
{
    QSignalBlocker b1(aerenderPathEdit);

    QString path = QDir::toNativeSeparators(arg1);
    aerenderPathEdit->setText(path);

    for( int i = 0; i < aeVersionBox->count(); i++)
    {
        if (aeVersionBox->itemData(i).toString() == path )
        {
            aeVersionBox->setCurrentIndex(i);
            return;
        }
    }

    settings.setValue( "aerender/path", path );
    settings.sync();

    aeVersionBox->setCurrentText( "Custom" );
}

void AESettingsWidget::aerenderBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the aerender executable binary",settings.value("aerender/path","").toString());
    if (path == "") return;
    aerenderPathEdit->setText(path);
}

void AESettingsWidget::updateAe()
{
    //aerender path
    QString aerenderPath = _ae->binary();
    if (aerenderPath == "") aerenderPath = "Not Found! You may need to install Adobe After Effects.";
    aerenderPath = QDir::toNativeSeparators(aerenderPath);

    QSignalBlocker b1(aerenderPathEdit);
    QSignalBlocker b2(aeVersionBox);

    aerenderPathEdit->setText(aerenderPath);
    //ae versions
    refreshAeVersionBox();

    //select the right item
    if ( _ae->useLatest() ) aeVersionBox->setCurrentText( "Latest" );
    else aeVersionBox->setCurrentText( _ae->currentName() );

    aerenderPathEdit->setEnabled( _ae->currentName() == "Custom" );
    aerenderBrowseButton->setEnabled( _ae->currentName() == "Custom" );
}

void AESettingsWidget::refreshAeVersionBox()
{
    aeVersionBox->clear();

    foreach(AfterEffectsVersion *ae, _ae->versions())
    {
        aeVersionBox->addItem(ae->name(), ae->path());
    }
    aeVersionBox->addItem("Latest", "Latest" );
    aeVersionBox->addItem("Custom", _ae->binary() );
}
