#ifndef FFTWINFO_H
#define FFTWINFO_H
#include <QtGlobal>
#include <fftw3.h>

struct FFTWInfo {
    FFTWInfo () : n(0), m(0), complexMatrix(nullptr), fftwPlan() {}

    quint32 n, m;

    fftw_complex *complexMatrix;
    fftw_plan fftwPlan;
};

#endif // FFTWINFO_H
