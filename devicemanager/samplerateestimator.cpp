#include <QDebug>
#include "samplerateestimator.h"
#ifdef USE_APPLICATION_TIME // it uses LSL library
#include "applicationtime.h"
#else
#include <QDateTime>
#endif

#define AMOUNT_SAMPLES_NEW_CALCULATION (1000)


double fit(double * x, double * y, int ndata, int position)
// Given a set of data points y[1..ndata], fit them to a straight line
// y = a + bx by minimizing ?2. Returned is b.
{
    int i;
    double t,sxoss,sx=0.0,sy=0.0,st2=0.0,ss;
    int indx = position;
    double b = 0.0;

    for (i = 0;i < ndata; i++) { //...or without weights.
        sx += x[indx];
        sy += y[indx];
        indx = (indx + 1) % ndata;
    }
    ss=ndata;
    sxoss=sx/ss;

    indx = position;
    for (i = 0; i < ndata; i++) {
        t=x[indx]-sxoss;
        st2 += t*t;
        b += t*y[indx];
        indx = (indx + 1) % ndata;
    }
    b /= st2; //Solve for a, b, sa, and sb.
    return b;
}


SampleRateEstimator::SampleRateEstimator()
{
    reset();
}

void SampleRateEstimator::reset ()
{
    QMutexLocker locker(&_mutex);

    _currentSize = 0;
    _currentPosition = 0;
    _counterSamples = 0;
}

void SampleRateEstimator::newSample ()
{
    QMutexLocker locker(&_mutex);

    _counterSamples++;
    if (_counterSamples < (5 * AMOUNT_SAMPLES_NEW_CALCULATION))
    {
        return;
    }
    if ((_counterSamples % AMOUNT_SAMPLES_NEW_CALCULATION) == 0)
    {
#ifdef USE_APPLICATION_TIME
        double currentTime = ApplicationTime::currentTime() * 1000;
#else
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
#endif
        _bufferY[_currentPosition] = currentTime;
        _bufferX[_currentPosition] = _counterSamples;
        _currentPosition = (_currentPosition + 1) % BUFFER_SIZE;
        _currentSize = (_currentSize < BUFFER_SIZE) ? _currentSize + 1 : _currentSize;
        // debug
        //int firstSamplePosition = (_currentPosition < _currentSize) ? _currentPosition : 0;
        //float b = fit(_bufferX, _bufferY, _currentSize, firstSamplePosition);
        //qDebug() << "************ fit" << (1000/b);
    }
}


float SampleRateEstimator::getSampleRate (int * totalSamples)
{
    QMutexLocker locker(&_mutex);

    if (totalSamples != NULL)
    {
        *totalSamples = _counterSamples;
    }
    int firstSamplePosition = (_currentPosition < _currentSize) ? _currentPosition : 0;
    float b = fit(_bufferX, _bufferY, _currentSize, firstSamplePosition);
    return (1000/b);
}
