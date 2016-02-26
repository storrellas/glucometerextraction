#include <QDebug>
#include <QDateTime>

#include "sampleratecorrector.h"

SampleRateCorrector::SampleRateCorrector(double sampleRate) :
        _logFile("LogSampleRateCorrector.txt")
{
    _sampleRate = (int)sampleRate;
    //_minSamplesBeforeStarting = sampleRate * 15; // 15 secs
    _minSamplesBeforeStarting = sampleRate * 30; // 15 secs
    reset ();
}

SampleRateCorrector::~SampleRateCorrector()
{
    //_logFile.close();
}

void SampleRateCorrector::reset ()
{
    _counterSamplesTotal = 0;
    _counterSamples = 0;
    _counterSamplesCorrected = 0;
    _counterUpdate = 0;
    _currentUpdateSamplesRatio = 0;
    _indexTimestampFirstSample = 0;
    _indexTimestampCurrentSample = 0;
    _maxNumberSamples = (LENGTH_BUFFER_TIMESTAMPS - 1) * _sampleRate;

    //_logFile.close();
    //_logFile.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
}

signed int SampleRateCorrector::newSample ()
{
    signed int ret = 0;
    static double actualSampleRate = 0;
    static double correctedSampleRate = 0;
    if (_counterSamplesTotal == 0) // First sample
    {
        _indexTimestampFirstSample = 0;
        _indexTimestampCurrentSample = 1;
        _timestamps[_indexTimestampFirstSample] = QDateTime::currentMSecsSinceEpoch();
        _timeFirstSample = _timestamps[_indexTimestampFirstSample];
    }
    _counterSamplesTotal++;
    _counterSamplesCorrected++;
    if(_counterSamples < _maxNumberSamples)
        _counterSamples++;

    if ((_counterSamplesTotal % _sampleRate) == 0)
    {
        //qDebug() << "counterSamples" << _counterSamples;
        _timestamps[_indexTimestampCurrentSample] = QDateTime::currentMSecsSinceEpoch();
        //qDebug() << "ini" << _indexTimestampFirstSample << "curr" << _indexTimestampCurrentSample;
        /*double*/ actualSampleRate = 1000 * (double)_counterSamples /
                                  (_timestamps[_indexTimestampCurrentSample] -
                                   _timestamps[_indexTimestampFirstSample]);
        //qDebug() << "actualSampleRate" << actualSampleRate;
        /*double */correctedSampleRate = 1000 * (double)_counterSamplesCorrected /
                                   (_timestamps[_indexTimestampCurrentSample] -
                                    _timeFirstSample);
        //qDebug() << "correctedSampleRate" << correctedSampleRate;
        double drifft = _sampleRate - actualSampleRate;
        if (drifft == 0)
        {
            qDebug() << "SampleRateCorrector::newSample 0-division avoided";
            drifft = 0.0000000001;
        }
        double updateSamplesRatio = 1 / drifft * _sampleRate;
        _currentUpdateSamplesRatio = qRound(updateSamplesRatio);
        //qDebug() << "currentUpdateSamplesRatio" << _currentUpdateSamplesRatio;
        if(++_indexTimestampCurrentSample >= LENGTH_BUFFER_TIMESTAMPS)
        {
            _indexTimestampCurrentSample = 0;
        }
        if (_indexTimestampCurrentSample == _indexTimestampFirstSample)
        {
            if(++_indexTimestampFirstSample >= LENGTH_BUFFER_TIMESTAMPS)
            {
                _indexTimestampFirstSample = 0;
            }
        }
    }
    _counterUpdate++;
    if (_counterUpdate >= abs(_currentUpdateSamplesRatio))
    {
        if (_currentUpdateSamplesRatio != 0)
        {
            if (_currentUpdateSamplesRatio >  0)
            {
                if(_counterSamplesTotal >= _minSamplesBeforeStarting)
                {
                    _counterSamplesCorrected++;
                }
                ret = 1;
            }
            else
            {
                if(_counterSamplesTotal >= _minSamplesBeforeStarting)
                {
                    _counterSamplesCorrected--;
                }
                ret = -1;
            }
        }
        _counterUpdate = 0;
    }

    if(_counterSamplesTotal < _minSamplesBeforeStarting)
        return 0;

    if (ret != 0)
    {
        int indexCurrentS = _indexTimestampCurrentSample - 1;
        if (indexCurrentS < 0) indexCurrentS = LENGTH_BUFFER_TIMESTAMPS - 1;
        //QString s;
        //s = QString::number(QDateTime::currentMSecsSinceEpoch()) + "\t" +
        //    QString::number(_currentUpdateSamplesRatio) + "\t" +
        //    QString::number(ret) + "\t" + QString::number(actualSampleRate) +
        //    "\t" + QString::number(correctedSampleRate) + "\t" +
        //    QString::number(_timestamps[indexCurrentS]) + "\t" + QString::number(indexCurrentS) + "\t" +
        //    QString::number(_timestamps[_indexTimestampFirstSample]) + "\n";
        //_logFile.write(s.toAscii());
    }

    return ret;
}

double  SampleRateCorrector::getActualSampleRate ()
{
    unsigned int indexLastTimestamp;
    if (_indexTimestampCurrentSample == 0)
    {
        indexLastTimestamp = (LENGTH_BUFFER_TIMESTAMPS - 1);
    }
    else
    {
        indexLastTimestamp = (_indexTimestampCurrentSample - 1);
    }
    unsigned int samples = _counterSamplesTotal -
                                    (_counterSamplesTotal % _sampleRate);

    return (1000 * (double)samples / (_timestamps[indexLastTimestamp] -
                      _timeFirstSample));
}

