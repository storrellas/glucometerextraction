#ifndef SAMPLERATEESTIMATOR_H
#define SAMPLERATEESTIMATOR_H
#include <QMutex>

#define BUFFER_SIZE 2400

class SampleRateEstimator
{
public:
    SampleRateEstimator();

    void reset ();
    void newSample ();
    float getSampleRate (int * totalSamples);
private:
    int _currentSize;
    int _currentPosition;
    int _counterSamples;
    double _bufferY[BUFFER_SIZE];
    double _bufferX[BUFFER_SIZE];
    QMutex _mutex;
};

#endif // SAMPLERATEESTIMATOR_H
