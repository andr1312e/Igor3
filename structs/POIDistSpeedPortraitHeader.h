#ifndef POIDISTSPEEDPORTRAITHEADER_H
#define POIDISTSPEEDPORTRAITHEADER_H

#include <QtGlobal>

#include "structs/timeval64.h"

struct POIDistSpeedPortraitHeader	{							// Заголовок дальностно-скоростного портрета
    timeval64 sTimeMeasurement;                                 // Время формирования ДСП
    int DistSamplesNum;											// Количество дальностных отсчетов
    int TimeSamplesNum;											// Количество импульсов в луче
    float Sensor_Azm;											// Значение датчика азимута
    float Sensor_Ugm;											// Значение датчика угла места
    int Value4Rmo;												// Значение для РМО//targetId
    quint32 RayCounter;											// Счетчик лучей ПОИ
    quint8 LaunchMode;											// Номер запуска
    quint8 RayNum_Azm;											// Номер луча по азимуту
    quint8 RayNum_Ugm;											// Номер луча по углу места
    quint8 TotalPartsNum;										// Общее количество передаваемых частей ДСП
    quint8 CurrentPartNum;										// Номер передаваемой части ДСП
    quint16 TimeSamplesNumInPack;								// Количество импульсов в текущей части ДСП
    quint8 Reserved[1];
};


#endif // POIDISTSPEEDPORTRAITHEADER_H
