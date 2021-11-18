#ifndef SPECROINFOWIDGET_H
#define SPECROINFOWIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "structs/colorranges.h"

#include "presenter/colorscheme/colorschemepresenter.h"

#include "ui/colorscheme/rangeslider.h"
#include "ui/colorscheme/gradientlabel.h"

class GradientInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GradientInfoWidget(ColorSchemePresenter *presenter, QWidget *parent);
    ~GradientInfoWidget();
    void ChangeGradient(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
private:
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
Q_SIGNALS:
    void ToRepaintComboBox();
private Q_SLOTS:
    void OnSaveButtonClick();
    void OnComboBoxPointsCountChange(int indexComboBoxValue);
public Q_SLOTS:
    void OnAppendGradient();

private:
    const QVector<int> m_doubleHandlesVector={0, 50};
    const QVector<int> m_tripleHandlesVector={0, 33, 66};
    const QVector<int> m_quadrupleHandlesVector={0, 25, 50, 75};
    const QVector<QColor> m_doubleHandlesColorVector={Qt::red, Qt::yellow};
    const QVector<QColor> m_tripleHandlesColorVector={Qt::red, Qt::yellow, Qt::green};
    const QVector<QColor> m_quadrupleHandlesColorVector={Qt::red, Qt::yellow, Qt::green, Qt::blue};

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
    ColorSchemePresenter *m_presenter;

};

#endif // SPECROINFOWIDGET_H
