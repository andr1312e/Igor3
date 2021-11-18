#ifndef COLORCHANGERWIDGET_H
#define COLORCHANGERWIDGET_H

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "colorCircles/include/QtColorWidgets/color_dialog.hpp"

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
    void ToUpdateColors(QVector<QColor> &colors);
public:
    void ChangeColors(const QVector<QColor> &colors);
private:
    QVBoxLayout *m_mainLayout;

    QGroupBox *m_groupBox;
    AllColorsWidget *m_allColorListWidget;

    QVBoxLayout *m_groupBoxLayout;

    ColorDialog  *m_colorDialog;
private:
    QVector<QColor> m_colors;
    const int m_maxColorCount;

};

#endif // COLORCHANGERWIDGET_H
