#include "customplotter.h"
#include <qwt_picker_machine.h>
#include <qwt_plot_picker.h>

CustomPlotterWidget::CustomPlotterWidget(QWidget *parent)
    : QWidget(parent)
    , m_contrastColors({QColor(0, 0, 0), QColor(255, 0, 0), QColor(0, 255, 0),
                       QColor(0, 0, 255), QColor(255, 255, 0), QColor(255, 0, 255),
                       QColor(0, 255, 255), QColor(255, 255, 255), QColor(125, 125, 125)})
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayouts();
    FillUI();

    m_qwtPlot->setAxisScale(QwtPlot::yLeft, 0, m_spectogramData->GetXMaxAxisValue());
    m_qwtPlot->setAxisScale(QwtPlot::xBottom, 0, m_spectogramData->GetYMaxAxisValue());

    CreateBehavoirObjects();
    InitBehavoirObjects();
    ConnectObjects();
}

CustomPlotterWidget::~CustomPlotterWidget()
{
    delete m_qwtPlot;
    delete m_bottomDspControlPanel;
}

void CustomPlotterWidget::CreateObjects()
{
    m_spectogramData = new SpectrogramData();

}

void CustomPlotterWidget::CreateUI()
{
    m_qwtPlot = new QwtPlot(this);
    d_spectrogram = new QwtPlotSpectrogram();

    m_chartFixer = new QwtPlotPanner(m_qwtPlot->canvas() );
    m_bottomDspControlPanel=new BottomDspControlPanel(this);
}

void CustomPlotterWidget::InsertWidgetsIntoLayouts()
{


    QVBoxLayout *mainLayout = new  QVBoxLayout();
    mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(m_qwtPlot, 1);
    mainLayout->addWidget(m_bottomDspControlPanel);

    setLayout(mainLayout);
}

void CustomPlotterWidget::FillUI()
{
    d_spectrogram->setRenderThreadCount( 2 ); // use system specific thread count
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );
    d_spectrogram->setData(m_spectogramData);
    d_spectrogram->attach(m_qwtPlot);

    QwtScaleWidget *distanceAxis = m_qwtPlot->axisWidget( QwtPlot::xBottom );//устанавливаем подписи к спектограмме
    distanceAxis->setTitle("Дистанция (отсчеты)");
    QwtScaleWidget *timeAxis=m_qwtPlot->axisWidget(QwtPlot::yLeft);
    timeAxis->setTitle("Время (отсчеты)");

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    QwtScaleWidget *rightAxis = m_qwtPlot->axisWidget( QwtPlot::yRight ); // Цветная полоса на правой оси
    rightAxis->setTitle( "Амплитуда (Децибелы)" );//устанавливаем подписи
    rightAxis->setColorBarEnabled( true );

    m_qwtPlot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    m_qwtPlot->enableAxis( QwtPlot::yRight );

    m_qwtPlot->plotLayout()->setAlignCanvasToScales( true );



}

void CustomPlotterWidget::CreateBehavoirObjects()
{
    m_pointViewer = new PointViewer( m_qwtPlot->canvas() );
}

void CustomPlotterWidget::InitBehavoirObjects()
{
    m_pointViewer->setMousePattern( QwtEventPattern::MouseSelect2,
                                    Qt::RightButton, Qt::ControlModifier );
    m_pointViewer->setMousePattern( QwtEventPattern::MouseSelect3,
                                    Qt::RightButton );

    m_chartFixer->setAxisEnabled( QwtPlot::yRight, false );
    m_chartFixer->setMouseButton( Qt::MiddleButton );

    const QFontMetrics fm( m_qwtPlot->axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = m_qwtPlot->axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "5.00" ) );

    const QColor c( Qt::white );//устанавливаем у зумера колор для ректангла
    m_pointViewer->setRubberBandPen( c );
    //    m_pointViewer->setTrackerPen( c );
}

