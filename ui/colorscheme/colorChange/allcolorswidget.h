#ifndef ALLCOLORSWIDGET_H
#define ALLCOLORSWIDGET_H

#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QWidget>

#include "colorCircles/include/QtColorWidgets/color_preview.hpp"

using namespace color_widgets;

class AllColorsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AllColorsWidget(QWidget *parent);
    ~AllColorsWidget();
private:
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
Q_SIGNALS:
    void ToButtonClicked();

private Q_SLOTS:
    void OnColorButtonClicked(const int id);

public Q_SLOTS:
    void OnSetColorToButton(const QColor &color);

public:
    void ChangeColors(const QVector<QColor> &colors);
private:
    void SetToDefault(int count);
    void SetButtonsEnabled(int count);
    void SetButtonChecked(const int buttonId);
    void SetAllButtonUnchecked();
private:
    QVBoxLayout *m_mainLayout;
    QGroupBox *m_mainGroupBox;
    QHBoxLayout *m_colorsLayout;
    QList<ColorPreview*> m_buttonColorsList;
    int m_currentIdChecked;
};

#endif // ALLCOLORSWIDGET_H
