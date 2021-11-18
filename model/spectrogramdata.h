#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H

#include <QtGlobal>
#include <QVector>
#include <QtGlobal>

#include <qwt_matrix_raster_data.h>

class SpectrogramData : public QwtMatrixRasterData {

public:
    SpectrogramData();
    SpectrogramData(quint16 xMax, quint16 yMax);
    void RefreshMatrix();
    void UpdateMatrix(const quint32 &n, const quint32 &m, const QVector<qreal> &vector);
    void SetBackground();

    quint16 xMaxInterval = 32;
    quint16 yMaxInterval = 2512;
    const quint16 zMaxInterval = 100;
};

#endif // SPECTROGRAMDATA_H
