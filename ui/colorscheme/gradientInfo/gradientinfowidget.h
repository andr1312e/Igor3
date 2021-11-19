#ifndef UI_COLORSSCHEME_GRADIENTINFO_SPECROINFOWIDGET_H
#define UI_COLORSSCHEME_GRADIENTINFO_SPECROINFOWIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "structs/colorscheme/colorranges.h"

#include "presenter/colorscheme/colorschemepresenter.h"

#include "ui/colorscheme/gradientInfo/rangeslider.h"
#include "ui/colorscheme/gradientInfo/gradientlabel.h"

class GradientInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GradientInfoWidget(QSharedPointer<ColorSchemePresenter> presenter, QWidget *parent);
    ~GradientInfoWidget();
private:
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
Q_SIGNALS:
    void ToColorsCountChanged(const int colorsCount);
    void ToUpdateGradient();
private Q_SLOTS:
    void OnSaveButtonClick();
    void OnComboBoxPointsCountChange(int indexComboBoxValue);
public Q_SLOTS:
    void OnAppendGradient();
    void OnChangeGradient(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
    void OnUpdateColors(const QVector<QColor> &colors);

private:

    QVBoxLayout *m_mainLayout;
    QGroupBox *m_mainGroupBox;

    QVBoxLayout *m_widgetsLayout;
    QLabel *m_colorSchemeNameLabel;
    QLineEdit  *m_schemeNameLineEdit;

    QLabel *m_colorNumberLabel;
    QComboBox *m_numberOfPointsComboBox;

    QLabel *m_rangesLabel;
    RangeSlider *m_rangesSlider;

    QLabel *m_resultGradientLabel;
    GradientLabel *m_gradientLabel;

    QLabel *m_actionLabel;
    QPushButton *m_saveButton;

private:
    QSharedPointer<ColorSchemePresenter> m_presenter;

};

#endif // UI_COLORSSCHEME_GRADIENTINFO_SPECROINFOWIDGET_H
