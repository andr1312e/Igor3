#ifndef DSPPRESENTER_H
#define DSPPRESENTER_H
#include <QObject>
#include <QHash>
#include <QDataStream>
#include <QFile>
#include <QDateTime>
#include <QtMath>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "structs/RegFileHeader.h"
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
    void ToStartMovie();
    void ToSetDSPDataOnPlotter(int frame);
public:
    void ReadDspFromFile(const QString &fileName);
    quint32 GetVectorOfDataElementsCount();
    const AmpBlockDSP GetElementOfData(int elementIndex);
private:
    void CalculateAmplitude();
    void ClearRepository();
    static qreal AbsoluteOfComplexValue(qreal &real, qreal &imaginary) ;
private:
    MapOfRawDSPData m_mapOfRawData;
    VectorOfDSPData m_vectorOfData;
};

#endif // DSPPRESENTER_H
