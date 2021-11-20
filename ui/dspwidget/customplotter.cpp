#include "customplotter.h"

quint16 xMaxInterval = 32;
quint16 yMaxInterval = 2512;
const quint16 zMaxInterval = 100;


CustomPlotterWidget::CustomPlotterWidget(qint32 _targetID, bool _displayPlot,  QWidget *parent)
    : QWidget(parent)
    , dspRefreshRate(200)
    , targetID(_targetID)
    , displayPlot(_displayPlot)
{
    CreateObjects();
    CreateUI();
    InitWindowsTitle();
    InsertWidgetsIntoLayouts();
    FillUI();
    CreateBehavoirObjects();
    InitBehavoirObjects();
    ConnectObjects();
}

CustomPlotterWidget::~CustomPlotterWidget()
{
//    delete m_spectogramData;
//    d_spectrogram;delete


    delete updatePlot;
    delete m_zoomer;
    delete m_panner;
    delete sliderControl;
    delete startStopButton;



    delete m_qwtPlot;

    delete m_colorSchemeWidget;

}

void CustomPlotterWidget::CreateObjects()
{
    m_spectogramData = new SpectrogramData();
    updatePlot = new QTimer(this);
}

void CustomPlotterWidget::CreateUI()
{
    m_qwtPlot = new QwtPlot();
    d_spectrogram = new QwtPlotSpectrogram();

    m_panner = new QwtPlotPanner( m_qwtPlot->canvas() );
    sliderControl = new QSlider();
    startStopButton = new QPushButton("Старт");

}

void CustomPlotterWidget::InitWindowsTitle()
{
    QString title;
    if (targetID < 0) {
        title = QString("ДСП по ЦУ №%1 ").arg(abs(targetID));
    }
    else {
        title = QString("ДСП по трассе №%1 ").arg(targetID);
    }
    setWindowTitle(title);
}

void CustomPlotterWidget::InsertWidgetsIntoLayouts()
{
    QHBoxLayout *plotControlsLayout = new QHBoxLayout();
    plotControlsLayout->addWidget(startStopButton);
    plotControlsLayout->addWidget(sliderControl);

    QVBoxLayout *mainLayout = new  QVBoxLayout();
    mainLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    if (displayPlot) {
        mainLayout->addWidget(m_qwtPlot, 1);
        mainLayout->addLayout(plotControlsLayout);
    }

    setLayout(mainLayout);
}

void CustomPlotterWidget::FillUI()
{
    d_spectrogram->setRenderThreadCount( 1 ); // use system specific thread count
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );
    d_spectrogram->setData(m_spectogramData);
    d_spectrogram->attach( m_qwtPlot );

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    QwtScaleWidget *rightAxis = m_qwtPlot->axisWidget( QwtPlot::yRight ); // Цветная полоса на правой оси
    rightAxis->setTitle( "Амплитуда" );
    rightAxis->setColorBarEnabled( true );

    m_qwtPlot->setAxisScale(QwtPlot::yLeft, 0, yMaxInterval);
    m_qwtPlot->setAxisScale(QwtPlot::xBottom, 0, xMaxInterval);

    m_qwtPlot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    m_qwtPlot->enableAxis( QwtPlot::yRight );

    m_qwtPlot->plotLayout()->setAlignCanvasToScales( true );



    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( m_qwtPlot->axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = m_qwtPlot->axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "5.00" ) );


    sliderControl->setOrientation(Qt::Horizontal);
    sliderControl->setSingleStep(1);
    sliderControl->setPageStep(1);
    sliderControl->setTickPosition(QSlider::TicksBelow);
    sliderControl->setMaximum(100);
}

void CustomPlotterWidget::CreateBehavoirObjects()
{
    m_zoomer = new MyZoomer(m_qwtPlot->canvas()  );
}

void CustomPlotterWidget::InitBehavoirObjects()
{
    m_zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
                               Qt::RightButton, Qt::ControlModifier );
    m_zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
                               Qt::RightButton );
    m_panner->setAxisEnabled( QwtPlot::yRight, false );
    m_panner->setMouseButton( Qt::MiddleButton );

    const QColor c( Qt::darkBlue );
    m_zoomer->setRubberBandPen( c );
    m_zoomer->setTrackerPen( c );

}

void CustomPlotterWidget::ConnectObjects()
{
    connect(m_zoomer, &QwtPlotZoomer::zoomed, this, &CustomPlotterWidget::OnZoomHandler);
    connect(m_panner, &QwtPlotPanner::panned, this, &CustomPlotterWidget::OnPannerHandler);
    connect(updatePlot, &QTimer::timeout, this, &CustomPlotterWidget::timerTimeout);
    connect(startStopButton, &QPushButton::clicked, this, &CustomPlotterWidget::startStopClicked);
    connect(sliderControl, &QSlider::sliderReleased, this, &CustomPlotterWidget::sliderOffset);
}

