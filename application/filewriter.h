#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QMutex>

#include "channeldata.h"
#include "trigger.h"
#include "commonparameters.h"

/*!
 * \class FileWriter FileWriter.h
 *
 * \brief This class writes to a text file the data received through the
 * ChannelData objects.
 */
class FileWriter : public QObject
{
    Q_OBJECT
public:
    /*!
     * Constructor.
     *
     * \param start If true the target file is opened so all is ready to
     * write the incoming data to the file.
     *
     * \param fileName Complete path to the file that will be opened (and
     * truncated) for writing. If the file does not exist then it will be
     * created.
     *
//     * \param appendTimestamp If it is true, the file name will be appended
//     * with a timestamp ("YYYYMMDDhhmmss_") before the provided file name.
     */
    FileWriter (bool start = false, QString fileName = "Patient01", QString outDir = "");
                //,bool appendTimestamp = true);

    /*!
     * Destructor.
     */
    ~FileWriter ();

    /*!
     * \enum FileWriterStatus
     *
     * Description of the possible status of the file writer.
     */
    enum FileWriterStatus
    {
        NO_ERROR_IN_FILE_WRITER = 0,
        ERROR_ON_OPENING = 1,
        ERROR_ON_WRITING = 2
    };

    /*!
     * It opens the target file in order to be ready to write the
     * incoming data to the file.
     *
     * \return true if the file was opened or created successfully, false
     * otherwise.
     */
    bool startWriting ();

    /*!
     * It opens the stimulation file in order to be ready to write the
     * incoming data to the file.
     *
     * \return true if the file was opened or created successfully, false
     * otherwise.
     */
    bool startWritingStimData ();

    /*!
     * It stops writing to the file and closes it. If startWriting is called
     * later the file will be truncated or a new file will be created with the
     * corresponding timestamp if this option is selected.
     */
    void stopWriting ();

    /*!
     * It informs if the incoming EEG data is being recorded.
     *
     * \return True if the incoming data is alredy being writen in the target
     * file, false otherwise.
     */
    bool isWriting ();

    /*!
     * It informs if the incoming EEG/stimulation data is being saved in the binary file.
     *
     * \return True if the incoming data is alredy being writen in the target
     * file, false otherwise.
     */
    bool isWritingBinaryData();

    /*!
     * It informs if the incoming stimulation data is being recorded.
     *
     * \return True if the incoming data is alredy being writen in the target
     * file, false otherwise.
     */
    bool isWritingStimData();

    /*!
     * It provides the root filename (timestamp + fileID).
     *
     * \return It returns the root filename.
     */
    QString getRootFilename ();

    /*!
     * It provides the target recording ID.
     *
     * \return It returns the recording ID.
     */
    QString getFileID ();

    /*!
     * It provides the target out directory.
     *
     * \return It returns the recording directory.
     */
    QString getOutDir ();

    /*!
     * It provides the target file name.
     *
     * \return It returns the complete path to the target file.
     */
    QString getFileName ();

    /*!
     * It provides the stimulation file name.
     *
     * \return It returns the complete path to the stimulation file.
     */
    QString getStimFileName ();

    /*!
     * \brief getEasyFileName returns the name of the easy file generated
     * \return
     */
    QString getEasyFileName ();

    /*!
     * \brief getCurrentStatus returns the _currentStatus of the file writer
     * \return int
     */
    int getCurrentStatus();

    /*!
     * it sets the number of EEG Channels.
     *
     * \param channels Number of channels.
     */
    void setNumOfChannels(int channels);


    /*!
     * It sets the name of the target output directory.
     *
     * \param outDir Complete path to the target file.
     *
     * \return It returns false if the file name could not be changed because
     * the file was already open, false otherwise.
     */
    bool setOutDir (QString outDir);

    /*!
     * It sets the name of the target file.
     *
     * \param fileName Complete path to the target file including its name.
     *
     * \return It returns false if the file name could not be changed because
     * the file was already open, false otherwise.
     */
    bool setFileID (QString fileName);

