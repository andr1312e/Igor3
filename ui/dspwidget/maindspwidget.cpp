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
    m_presenter.reset();

    delete m_mainLayout;
    delete m_topUserPanel;
    delete m_customPlotterWidget;
}

void MainDspWidget::CreateObjects()
{
    m_presenter=QSharedPointer<DspPresenter>(new DspPresenter(Q_NULLPTR));
}

void MainDspWidget::CreateUI()
{
    m_mainLayout = new QVBoxLayout(this);

    m_topUserPanel=new TopUserPanel(m_presenter, this);
    m_customPlotterWidget = new CustomPlotterWidget(this);


}

void MainDspWidget::InsertWidgetsIntoLayouts()
{
    m_mainLayout->addWidget(m_topUserPanel);
    m_mainLayout->addWidget(m_customPlotterWidget);
    setLayout(m_mainLayout);
}

void MainDspWidget::FillUI()
{
    m_customPlotterWidget->hide();
    this->setWindowTitle(QStringLiteral("Просмотр ДСП. Версия ")+APP_VERSION);
}

void MainDspWidget::ConnectObjects()
{
    connect(m_topUserPanel, &TopUserPanel::ToChangeGradient, m_customPlotterWidget, &CustomPlotterWidget::OnChangeGradient);
    connect(&(*m_presenter), &DspPresenter::ToSetSliderLimit, m_customPlotterWidget, &CustomPlotterWidget::ToSetSliderLimit);
    connect(&(*m_presenter), &DspPresenter::ToSetSliderLimit, m_customPlotterWidget, &CustomPlotterWidget::show);
    connect(&(*m_presenter), &DspPresenter::ToSetDSPDataOnPlotter, this, &MainDspWidget::OnRequestDSPData);
    connect(m_customPlotterWidget, &CustomPlotterWidget::ToRequestDSPData, this, &MainDspWidget::OnRequestDSPData);
}

void MainDspWidget::OnRequestDSPData(int frame)
{
    if ((quint32)frame < m_presenter->GetVectorOfDataElementsCount())
    {
        const AmpBlockDSP &firstFrame = m_presenter->GetElementOfData(frame);
        m_customPlotterWidget->UpdateData(firstFrame.header.DistSamplesNum, firstFrame.header.TimeSamplesNum, firstFrame.data);
    }
}
