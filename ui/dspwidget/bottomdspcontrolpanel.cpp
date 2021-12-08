#include "bottomdspcontrolpanel.h"

BottomDspControlPanel::BottomDspControlPanel(QWidget *parent)
    : QWidget(parent)
    , dspRefreshMiliseconds(200)
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

BottomDspControlPanel::~BottomDspControlPanel()
{
    delete m_timerToUpdatePlot;

    delete sliderControl;
    delete m_startStopButton;
    delete m_frameLabel;
    delete m_frameValue;
    delete m_speedFrameComboBox;

    delete m_frameMinusButton;
    delete m_framePlusButton;
}

void BottomDspControlPanel::CreateObjects()
{
    m_timerToUpdatePlot = new QTimer(this);
    m_frameSpeed={{"Быстрая скорость", 500}, {"Нормальная скорость", 1000}, {"Медленная скорость", 1600}};
}

void BottomDspControlPanel::CreateUI()
{
    sliderControl = new QSlider();
    m_startStopButton = new QPushButton();
    m_frameLabel=new QLabel();
    m_frameValue=new QLabel();
    m_speedFrameComboBox=new QComboBox();
    m_framePlusButton=new QPushButton();
    m_frameMinusButton=new QPushButton();
}

void BottomDspControlPanel::InsertWidgetsIntoLayouts()
{
    m_mainLayout=new QHBoxLayout();
    m_mainLayout->addWidget(m_frameLabel);
    m_mainLayout->addWidget(m_frameValue);
    m_mainLayout->addWidget(m_frameMinusButton);
    m_mainLayout->addWidget(m_startStopButton);
    m_mainLayout->addWidget(m_framePlusButton);
    m_mainLayout->addWidget(sliderControl);
    m_mainLayout->addWidget(m_speedFrameComboBox);
    setLayout(m_mainLayout);
}

void BottomDspControlPanel::FillUI()
{
    sliderControl->setOrientation(Qt::Horizontal);
    sliderControl->setSingleStep(1);
    sliderControl->setPageStep(1);
    sliderControl->setTickPosition(QSlider::TicksBelow);
    sliderControl->setMaximum(100);


    m_startStopButton->setText("Старт");
    m_frameLabel->setText("Номер кадра:");
    m_frameValue->setText("0     ");
    const QStringList comboBoxValues(m_frameSpeed.keys());
    m_speedFrameComboBox->addItems(comboBoxValues);
    m_speedFrameComboBox->setEditable(false);
    m_timerToUpdatePlot->setInterval(m_frameSpeed.value(comboBoxValues.first()));

    m_frameMinusButton->setText("Кадр назад");
    m_framePlusButton->setText("Кадр вперед");
}

void BottomDspControlPanel::ConnectObjects()
{
    connect(m_frameMinusButton, &QPushButton::clicked, this, &BottomDspControlPanel::OnFrameMinusButtonClicked);
    connect(m_framePlusButton, &QPushButton::clicked, this, &BottomDspControlPanel::OnFramePlusButtonClicked);
    connect(m_startStopButton, &QPushButton::clicked, this, &BottomDspControlPanel::OnStartStopClicked);
    connect(sliderControl, &QSlider::sliderReleased, this, &BottomDspControlPanel::OnSliderOffset);
    connect(m_timerToUpdatePlot, &QTimer::timeout, this, &BottomDspControlPanel::OnTimerTimeout);
    connect(m_speedFrameComboBox, &QComboBox::currentTextChanged, this, &BottomDspControlPanel::OnComboBoxTextChanged);
    connect(sliderControl, &QSlider::valueChanged, this , &BottomDspControlPanel::OnSliderValueChanged);
}

void BottomDspControlPanel::OnSetSliderLimit(int counter)
{
    sliderControl->setMaximum(counter);
}

void BottomDspControlPanel::SetPageNumForGif(int value)
{
    m_frameValue->setNum(value);
}

void BottomDspControlPanel::OnStartTimer()
{
    m_timerToUpdatePlot->start();
}

void BottomDspControlPanel::OnStartStopClicked()
{
    if (m_startStopButton->text() == QString("Старт"))
    {
        if (sliderControl->value() == sliderControl->maximum())
        {
            m_timerToUpdatePlot->stop();
        }
        else
        {
            m_timerToUpdatePlot->start(dspRefreshMiliseconds);
            m_startStopButton->setText(QString("Стоп"));
        }
    }
    else
    {
        m_timerToUpdatePlot->stop();
        m_startStopButton->setText(QString("Старт"));
    }
}

void BottomDspControlPanel::OnSliderOffset()
{
    Q_EMIT ToRequestDSPData(sliderControl->value());
}

void BottomDspControlPanel::OnTimerTimeout()
{
    if (sliderControl->value() == sliderControl->maximum())
    {
        OnStartStopClicked();
        sliderControl->setValue(0);
    }
    else
    {
        sliderControl->setValue(sliderControl->value() + 1);
        Q_EMIT ToRequestDSPData(sliderControl->value() + 1);
    }
}

void BottomDspControlPanel::OnComboBoxTextChanged(const QString &text)
{
    int timerValue=m_frameSpeed.value(text);
    m_timerToUpdatePlot->setInterval(timerValue);
}

void BottomDspControlPanel::OnFrameMinusButtonClicked()
{
    if (sliderControl->value()!=0)
    {
        sliderControl->setValue(sliderControl->value() - 1);
        Q_EMIT ToRequestDSPData(sliderControl->value() - 1);
    }
}

void BottomDspControlPanel::OnFramePlusButtonClicked()
{
    if (sliderControl->value()!=sliderControl->maximum())
    {
        sliderControl->setValue(sliderControl->value() + 1);
        Q_EMIT ToRequestDSPData(sliderControl->value() + 1);
    }
}

void BottomDspControlPanel::OnSliderValueChanged(int value)
{
       m_frameValue->setText(QString::number(value));
}