void CustomPlotterWidget::UpdateData(const quint32 &n,const quint32 &m,const quint32 &counter, const QVector<qreal> &vector)
{
    //if (sliderControl->value() == sliderControl->maximum() || (startStopButton->text() == QString("Стоп"))) {
    //sp_data->updateMatrix(n, m, vector);
    //}

    if (xMaxInterval != n || yMaxInterval != m) {
        xMaxInterval = n;
        yMaxInterval = m;
        m_qwtPlot->setAxisScale(QwtPlot::xBottom, -10.0, xMaxInterval);
        m_qwtPlot->setAxisScale(QwtPlot::yLeft, -10.0, yMaxInterval);
    }

    m_spectogramData->UpdateMatrix(n, m, vector);

    //    sliderControl->setMaximum(counter);
    //    if (sliderControl->value() == sliderControl->maximum())
    //        sliderControl->setValue(counter);

    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();

    m_qwtPlot->replot();
}

void CustomPlotterWidget::ForceSendColorRangesToScene()
{
    currentSchemeChanged();
}

void CustomPlotterWidget::OnStartMovie()
{

}

void CustomPlotterWidget::OnChangeGradient(const ColorRanges &range)
{
    Q_EMIT ToSendColorRangesToScene(targetID, range);
}

void CustomPlotterWidget::OnShowContour( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnShowSpectrogram( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen(
                on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnSetColorsMap(const ColorsMap &colors)
{
    QwtScaleWidget *axis = m_qwtPlot->axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    d_mapType = ColorMap::RGBMap;

    d_spectrogram->setColorMap( new LinearColorMapRGB(colors) );
    axis->setColorMap( zInterval, new LinearColorMapRGB(colors) );

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnUpdatePlotSlot()
{
    m_spectogramData->RefreshMatrix();
    d_spectrogram->invalidateCache();
    d_spectrogram->itemChanged();

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnPannerHandler(int dx, int dy)
{
    QwtPlotPanner *panner = qobject_cast<QwtPlotPanner *>(sender());
    QwtPlot *plot_ptr = panner->plot();

    QwtInterval intervalY = plot_ptr->axisInterval(QwtPlot::yLeft);
    QwtInterval intervalX = plot_ptr->axisInterval(QwtPlot::xBottom);

    //qDebug() << intervalX << intervalY;

    QwtInterval correctIntervalY;
    if (intervalY.minValue() < 0) {
        correctIntervalY.setMinValue(0);
    }
    else {
        correctIntervalY.setMinValue(intervalY.minValue());
    }
    if (intervalY.maxValue() > yMaxInterval) {
        correctIntervalY.setMaxValue(yMaxInterval);
    }
    else {
        correctIntervalY.setMaxValue(intervalY.maxValue());
    }

    QwtInterval correctIntervalX;
    if (intervalX.minValue() < 0) {
        correctIntervalX.setMinValue(0);
    }
    else {
        correctIntervalX.setMinValue(intervalX.minValue());
    }
    if (intervalX.maxValue() > xMaxInterval) {
        correctIntervalX.setMaxValue(xMaxInterval);
    }
    else {
        correctIntervalX.setMaxValue(intervalX.maxValue());
    }


    plot_ptr->setAxisScale(QwtPlot::yLeft, correctIntervalY.minValue(), correctIntervalY.maxValue());
    plot_ptr->setAxisScale(QwtPlot::xBottom, correctIntervalX.minValue(), correctIntervalX.maxValue());

    m_qwtPlot->replot();
}

void CustomPlotterWidget::OnZoomHandler(const QRectF &rect)
{
    QwtPlotZoomer *zoomer = qobject_cast<QwtPlotZoomer *>(sender());
    //QwtPlot *plot_ptr = panner->plot();
    qDebug()<< "onzoom";
    QRectF resultRect(rect);
    qDebug()<< rect;
    qDebug()<< "r" << rect.right() << " h " << rect.height();
    if (rect.x() < 0) {
        resultRect.setX(0);
    }
    else if (rect.x() > (xMaxInterval - 10)) {
        resultRect.setX(xMaxInterval - 10);
        if (rect.width() > 10)
            resultRect.setWidth(10);
    }
    if (rect.y() < 0) {
        resultRect.setY(0);
    }
    else if (rect.y() > (yMaxInterval - 10)) {
        resultRect.setY(yMaxInterval - 10);
        if (rect.height() > 10)
            resultRect.setHeight(10);
    }

    if (rect.right() > xMaxInterval) {
        resultRect.setWidth(xMaxInterval - rect.left());
    }

    if (rect.bottom() > yMaxInterval) {
        resultRect.setHeight(yMaxInterval - rect.top());
    }

    if (resultRect != rect) {
        zoomer->zoom(-1);
        zoomer->zoom(resultRect);

        m_qwtPlot->replot();
    }
//    else
//    {
//        zoomer->zoom(0);
//         m_qwtPlot->replot();
//    }

    //qDebug() << rect;
    //qDebug() << zoomer->zoomBase();
}

void CustomPlotterWidget::setItemToPresetComboBox(const quint32 &index, const MapOfColorRanges::iterator &itemOfColorMap)
{
//    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(presetComboBox->model());

//    QSize pixmapSize(100,14);
//    QPixmap pixmap(pixmapSize);
//    pixmap.fill(Qt::transparent);

//    QPainter painter(&pixmap);
//    QRect r = pixmap.rect();

//    QLinearGradient gradient(r.topLeft(), r.topRight());
//    for (quint16 indexOfArray = 0; indexOfArray != itemOfColorMap.value().colorsForGradientLabel.size(); indexOfArray++){
//        gradient.setColorAt(itemOfColorMap.value().rangesForRangeSlider.at(indexOfArray) / 100.,
//                            itemOfColorMap.value().colorsForGradientLabel.at(indexOfArray));
//    }

//    painter.setBrush(QBrush(gradient));
//    painter.fillRect(r, painter.brush());
//    painter.end();

//    QStandardItem* sItem;
//    if (index >= (quint32)presetComboBox->count()) {
//        sItem = new QStandardItem;
//        model->setItem(model->rowCount(), 0, sItem);
//        sItem = model->item(model->rowCount() - 1);
//    }
//    else {
//        sItem = model->item(index);
//    }
//    sItem->setText(itemOfColorMap.key());
//    sItem->setIcon(QIcon(pixmap));

//    presetComboBox->setIconSize(pixmapSize);
}

void CustomPlotterWidget::createColorArray(const ColorRanges &colorRanges)
{
    ColorsMap colorMap;
    for (quint8 index = 0; index !=  colorRanges.colorsForGradientLabel.size(); index++) {
        colorMap.push_back(qMakePair(colorRanges.rangesForRangeSlider.at(index),
                                     colorRanges.colorsForGradientLabel.at(index)));
    }
    OnSetColorsMap(colorMap);
    if (!displayPlot)
        emit ToSendColorRangesToScene(targetID, colorRanges);
}

void CustomPlotterWidget::startStopClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button->text() == QString("Старт")) {
        if (sliderControl->value() == sliderControl->maximum()) {
            updatePlot->stop();
        }
        else {
            updatePlot->start(dspRefreshRate);
        }
        button->setText(QString("Стоп"));
    }
    else {
        updatePlot->stop();
        button->setText(QString("Старт"));
    }
}

void CustomPlotterWidget::sliderOffset()
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    Q_EMIT ToSetDSPDataOnPlotter(targetID, slider->value());
}

void CustomPlotterWidget::timerTimeout()
{
    if (sliderControl->value() != sliderControl->maximum())
    {
        sliderControl->setValue(sliderControl->value() + 1);
    }
    else
    {
        sliderControl->setValue(0);
    }
}

void CustomPlotterWidget::OnSchemeChange(QString &nameScheme, QString &newNameScheme, QVector<QColor> &colors, QVector<int> &ranges)
{
//    auto itemOfMap = mapOfColorRanges.find(nameScheme);
//    if (itemOfMap != mapOfColorRanges.end()) {
//        if (nameScheme != newNameScheme) {
//            mapOfColorRanges.erase(itemOfMap);
//            itemOfMap = mapOfColorRanges.insert(newNameScheme, ColorRanges());
//        }
//    }
//    else {
//        itemOfMap = mapOfColorRanges.insert(newNameScheme, ColorRanges());
//    }
//    itemOfMap.value().colorsForGradientLabel = colors;
//    itemOfMap.value().rangesForRangeSlider = ranges;


//    int searchingIndex = presetComboBox->findText(nameScheme),
//            currentIndex = presetComboBox->currentIndex();
//    if (searchingIndex == -1) {
//        setItemToPresetComboBox(presetComboBox->count(), itemOfMap);
//        if (currentIndex == -1) {
//            presetComboBox->setCurrentIndex(0);
//            currentSchemeChanged();
//        }
//    }
//    else {
//        setItemToPresetComboBox(searchingIndex, itemOfMap);
//        if (searchingIndex == currentIndex) {
//            createColorArray(itemOfMap.value());
//        }
//    }
}

void CustomPlotterWidget::OnSchemeDelete(QString nameScheme)
{
//    auto itemOfMap = mapOfColorRanges.find(nameScheme);
//    if (itemOfMap != mapOfColorRanges.end()) {
//        mapOfColorRanges.erase(itemOfMap);
//    }

//    int searchingIndex = presetComboBox->findText(nameScheme);
//    presetComboBox->removeItem(searchingIndex);

//    QString currentStr = presetComboBox->currentText();
//    itemOfMap = mapOfColorRanges.find(currentStr);
//    if (itemOfMap != mapOfColorRanges.end()) {
//        createColorArray(itemOfMap.value());
//    }
}

void CustomPlotterWidget::OnSetSliderLimit(quint32 counter)
{
    sliderControl->setMaximum(counter);
}


void CustomPlotterWidget::currentSchemeChanged()
{
//    //QComboBox *cb = qobject_cast<QComboBox*>(sender());
//    QString str = presetComboBox->currentText();

//    auto itemOfMap = mapOfColorRanges.find(str);
//    if (itemOfMap != mapOfColorRanges.end()) {
//        createColorArray(itemOfMap.value());
//    }
//    else {
//        qDebug() << "CustomPlotterWidget::currentSchemeChanged - Can't find name of scheme";
//    }
}
