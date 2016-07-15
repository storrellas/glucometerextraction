#include <QStringList>
#include <QDebug>
#include "realtimefile.h"

RealTimeFile::RealTimeFile (QString fileName)
{
    _fileWithRealTimeSignal.setFileName(fileName);
}

RealTimeFile::~RealTimeFile ()
{
    if (_fileWithRealTimeSignal.isOpen())
    {
        _fileWithRealTimeSignal.close();
    }
}

int RealTimeFile::readSamples (int numberOfSamples, unsigned char * buffer,
                              int * size, unsigned char *channelInfo, bool *isLast)
{
    *isLast = false;
    if (!_fileWithRealTimeSignal.isOpen())
    {
        if (!_fileWithRealTimeSignal.open(QIODevice::ReadOnly))
        {
            return (-1);
        }
        _textStream.setDevice(&_fileWithRealTimeSignal);
    }
    int readSamples = 0;
    int index = 0;
    *channelInfo = 0;
    *size = 0;
    for (int i = 0; i < numberOfSamples; i++)
    {
        QString line = _textStream.readLine();
        if (line.isNull())
        {
            if(!_textStream.atEnd())
            {
                _fileWithRealTimeSignal.close();
                return (-1);
            }
            break;
        }
        QStringList samples = line.split("\t", QString::SkipEmptyParts);
        if (samples.size() < 8)
        {
            _fileWithRealTimeSignal.close();
            return (-2);
        }
        for (int j = 0; j < 8; j++)
        {
            QString sample = samples.at(j);
            if (sample.compare("*") == 0) // channel not reported
            {
                continue;
            }
            bool ok;
            int sampleValue;
            sampleValue = sample.toInt(&ok);
            if(!ok)
            {
                _fileWithRealTimeSignal.close();
                return (-3);
            }
            buffer[index++] = (unsigned char)((sampleValue & 0xff00) >> 8);
            buffer[index++] = (unsigned char)sampleValue;
            *channelInfo |= (1 << j);
        }
        readSamples++;
    }
    *size = index;
    if(_textStream.atEnd())
    {
        *isLast = true;
        readSamples = readSamples + 0x80;
    }
    return readSamples;
}

void RealTimeFile::setFileName (QString fileName)
{
    if (_fileWithRealTimeSignal.fileName().compare(fileName) != 0)
    {
        if (_fileWithRealTimeSignal.isOpen())
        {
            _fileWithRealTimeSignal.close();
        }
        _fileWithRealTimeSignal.setFileName(fileName);
    }
}
