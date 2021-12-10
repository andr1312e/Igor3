#ifndef UI_DSP_WIDGET_TOPUSERPANEL_H
#define UI_DSP_WIDGET_TOPUSERPANEL_H

#include <QHBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSharedDataPointer>
#include <QFileDialog>

#include "structs/colorscheme/colorranges.h"

#include "presenter/dsppresenter.h"
#include "presenter/colorschemepresenter.h"

#include "ui/colorscheme/maincolorschemewidget.h"

class TopUserPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TopUserPanel(DspPresenter *presenter, QWidget *parent);
    ~TopUserPanel();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void RepaintComboBox();
    void ConnectObjects();
Q_SIGNALS:
    void ToChangeGradient(const ColorRanges &range);
    void ToSaveIntoGif();
private Q_SLOTS:
    void OnSelectFileButtonClicked();
    void OnGradrientUpdate();
    void OnComboBoxUpdated(int index);
public:
    void SetAdditionalInfo(const float &Sensor_Azm, const float &Sensor_Ugm);
private:
    void AddGradientToComboBox(const QString &gradientName, const QVector<QColor> &colors, const QVector<int> &ranges);
private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonsLayout;
    QGridLayout *m_labelsLayout;

    QPushButton *m_selectFileButton;
    QPushButton *m_openColorPanelButton;
    QPushButton *m_saveToGifButton;

    QLabel *m_label;

    QLabel *m_sensorAzmLabel;
    QLabel *m_sensorAzmValue;

    QLabel *m_sensorUgmLabel;
    QLabel *m_sensorUgmValue;

    QComboBox *m_gradientsComboBox;

    GradientColorChangerWidget *m_gradientColorChangerWidget;

private:
    DspPresenter *m_dspPresenter;
    QSharedPointer<ColorSchemePresenter> m_colorPresenter;
    const QSize m_pixmapSize=QSize(100,14);
};

#endif // UI_DSP_WIDGET_TOPUSERPANEL_H
