#ifndef REGFILEHEADER_H
#define REGFILEHEADER_H
#include <sys/time.h>

struct RegFileHeader {
    timeval sTimeMeasurement;           // Время начала регистрации в этом файле
    unsigned int partNumber;            // Номер изделия
    unsigned int operatorId;            // Идентификатор оператора
    unsigned int operationMode;         // Режим работы (имитация, реальная работа)
    unsigned int softwareVersion;       // Версия ПО (версия файла)
    unsigned int reserve1;              // Резерв
    unsigned int reserve2;              // Резерв
};
#endif // REGFILEHEADER_H
