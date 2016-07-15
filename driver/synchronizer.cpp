#include <QMutexLocker>
#include "synchronizer.h"

#define SYNCH_BUFFER_LENGTH     100000
#define SYNCH_MINIMUM_LENGTH    200

Synchronizer::Synchronizer() :
    _synchIndx(0)
{
    _synchXBuffer = new double[SYNCH_BUFFER_LENGTH];
    _synchYBuffer = new double[SYNCH_BUFFER_LENGTH];
    reset();
}

Synchronizer::~Synchronizer()
{
    delete _synchXBuffer;
    delete _synchYBuffer;
}


void Synchronizer::reset()
{
    QMutexLocker locker(&_mutex);
    _synchIndx = 0;
}

double Synchronizer::computeOffset(double t0, double t1, double t2, double t3)
{
    QMutexLocker locker(&_mutex);

    unsigned int idx = _synchIndx % SYNCH_BUFFER_LENGTH;
    _synchYBuffer[idx] = (-(((t1 - t0) + (t2 - t3)) / 2));
    _synchXBuffer[idx] =  t0;
    _synchIndx++;
    return _synchYBuffer[idx];
}

int Synchronizer::_fit(const double *x, const double *y, unsigned int n, double *b, double *a)
{
    int     error = 0;
    double  sumx = 0,
            sumy = 0,
            sumx2 = 0,
            sumxy = 0;
    double  dn = (double) n;
    unsigned int  i;
    if (n <= 1) {
        *b = 0;
        *a = 0;
        error = 1;
    } else {
        double  divisor;
        error = 0;
        for (i = 0; i < n; i++) {
            sumx += x[i];
            sumy += y[i];
            sumx2 += (x[i] * x[i]);
            sumxy += (x[i] * y[i]);
        }
        divisor = (sumx2 - ((sumx * sumx) / dn));
        if (divisor != 0) {
            *b = (sumxy - ((sumx * sumy) / dn)) / divisor;
            *a = (sumy - ((*b) * sumx)) / dn;
        } else {
            *b = 0;
            *a = 0;
            error = 2;
        }
    }
    return(error);
}

double Synchronizer::computeFilteredOffset(double t)
{
    QMutexLocker locker(&_mutex);

    int nData = (_synchIndx > SYNCH_BUFFER_LENGTH) ? SYNCH_BUFFER_LENGTH : _synchIndx;
    double a;
    double b;
    if (nData < SYNCH_MINIMUM_LENGTH) // few values -> mean
    {
        double ret = 0.0;
        for(int i = 0; i < nData; i++)
        {
            ret += _synchYBuffer[i];
        }
        return (ret/nData);
    }
    else // linear fit
    {
        _fit(_synchXBuffer, _synchYBuffer, nData, &b, &a);
        return ((b * 1000 * t) + a);
    }
}

float Synchronizer::getDriftClock(int * measuredTime)
{
    QMutexLocker locker(&_mutex);

    int nData = (_synchIndx > SYNCH_BUFFER_LENGTH) ? SYNCH_BUFFER_LENGTH : _synchIndx;
    int tIni = _synchXBuffer[(_synchIndx - nData) % SYNCH_BUFFER_LENGTH];
    int tEnd = _synchXBuffer[(_synchIndx - 1) % SYNCH_BUFFER_LENGTH];
    *measuredTime = (tEnd - tIni) / 1000;
    if (nData < SYNCH_MINIMUM_LENGTH) // few values -> zero slope
    {
        return 0.0;
    }
    else
    {
        double a;
        double b;
        _fit(_synchXBuffer, _synchYBuffer, nData, &b, &a);

        return b;
    }
}
