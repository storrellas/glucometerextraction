#ifndef SAMPLERATECORRECTOR_H
#define SAMPLERATECORRECTOR_H

#include <QFile>

#define LENGTH_BUFFER_TIMESTAMPS    600 // 10 minutes

class SampleRateCorrector
{
public:
    SampleRateCorrector(double sampleRate);
    ~SampleRateCorrector();
    signed int newSample ();
    void reset ();
    double getActualSampleRate ();
private:
    int _sampleRate;
    unsigned int _counterSamplesTotal;
    unsigned int _counterSamples;
    unsigned int _counterSamplesCorrected;
    unsigned int _minSamplesBeforeStarting;
    unsigned int _maxNumberSamples;
    unsigned long long _timeFirstSample;
    int _currentUpdateSamplesRatio;
    unsigned int _counterUpdate;
    unsigned int _indexTimestampFirstSample;
    unsigned int _indexTimestampCurrentSample;
    long long _timestamps[LENGTH_BUFFER_TIMESTAMPS];

    QFile _logFile;
};

#endif // SAMPLERATECORRECTOR_H
