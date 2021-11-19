#include "dsppresenter.h"

#include <QMessageBox>

DspPresenter::DspPresenter(QObject *parent)
    :QObject(parent)
{

}

DspPresenter::~DspPresenter()
{

}

void DspPresenter::ReadDspFromFile(const QString &fileName)
{
    QFile dspFile(fileName);
    if (!dspFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "MainWidget::OnFileSelect() - Не можем открыть файл", dspFile.errorString());
        return;
    }
    else
    {
        QFile dspFile(fileName);
        QByteArray data(dspFile.readAll());
        dspFile.close();

        QDataStream in(&data, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_3);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);

        qint64 counter = 0;
        RawBlockDSP rawBlock;
        quint32 sizeDSPBlock = 0, currentPositionInBlock = 0;

        while (!in.atEnd())
        {
            POIDistSpeedPortrait msg;
            in.readRawData(reinterpret_cast<char *>(&msg), sizeof(POIDistSpeedPortrait));

            if (msg.POI_DSP_Header.CurrentPartNum == 0)
            {
                if (sizeDSPBlock == currentPositionInBlock)
                {
                    if (counter != 0)
                    {
                        m_mapOfRawData.insert(counter, rawBlock);
                        currentPositionInBlock = 0;
                    }
                }
                else
                {
                    delete[] rawBlock.data;
                    currentPositionInBlock = 0;
                }

                rawBlock.header = msg.POI_DSP_Header;
                counter = QDateTime::currentSecsSinceEpoch();

                sizeDSPBlock = 2 * msg.POI_DSP_Header.DistSamplesNum * msg.POI_DSP_Header.TimeSamplesNum;
                rawBlock.data = new short[sizeDSPBlock];
            }

            quint32 currentSizeData = 2 * msg.POI_DSP_Header.TimeSamplesNumInPack * msg.POI_DSP_Header.DistSamplesNum;
            for (quint32 index = 0; index != currentSizeData; index++, currentPositionInBlock++)
            {
                rawBlock.data[currentPositionInBlock] = msg.data[index];
            }

        };

        if (sizeDSPBlock == currentPositionInBlock)
        {
            if (counter != 0)
            {
                m_mapOfRawData.insert(counter, rawBlock);
            }
        }
        else
        {
            delete[] rawBlock.data;
        }
        Q_EMIT ToSetSliderLimit((quint32)m_mapOfRawData.count());

        CalculateAmplitude();
    }
}

quint32 DspPresenter::GetVectorOfDataElementsCount()
{
    return (quint32)m_vectorOfData.count();
}

const AmpBlockDSP DspPresenter::GetElementOfData(int elementIndex)
{
    if(elementIndex<0 || elementIndex>m_vectorOfData.count())
    {
        return m_vectorOfData.at(elementIndex);
    }
    else
    {
        Q_UNREACHABLE();
    }
}

void DspPresenter::CalculateAmplitude()
{
    quint16 lastTimeSamples = 0;
    m_vectorOfData.clear();
    FFTWInfo fftwInfo;
    for (RawBlockDSP &item : m_mapOfRawData)
    {
        auto &dSamples = item.header.DistSamplesNum, &tSamples = item.header.TimeSamplesNum;
        AmpBlockDSP resultBlock;
        resultBlock.header = item.header;

        if (lastTimeSamples == 0 || lastTimeSamples != tSamples)
        {
            fftwInfo.complexMatrix = fftw_alloc_complex(tSamples);
            fftwInfo.fftwPlan = fftw_plan_dft_1d(tSamples,
                                                 fftwInfo.complexMatrix, fftwInfo.complexMatrix,
                                                 FFTW_FORWARD, FFTW_ESTIMATE);
            lastTimeSamples = tSamples;
        }
        for (quint16 indexOfDist = 0; indexOfDist != dSamples; indexOfDist++)
        {

            for (int index = 0, indexFFT = 0; index != tSamples; index++, indexFFT++)
            {
                fftwInfo.complexMatrix[indexFFT][0] = item.data[2 * (dSamples * index + indexOfDist)];
                fftwInfo.complexMatrix[indexFFT][1] = item.data[2 * (dSamples * index + indexOfDist) + 1];
            }

            fftw_execute(fftwInfo.fftwPlan);

            for (int index = 0; index != tSamples; index++)
            {
                qreal result = AbsoluteOfComplexValue(fftwInfo.complexMatrix[index][0],
                        fftwInfo.complexMatrix[index][1]);
                result = 20. * log10(result);
                resultBlock.data.append(result);
            }
        }

        m_vectorOfData.push_back(resultBlock);
    }

    fftw_destroy_plan(fftwInfo.fftwPlan);
    fftw_free(fftwInfo.complexMatrix);

    Q_EMIT ToSetDSPDataOnPlotter(0, 0);
}

qreal DspPresenter::AbsoluteOfComplexValue(qreal &real, qreal &imaginary)
{
    return qSqrt(real * real + imaginary * imaginary);
}
