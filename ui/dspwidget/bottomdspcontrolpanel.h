#ifndef BOTTOMDSPCONTROLPANEL_H
#define BOTTOMDSPCONTROLPANEL_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QWidget>

class BottomDspControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit BottomDspControlPanel(QWidget *parent);
    ~BottomDspControlPanel();
private:
    void CreateObjects();
    void CreateUI();
    void InsertWidgetsIntoLayouts();
    void FillUI();
    void ConnectObjects();
Q_SIGNALS:
    void ToStopUpdatePlot();
    void ToRequestDSPData(int sliderPosition);
public Q_SLOTS:
    void OnSetSliderLimit(quint32 counter);
    void OnStartTimer();
private Q_SLOTS:
    void OnStartStopClicked();
    void OnSliderOffset();
    void OnTimerTimeout();
    void OnComboBoxTextChanged(const QString &text);
    void OnFrameMinusButtonClicked();
    void OnFramePlusButtonClicked();
    void OnSliderValueChanged(int value);
private:
    QHBoxLayout *m_mainLayout;
    QLabel *m_frameLabel;
    QLabel *m_frameValue;
    QPushButton *m_frameMinusButton;
    QPushButton *m_startStopButton;
    QPushButton *m_framePlusButton;
    QSlider *sliderControl;
    QComboBox *m_speedFrameComboBox;

    QTimer *m_timerToUpdatePlot;

    QMap<QString, int> m_frameSpeed;
private:
    const quint16 dspRefreshMiliseconds;

};

#endif // BOTTOMDSPCONTROLPANEL_H
