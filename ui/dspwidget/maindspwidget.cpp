#include "maindspwidget.h"



MainDspWidget::MainDspWidget(QWidget *parent)
    : QWidget(parent)
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();
    ConnectObjects();
}

MainDspWidget::~MainDspWidget()
{
    delete m_presenter;

    delete m_mainLayout;

    delete m_customPlotterWidget;

    delete m_dspHistoryWidget;
}

void MainDspWidget::CreateObjects()
{
    m_presenter=new DspPresenter(nullptr);
}

void MainDspWidget::CreateUI()
{
    m_mainLayout = new QVBoxLayout();

    m_topUserPanel=new TopUserPanel(m_presenter, this);
    m_customPlotterWidget = new CustomPlotterWidget(10, true, this);

    m_dspHistoryWidget=new DSPHistoryWidget(nullptr);

}

void MainDspWidget::InsertWidgetsIntoLayouts()
{
    m_mainLayout->addWidget(m_topUserPanel);
    m_mainLayout->addWidget(m_customPlotterWidget);
    setLayout(m_mainLayout);
}

void MainDspWidget::FillUI()
{

}

void MainDspWidget::ConnectObjects()
{
    connect(m_topUserPanel, &TopUserPanel::ToShowDspHistoryWidget, m_dspHistoryWidget, &DSPHistoryWidget::show);
    connect(m_topUserPanel, &TopUserPanel::ToChangeGradient, m_customPlotterWidget, &CustomPlotterWidget::OnChangeGradient);
    connect(m_customPlotterWidget, &CustomPlotterWidget::ToSetDSPDataOnPlotter, this, &MainDspWidget::OnSetDSPDataOnPlotter);

    connect(m_presenter, &DspPresenter::ToSetSliderLimit, m_customPlotterWidget, &CustomPlotterWidget::OnSetSliderLimit);
    connect(m_presenter, &DspPresenter::ToSetDSPDataOnPlotter, this, &MainDspWidget::OnSetDSPDataOnPlotter);
}

void MainDspWidget::OnSetDSPDataOnPlotter(quint32 targetId, quint32 counter)
{
    Q_UNUSED(targetId)
    if (counter >= m_presenter->GetVectorOfDataElementsCount())
        return;
    const AmpBlockDSP &firstFrame = m_presenter->GetElementOfData(counter);
    m_customPlotterWidget->UpdateData(firstFrame.header.DistSamplesNum, firstFrame.header.TimeSamplesNum, counter, firstFrame.data);
}
