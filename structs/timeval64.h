#ifndef TIMEVAL64_H
#define TIMEVAL64_H
#include <QtGlobal>

typedef struct timeval64   {								// Время
    quint64 secs;                        // Cекунды
    quint64 usecs;                       // Микросекунды
}timeval64;


#endif // TIMEVAL64_H