    /*!
     * It provides the date and time when the recording was started.
     *
     * \return Date and time when the recording started.
     */
    QDateTime getDateTime ();

//    /*!
//     * It informs whether the timestamp is being added to the target file name.
//     *
//     * \return True if the name of the target file is appended with a timestamp
//     * at the beggining, false otherwise.
//     */
//    bool isAppendingTimestamp ();

//    /*!
//     * It configures if a timestamps is appended at the beginning of the target
//     * file name.
//     * \param append. If it is true the timestamp is appended at the beginning
//     * of the target file name.
//     */
//    void setAppendingTimestamp (bool append);

    /*!
     * It configures if "PreEEG_" is appended at the beginning of the target
     * file name.
     * \param append. If it is true "PreEEG_" is appended at the beginning
     * of the target file name.
     */
    void setAppendingPreEEG (bool append);

    /*!
     *
     * Returns whether the current file is a Pre EEG file.
     */
    bool getIsAppendingPreEEG ();

    /*!
     * It configures if "PostEEG_" is appended at the beginning of the target
     * file name.
     * \param append. If it is true "PostEEG_" is appended at the beginning
     * of the target file name.
     */
    void setAppendingPostEEG (bool append);

    /*!
     *
     * Returns whether the current file is a Post EEG file.
     */
    bool getIsAppendingPostEEG ();

    /*!
     * It configures if the additional channel data should be recorded in the file
     *
     * \param record. If it is true we do the additional channel file recording
     * of the target file name.
     */
    void setRecordingAdditionalChannel (bool record);

    /*!
     * It configures if the accelerometer data should be recorded in the file
     *
     * \param record. If it is true we do the accelerometer file recording
     * of the target file name.
     */
    void setRecordingAccelerometer (bool record);

    /*!
     * \property FileWriter::strErrorOnOpening
     *
     * Error opening string.
     */
    static const QString strErrorOnOpening;

    /*!
     * \property FileWriter::strErrorOnWriting
     *
     * Error on writing string.
     */
    static const QString strErrorOnWriting;

    int _countTrigger255;

    int _countTrigger254;

    /*!
     *
     * Returns EEG timestamp of first sample.
     */
    unsigned long long getFirstEEGTimestamp();

    /*!
     *
     * Returns accelerometer timestamp of first sample.
     */
    unsigned long long getFirstAccelerometerTimestamp();

    /*!
     *
     * Returns Stim timestamp of first sample.
     */
    unsigned long long getFirstStimTimestamp();

    /*!
     * \param firstTimestamp
     *
     * Sets the property _firstTimestamp to firstTimestamp.
     */
    void setFirstStimTimestamp( unsigned long long firstTimestamp);

    /*!
     *
     * Returns number of EEG samples.
     */
    int getNumberOfEEGSamples();

    /*!
     * \param numberOfSamples
     *
     * Sets the property _numberOfEEGSamples to numberOfSamples.
     */
    void setNumberOfEEGSamples( int numberOfSamples);

    /*!
     *
     * Returns number of stimulation samples.
     */
    int getNumberOfStimSamples();

    /*!
     * \param numberOfSamples
     *
     * Sets the property _numberOfStimSamples to numberOfSamples.
     */
    //void setNumberOfStimSamples( int numberOfSamples);

    void setRecordingNEDFFile (bool recordNEDF);
    void setRecordingEASYFile (bool recordEASY);
    void setRecordingSTIMFile (bool recordSTIM);

    bool _isStimulating;

    //This boolean is useful to emit EEGNotes after a stim session
    //when no file is recorded
    bool _isEEGNotesAfterStimPending;

    bool getIsEEGNotesAfterStimPending();

signals:
    /*!
     *  This signal is emitted whenever there is a new status to report.
     *
     * \param status New status to be reported.
     */
    void statusChanged (FileWriter::FileWriterStatus status);

public slots:

    void setIsEEGNotesAfterStimPending(bool value);

    /*!
     * This slot is called whenever a new data is desired to be recorded in the
     * target file.
     *
     * \param data The data that will be recorded in the target file.
     */
    void onNewData (ChannelData data);

    /*!
     * This slot is called whenever a new trigger is desired to be recorded in
     * the target file.
     *
     * \param trigger The trigger.
     */
    void onNewTrigger (Trigger trigger);