void CustomPlotterWidget::ConnectObjects()
{
    connect(this, &CustomPlotterWidget::ToStartTimer, m_bottomDspControlPanel, &BottomDspControlPanel::OnStartTimer);
    connect(m_pointViewer, &QwtPlotZoomer::zoomed, this, &CustomPlotterWidget::OnZoomHandler);
    connect(m_chartFixer, &QwtPlotPanner::panned, this, &CustomPlotterWidget::OnPannerHandler);
    connect(this, &CustomPlotterWidget::ToSetSliderLimit, m_bottomDspControlPanel, &BottomDspControlPanel::OnSetSliderLimit);
    connect(m_bottomDspControlPanel, &BottomDspControlPanel::ToRequestDSPData, this, &CustomPlotterWidget::ToRequestDSPData);
}

void CustomPlotterWidget::UpdateData(const quint32 &distSamplesNum, const quint32 &timeSamplesNum, const QVector<qreal> &data)
{ 
    if (m_spectogramData->GetXMaxAxisValue() != distSamplesNum || m_spectogramData->GetYMaxAxisValue() != timeSamplesNum)
    {
        m_spectogramData->SetXMaxInterval(distSamplesNum);
        m_spectogramData->SetYMaxInterval(timeSamplesNum);
        m_qwtPlot->setAxisScale(QwtPlot::xBottom, m_spectogramData->xMinValueAxis, m_spectogramData->GetXMaxAxisValue());
        m_qwtPlot->setAxisScale(QwtPlot::yLeft, m_spectogramData->yMinValueAxis, m_spectogramData->GetYMaxAxisValue());
    }

    m_spectogramData->UpdateMatrix(distSamplesNum, timeSamplesNum, data);


    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();
    m_qwtPlot->replot();
}

void CustomPlotterWidget::SetPageNumForGif(int value)
{
    m_bottomDspControlPanel->SetPageNumForGif(value);
}

const QColor CustomPlotterWidget::GetMaxContrastColor(const QVector<QColor> &currentSpectorColors)
{
    QVarLengthArray<QColor, 9>::const_iterator it=m_contrastColors.cbegin();
    QColor contrastColor=*it;
    int maxDistance=CalculateColorDistance(contrastColor, currentSpectorColors);
    ++it;
    while (it!=m_contrastColors.cend())
    {
        int currentDistance=CalculateColorDistance(*it, currentSpectorColors);
        if (currentDistance>maxDistance)
        {
            maxDistance=currentDistance;
            contrastColor=*it;
        }
        ++it;
    }
    return contrastColor;
}

int CustomPlotterWidget::CalculateColorDistance(const QColor &color, const QVector<QColor> &spectorColors)
{
    int distance=0;
    for (auto spectorColor:spectorColors)
    {
        distance+=qAbs(spectorColor.red()-color.red())+qAbs(spectorColor.green()-color.green())+qAbs(spectorColor.blue()-color.blue());
    }
    return distance;
}

