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

    delete m_mainLayout;
    delete m_topUserPanel;
    delete m_customPlotterWidget;
}

void MainDspWidget::CreateObjects()
{
    m_presenter=new DspPresenter(this);
}

void MainDspWidget::CreateUI()
{
    m_mainLayout = new QVBoxLayout(this);

    m_topUserPanel=new TopUserPanel(m_presenter, this);
    m_customPlotterWidget = new CustomPlotterWidget(this);
    m_convertingProgressBar=new QProgressBar(this);


}

void MainDspWidget::InsertWidgetsIntoLayouts()
{
    m_mainLayout->addWidget(m_topUserPanel);
    m_mainLayout->addWidget(m_customPlotterWidget);
    m_mainLayout->addWidget(m_convertingProgressBar);
    setLayout(m_mainLayout);
}

void MainDspWidget::FillUI()
{
    m_customPlotterWidget->hide();
    m_convertingProgressBar->setRange(0, 100);
    this->setWindowTitle(QStringLiteral("Просмотр ДСП. Версия ")+APP_VERSION);
}

void MainDspWidget::ConnectObjects()
{
    connect(m_topUserPanel, &TopUserPanel::ToChangeGradient, m_customPlotterWidget, &CustomPlotterWidget::OnChangeGradient);
    connect(m_presenter, &DspPresenter::ToSetSliderLimit, m_customPlotterWidget, &CustomPlotterWidget::ToSetSliderLimit);
    connect(m_presenter, &DspPresenter::ToSetSliderLimit, m_customPlotterWidget, &CustomPlotterWidget::show);
    connect(m_presenter, &DspPresenter::ToSetDSPDataOnPlotter, this, &MainDspWidget::OnRequestDSPData);
    connect(m_customPlotterWidget, &CustomPlotterWidget::ToRequestDSPData, this, &MainDspWidget::OnRequestDSPData);
    connect(m_topUserPanel, &TopUserPanel::ToSaveIntoGif, this, &MainDspWidget::OnSetImagesToGif);
    connect(m_presenter, &DspPresenter::ToSetTitileInfo, this, &MainDspWidget::OnSetTitileInfo);
    connect(m_presenter, &DspPresenter::ToConverted, this, &MainDspWidget::OnConverted);
}

void MainDspWidget::OnSetTitileInfo(int &dspType, float &sensorAzm, float &sensorUgm)
{
    InitWindowTitle(dspType);
    m_topUserPanel->SetAdditionalInfo(sensorAzm, sensorUgm);

}

void MainDspWidget::OnRequestDSPData(int frame)
{
    if (frame < m_presenter->GetVectorOfDataElementsCount())
    {
        const AmpBlockDSP &firstFrame = m_presenter->GetElementOfData(frame);
        m_customPlotterWidget->UpdateData(firstFrame.header.DistSamplesNum, firstFrame.header.TimeSamplesNum, firstFrame.data);
    }
}

void MainDspWidget::OnSetImagesToGif()
{
    m_convertingProgressBar->setValue(0);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_topUserPanel->setDisabled(true);
    m_presenter->ClearImagesList();
    for (int frame = 0; frame < m_presenter->GetVectorOfDataElementsCount(); ++frame)
    {
        const AmpBlockDSP &firstFrame = m_presenter->GetElementOfData(frame);
        m_customPlotterWidget->UpdateData(firstFrame.header.DistSamplesNum, firstFrame.header.TimeSamplesNum, firstFrame.data);
        m_customPlotterWidget->SetPageNumForGif(frame);
        m_presenter->OnAppendImageIntoList(m_customPlotterWidget->grab().toImage());
    }
    quint64 timeToConvertTwoImages=m_presenter->GetElapsedTimeOfTwoImages();
    quint64 timeToConvertAllImages=quint64(qCeil(m_presenter->GetVectorOfDataElementsCount()/2))*timeToConvertTwoImages;
    qDebug()<< "timeToConvertTwoImages" << timeToConvertTwoImages << "timeToConvertAllImages" << timeToConvertAllImages;

    this->startTimer(timeToConvertAllImages/100, Qt::CoarseTimer);
    QFuture<void> done=QtConcurrent::run(m_presenter, &DspPresenter::OnTheadConvertToGif);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void MainDspWidget::OnConverted()
{
    m_convertingProgressBar->setValue(100);
    m_topUserPanel->setEnabled(true);
}

void MainDspWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if(m_convertingProgressBar->value()<m_convertingProgressBar->maximum()-1)
    {
        m_convertingProgressBar->setValue(m_convertingProgressBar->value()+1);
    }
    else
    {
        this->killTimer(event->timerId());
    }
    this->repaint();
}

void MainDspWidget::InitWindowTitle(int dspType)
{
    if (dspType>=0)
    {
        this->setWindowTitle(QStringLiteral("Просмотр ДСП. Версия ")+APP_VERSION + " Тип РСП: Трасса, номер трассы: " + QString::number(dspType));
    }
    else
    {
        this->setWindowTitle(QStringLiteral("Просмотр ДСП. Версия ")+APP_VERSION + " Тип ДСП: Сектор, номер сектора: " + QString::number((-1)*dspType));
    }
}
