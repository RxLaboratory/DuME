#include "duqf-app/app-style.h"

FrameLessWindow::FrameLessWindow(QMainWindow *target) :
    _target(target),
    _cursorchanged(false),
    _leftButtonPressed(false),
    _borderWidth(10),
    _dragPos(QPoint())
{
    _target->setMouseTracking(true);
    _target->setWindowFlags(Qt::FramelessWindowHint);
    _target->setAttribute(Qt::WA_Hover);
    _target->installEventFilter(this);
    _rubberband = new QRubberBand(QRubberBand::Rectangle);
}

bool FrameLessWindow::eventFilter(QObject *o, QEvent*e) {
    if (e->type() == QEvent::MouseMove)
    {
        mouseMove(static_cast<QMouseEvent*>(e));
        return true;
    }
    else if (e->type() == QEvent::HoverMove)
    {
        mouseHover(static_cast<QHoverEvent*>(e));
        return true;
    }
    else if (e->type() == QEvent::Leave)
    {
        mouseLeave();
        return true;
    }
    else if (e->type() == QEvent::MouseButtonPress)
    {
        mousePress(static_cast<QMouseEvent*>(e));
        return true;
    }
    else if (e->type() == QEvent::MouseButtonRelease)
    {
        mouseRealese(static_cast<QMouseEvent*>(e));
        return true;
    }
    else {
        return _target->eventFilter(o, e);
    }
}

void FrameLessWindow::mouseHover(QHoverEvent *e) {
    updateCursorShape(_target->mapToGlobal(e->pos()));
}

void FrameLessWindow::mouseLeave() {
    if (!_leftButtonPressed) {
        _target->unsetCursor();
    }
}

void FrameLessWindow::mousePress(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        _leftButtonPressed = true;
        calculateCursorPosition(e->globalPos(), _target->frameGeometry(), _mousePress);
        if (!_mousePress.testFlag(Edge::None)) {
            _rubberband->setGeometry(_target->frameGeometry());
        }
        if (_target->rect().marginsRemoved(QMargins(borderWidth(), borderWidth(), borderWidth(), borderWidth())).contains(e->pos())) {
            _dragStart = true;
            _dragPos = e->pos();
        }
    }
}

void FrameLessWindow::mouseRealese(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        _leftButtonPressed = false;
        _dragStart = false;
    }
}

void FrameLessWindow::mouseMove(QMouseEvent *e) {
    if (_leftButtonPressed) {
        if (_dragStart) {
            _target->setCursor(Qt::SizeAllCursor);
            _target->move(e->globalPos() - _dragPos);
        }

        if (!_mousePress.testFlag(Edge::None)) {
            int left = _rubberband->frameGeometry().left();
            int top = _rubberband->frameGeometry().top();
            int right = _rubberband->frameGeometry().right();
            int bottom = _rubberband->frameGeometry().bottom();
            switch (_mousePress) {
            case Edge::Top:
                top = e->globalPos().y();
                break;
            case Edge::Bottom:
                bottom = e->globalPos().y();
                break;
            case Edge::Left:
                left = e->globalPos().x();
                break;
            case Edge::Right:
                right = e->globalPos().x();
                break;
            case Edge::TopLeft:
                top = e->globalPos().y();
                left = e->globalPos().x();
                break;
            case Edge::TopRight:
                right = e->globalPos().x();
                top = e->globalPos().y();
                break;
            case Edge::BottomLeft:
                bottom = e->globalPos().y();
                left = e->globalPos().x();
                break;
            case Edge::BottomRight:
                bottom = e->globalPos().y();
                right = e->globalPos().x();
                break;
            }
            QRect newRect(QPoint(left, top), QPoint(right, bottom));
            if (newRect.width() < _target->minimumWidth()) {
                left = _target->frameGeometry().x();
            }
            else if (newRect.height() < _target->minimumHeight()) {
                top = _target->frameGeometry().y();
            }
            _target->setGeometry(QRect(QPoint(left, top), QPoint(right, bottom)));
            _rubberband->setGeometry(QRect(QPoint(left, top), QPoint(right, bottom)));
        }
    }
    else {
        updateCursorShape(e->globalPos());
    }
}