bool CustomPlotterWidget::CompareRects(const QRectF &firstRect, const QRectF &secondRect)
{
    if (qFuzzyIsNull(firstRect.x()) || qFuzzyIsNull(firstRect.y()) || qFuzzyIsNull(secondRect.x()) || qFuzzyIsNull(secondRect.y()))
    {
        if((firstRect.width()==secondRect.width())&& (firstRect.height()==secondRect.height()))
        {
            if (qFuzzyIsNull(firstRect.x()) || qFuzzyIsNull(secondRect.x()))
            {
                if (qFuzzyIsNull(firstRect.x()) && qFuzzyIsNull(secondRect.x()))
                {
                    if (qFuzzyIsNull(firstRect.y()) || qFuzzyIsNull(secondRect.y()))
                    {
                        if (qFuzzyIsNull(firstRect.y()) && qFuzzyIsNull(secondRect.y()))
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return qFuzzyCompare(firstRect.y(), secondRect.y());
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (qFuzzyCompare(firstRect.x(), secondRect.x()))
                {
                    if (qFuzzyIsNull(firstRect.y()) && qFuzzyIsNull(secondRect.y()))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return !qFuzzyCompare(firstRect.x(), secondRect.x()) || !qFuzzyCompare(firstRect.y(), secondRect.y())
                || !qFuzzyCompare(firstRect.width(), secondRect.width() || !qFuzzyCompare(firstRect.height(), secondRect.height()));
    }
}

void CustomPlotterWidget::OnChangeGradient(const ColorRanges &range)
{
    ColorsMap colorMap;
    for (int index = 0; index <  range.colorsForGradientLabel.size(); index++)
    {
        colorMap.push_back(qMakePair(range.rangesForRangeSlider.at(index)/100.,
                                     range.colorsForGradientLabel.at(index)));
    }

    QwtScaleWidget *axis = m_qwtPlot->axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    d_spectrogram->setColorMap( new LinearColorMapRGB(colorMap));
    axis->setColorMap( zInterval, new LinearColorMapRGB(colorMap) );

    const QColor c=GetMaxContrastColor(range.colorsForGradientLabel);
    m_pointViewer->setRubberBandPen( c );


    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnPannerHandler(int dx, int dy)
{
    //    QwtPlotPanner *panner = qobject_cast<QwtPlotPanner *>(sender());
    QwtPlot *plot_ptr = m_chartFixer->plot();

    QwtInterval intervalY = plot_ptr->axisInterval(QwtPlot::yLeft);
    QwtInterval intervalX = plot_ptr->axisInterval(QwtPlot::xBottom);

    //qDebug() << intervalX << intervalY;

    QwtInterval correctIntervalY;

    if (intervalY.minValue() < 0)
    {
        correctIntervalY.setMinValue(0);
    }
    else
    {
        correctIntervalY.setMinValue(intervalY.minValue());
    }

    if (intervalY.maxValue() > m_spectogramData->GetYMaxAxisValue())
    {
        correctIntervalY.setMaxValue(m_spectogramData->GetYMaxAxisValue());
    }
    else
    {
        correctIntervalY.setMaxValue(intervalY.maxValue());
    }

    QwtInterval correctIntervalX;
    if (intervalX.minValue() < 0)
    {
        correctIntervalX.setMinValue(0);
    }
    else
    {
        correctIntervalX.setMinValue(intervalX.minValue());
    }
    if (intervalX.maxValue() > m_spectogramData->GetXMaxAxisValue())
    {
        correctIntervalX.setMaxValue(m_spectogramData->GetXMaxAxisValue());
    }
    else
    {
        correctIntervalX.setMaxValue(intervalX.maxValue());
    }


    plot_ptr->setAxisScale(QwtPlot::yLeft, correctIntervalY.minValue(), correctIntervalY.maxValue());
    plot_ptr->setAxisScale(QwtPlot::xBottom, correctIntervalX.minValue(), correctIntervalX.maxValue());

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnZoomHandler(const QRectF &rect)
{
    //QwtPlot *plot_ptr = panner->plot();
    QRectF correctRect(rect);
    if(qAbs(rect.width())==0||qAbs(rect.height())==0)
    {
        correctRect.setWidth(1);
        correctRect.setHeight(1);
    }
    if (rect.x() < 0)
    {
        correctRect.setX(0);
    }
    else if (rect.x() > (m_spectogramData->GetXMaxAxisValue() - 10))
    {
        correctRect.setX(m_spectogramData->GetXMaxAxisValue() - 10);
        if (rect.width() > 10)
        {
            correctRect.setWidth(10);
        }
    }
    if (rect.y() < 0)
    {
        correctRect.setY(0);
    }
    else if (rect.y() > (m_spectogramData->GetYMaxAxisValue() - 10))
    {
        correctRect.setY(m_spectogramData->GetYMaxAxisValue() - 10);
        if (rect.height() > 10)
        {
            correctRect.setHeight(10);
        }
    }

    if (rect.right() > m_spectogramData->GetXMaxAxisValue())
    {
        correctRect.setWidth(m_spectogramData->GetXMaxAxisValue() - rect.left());
    }

    if (rect.bottom() > m_spectogramData->GetYMaxAxisValue())
    {
        correctRect.setHeight(m_spectogramData->GetYMaxAxisValue() - rect.top());
    }

//    if (correctRect!=rect)
        if (!CompareRects(correctRect, rect))
        {
            m_pointViewer->zoom(-1);
            m_pointViewer->zoom(correctRect);

            m_qwtPlot->replot();
        }
}


