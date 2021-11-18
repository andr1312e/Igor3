#include "spectrogramdata.h"

SpectrogramData::SpectrogramData()
{
    setInterval( Qt::XAxis, QwtInterval( 0, xMaxInterval ) );
    setInterval( Qt::YAxis, QwtInterval( 0, yMaxInterval ) );
    setInterval( Qt::ZAxis, QwtInterval( 0.0, zMaxInterval ) );

    setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

    RefreshMatrix();
}

SpectrogramData::SpectrogramData(quint16 xMax, quint16 yMax) {
    setInterval( Qt::XAxis, QwtInterval( -10.0, xMax ) );
    setInterval( Qt::YAxis, QwtInterval( -10.0, yMax ) );
    setInterval( Qt::ZAxis, QwtInterval( 0.0, zMaxInterval ) );

    setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

    SetBackground();
}

void SpectrogramData::RefreshMatrix()
{
    static int x = 10;
    int y = x / 2;
    QVector<double> zVector;
    const quint32 col = xMaxInterval, row = yMaxInterval;
    const quint32 colAdd = 0, rowAdd = 0;
    const quint32 totalCol = col + colAdd,
            totalRow = row + rowAdd;

    quint32 step = 3;
    quint32 xLeftLimit = x - step, xRightLimit = x + step,
            yLeftLimit = y - step, yRightLimit = y + step;
    if (yRightLimit > totalRow) {
        x = 10;
        yRightLimit = row;
    }
    const quint32 sizeOfVector = totalCol * totalRow;//не используется
    for (quint32 currentRow = 0; currentRow < totalRow; currentRow++) {
        for (quint32 currentCol = 0; currentCol < totalCol; currentCol++) {
            if (xLeftLimit <= currentCol && currentCol <= xRightLimit) {
                if (yLeftLimit <= currentRow && currentRow <= yRightLimit) {
                    zVector.push_back(100 - (qrand() % 4) * 10);
                }
                else
                    zVector.push_back(10);
            }
            else
                zVector.push_back(10);
        }
    }
    x++;
//    for (quint32 index = 0; index < sizeOfVector; index++ ) {
//        zVector.push_back(qrand() % 100);
//    }

    setValueMatrix(zVector, totalCol);
}

void SpectrogramData::UpdateMatrix(const quint32 &n, const quint32 &m, const QVector<qreal> &vector)
{
    setInterval( Qt::XAxis, QwtInterval( -10.0, xMaxInterval ) );
    setInterval( Qt::YAxis, QwtInterval( -10.0, yMaxInterval ) );
    setValueMatrix(vector, n);
}

void SpectrogramData::SetBackground()
{
    const quint32 vecSize = 110;
    QVector<double> zVector(vecSize * vecSize, 0);

    setValueMatrix(zVector, vecSize);
}