void FrameLessWindow::updateCursorShape(const QPoint &pos) {
    if (_target->isFullScreen() || _target->isMaximized()) {
        if (_cursorchanged) {
            _target->unsetCursor();
        }
        return;
    }
    if (!_leftButtonPressed) {
        calculateCursorPosition(pos, _target->frameGeometry(), _mouseMove);
        _cursorchanged = true;
        if (_mouseMove.testFlag(Edge::Top) || _mouseMove.testFlag(Edge::Bottom)) {
            _target->setCursor(Qt::SizeVerCursor);
        }
        else if (_mouseMove.testFlag(Edge::Left) || _mouseMove.testFlag(Edge::Right)) {
            _target->setCursor(Qt::SizeHorCursor);
        }
        else if (_mouseMove.testFlag(Edge::TopLeft) || _mouseMove.testFlag(Edge::BottomRight)) {
            _target->setCursor(Qt::SizeFDiagCursor);
        }
        else if (_mouseMove.testFlag(Edge::TopRight) || _mouseMove.testFlag(Edge::BottomLeft)) {
            _target->setCursor(Qt::SizeBDiagCursor);
        }
        else if (_cursorchanged) {
            _target->unsetCursor();
            _cursorchanged = false;
        }
    }
}

void FrameLessWindow::calculateCursorPosition(const QPoint &pos, const QRect &framerect, Edges &_edge) {
    bool onLeft = pos.x() >= framerect.x() - _borderWidth && pos.x() <= framerect.x() + _borderWidth &&
        pos.y() <= framerect.y() + framerect.height() - _borderWidth && pos.y() >= framerect.y() + _borderWidth;

    bool onRight = pos.x() >= framerect.x() + framerect.width() - _borderWidth && pos.x() <= framerect.x() + framerect.width() &&
        pos.y() >= framerect.y() + _borderWidth && pos.y() <= framerect.y() + framerect.height() - _borderWidth;

    bool onBottom = pos.x() >= framerect.x() + _borderWidth && pos.x() <= framerect.x() + framerect.width() - _borderWidth  &&
        pos.y() >= framerect.y() + framerect.height() - _borderWidth && pos.y() <= framerect.y() + framerect.height();

    bool onTop = pos.x() >= framerect.x() + _borderWidth && pos.x() <= framerect.x() + framerect.width() - _borderWidth &&
        pos.y() >= framerect.y() && pos.y() <= framerect.y() + _borderWidth;

    bool  onBottomLeft = pos.x() <= framerect.x() + _borderWidth && pos.x() >= framerect.x() &&
        pos.y() <= framerect.y() + framerect.height() && pos.y() >= framerect.y() + framerect.height() - _borderWidth;

    bool onBottomRight = pos.x() >= framerect.x() + framerect.width() - _borderWidth && pos.x() <= framerect.x() + framerect.width() &&
        pos.y() >= framerect.y() + framerect.height() - _borderWidth && pos.y() <= framerect.y() + framerect.height();

    bool onTopRight = pos.x() >= framerect.x() + framerect.width() - _borderWidth && pos.x() <= framerect.x() + framerect.width() &&
        pos.y() >= framerect.y() && pos.y() <= framerect.y() + _borderWidth;

    bool onTopLeft = pos.x() >= framerect.x() && pos.x() <= framerect.x() + _borderWidth &&
        pos.y() >= framerect.y() && pos.y() <= framerect.y() + _borderWidth;

    if (onLeft) {
        _edge = Left;
    }
    else if (onRight) {
        _edge = Right;
    }
    else if (onBottom) {
        _edge = Bottom;
    }
    else if (onTop) {
        _edge = Top;
    }
    else if (onBottomLeft) {
        _edge = BottomLeft;
    }
    else if (onBottomRight) {
        _edge = BottomRight;
    }
    else if (onTopRight) {
        _edge = TopRight;
    }
    else if (onTopLeft) {
        _edge = TopLeft;
    }
    else {
        _edge = None;
    }
}

DuUI *DuUI::_instance = nullptr;

DuUI *DuUI::instance() {
    if (!_instance) _instance = new DuUI();
    return _instance;
}

void DuUI::updateCSS(QString cssFileName)
{
    qApp->setStyleSheet("");
    QStringList cssFiles(cssFileName);
    //check if there's a dume file to include
    QFileInfo cssFileInfo(cssFileName);
    QString includeName = cssFileInfo.completeBaseName() + "-" + QString(STR_INTERNALNAME).toLower();
    QString includePath = cssFileInfo.path() + "/" + includeName + ".css";
    QFile includeFile(includePath);
    includePath = cssFileInfo.path() + "/" + includeName;
    if (!includeFile.exists()) includeFile.setFileName(includePath);
    if (includeFile.exists())
    {
        cssFiles << includePath;
    }
    QString css = DuUI::loadCSS(cssFiles);
    qApp->setStyleSheet(css);
}

