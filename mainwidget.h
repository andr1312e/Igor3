#ifndef DSPWIDGET_H
#define DSPWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDataStream>
#include <QDateTime>
#include <QtMath>

#include "structs/AmpBlockDSP.h"
#include "structs/RawBlockDSP.h"
#include "structs/fftwinfo.h"
#include "structs/POIDistSpeedPortrait.h"

#include <customplotter.h>

// ID: POI_DSP_MESSAGE

using MapOfRawDSPData = QMap<quint32, RawBlockDSP>;
using VectorOfDSPData = QVector<AmpBlockDSP>;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent);
    ~MainWidget();

Q_SIGNALS:
    void ToSetSliderLimit(quint32 counter);
    //void sendFrame(quint32 distSamles, quint32 timeSamples, QVector<float> data, quint32 indexFrame);

private Q_SLOTS:
    void OnFileSelect();
    void OnSetDSPDataOnPlotter(int targetID, quint32 counter);

private:
    void CalculateAmplitude();
    void TransposeMatrix(qint32 distSamples, qint32 timeSamples, int *in, int *out);
    static qreal AbsoluteOfComplexValue(qreal &real, qreal &imaginary) ;

private:
    CustomPlotterWidget *m_customPlotterWidget;
    MapOfRawDSPData m_mapOfRawData;
    VectorOfDSPData m_vectorOfData;
};

#endif // DSPWIDGET_H
