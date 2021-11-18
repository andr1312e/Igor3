#ifndef PRESETCOLORWIDGET_H
#define PRESETCOLORWIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QPainter>

#include "presenter/colorscheme/colorschemepresenter.h"

class AllGradientsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AllGradientsWidget(ColorSchemePresenter *presenter,const QMargins &margin, QWidget *parent);
    ~AllGradientsWidget();
private:
    void CreateUI(const QMargins &margin);
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void InitComboBoxValues();
    void AddGradientToComboBox(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
    void ConnectObjects();

Q_SIGNALS:
    void ToAppendGradient();
    void ToCurrentGradientChanged(const QString &gradientName);
public Q_SLOTS:
    void OnRepaintComboBox();
private Q_SLOTS:
    void OnDeleteScheme();
    void OnComboBoxItemChangedByUser(int index);

private:
    QVBoxLayout *m_mainLayout;

    QVBoxLayout *m_savedPreSetLayout;
    QLabel *m_savedPreSetLabel;
    QComboBox *m_preSetComboBox;

    QHBoxLayout *m_buttonsLayout;
    QPushButton *m_addButton;
    QPushButton *m_delButton;

private:
    ColorSchemePresenter *m_presenter;

    const QSize m_pixmapSize=QSize(100,14);


};

#endif // PRESETCOLORWIDGET_H
