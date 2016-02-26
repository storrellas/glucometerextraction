#include <QDir>
#include <QtCore>
#include <QTextStream>

#include "filewriter.h"
#include "EnobioData.h"



const QString FileWriter::strErrorOnOpening =
        "Error opening the recording file. Check that the file is not already "
        "in use and that the output directory exists.";

const QString FileWriter::strErrorOnWriting =
        "Error writing to the recording file. Check that you have enough space"
        " in the disk.";

FileWriter::FileWriter (bool start, QString fileName, QString outDir) :
    //_newDataForFile(this),
    _outDir(outDir),
    _fileID(fileName),
    _handleFile(_outDir + _fileID),
    _handleStimFile(_outDir +_fileID),
    _handleBinaryFile(_outDir +_fileID),
    //_appendTimestamp(appendTimestamp),
    //_trigger(0)
    //_triggersMutex(QMutex::Recursive),
    _isPreEEG(false),
    _isPostEEG(false),
    _isRecordingAdditionalChannel(false),
    _isRecordingAccelerometer(false),
    _firstEEGTimeStamp(0),
    _firstStimTimeStamp(0),
    _firstAccelerometerTimeStamp(0),
    _numberOfEEGSamples(0),
    _isStimulating(0),
    _isEEGNotesAfterStimPending(0)
{

//    _isRecordingNEDF = false;
//    _isRecordingSTIM = false;
//    _isRecordingEASY = false;

    if (start)
        startWriting();

    _currentStatus=NO_ERROR_IN_FILE_WRITER;

    dataToWrite.clear();
    stimDataToWrite.clear();
    triggerToWrite.clear();
    accelerometerToWrite.clear();
    additionalToWrite.clear();
    binaryEEGDataToWrite.clear();
    binaryStimDataToWrite.clear();
    binaryTriggerToWrite.clear();
    binaryAccelerometerDataToWrite.clear();
    _handleStimFile.setFileName("");

    //AdditionaldataToWrite.clear();
    //AccelerometerDataToWrite.clear();
}

FileWriter::~FileWriter ()
{
    if (isWriting())
        stopWriting();
}

bool FileWriter::startWriting ()
{
    if (isWriting())
    {
        return true;
    }

    _firstEEGTimeStamp=0;
    _firstAccelerometerTimeStamp =0;
    _numberOfEEGSamples = 0;
    counterAcc = 1;

    //incrementEEGIndex = false;
    writeEEG = false;

    //incrementStimIndex = false;

    _countTrigger255=0;
    _countTrigger254=0;

    dataToWrite.clear();
    triggerToWrite.clear();
    accelerometerToWrite.clear();
    additionalToWrite.clear();
    binaryTriggerToWrite.clear();
    binaryEEGDataToWrite.clear();
    binaryAccelerometerDataToWrite.clear();

    _lastAccelerometerValue[0] = 0;
    _lastAccelerometerValue[1] = 0;
    _lastAccelerometerValue[2] = 0;

    _latestAccelerometerWritten.setData(0, 0);
    _latestAccelerometerWritten.setData(1, 0);
    _latestAccelerometerWritten.setData(2, 0);

    _lastAdditionalValue = 0;

    //_previousTextLine = "";

    //AdditionaldataToWrite.clear();
    //AccelerometerDataToWrite.clear();

    if (_outDir.lastIndexOf("/")+1 != _outDir.length())
        _outDir.append("/");

    QString fileName = _outDir + _fileID;
    //QString outDir = _outDir;
    if (_isPreEEG)
    {
        int posFileName = fileName.lastIndexOf("/");
        // If the separator was not found (-1) then the position is set to 0
        // Otherwise the position is set after the separator.
        posFileName++;

        fileName.insert(posFileName, "PreEEG_");
    }

    if (_isPostEEG)
    {
        int posFileName = fileName.lastIndexOf("/");
        // If the separator was not found (-1) then the position is set to 0
        // Otherwise the position is set after the separator.
        posFileName++;

        fileName.insert(posFileName, "PostEEG_");
    }

    _dateTime = QDateTime::currentDateTime();
    //if (_appendTimestamp)
    //We always append timestamp
    {
        int posFileName = fileName.lastIndexOf("/");
        // If the separator was not found (-1) then the position is set to 0
        // Otherwise the position is set after the separator.
        posFileName++;
        QString format("yyyyMMddHHmmss_");
        fileName.insert(posFileName, _dateTime.toString(format));
        _rootFilename = _fileID;
        _rootFilename.insert(0, _dateTime.toString(format));
    }


    QString easyFileName = fileName;
    if (!easyFileName.endsWith(".easy"))
        easyFileName.append(".easy");

    _handleFile.setFileName(easyFileName);
    //_handleTrigger.setFileName(fileName + ".trigg");


    if (_isRecordingEASY)
    {
        qDebug()<<"FileWriter::startWriting _isRecordingEASY"<<_isRecordingEASY<<easyFileName;
        if (!_handleFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug()<<"Error opening file"<<easyFileName;
            _currentStatus=ERROR_ON_OPENING;
            emit statusChanged(FileWriter::ERROR_ON_OPENING);
            return false;
        }

        _currentStatus=NO_ERROR_IN_FILE_WRITER;
    }

    QString binaryFilename = fileName;
    if (!binaryFilename.endsWith(".nedf"))
        binaryFilename.append(".nedf");

    _handleBinaryFile.setFileName(binaryFilename);

    if (_isRecordingNEDF)
    {
        if (!_handleBinaryFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug()<<"Error opening binary file"<<binaryFilename;
            _currentStatus=ERROR_ON_OPENING;
            emit statusChanged(FileWriter::ERROR_ON_OPENING);
            return false;
        }

        _handleBinaryFile.seek(5120);
        _currentStatus=NO_ERROR_IN_FILE_WRITER;
    }

    return true;
}

