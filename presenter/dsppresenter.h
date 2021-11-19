#ifndef DSPPRESENTER_H
#define DSPPRESENTER_H
#include <QObject>
#include <QHash>
#include <QDataStream>
#include <QFile>
#include <QDateTime>
#include <QtMath>

#include "structs/RawBlockDSP.h"
#include "structs/POIDistSpeedPortrait.h"
#include "structs/AmpBlockDSP.h"
#include "structs/fftwinfo.h"

using MapOfRawDSPData = QHash<quint32, RawBlockDSP>;
using VectorOfDSPData = QVector<AmpBlockDSP>;

class DspPresenter: public QObject
{
    Q_OBJECT
public:
    explicit DspPresenter(QObject *parent);
    ~DspPresenter();
Q_SIGNALS:
    void ToSetSliderLimit(quint32 limit);
    void ToSetDSPDataOnPlotter(quint32 targetId, quint32 counter);
public:
    void ReadDspFromFile(const QString &fileName);
    quint32 GetVectorOfDataElementsCount();
    const AmpBlockDSP GetElementOfData(int elementIndex);
private:
    void CalculateAmplitude();
    static qreal AbsoluteOfComplexValue(qreal &real, qreal &imaginary) ;
private:
    MapOfRawDSPData m_mapOfRawData;
    VectorOfDSPData m_vectorOfData;
};

#endif // DSPPRESENTER_H