    /*!
     * This slot is called whenever a new data is desired to be recorded in the
     * target file.
     *
     * \param data The data that will be recorded in the target file.
     */
    void onNewAdditionalData (ChannelData data);

    /*!
     * This slot is called whenever a new data is desired to be recorded in the
     * target file.
     *
     * \param data The data that will be recorded in the target file.
     */
    void onNewAccelerometerData (ChannelData data);

    /*!
     * This slot is called whenever a new data is desired to be recorded in the
     * target file.
     *
     * \param data The data that will be recorded in the target file.
     */
    void onNewStimData (ChannelData data);

    void setStimulating (bool isStimulating);

    bool getIsStimulating();



private:

    //bool incrementEEGIndex;
    //bool incrementStimIndex;

    bool writeEEG;
    bool writeStim;

    int counterAcc;

    void EEGbinaryDataToFile(int numSamples);

    void StimEEGbinaryDataToFile(int numSamples, bool endOfRecording);

    void _dataToFile(int numSamples);

    /*!
     * \property FileWriter::_numOfChannels
     *
     * The number of channels received.
     */
    int _numOfChannels;

    /*!
     * \property FileWriter::_outDir
     *
     * The output directory.
     */
    QString _outDir;

    /*!
     * \property FileWriter::_outDir
     *
     * The name of the output file (timestamp + fileID).
     */
    QString _rootFilename;

    /*!
     * \property FileWriter::_fileID
     *
     * The name of the file.
     */
    QString _fileID;

    /*!
     * \property FileWriter::_dateTime
     *
     * The date when the file started to be written.
     */
    QDateTime _dateTime;

    /*!
     * \property FileWriter::_handleFile
     *
     * Handle to the target file.
     */
    QFile _handleFile;

    /*!
     * \property FileWriter::_handleStimFile
     *
     * Handle to the stimulation file.
     */
    QFile _handleStimFile;

    /*!
     * \property FileWriter::_handleBinaryFile
     *
     * Handle to the binary file.
     */
    QFile _handleBinaryFile;



    bool _isPreEEG;
    bool _isPostEEG;
    bool _isRecordingAdditionalChannel;
    bool _isRecordingAccelerometer;
    bool _isRecordingNEDF;
    bool _isRecordingSTIM;
    bool _isRecordingEASY;    

    //QStringList dataToWrite;
    QList<ChannelData> dataToWrite;
    QList<ChannelData> stimDataToWrite;
    QList<ChannelData> binaryEEGDataToWrite;
    QList<ChannelData> binaryStimDataToWrite;
    QList<ChannelData> binaryAccelerometerDataToWrite;
    QList<Trigger> binaryTriggerToWrite;
    QList<Trigger> triggerToWrite;

    QStringList AdditionaldataToWrite;
    QStringList AccelerometerDataToWrite;
    QList<ChannelData> accelerometerToWrite;
    QList<ChannelData> additionalToWrite;

    int _lastAccelerometerValue[3];

    int _lastAdditionalValue;

    unsigned long long _firstEEGTimeStamp;
    unsigned long long _firstStimTimeStamp;
    unsigned long long _firstAccelerometerTimeStamp;

    /*!
     * \property FileWriter::_latestAccelerometerWritten
     *
     * It stores the latest value of Accelerometer. Used when writing nedf files if
     * no new accelerometer data is available (in order to avoid writing 0 0 0 ).
     */
    ChannelData _latestAccelerometerWritten;

    int _numberOfEEGSamples;
    int _numberOfStimSamples;

    /*!
     * \property FileWriter::_currentStatus
     *
     * It stores the status of the filewriter.
     */
    int _currentStatus;

    /*!
     * \property FileWriter::_trigger
     *
     * It stores the vaue of the received trigger.
     */
    //int _trigger;
    //QList<int> _triggers;

    /*!
     * \property FileWriter::_triggersMutex
     *
     * This mutex is used to protect the accesses to the FileWriter::_triggers
     * list from different threads
     */
    //QMutex _triggersMutex;
    QMutex _mutex;
    //QString _previousTextLine;
};

#endif // FILEWRITER_H