void FileWriter::setRecordingNEDFFile (bool recordNEDF)
{
    _isRecordingNEDF = recordNEDF;
}

void FileWriter::setRecordingEASYFile (bool recordEASY)
{
    _isRecordingEASY = recordEASY;
}

void FileWriter::setRecordingSTIMFile (bool recordSTIM)
{
    _isRecordingSTIM = recordSTIM;
}


bool FileWriter::startWritingStimData ()
{
    if (isWritingStimData())
        return true;

    stimDataToWrite.clear();

    _firstStimTimeStamp=0;
#ifndef NICBENCHMARK
    qDebug()<<"_firstStimTimeStamp"<<_firstStimTimeStamp;
#endif
    writeStim = false;
    binaryStimDataToWrite.clear();

    _numberOfStimSamples = 0;

    //QString fileName = _handleFile.fileName();

    if (_outDir.lastIndexOf("/")+1 != _outDir.length())
        _outDir.append("/");

    QString fileName = _outDir + _fileID;

    int posFileName = fileName.lastIndexOf("/");
    // If the separator was not found (-1) then the position is set to 0
    // Otherwise the position is set after the separator.
    posFileName++;
    QString format("yyyyMMddHHmmss_");
    fileName.insert(posFileName, QDateTime::currentDateTime().toString(format));

    //fileName.chop(3);
    fileName=fileName.append(".stim");

    _handleStimFile.setFileName(fileName);

    if (_isRecordingSTIM)
    {
        if (!_handleStimFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug()<<"Error opening file"<<fileName;
            _currentStatus=ERROR_ON_OPENING;
            emit statusChanged(FileWriter::ERROR_ON_OPENING);
            return false;
        }

        _currentStatus=NO_ERROR_IN_FILE_WRITER;
    }
#ifndef NICBENCHMARK
    qDebug()<<"FileWriter::startWritingStimData"<<fileName;
#endif

    return true;
}

int FileWriter::getNumberOfStimSamples()
{
    return _numberOfStimSamples;
}

//void FileWriter::setNumberOfStimSamples(int numberOfSamples)
//{
//    _numberOfStimSamples=numberOfSamples;
//}

int FileWriter::getNumberOfEEGSamples()
{
    qDebug()<<"FileWriter::getNumberOfEEGSamples()"<<_numberOfEEGSamples;
    return _numberOfEEGSamples;
}

void FileWriter::setNumberOfEEGSamples(int numberOfSamples)
{
    _numberOfEEGSamples=numberOfSamples;
}

unsigned long long FileWriter::getFirstAccelerometerTimestamp()
{
    return _firstAccelerometerTimeStamp;
}

unsigned long long FileWriter::getFirstEEGTimestamp()
{
    return _firstEEGTimeStamp;
}

unsigned long long FileWriter::getFirstStimTimestamp()
{
    return _firstStimTimeStamp;
}

void FileWriter::setFirstStimTimestamp(unsigned long long firstTimestamp)
{
    _firstStimTimeStamp=firstTimestamp;
}

void FileWriter::stopWriting (){


    QtMessageHandler handler = qInstallMessageHandler(0);


    QMutexLocker locker(&_mutex);

    int remainingEEGSamples = dataToWrite.size();
    int remainingEEGBinarySamples = binaryEEGDataToWrite.size();
    int remainingStimBinarySamples = binaryStimDataToWrite.size();
    int remainingStimSamples = stimDataToWrite.size();

    qDebug() << "FileWriter::stopWriting dataToWrite" << remainingEEGSamples;
    qDebug() << "FileWriter::stopWriting EEGbinaryDataToWrite" << remainingEEGBinarySamples;
    qDebug() << "FileWriter::stopWriting StimbinaryDataToWrite" << remainingStimBinarySamples;
    qDebug() << "FileWriter::stopWriting stimDataToWrite" << remainingStimSamples;

    // Write remaining EEG samples
    _dataToFile(remainingEEGSamples);

    //qDebug() << "Stop Writing:" << _handleFile.fileName() << " data gathered " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    if (_isRecordingSTIM && isWritingStimData())
    {
        qDebug() << "FileWriter::stopWriting stimDataToWrite" << remainingStimSamples;

        QString text;

        //Stimulation data
        //for (int i=0;i<stimDataToWrite.size();i++)
        for (int i=0;i<remainingStimSamples;i++)
        {
            text="";
            int auxNumber=0;

            for (int j = 0; j < _numOfChannels; j ++)
            {
                if (stimDataToWrite[i].channelInfo() & (1 << j))
                {
                    auxNumber=stimDataToWrite[i].data()[j];

                    if (auxNumber>32768)
                        auxNumber -= 65536;

                    auxNumber*=2000;
                    auxNumber/=32768;

                    text += QString::number(auxNumber);
                }
                else
                {
                    text += "-1";
                }

                text += "\t";
            }

            text+=QString::number(stimDataToWrite[i].timestamp());
            text += "\n";

            QByteArray bytes = text.toLatin1();
            qint64 writenBytes = _handleStimFile.write(bytes);
            if (writenBytes < bytes.length())
            {
                _currentStatus=ERROR_ON_WRITING;
                emit statusChanged(FileWriter::ERROR_ON_WRITING);
            }
        }

        //EEG data
        /*for (int i=0;i<dataToWrite.size();i++)
        {
            text="";
            int auxNumber=0;

            for (int j = 0; j < _numOfChannels; j ++)
            {
                if (dataToWrite[i].channelInfo() & (1 << j))
                {
                    auxNumber=dataToWrite[i].data()[j];

                    text += QString::number(auxNumber);
                }
                else
                {
                    text += "-1";
                }
                text += "\t";
            }

            text+=QString::number(dataToWrite[i].timestamp());
            text += "\n";

            QByteArray bytes = text.toLatin1();
            qint64 writenBytes = _handleFile.write(bytes);
            if (writenBytes < bytes.length())
            {
                emit statusChanged(FileWriter::ERROR_ON_WRITING);
            }
        }*/

        _handleStimFile.close();
    }
    else
    {
        //If we were not recording EASY
        qDebug()<<"\tWe were not recording STIM info";
        //_firstStimTimeStamp=0;
    }

    if (_isRecordingNEDF)
    {
        if (_handleBinaryFile.isOpen())
        {
            if (_isStimulating)
                StimEEGbinaryDataToFile(remainingEEGBinarySamples,true);
            else if (binaryEEGDataToWrite.size() > 0)
                EEGbinaryDataToFile(remainingEEGBinarySamples);
        }
    }

    writeEEG = false;
    writeStim = false;

    if ((!_handleFile.isOpen())&&(!_handleBinaryFile.isOpen()))
    {
        //If we were not recording EASY
        qDebug()<<"\tWe were not recording EEG";
        _firstEEGTimeStamp=0;
    }

    _handleFile.close();
    _handleBinaryFile.close();


    qInstallMessageHandler(handler);
}

