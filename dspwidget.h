#ifndef DSPWIDGET_H
#define DSPWIDGET_H

#include <QWidget>
#include <customplotter.h>

typedef struct timeval64   {								// Время
    quint64 secs;                        // Cекунды
    quint64 usecs;                       // Микросекунды
}timeval64;

// ID: POI_DSP_MESSAGE
struct POIDistSpeedPortraitHeader	{							// Заголовок дальностно-скоростного портрета
    timeval64 sTimeMeasurement;                                 // Время формирования ДСП
    int DistSamplesNum;											// Количество дальностных отсчетов
    int TimeSamplesNum;											// Количество импульсов в луче
    float Sensor_Azm;											// Значение датчика азимута
    float Sensor_Ugm;											// Значение датчика угла места
    int Value4Rmo;												// Значение для РМО
    quint32 RayCounter;											// Счетчик лучей ПОИ
    quint8 LaunchMode;											// Номер запуска
    quint8 RayNum_Azm;											// Номер луча по азимуту
    quint8 RayNum_Ugm;											// Номер луча по углу места
    quint8 TotalPartsNum;										// Общее количество передаваемых частей ДСП
    quint8 CurrentPartNum;										// Номер передаваемой части ДСП
    quint16 TimeSamplesNumInPack;								// Количество импульсов в текущей части ДСП
    quint8 Reserved[1];
};

struct POIDistSpeedPortrait     {
    struct POIDistSpeedPortraitHeader POI_DSP_Header;
    short data[2*2512];
};

struct RawBlockDSP {
    POIDistSpeedPortraitHeader header;
    short *data;
};

struct AmpBlockDSP {
    POIDistSpeedPortraitHeader header;
    QVector<qreal> data;
};

struct FFTWInfo {
    FFTWInfo () : n(0), m(0), complexMatrix(nullptr), fftwPlan() {}

    quint32 n, m;

    fftw_complex *complexMatrix;
    fftw_plan fftwPlan;
};

using MapOfRawDSPData = QMap<quint32, RawBlockDSP>;
using VectorOfDSPData = QVector<AmpBlockDSP>;

class DSPWidget : public QWidget
{
    Q_OBJECT

public:
    DSPWidget(QWidget *parent = 0);
    ~DSPWidget();

signals:
    void setMaxCounter(quint32 counter);
    //void sendFrame(quint32 distSamles, quint32 timeSamples, QVector<float> data, quint32 indexFrame);

public slots:
    void selectFileSlot();
    void setDSPDataOnPlotter(int targetID, quint32 counter);

private:
    void calcAmp();
    void transposeMatrix(qint32 distSamples, qint32 timeSamples, int *in, int *out);
    qreal moduleOfComplexValue(qreal &real, qreal &imag);

private:
    CustomPlotterWidget *customWidget;
    MapOfRawDSPData mapOfRawData;
    VectorOfDSPData vectorOfData;
};

#endif // DSPWIDGET_H
