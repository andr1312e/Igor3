#ifndef UI_COLORSSCHEME_PRESETCOLORWIDGET_H
#define UI_COLORSSCHEME_PRESETCOLORWIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QPainter>

#include "presenter/colorschemepresenter.h"

class AllGradientsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AllGradientsWidget(QSharedPointer<ColorSchemePresenter> presenter, QWidget *parent);
    ~AllGradientsWidget();
private:
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void InitComboBoxValues();
    void AddGradientToComboBox(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
    void ConnectObjects();

Q_SIGNALS:
    void ToAppendGradient();
    void ToChangeGradient(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
public Q_SLOTS:
    void OnUpdateGradient();
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
    const QMargins m_margin;

private:
    QSharedPointer<ColorSchemePresenter> m_presenter;

    const QSize m_pixmapSize=QSize(100,14);


};

#endif // UI_COLORSSCHEME_PRESETCOLORWIDGET_H