bool FileWriter::isWriting ()
{
    return _handleFile.isOpen();
}

bool FileWriter::isWritingBinaryData ()
{
    return _handleBinaryFile.isOpen();
}

bool FileWriter::isWritingStimData ()
{
    return _handleStimFile.isOpen();
}

QDateTime FileWriter::getDateTime ()
{
    return _dateTime;
}

QString FileWriter::getRootFilename ()
{
    return _rootFilename;
}

QString FileWriter::getFileID ()
{
    return _fileID;
}

QString FileWriter::getOutDir ()
{
    return _outDir;
}

QString FileWriter::getFileName ()
{
    qDebug()<<"FileWriter::getFileName _isRecordingEASY"<<_isRecordingEASY<<"_isRecordingNEDF"<<_isRecordingNEDF<<"_isRecordingSTIM"<<_isRecordingSTIM<<"_firstEEGTimeStamp"<<_firstEEGTimeStamp<<"_firstStimTimeStamp"<<_firstStimTimeStamp;
    qDebug()<<"\t isWriting()"<<isWriting()<<"isWritingStimData()"<<isWritingStimData()<<"isWritingBinaryData()"<<isWritingBinaryData();

    if ((_isRecordingEASY)&&(_firstEEGTimeStamp>0))
        return _handleFile.fileName();
    else if ((_isRecordingNEDF)&&(_firstEEGTimeStamp>0))
        return _handleBinaryFile.fileName();
    else if ((_isRecordingSTIM)&&(_firstStimTimeStamp>0))
        return _handleStimFile.fileName();
    else
    {
        //If we arrive here it is because no file was created and we want to generate the info file
        QString format("yyyyMMddHHmmss_");
        QString infoFileName;
        infoFileName = _outDir+QDateTime::currentDateTime().toString(format)+_fileID+".info";

        return infoFileName;
    }
}

QString FileWriter::getStimFileName ()
{
    return _handleStimFile.fileName();
}


QString FileWriter::getEasyFileName()
{
    QFileInfo fileInfo( _handleFile.fileName() );
    return fileInfo.fileName();
}

bool FileWriter::setOutDir (QString outDir)
{
    QDir qOutDir(outDir);


    //Commented in 11/12/2014
    //We will check if the directory and file exists when starting recording

//    if (!qOutDir.exists())
//    {
//        return false;
//    }

    _outDir = qOutDir.absolutePath();

    QString filename = _fileID;
    _handleFile.setFileName(outDir + filename);

    return true;
}


bool FileWriter::setFileID (QString fileID)
{
    if (_handleFile.isOpen())
    {
        return false;
    }
    _fileID = fileID;
    QString outDir = _outDir;
    _handleFile.setFileName(outDir + fileID);
    //_handleTrigger.setFileName(fileName + ".trigg");
    return true;
}

//bool FileWriter::isAppendingTimestamp ()
//{
//    return _appendTimestamp;
//}

//void FileWriter::setAppendingTimestamp (bool append)
//{
//    _appendTimestamp = append;
//}

void FileWriter::setAppendingPostEEG (bool append)
{
    _isPostEEG = append;
}

bool FileWriter::getIsAppendingPostEEG ()
{
    return _isPostEEG;
}

void FileWriter::setRecordingAdditionalChannel (bool record)
{
    _isRecordingAdditionalChannel = record;
}

void FileWriter::setRecordingAccelerometer (bool record)
{
    _isRecordingAccelerometer = record;
}

void FileWriter::setAppendingPreEEG (bool append)
{
    _isPreEEG = append;
}

bool FileWriter::getIsAppendingPreEEG ()
{
    return _isPreEEG;
}

void FileWriter::setNumOfChannels(int channels)
{
    _numOfChannels=channels;
}


