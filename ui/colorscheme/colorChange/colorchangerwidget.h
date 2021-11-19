#ifndef UI_COLORSSCHEME_COLORCHANGE_COLORCHANGERWIDGET_H
#define UI_COLORSSCHEME_COLORCHANGE_COLORCHANGERWIDGET_H

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "colorCircles/include/QtColorWidgets/color_dialog.hpp"

#include "ui/colorscheme/basiccolors.h"
#include "ui/colorscheme/colorChange/allcolorswidget.h"

using namespace color_widgets;

class ColorChangerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorChangerWidget(QWidget *parent);
    ~ColorChangerWidget();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();

Q_SIGNALS:
    void ToUpdateColors(const QVector<QColor> &colors);
private Q_SLOTS:
    void OnColorChange(const QColor &color);
public Q_SLOTS:
    void OnAppendGradient();
    void OnChangeGradient(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
    void OnColorsCountChanged(const int colorCounts);
private:
    QVBoxLayout *m_mainLayout;

    QGroupBox *m_groupBox;
    AllColorsWidget *m_allColorListWidget;

    QVBoxLayout *m_groupBoxLayout;

    ColorDialog  *m_colorDialog;

};

#endif // UI_COLORSSCHEME_COLORCHANGE_COLORCHANGERWIDGET_H
