#include "dsppresenter.h"


DspPresenter::DspPresenter(QObject *parent)
    :QObject(parent)
{

}

DspPresenter::~DspPresenter()
{

}

void DspPresenter::ReadDspFromFile(const QString &fileName)
{

    if (fileName.isEmpty())
        return;
    m_vectorOfData.clear();
    m_mapOfRawData.clear();

    QFile dspFile(fileName);
    if (!dspFile.open(QIODevice::ReadOnly)) {
        qDebug() << "DSPWidget::selectFileSlot - Can't open dsp data file";
        return;
    }

    QString endStr(fileName.mid(fileName.size() - 3, 3));
    bool isBinFile = true;
    if (endStr == QString("dsp"))
    {
        isBinFile = false;
    }

    QByteArray data(dspFile.readAll());
    int sizeOfByteArray = data.size();
    qint64 currentPosInByteArray = 0;

    dspFile.close();

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_3);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    qint64 counter = 0;
    RawBlockDSP rawBlock;
    quint32 sizeDSPBlock = 0, currentPositionInBlock = 0;

    quint16 size;
    quint8 id;
    timeval tv1;

    if (isBinFile) {

        struct RegFileHeader {
            timeval sTimeMeasurement;           // Время начала регистрации в этом файле
            unsigned int partNumber;            // Номер изделия
            unsigned int operatorId;            // Идентификатор оператора
            unsigned int operationMode;         // Режим работы (имитация, реальная работа)
            unsigned int softwareVersion;       // Версия ПО (версия файла)
            unsigned int reserve1;              // Резерв
            unsigned int reserve2;              // Резерв
        };

        RegFileHeader header;
        in >> size;
        in.readRawData((char *) &header, sizeof(RegFileHeader));
    }

    QVector<POIDistSpeedPortraitHeader> vectorOfPOIDist;
    QSet<quint8> numberOfBlock;
    bool skipFrame = false;

    while (!in.atEnd()) {
        if (isBinFile) {
            in.readRawData((char *) &tv1, sizeof(timeval));
            in >> size;
            in >> id;
        }

        if (id == 54 || !isBinFile) {
            POIDistSpeedPortrait msg;
            in.readRawData(reinterpret_cast<char *>(&msg), sizeof(POIDistSpeedPortrait));
            vectorOfPOIDist.append(msg.POI_DSP_Header);
            counter++;

            auto numberItem = numberOfBlock.find(msg.POI_DSP_Header.CurrentPartNum);
            if (numberItem != numberOfBlock.end()) {
                if (sizeDSPBlock == currentPositionInBlock) {
                    m_mapOfRawData.insert(counter, rawBlock);
                }
                else {
                    if (!skipFrame)
                        delete[] rawBlock.data;
                }
                currentPositionInBlock = 0;
                numberOfBlock.clear();
                if (msg.POI_DSP_Header.CurrentPartNum != 0) {
                    skipFrame = true;
                }
                else {
                    skipFrame = false;
                    rawBlock.header = msg.POI_DSP_Header;
                    //counter = QDateTime::currentSecsSinceEpoch();

                    sizeDSPBlock = 2 * msg.POI_DSP_Header.DistSamplesNum * msg.POI_DSP_Header.TimeSamplesNum;
                    rawBlock.data = new short[sizeDSPBlock];
                }
            }
            else {
                if (msg.POI_DSP_Header.CurrentPartNum == 0) {
                    skipFrame = false;
                    numberOfBlock.clear();
                    currentPositionInBlock = 0;

                    rawBlock.header = msg.POI_DSP_Header;
                    //counter = QDateTime::currentSecsSinceEpoch();

                    sizeDSPBlock = 2 * msg.POI_DSP_Header.DistSamplesNum * msg.POI_DSP_Header.TimeSamplesNum;
                    rawBlock.data = new short[sizeDSPBlock];
                }
                else {
                    if (numberOfBlock.isEmpty())
                        skipFrame = true;
                }
            }
            numberOfBlock.insert(msg.POI_DSP_Header.CurrentPartNum);

            if (!skipFrame) {
                //quint32 currentSizeData = 2 * msg.POI_DSP_Header.TimeSamplesNumInPack; //new version
                quint32 currentSizeData = 2 * msg.POI_DSP_Header.TimeSamplesNumInPack * msg.POI_DSP_Header.DistSamplesNum;
                for (quint32 index = 0; index != currentSizeData; index++, currentPositionInBlock++) {
                    rawBlock.data[currentPositionInBlock] = msg.data[index];
                }
            }
        }
        else {
            in.skipRawData(size - 1);
        }

        currentPosInByteArray = in.device()->pos();

    };

    if (sizeDSPBlock == currentPositionInBlock) {
//        if (counter != 0)
        m_mapOfRawData.insert(counter, rawBlock);
    }
    else {
        delete[] rawBlock.data;
    }
    qDebug()<< "кадров" << m_mapOfRawData.size();
     CalculateAmplitude();
    Q_EMIT ToSetSliderLimit(m_mapOfRawData.count());




}

//void DspPresenter::ReadDspFromDspFile(const QString &fileName)
//{
//    QFile dspFile(fileName);
//    if (!dspFile.open(QIODevice::ReadOnly)) {
//        qDebug() << QStringLiteral("Не может отпрыть дсп по дсп файу");
//        return;
//    }

//    QByteArray data(dspFile.readAll());

//    QDataStream in(&data, QIODevice::ReadOnly);
//    in.setVersion(QDataStream::Qt_5_3);
//    in.setByteOrder(QDataStream::LittleEndian);
//    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

//    qint64 counter = 0;
//    RawBlockDSP rawBlock;
//    quint32 sizeDSPBlock = 0, currentPositionInBlock = 0;