void FileWriter::_dataToFile(int numSamples)
{
    qint64 currentTriggerTimestamp = 0;
    qint64 currentAccelerometerTimestamp = 0;
    qint64 currentAdditionalTimestamp = 0;


    QTextStream stream(&_handleFile);
    for (int i = 0; i < numSamples; i++)
    {
        // get timestamps from trigger, accelerometer and additional
        if (currentTriggerTimestamp == 0 && triggerToWrite.size() > 0)
        {
            currentTriggerTimestamp = (*triggerToWrite.begin()).getTimestamp();
        }
        if (currentAccelerometerTimestamp == 0 && accelerometerToWrite.size() > 0)
        {
            currentAccelerometerTimestamp = (*accelerometerToWrite.begin()).timestamp();
        }
        if (currentAdditionalTimestamp == 0 && additionalToWrite.size() > 0)
        {
            currentAdditionalTimestamp = (*additionalToWrite.begin()).timestamp();
        }

        QString text = "";
        QList<ChannelData>::iterator data = dataToWrite.begin();
        // EEG to file
        for (int i = 0; i < _numOfChannels; i ++)
        {
            if ((*data).channelInfo() & (1 << i))
            {
                text += QString::number((*data).data()[i]);
            }
            else
            {
                text += "-1";
            }
            text += "\t";
        }

        // Accelerometer to file
        if (currentAccelerometerTimestamp > 0 &&
            (*data).timestamp() >= currentAccelerometerTimestamp)
        {
            // ACCEL timestamp coincides with EEG
            if (((*data).timestamp() - currentAccelerometerTimestamp) != 0)
            {
                //qDebug() << "_dataToFile" << ((*data).timestamp() - currentAccelerometerTimestamp) <<
                        //(*data).timestamp() << currentAccelerometerTimestamp;
            }
            //loggerMacroDebug("Update on accel value at " + QString::number((*accelerometerToWrite.begin()).timestamp()))
            _lastAccelerometerValue[0] = (*accelerometerToWrite.begin()).data()[0];
            text += QString::number(_lastAccelerometerValue[0]) + "\t";
            _lastAccelerometerValue[1] = (*accelerometerToWrite.begin()).data()[1];
            text += QString::number(_lastAccelerometerValue[1]) + "\t";
            _lastAccelerometerValue[2] = (*accelerometerToWrite.begin()).data()[2];
            text += QString::number(_lastAccelerometerValue[2]) + "\t";
            accelerometerToWrite.pop_front();
            currentAccelerometerTimestamp = 0;

        }
        else if(_isRecordingAccelerometer)
        {
            // ACCEL timestamp coincides with EEG => store last accel value
            //loggerMacroDebug("Repeating last accel sample")
            text += QString::number(_lastAccelerometerValue[0]) + "\t";
            text += QString::number(_lastAccelerometerValue[1]) + "\t";
            text += QString::number(_lastAccelerometerValue[2]) + "\t";
        }

        //_previousTextLine = text;
        if (currentAdditionalTimestamp > 0 &&
            (*data).timestamp() >= currentAdditionalTimestamp)
        {
            /*
            if ((*data).timestamp() != currentAdditionalTimestamp &&
                !_previousTextLine.isEmpty())
            {

                // insert new timestamp with previous EEG sample and trigger forced to zero
                _previousTextLine += QString::number((*additionalToWrite.begin()).data()[0]) + "\t";
                _previousTextLine += "0\t" + QString::number(currentAdditionalTimestamp) + "\n";
                int nBytes = text.size();
                if (_handleFile.write(_previousTextLine.toLatin1()) < nBytes)
                {
                    emit statusChanged(FileWriter::ERROR_ON_WRITING);
                }
            }
            */
            _lastAdditionalValue = (*additionalToWrite.begin()).data()[0];
            text += QString::number(_lastAdditionalValue) + "\t";
            additionalToWrite.pop_front();
            currentAdditionalTimestamp = 0;
        }
        else if (_isRecordingAdditionalChannel)
        {
            text += QString::number(_lastAdditionalValue) + "\t";
        }

        if (currentTriggerTimestamp > 0 &&
            (*data).timestamp() >= currentTriggerTimestamp)
        {
            //qDebug() << ((*data).timestamp() - currentTriggerTimestamp);
            text += QString::number((*triggerToWrite.begin()).getCode());
            triggerToWrite.pop_front();
            currentTriggerTimestamp = 0;
        }
        else
        {
            if ((*data).isRepeated())
            {
                _countTrigger255++;
                text += QString::number(255);
            }
//            else if ((*data).isCompressionOverflow())
//            {
//                _countTrigger254++;
//                text += QString::number(254);
//                //qDebug()<<"_dataToFile trigger 254"<<(*data).timestamp();
//            }
            else
                text += "0";
        }

        text += "\t";
        text += QString::number((*data).timestamp());
        //text += "\n";
        /*
        int nBytes = text.size();
        if (_handleFile.write(text.toLatin1()) < nBytes)
        {
            emit statusChanged(FileWriter::ERROR_ON_WRITING);
        }
        */

        //                        if ((_currentTimestamp-_firstEEGTimeStamp)>3000)
//                                    emit newEEGData(_lastEEGData);
        //                        else
        //                            qDebug()<<"Timestamps dif"<<(_currentTimestamp-_firstEEGTimeStamp);

        stream << text << endl;

        stream.flush();
        dataToWrite.pop_front();
    }
}

void FileWriter::onNewData(ChannelData data)
{
    QMutexLocker locker(&_mutex);

    /*if (!_handleFile.isOpen())
    {
        return;
    }*/

    bool startedStoringData= false;
    if (_handleFile.isOpen()||_handleBinaryFile.isOpen())
    {
        startedStoringData = true;

        _numberOfEEGSamples++;
    }

    //EASY file
    if (_isRecordingEASY)
    {
        if (_handleFile.isOpen()||startedStoringData == true)
        {
            if (_firstEEGTimeStamp==0)
            {
               _firstEEGTimeStamp=data.timestamp();
// NOTE: NIC-Benchmark needs to avoid this call to make
// onNewData avoid from moving out from this thread
#ifndef NICBENCHMARK
                qDebug()<<"_firstEEGTimeStamp"<<_firstEEGTimeStamp;
#endif
            }

            dataToWrite.push_back(data);
            //qDebug()<<"dataToWrite.size()"<<dataToWrite.size()<<_isRecordingEASY;

            if (dataToWrite.size() >= 5000) // wait for ten seconds of data on the list
            //if (dataToWrite.size() >= 500) // wait for ten seconds of data on the list
            {
                //qDebug() << "FileWriter::onNewData" << dataToWrite.size();
                _dataToFile(250);
            }
        }
    }

    // NEDF file
    if (_isRecordingNEDF)
    {
        if (_handleBinaryFile.isOpen()||startedStoringData == true)
        {
            if (_firstEEGTimeStamp==0)
            {
               _firstEEGTimeStamp=data.timestamp();

                qDebug()<<"_firstEEGTimeStamp"<<_firstEEGTimeStamp;
            }

            binaryEEGDataToWrite.push_back(data);

            if (binaryEEGDataToWrite.size() >= 500) // wait for one second of data on the list
            {
                //qDebug()<<"binaryEEGDataToWrite.size()"<<binaryEEGDataToWrite.size();
                if (_isStimulating)
                    StimEEGbinaryDataToFile(250,false);
                else
                    EEGbinaryDataToFile(250);
            }
        }
    }
}

