#include "cachesettingswidget.h"

CacheSettingsWidget::CacheSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _freezeUI = true;

    //Temp path
    QString cachePath = QDir::toNativeSeparators( CacheManager::instance()->getRootCacheDir().absolutePath() );
    cacheEdit->setText(cachePath);

    _freezeUI = false;
}

void CacheSettingsWidget::on_cacheEdit_textChanged(const QString &arg1)
{
    if (_freezeUI) return;
    bool frozen = _freezeUI;
    _freezeUI = true;
    QString path = arg1;
    if (!path.endsWith("/") || !path.endsWith("\\")) path = path + "/";
    path = QDir::toNativeSeparators(path);

    if (QDir(arg1).exists())
    {
        CacheManager::instance()->setRootCacheDir(path);
        cacheEdit->setText(path);
    }
    _freezeUI = frozen;
}

void CacheSettingsWidget::on_cacheBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the cache directory",CacheManager::instance()->getRootCacheDir().absolutePath());
    if (path == "") return;
    cacheEdit->setText(path);
}

void CacheSettingsWidget::on_openButton_clicked()
{
    FileUtils::openInExplorer(CacheManager::instance()->getRootCacheDir().absolutePath());
}