//    while (!in.atEnd()) {
//        POIDistSpeedPortrait msg;
//        in.readRawData(reinterpret_cast<char *>(&msg), sizeof(POIDistSpeedPortrait));

//        if (msg.POI_DSP_Header.CurrentPartNum == 0) {
//            if (sizeDSPBlock == currentPositionInBlock) {
//                if (counter != 0) {
//                    m_mapOfRawData.insert(counter, rawBlock);
//                    currentPositionInBlock = 0;
//                }
//            }
//            else {
//                delete[] rawBlock.data;
//                currentPositionInBlock = 0;
//            }

//            rawBlock.header = msg.POI_DSP_Header;
//            counter = QDateTime::currentSecsSinceEpoch();

//            sizeDSPBlock = 2 * msg.POI_DSP_Header.DistSamplesNum * msg.POI_DSP_Header.TimeSamplesNum;
//            rawBlock.data = new short[sizeDSPBlock];
//        }

//        quint32 currentSizeData = 2 * msg.POI_DSP_Header.TimeSamplesNumInPack * msg.POI_DSP_Header.DistSamplesNum;
//        for (quint32 index = 0; index != currentSizeData; index++, currentPositionInBlock++) {
//            rawBlock.data[currentPositionInBlock] = msg.data[index];
//        }

//    };

//    if (sizeDSPBlock == currentPositionInBlock) {
//        if (counter != 0)
//            m_mapOfRawData.insert(counter, rawBlock);
//    }
//    else {
//        delete[] rawBlock.data;
//    }

//    dspFile.close();
//}

//void DspPresenter::ReadDspFromBinFile(const QString &fileName)
//{
//    QFile dspFile(fileName);
//    if (!dspFile.open(QIODevice::ReadOnly)) {
//        qDebug() << QStringLiteral("Не может отпрыть дсп по бин файу");
//        return;
//    }
//    QByteArray data(dspFile.readAll());
//    dspFile.close();
//    QDataStream in(&data, QIODevice::ReadOnly);
//    in.setVersion(QDataStream::Qt_5_3);
//    in.setByteOrder(QDataStream::LittleEndian);
//    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

//    qint64 counter = 0;
//    RawBlockDSP rawBlock;
//    quint32 sizeDSPBlock = 0, currentPositionInBlock = 0;

//    struct RegFileHeader {
//        timeval sTimeMeasurement;           // Время начала регистрации в этом файле
//        unsigned int partNumber;            // Номер изделия
//        unsigned int operatorId;            // Идентификатор оператора
//        unsigned int operationMode;         // Режим работы (имитация, реальная работа)
//        unsigned int softwareVersion;       // Версия ПО (версия файла)
//        unsigned int reserve1;              // Резерв
//        unsigned int reserve2;              // Резерв
//    };

//    quint16 size;
//    quint8 id;

//    RegFileHeader header;
//    in >> size;
//    in.readRawData((char *) &header, sizeof(RegFileHeader));
//    timeval tv1;

//    while (!in.atEnd()) {
//        in.readRawData((char *) &tv1, sizeof(timeval));
//        in >> size;
//        in >> id;

//        if (id == 54) {
//            qDebug()<< "Нашли сообщение с нужным ид: " << id;
//            POIDistSpeedPortrait msg;
//            in.readRawData(reinterpret_cast<char *>(&msg), sizeof(POIDistSpeedPortrait));

//            if (msg.POI_DSP_Header.CurrentPartNum == 0)
//            {
//                if (sizeDSPBlock == currentPositionInBlock)
//                {
//                    if (counter != 0)
//                    {
//                        m_mapOfRawData.insert(counter, rawBlock);
//                        currentPositionInBlock = 0;
//                    }
//                }
//                else {
//                    delete[] rawBlock.data;
//                    currentPositionInBlock = 0;
//                }

//                rawBlock.header = msg.POI_DSP_Header;
//                counter = QDateTime::currentSecsSinceEpoch();

//                sizeDSPBlock = 2 * msg.POI_DSP_Header.DistSamplesNum * msg.POI_DSP_Header.TimeSamplesNum;
//                rawBlock.data = new short[sizeDSPBlock];
//            }

//            msg.POI_DSP_Header.TimeSamplesNumInPack = 1;
//            quint32 currentSizeData = 2 * msg.POI_DSP_Header.TimeSamplesNumInPack * msg.POI_DSP_Header.DistSamplesNum;
//            for (quint32 index = 0; index != currentSizeData; index++, currentPositionInBlock++) {
//                rawBlock.data[currentPositionInBlock] = msg.data[index];
//            }
//        }
//        else {
//            qDebug()<< "ид: " << id;
//            in.skipRawData(size - 1);
//        }

//    };

//    if (sizeDSPBlock == currentPositionInBlock) {
//        if (counter != 0)
//            m_mapOfRawData.insert(counter, rawBlock);
//    }
//    else {
//        delete[] rawBlock.data;
//    }
//}

quint32 DspPresenter::GetVectorOfDataElementsCount()
{
    return (quint32)m_vectorOfData.count();
}

const AmpBlockDSP DspPresenter::GetElementOfData(int elementIndex)
{
    if(elementIndex>=0 && elementIndex<m_vectorOfData.count())
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

    Q_EMIT ToSetDSPDataOnPlotter(0);
}

void DspPresenter::ClearRepository()
{
    m_mapOfRawData.clear();
    m_vectorOfData.clear();
}

qreal DspPresenter::AbsoluteOfComplexValue(qreal &real, qreal &imaginary)
{
    return qSqrt(real * real + imaginary * imaginary);
}