void FileWriter::setStimulating (bool isStimulating)
{
    qDebug()<<"FileWriter::setStimulating"<<isStimulating;
    _isStimulating = isStimulating;

//    if (isStimulating)
//        setIsEEGNotesAfterStimPending (true);
}

bool FileWriter::getIsStimulating ()
{
    return _isStimulating;
}

bool FileWriter::getIsEEGNotesAfterStimPending ()
{
    return _isEEGNotesAfterStimPending;
}

void FileWriter::setIsEEGNotesAfterStimPending (bool value)
{
    _isEEGNotesAfterStimPending=value;
}

void FileWriter::onNewTrigger (Trigger trigger)
{
    QMutexLocker locker(&_mutex);

    if (dataToWrite.size() == 0 && binaryEEGDataToWrite.size() == 0) // No EEG data received yet
    {
        return;
    }

    bool startedStoringData = false;
    if (_handleFile.isOpen()||_handleBinaryFile.isOpen())
    {
        startedStoringData = true;
    }

    //EASY file
    if (_isRecordingEASY)
    {
        if (_handleFile.isOpen()||startedStoringData == true)
        {
            triggerToWrite.push_back(trigger);
        }
    }

    //NEDF file
    if (_isRecordingNEDF)
    {
        if (_handleBinaryFile.isOpen()||startedStoringData == true)
        {
            binaryTriggerToWrite.push_back(trigger);
        }
    }
}

void FileWriter::onNewAdditionalData (ChannelData data)
{
    QMutexLocker locker(&_mutex);

    if ((!isWriting()) || (!_isRecordingAdditionalChannel))
    {
        return;
    }
    if (dataToWrite.size() == 0) // No EEG data received yet
    {
        return;
    }
    additionalToWrite.push_back(data);
/*
    QString strNewSample="";

    if ((data.channelInfo() & (1 << 0)) > 0)
    {
        strNewSample = QString::number(data.timestamp())+ '\t';
        strNewSample += QString::number(data.data()[0])  + "\n" ;
    }

//    qDebug()<<strNewSample;

    AdditionaldataToWrite.push_back(strNewSample);
    if (AdditionaldataToWrite.size()>=100)
    {
        for (int i=0;i<AdditionaldataToWrite.size();i++)
        {
            QByteArray bytes = AdditionaldataToWrite[i].toLatin1();
            qint64 writenBytes = _handleAdditional.write(bytes);
            if (writenBytes < bytes.length())
            {
                emit statusChanged(FileWriter::ERROR_ON_WRITING);
            }
        }
        AdditionaldataToWrite.clear();
    }

//    QByteArray bytes = strNewSample.toLatin1();
//    qint64 writenBytes = _handleAdditional.write(bytes);
//    if (writenBytes < bytes.length())
//    {
//        emit statusChanged(FileWriter::ERROR_ON_WRITING);
//    }
    */
}

void FileWriter::onNewAccelerometerData (ChannelData data)
{
    QMutexLocker locker(&_mutex);

    /*if ((!isWriting()&&!isWritingBinaryData()) || (!_isRecordingAccelerometer))
    {
        return;
    }*/

    if (!_isRecordingAccelerometer)
    {
        return;
    }
    if (dataToWrite.size() == 0 && binaryEEGDataToWrite.size() == 0) // No EEG data received yet
    {
        return;
    }

    bool startedStoringData = false;
    if (_handleFile.isOpen()||_handleBinaryFile.isOpen())
    {
        startedStoringData = true;
    }

    //EASY file
    if (_isRecordingEASY)
    {
        if (_handleFile.isOpen()||startedStoringData == true)
        {
            if (_firstAccelerometerTimeStamp == 0)
            {
                _firstAccelerometerTimeStamp = data.timestamp();
            }
            accelerometerToWrite.push_back(data);
        }
    }

    //NEDF file
    if (_isRecordingNEDF)
    {
        if (_handleBinaryFile.isOpen()||startedStoringData == true)
        {
            if (_firstAccelerometerTimeStamp == 0)
            {
                _firstAccelerometerTimeStamp = data.timestamp();
            }
            binaryAccelerometerDataToWrite.push_back(data);
        }
    }

}