QString DuUI::loadCSS(QString cssFileName)
{
    return loadCSS(QStringList(cssFileName));
}

QString DuUI::loadCSS(QStringList cssFileNames, QString styleValues)
{
    QString css = "";

    //Build a single CSS with the files
    foreach(QString file,cssFileNames)
    {
        QFile cssFile(file);
        if (cssFile.exists())
        {
            if (cssFile.open(QFile::ReadOnly))
            {
                css += QString(cssFile.readAll());
                cssFile.close();
            }
        }
    }

    //replace variables
    //find values
    QFileInfo cssInfo(cssFileNames[0]);
    if (styleValues =="")
    {
        styleValues = cssInfo.path() + "/" + cssInfo.completeBaseName() + "-values";
    }
    QFile valuesFile(styleValues);
    if (valuesFile.exists())
    {
        if (valuesFile.open(QFile::ReadOnly))
        {
            DuUI::instance()->clearCssValues();
            css += "\n";
            //read lines
            while(!valuesFile.atEnd())
            {
                QString line = valuesFile.readLine();

                //get value and name
                QRegularExpression re("(@(\\w+)(?:-(\\w+(?:-\\w+)*))?) *= *(\\S+)");
                QRegularExpressionMatch match = re.match(line);
                if (match.hasMatch())
                {
                    QString type = match.captured(2);
                    QString typeName = match.captured(3);
                    QString name = match.captured(1);
                    QString value = match.captured(4);
                    css.replace(name,value);
                    QStringList cssValue;
                    cssValue << type << typeName << value;
                    DuUI::instance()->addCssValue(cssValue);
                }
            }
        }
    }

#ifdef QT_DEBUG
    qDebug() << "DuUI: CSS Ready";
#endif

    return css;
}

void DuUI::setFont(QString family, int size, int weight)
{
    Q_UNUSED(size);
    Q_UNUSED(weight);
    if (family == "Ubuntu")
    {
        //add fonts
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-B");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-BI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-C");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-L");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-LI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-M");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-MI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-R");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-RI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-Th");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-B");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-BI");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-R");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-RI");
    }

    qApp->setFont(QFont(family),"QWidget");
}

void DuUI::setToolButtonStyle(int styleIndex)
{
    Qt::ToolButtonStyle toolStyle = Qt::ToolButtonTextUnderIcon;
    if (styleIndex == 0) toolStyle = Qt::ToolButtonIconOnly;
    else if (styleIndex == 1) toolStyle = Qt::ToolButtonTextOnly;
    else if (styleIndex == 2) toolStyle = Qt::ToolButtonTextUnderIcon;
    else if (styleIndex == 3) toolStyle = Qt::ToolButtonTextBesideIcon;
    DuUI::setToolButtonStyle(toolStyle);
}

void DuUI::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    foreach( QWidget *w, qApp->allWidgets())
    {
        if(QMainWindow *mw = qobject_cast<QMainWindow*>(w))
        {
            mw->setToolButtonStyle(style);
        }
        else if (QToolBar *tb = qobject_cast<QToolBar*>(w))
        {
            tb->setToolButtonStyle(style);
        }
        else if (QToolButton *tb = qobject_cast<QToolButton*>(w))
        {
            if (tb->objectName() == "windowButton") continue;
            if (tb->objectName() == "menuButton") continue;
            if (tb->text() == "") continue;
            if (tb->icon().isNull()) continue;
            tb->setToolButtonStyle(style);
        }
    }
}

QColor DuUI::getColor(QString colorName)
{
    foreach(QStringList cssValue, DuUI::instance()->cssValues())
    {
        if (cssValue[0] == "color" && cssValue[1] == colorName) return QColor(cssValue[2]);
    }
    return QColor();
}

int DuUI::getSize(QString type, QString name)
{
    foreach(QStringList cssValue, DuUI::instance()->cssValues())
    {
        if (cssValue[0] == type && cssValue[1] == name) return cssValue[2].replace("px", "").toInt();
    }
    return 0;
}

void DuUI::addCssValue(QStringList value) {
    _cssValues << value;
}

QList<QStringList> DuUI::cssValues()
{
    return _cssValues;
}

void DuUI::clearCssValues()
{
    _cssValues.clear();
}
