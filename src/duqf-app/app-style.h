#ifndef APPSTYLE_H
#define APPSTYLE_H

#include "app-version.h"

#include <QString>
#include <QStringList>
#include <QFontDatabase>
#include <QFileInfo>
#include <QApplication>
#include <QtDebug>
#include <QRegularExpression>
#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>

class DuUI
{
public:
    static DuUI *instance();

    static void updateCSS(QString cssFileName);

    /**
     * @brief loadCSS Loads a CSS File
     * If any cssFileName-values.val file is found, uses these values in the CSS
     * Note: in the values file name, the .val extension can be omitted, or replaced by .txt
     * @param cssFileName The file name (with complete path) of the CSS
     * @return the CSS string
     */
    static QString loadCSS(QString cssFileName = ":/styles/default");

    /**
     * @brief loadCSS Loads multiple CSS Files
     * If any cssFileName[0]-values.val file is found, uses these values in the CSS
     * Note: in the values file name, the .val extension can be omitted, or replaced by .txt
     * @param cssFileNames The file names (with complete path) of the CSS
     * @return the CSS string
     */
    static QString loadCSS(QStringList cssFileNames, QString styleValues = "");

    static void setFont(QString family = "Ubuntu", int size=10, int weight=400);

    static void setToolButtonStyle(int styleIndex);

    static void setToolButtonStyle(Qt::ToolButtonStyle style);

    static QColor getColor(QString colorName);

    static int getSize(QString type, QString name = "");

    void addCssValue(QStringList value);
    QList<QStringList> cssValues();
    void clearCssValues();

protected:
    static DuUI *_instance;

private:
    DuUI() {};
    QList<QStringList> _cssValues;
};


//seen at https://stackoverflow.com/questions/5752408/qt-resize-borderless-widget/37507341#37507341
//posted by https://stackoverflow.com/users/5446734/iman4k

#pragma once
#include <QtWidgets/QWidget>
#include <QtWidgets/QRubberBand>
#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QRect>
#include <QtCore/QPoint>
#include <QtCore/Qt>
#include <QtGui/QHoverEvent>
#include <QtGui/QMouseEvent>
#include <QMainWindow>

class FrameLessWindow : public QObject {
    Q_OBJECT

public:
    enum Edge {
        None = 0x0,
        Left = 0x1,
        Top = 0x2,
        Right = 0x4,
        Bottom = 0x8,
        TopLeft = 0x10,
        TopRight = 0x20,
        BottomLeft = 0x40,
        BottomRight = 0x80,
    };
    Q_ENUM(Edge)
    Q_DECLARE_FLAGS(Edges, Edge)

    FrameLessWindow(QMainWindow *target);

    void setBorderWidth(int w) {
        _borderWidth = w;
    }
    int borderWidth() const {
        return _borderWidth;
    }

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void mouseHover(QHoverEvent*);
    void mouseLeave();
    void mousePress(QMouseEvent*);
    void mouseRealese(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void updateCursorShape(const QPoint &);
    void calculateCursorPosition(const QPoint &, const QRect &, Edges &);

private:
    QWidget *_target = nullptr;
    QRubberBand *_rubberband = nullptr;
    bool _cursorchanged;
    bool _leftButtonPressed;
    Edges _mousePress = Edge::None;
    Edges _mouseMove = Edge::None;
    int _borderWidth;

    QPoint _dragPos;
    bool _dragStart = false;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FrameLessWindow::Edges)

#endif // APPSTYLE_H