void FileWriter::EEGbinaryDataToFile (int numSamples)
{
    QDataStream data(&_handleBinaryFile);
    int auxNumber,aux;

    unsigned long long timestampEEGlow, timestampEEGhigh;
    long long timestampEEG;
    long long timestampTrigger;
    unsigned long long timestampAccelerometer;
    int trigger;
    int p;

    char b;

    for (int k = 0; k < numSamples; k ++)
    {
        timestampEEG = (*binaryEEGDataToWrite.begin()).timestamp();
        if (counterAcc == 1)
        {
            timestampEEGlow = timestampEEG;
        }

        bool dataWritten;
        if (_isRecordingAccelerometer==true)
        {
            if (counterAcc == 1)
            {
                counterAcc = 5;
                timestampEEGhigh = timestampEEGlow+8;
                dataWritten = false;
                if (binaryAccelerometerDataToWrite.size() >0)
                {
                    while (binaryAccelerometerDataToWrite.size() >0)
                    {
                        timestampAccelerometer = binaryAccelerometerDataToWrite[0].timestamp();

                        //qDebug() << "timestampAccelerometer" << timestampAccelerometer;
                        //qDebug() << "timestampEEGlow" << timestampEEGlow;

                        if ((timestampAccelerometer >= timestampEEGlow)&&(timestampAccelerometer <= timestampEEGhigh))
                        {
                            if (dataWritten == false)
                            {
                                _latestAccelerometerWritten=binaryAccelerometerDataToWrite[0];
                                for (p=0;p<3;p++)
                                {
                                    auxNumber = (*binaryAccelerometerDataToWrite.begin()).data()[p];
                                    //streamEEG << accelerometer;
                                    for (int i=2;i>=0;i--)
                                    {
                                        b = ((auxNumber>>i*8) & 0xff);
                                        data.writeRawData(&b,1);
                                    }
                                    //outArray << sample;
                                }

                                dataWritten = true;
                            }
                            binaryAccelerometerDataToWrite.pop_front();
                        }
                        else
                        {
                            if (dataWritten == false)
                            {
                                for (p=0;p<3;p++)
                                {
                                    auxNumber = _latestAccelerometerWritten.data()[p];
                                    //streamEEG << accelerometer;
                                    for (int i=2;i>=0;i--)
                                    {
                                        b = ((auxNumber>>i*8) & 0xff);
                                        data.writeRawData(&b,1);
                                    }
                                }

                                dataWritten = true;
                            }
                            break;
                        }
                    }
                }
                else //Accelerometer data has not stated yet
                {
                    for (p=0;p<3;p++)
                    {
                        auxNumber = 0;
                        //streamEEG << accelerometer;
                        for (int i=2;i>=0;i--)
                        {
                            b = ((auxNumber>>i*8) & 0xff);
                            data.writeRawData(&b,1);
                        }
                    }
                }
            }
            else
            {
                counterAcc--;
            }
        }

        for (int j = 0; j < _numOfChannels; j ++)
        {
            // EEG to file
            if ((*binaryEEGDataToWrite.begin()).channelInfo() & (1 << j))
            {
                //data << (*binaryEEGDataToWrite.begin()).data()[j];
                aux = (*binaryEEGDataToWrite.begin()).data()[j];
                /*dataEEG[i] = (dataEEG[i] * 2.4 * 1000000000) /
                8388607.0 / 6.0;*/
                auxNumber = round(aux /2.4/1000000000* 6.0 * 8388607.0);
                b = ((auxNumber>>3*8) & 0xff);
                for (int i=2;i>=0;i--)
                {
                    b = ((auxNumber>>i*8) & 0xff);
                    data.writeRawData(&b,1);
                }
            }
            else
            {
                //data << -1;

                auxNumber = -1;
                //Write only the first 3 bytes
                for (int i=2;i>=0;i--)
                {
                    b = ((auxNumber>>i*8) & 0xff);
                    data.writeRawData(&b,1);
                }
            }
        }

        if (binaryTriggerToWrite.size() > 0)
        {
            timestampTrigger = binaryTriggerToWrite[0].getTimestamp();


            //if (timestampTrigger >= timestampEEG)
            if (timestampEEG >= timestampTrigger)
            {
                trigger = binaryTriggerToWrite[0].getCode();
                b = ((trigger>>0*8) & 0xff);
                data.writeRawData(&b,1);
                binaryTriggerToWrite.pop_front();
            }
            else
            {
                if ((*binaryEEGDataToWrite.begin()).isRepeated())
                {
                    trigger = 255;
                    if (!_isRecordingEASY)
                        _countTrigger255++;
                }
//                else if ((*binaryEEGDataToWrite.begin()).isCompressionOverflow())
//                {
//                    trigger = 254;
//                    if (!_isRecordingEASY)
//                        _countTrigger254++;
//                    //qDebug()<<"EEGBinaryDataToFile trigger 254"<<(*binaryEEGDataToWrite.begin()).timestamp();
//                }
                else
                    trigger = 0;

                b = ((trigger>>0*8) & 0xff);
                data.writeRawData(&b,1);
            }
        }
        else
        {
            if ((*binaryEEGDataToWrite.begin()).isRepeated())
            {
                trigger = 255;
                if (!_isRecordingEASY)
                    _countTrigger255++;
            }
//            else if ((*binaryEEGDataToWrite.begin()).isCompressionOverflow())
//            {
//                trigger = 254;
//                if (!_isRecordingEASY)
//                    _countTrigger254++;
//                //qDebug()<<"EEGBinaryDataToFile trigger 254"<<(*binaryEEGDataToWrite.begin()).timestamp();
//            }
            else
                trigger = 0;

            b = ((trigger>>0*8) & 0xff);
            data.writeRawData(&b,1);
        }

        binaryEEGDataToWrite.pop_front();

    }
}

void FileWriter::StimEEGbinaryDataToFile (int numSamples, bool endOfRecording)
{
    QDataStream data(&_handleBinaryFile);

    unsigned long long timestampEEG;
    unsigned long long timestampStim;
    //unsigned long long timestampEEGlow, timestampEEGhigh;
    //unsigned long long timestampStimLow,timestampStimHigh;

    unsigned long long timestampLow,timestampHigh;
    unsigned long long timestampAccelerometer;

    int auxNumber,aux,p;
    auxNumber = -1;

   /* int sizeEEG = binaryEEGDataToWrite.size();
    int sizeStim = binaryStimDataToWrite.size();
    int numSamples;

    if (endOfRecording ==false)
    {
        //Select the smallest
        if (sizeEEG>sizeStim/2)
            numSamples = sizeStim/2;
        else
            numSamples = sizeEEG;
    }
    else
    {
        //Select the highest
        if (sizeEEG>sizeStim/2)
            numSamples = sizeEEG;
        else
            numSamples = ceil(sizeStim/2);
    }*/


    char b;
    int trigger;

    int diff;

    if ((endOfRecording == true))
        qDebug() << "Size: " << binaryStimDataToWrite.size();

    for (int k = 0; k < numSamples; k ++)
    {
        if ((binaryEEGDataToWrite.size()>0)&&(binaryStimDataToWrite.size()>0))
        {
            timestampEEG = (*binaryEEGDataToWrite.begin()).timestamp();
            timestampStim = (*binaryStimDataToWrite.begin()).timestamp();
            if (counterAcc == 1)
            {
                timestampLow = timestampEEG;
            }

            if ((writeStim == false)||(writeEEG == false))
            {
                if (timestampEEG > timestampStim)
                {
                    diff = timestampEEG - timestampStim;
                    if (diff%2 != 0) //Odd number
                    {
                        if (timestampEEG == timestampStim + 1)
                        {
                            writeStim = true;
                            writeEEG = true;
                        }
                        else
                        {
                            writeStim = true;
                            writeEEG = false;
                        }

                    }
                    else //Even number
                    {
                        writeStim = true;
                        writeEEG = false;
                    }
                }
                else if (timestampEEG < timestampStim)
                {
                    diff = timestampStim - timestampEEG;
                    if (diff%2 != 0) //Odd number
                    {
                        if (timestampStim == timestampEEG + 1)
                        {
                            writeStim = true;
                            writeEEG = true;
                        }
                        else
                        {
                            writeStim = false;
                            writeEEG = true;
                        }

                    }
                    else //Even number
                    {
                        writeStim = false;
                        writeEEG = true;
                    }
                }
                else //Same timestamps
                {
                    writeStim = true;
                    writeEEG = true;
                }
            }
        }

        bool dataWritten = false;

        if (_isRecordingAccelerometer==true)
        {
            if (counterAcc == 1)
            {
                counterAcc = 5;
                timestampHigh = timestampLow + 8;
                if (binaryAccelerometerDataToWrite.size() >0)
                {
                    dataWritten = false;
                    while (binaryAccelerometerDataToWrite.size() >0)
                    {
                            timestampAccelerometer = binaryAccelerometerDataToWrite[0].timestamp();

                            while (timestampAccelerometer < timestampLow)
                            {
                                binaryAccelerometerDataToWrite.pop_front();
                                if (binaryAccelerometerDataToWrite.size()>0)
                                    timestampAccelerometer = binaryAccelerometerDataToWrite[0].timestamp();
                                else
                                    break;
                            }

                            if (binaryAccelerometerDataToWrite.size()<=0)
                                break;


                        if (((timestampAccelerometer >= timestampLow)&&(timestampAccelerometer <= timestampHigh)))                        
                        {
                            if (dataWritten == false)
                            {
                                _latestAccelerometerWritten=binaryAccelerometerDataToWrite[0];

                                for (p=0;p<3;p++)
                                {
                                    auxNumber = (*binaryAccelerometerDataToWrite.begin()).data()[p];
                                    for (int i=2;i>=0;i--)
                                    {
                                        b = ((auxNumber>>i*8) & 0xff);
                                        data.writeRawData(&b,1);
                                    }
                                }
                                dataWritten = true;
                            }
                            binaryAccelerometerDataToWrite.pop_front();

                            if (binaryAccelerometerDataToWrite.size()<=0)
                                break;
                        }
                        else
                        {
                            if (dataWritten == false)
                            {
                                for (p=0;p<3;p++)
                                {
                                    auxNumber = _latestAccelerometerWritten.data()[p];
                                    for (int i=2;i>=0;i--)
                                    {
                                        b = ((auxNumber>>i*8) & 0xff);
                                        data.writeRawData(&b,1);
                                    }
                                }
                                dataWritten = true;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    for (p=0;p<3;p++)
                    {
                        auxNumber = 0;
                        for (int i=2;i>=0;i--)
                        {
                            b = ((auxNumber>>i*8) & 0xff);
                            data.writeRawData(&b,1);
                        }
                    }
                }
            }
            else
            {
                counterAcc--;
            }
        }


        for (int j = 0 ; j < _numOfChannels ; j ++)
        {
            // EEG to file
            if((writeEEG == true)&&(binaryEEGDataToWrite.size()>0)) //EEG data starts later
            {
                //incrementEEGIndex = true;
                if ((*binaryEEGDataToWrite.begin()).channelInfo() & (1 << j))
                {
                    aux = (*binaryEEGDataToWrite.begin()).data()[j];
                    auxNumber = round(aux /2.4/1000000000* 6.0 * 8388607.0);
                    //Write only the first 3 bytes
                    for (int i=2;i>=0;i--)
                    {
                        b = ((auxNumber>>i*8) & 0xff);
                        data.writeRawData(&b,1);
                    }
                }
                else
                {
                    auxNumber = -1;
                    //Write only the first 3 bytes
                    for (int i=2;i>=0;i--)
                    {
                        b = ((auxNumber>>i*8) & 0xff);
                        data.writeRawData(&b,1);
                    }
                }
            }
            else
            {
                auxNumber = -1;
                //Write only the first 3 bytes
                for (int i=2;i>=0;i--)
                {
                    b = ((auxNumber>>i*8) & 0xff);
                    data.writeRawData(&b,1);
                }

            }
        }

        /*if (auxNumber != -1)
            binaryEEGDataToWrite.pop_front();*/

        for (p=0;p<2;p++)
        //for (int j = 0 ; j < _numOfChannels ; j ++)
        {
            if ((p==1)&&(binaryStimDataToWrite.size()==1))
            {
                //qDebug() << "Just one stimulation data left";
                break;
            }

            for (int j = 0 ; j < _numOfChannels ; j ++)
            {
                //Stim data to file
                if ((writeStim == true)&&(binaryStimDataToWrite.size()>0))
                {
                    if (binaryStimDataToWrite[p].channelInfo() & (1 << j))
                    {
                        auxNumber=binaryStimDataToWrite[p].data()[j];

                        if (auxNumber>32768)
                            auxNumber -= 65536;

                        auxNumber*=2000;
                        auxNumber/=32768;

                        //Write only the first 3 bytes
                        for (int i=2;i>=0;i--)
                        {
                            b = ((auxNumber>>i*8) & 0xff);
                            data.writeRawData(&b,1);
                        }
                    }
                    else
                    {
                        auxNumber = -1;
                        //Write only the first 3 bytes
                        for (int i=2;i>=0;i--)
                        {
                            b = ((auxNumber>>i*8) & 0xff);
                            data.writeRawData(&b,1);
                        }
                    }
                }
                else //Stimulation data starts later
                {
                    auxNumber = -1;
                    //Write only the first 3 bytes
                    for (int i=2 ;i>=0;i--)
                    {
                        b = ((auxNumber>>i*8) & 0xff);
                        data.writeRawData(&b,1);
                    }
                }
            }
        }

        long long timestampTrigger;

        if (binaryTriggerToWrite.size() > 0)
        {
            timestampTrigger = binaryTriggerToWrite[0].getTimestamp();

            //if (timestampTrigger >= timestampEEG)
            if (timestampEEG >= timestampTrigger)
            {
                trigger = binaryTriggerToWrite[0].getCode();
                b = ((trigger>>0*8) & 0xff);
                data.writeRawData(&b,1);
                binaryTriggerToWrite.pop_front();
            }
            else
            {
                if ((*binaryEEGDataToWrite.begin()).isRepeated())
                {
                    trigger = 255;
                    if (!_isRecordingEASY)
                        _countTrigger255++;
                }
//                else if ((*binaryEEGDataToWrite.begin()).isCompressionOverflow())
//                {
//                    trigger = 254;
//                    if (!_isRecordingEASY)
//                        _countTrigger254++;
//                    //qDebug()<<"EEGBinaryDataToFile trigger 254"<<(*binaryEEGDataToWrite.begin()).timestamp();
//                }
                else
                    trigger = 0;

                b = ((trigger>>0*8) & 0xff);
                data.writeRawData(&b,1);
            }
        }
        else
        {
            if ((*binaryEEGDataToWrite.begin()).isRepeated())
            {
                trigger = 255;
                if (!_isRecordingEASY)
                    _countTrigger255++;
            }
//            else if ((*binaryEEGDataToWrite.begin()).isCompressionOverflow())
//            {
//                trigger = 254;
//                if (!_isRecordingEASY)
//                    _countTrigger254++;
//                //qDebug()<<"EEGBinaryDataToFile trigger 254"<<(*binaryEEGDataToWrite.begin()).timestamp();
//            }
            else
                trigger = 0;

            b = ((trigger>>0*8) & 0xff);
            data.writeRawData(&b,1);
        }

        if ((writeStim == true)&&(binaryStimDataToWrite.size()>0))
        {
            if (binaryStimDataToWrite.size()>0)
            {
                binaryStimDataToWrite.pop_front();
            }

            if (binaryStimDataToWrite.size()>0)
            {
                binaryStimDataToWrite.pop_front();
            }
        }

        if ((writeEEG == true)&&(binaryEEGDataToWrite.size()>0))
        {
            if (binaryEEGDataToWrite.size()>0)
                binaryEEGDataToWrite.pop_front();
        }
    }
}

void FileWriter::onNewStimData (ChannelData data)
{
    QMutexLocker locker(&_mutex);
    /*if (!_isRecordingSTIM)
    {
        return;
    }

    if (!_handleStimFile.isOpen())
    {
        //qDebug()<<"##########FileWriter::onNewStimData file not open!!!!";
        return;
    }*/

    bool storingData=false;

    if (_handleStimFile.isOpen()||_handleBinaryFile.isOpen())
    {
        storingData = true;

        _numberOfStimSamples++;
    }

    if (_isRecordingSTIM)
    {        
        if (_handleStimFile.isOpen()||storingData == true)
        {
            if (_firstStimTimeStamp==0)
            {
               _firstStimTimeStamp=data.timestamp();
#ifndef NICBENCHMARK
                qDebug()<<"_firstStimTimeStamp"<<_firstStimTimeStamp;
#endif
            }

            stimDataToWrite.push_back(data);

            if (stimDataToWrite.size() >= 500) // wait for one second of data on the list
            {
                //qDebug() << "FileWriter::onNewData" << dataToWrite.size();

                QString text = "";
                int auxNumber=0;

                for (int i=0;i<stimDataToWrite.size();i++)
                {
                    text="";

                    for (int j = 0; j < _numOfChannels; j ++)
                    {
                        if (stimDataToWrite[i].channelInfo() & (1 << j))
                        {
                            auxNumber=stimDataToWrite[i].data()[j];

                            if (auxNumber>32768)
                                auxNumber -= 65536;

                            auxNumber*=2000;
                            auxNumber/=32768;

                            text += QString::number(auxNumber);
                        }
                        else
                        {
                            text += "-1";
                        }
                        text += "\t";
                    }

                    text+=QString::number(stimDataToWrite[i].timestamp());
                    text += "\n";

                    QByteArray bytes = text.toLatin1();
                    qint64 writtenBytes = _handleStimFile.write(bytes);
                    if (writtenBytes < bytes.length())
                    {
                        _currentStatus=ERROR_ON_WRITING;
                        emit statusChanged(FileWriter::ERROR_ON_WRITING);
                    }
                }
                stimDataToWrite.clear();
            }
        }
    }

    if(_isRecordingNEDF)
    {
        if (_handleBinaryFile.isOpen()||storingData == true)
        {
            binaryStimDataToWrite.push_back(data);

            if (_firstStimTimeStamp==0)
            {
               _firstStimTimeStamp=data.timestamp();
                qDebug()<<"_firstStimTimeStamp"<<_firstStimTimeStamp;
            }
        }
    }
}

int FileWriter::getCurrentStatus()
{
    return _currentStatus;
}
