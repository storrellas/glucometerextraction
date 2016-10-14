#ifndef DEVICEMANAGERPOLL_H
#define DEVICEMANAGERPOLL_H

#define MAX_N_REGISTER         65536  // [bytes] maximum number of registers in a single bank
#define MAX_LENGTH_RX_BUFFER   2048   // [bytes] maximum length of read operation
#define SAMPLES_PER_SECOND     500

// Qt includes
#include <QObject>
#include <QElapsedTimer>
#include <QMutex>
#include <QWaitCondition>
#include <QDateTime>
#include <QtMsgHandler>

// Project includes
#include "commonparameters.h"
#include "wifidevice.h"
#include "icognosprotocol.h"
#include "icognosregister.h"
#include "devicemanagertypes.h"
#include "fw/eeg_mgr.h"
#include "fw/stim_mgr.h"
#include "fw/accel_mgr.h"
#include "fw/sdcard_mgr.h"

class StarstimCom : public QObject
{
    Q_OBJECT
public:

    // ATTRIBUTES
    // ----------------


    // Register bank

    /*!
     * \brief _registerLocal Holds the lastest data from read register operation
     */
    unsigned char _regContent[MAX_N_REGISTER];


    // FUNCTIONS
    // ----------------

    /*!
     * \brief DeviceManagerPoll Public constructor
     * \param wifiDevice
     * \param parent
     */
    StarstimCom(QObject *parent = 0 );

    /*!
     * It returns the firmware version.
     */
    int getFirmwareVersion (){ return _protocol.getFirmwareVersion(); }

    /*!
     * \brief setDeviceType getter/setter for DeviceType
     * \param type
     */
    DeviceManagerTypes::DeviceType getDeviceType(){ return _deviceType; }
    QString deviceType2String(){ return _deviceType==DeviceManagerTypes::ENOBIO?QString("ENOBIO"):QString("STARSTIM"); }

    /*!
     * Getter/Setter for _streamingAccel;
     */
    void setStreamingAccel(bool val){ _deviceStatusStruct.ACCEL = val; }


    /*!
     * \brief getDeviceStatus returns the device status
     * \return
     */
    DeviceManagerTypes::DeviceStatus getDeviceStatus(){ return _deviceStatusStruct; }

    /*!
     * \brief getNumOfChannels returns the number of channels of the device
     * \return
     */
    int getNumOfChannels(){ return _numOfChannels; }

    /*!
     * \brief setSampleRate Getter and setter for sample Rate
     * \param sampleRate
     */
    void setSampleRate( DeviceManagerTypes::SampleRate sampleRate ){ _sampleRate = sampleRate; }
    DeviceManagerTypes::SampleRate getSampleRate(){ return _sampleRate; }


    /*!
     * It performs the required operations to open and initialize any device
     *
     * \param ipAddress pointer to the ip address
     *
     * \port port for connecting
     *
     * \return If the device opens correctly the functionn returns true, false
     * otherwise. This method returning false might be because either the
     * device is turned off or it is out of battery or it is out of range.
     */
    bool openDevice (const char *ipAddress, int port);

    /*!
     * It performs the required operations to close the hardware.
     *
     * \return True if the device is closed correctly, false otherwise.
     */
    bool closeDevice ();


    /*!
     * It starts the thread where the DeviceManager::_poll works, so the
     * incoming data is processed on the background.
     */
    void startPollThread ();

    /*!
     * It stops the thread where the DeviceManager::_poll works, so the
     * incoming data stops of being processed on the background.
     */
    void stopPollThread ();

    /*!
     * \brief request slot for performing a request in the device. Note that parameters are
     * only to be used with WRITE_REGISTER_REQUEST and READ_REGISTER_REQUEST
     * \param request
     * \param familiy
     * \param address
     * \param frame
     *
     * \return True if the operation was completed, False otherwise
     */
    bool request(DeviceManagerTypes::StarstimRequest request,
                 DeviceManagerTypes::StarstimRegisterFamily family = DeviceManagerTypes::EEG_REGISTERS,
                 int address = 0, QByteArray frame = QByteArray());

private:



    // ATTRIBUTES
    // ----------------

    /*!
     * \property DeviceManager::_device
     *
     * Instance of the Wifi device to communicate with the
     * icognos3G/StarStim device.
     */
    WifiDevice* _wifiDevice;


    /*!
     * \property DeviceManager::_protocol
     *
     * Parser of the icognos3G/StarStim protocol.
     */
    StarStimProtocol _protocol;

    // Profile variables

    /*!
     * \property DeviceManager::_deviceStatus
     *
     * This keeps the reported status of the device.
     */
    int _deviceStatus;
    DeviceManagerTypes::DeviceStatus _deviceStatusStruct;

    /*!
     * \brief _isStarstim Indicates whether the current device is an icognos
     */
    DeviceManagerTypes::DeviceType _deviceType;

    /*!
     * Indicates the number of EEG samples per beacon received
     */
    int _samplesPerBeacon;

    /*!
     * \brief _numOfChannels indicates the number of channels for the current device
     */
    int _numOfChannels;

    /*!
     * \brief _sampleRate Indicates the current sampling rate
     */
    DeviceManagerTypes::SampleRate _sampleRate;

    // Polling variables

    /*!
     * \property DeviceManager::_rxBuffer
     *
     * Buffer that stores the received bytes from rfcomm device.
     */
    char _rxBuffer[MAX_LENGTH_RX_BUFFER];

    /*!
     * \brief pollThread Internal thread used to poll the TCP socket
     */
    QThread* pollThread;

    /*!
     * \brief formerThread holds the initial thread from which DeviceManager was created
     */
    QThread* formerThread;

    /*!
     * \brief _terminate indicates whether the thread needs to be terminated
     */
    bool _isPollThreadRunning;

    /*!
     * \brief sync Mutex to access the pending operation boolean
     */
    QMutex sync;

    /*!
     * \brief reqSyncMutex synchronisation variables for command finished
     */
    QMutex reqSyncMutex;
    QWaitCondition reqSyncCond;

    /*!
     * \brief waitStopSync synchronisation variables in order to avoiding QEventLoop in Testing Framework
     */
    QMutex stopSync;
    QWaitCondition waitStopSync;

    /*!
     * \brief sharedTxBuffer object holding next transference
     */
    QByteArray sharedTxBuffer;

    /*!
     * \brief requestBlock Indicates whether the call to request() method will be blocking
     */
    bool requestBlock;

    // Stimulation variables

    /*!
     * \property DeviceManager::_firstStimSampleReceived
     *
     * This boolean informs whether a Stimulation sample has been received after
     * requesting the start stimulating.
     */
    bool _firstStimSampleReceived;

    /*!
     * \property DeviceManager::_timeRequestFirstStimSample
     *
     * This variable is used for knowing when the start stimulation was requested
     * so it can be known the intial latency.
     */
    qint64 _timeRequestFirstStimSample;

    /*!
     * \property DeviceManager::_currentStimTimestamp
     *
     * It keeps the currentTimestamp to be set to the received Stimulation samples.
     */
    unsigned long long _currentStimTimestamp;


    /*!
     * \property DeviceManager::_beaconCounterStayAlive
     *
     * Counter used for periodically send the null request for staying alive
     * the communication with the device.
     */
    unsigned int _beaconCounterStayAlive;

    /*!
     * \brief nullRequestTimer controls the last ms in which null Request was sent
     */
    QElapsedTimer _nullRequestTimer;


    // EEG variables

    /*!
     * \property DeviceManager::_waitingFirstEEGSample
     *
     * This boolean informs whether an EEG sample has been received after
     * requesting the start EEG streaming.
     */
    bool _firstEEGSampleReceived;

    /*!
     * \property DeviceManager::_timeRequestFirstSample
     *
     * This variable is used for knowing when the start streaming was requested
     * so it can be known the intial latency.
     */
    qint64 _firstTimestampRequest;

    /*!
     * \brief _countPacketsPer30Seconds Counts the number of packets received for the last 30 seconds
     */
    int _countPacketsPer30Seconds;

    /*!
     * \brief _countPacketsLostPer30Seconds counts the number of packets lost for the last 30 seconds
     */
    int _countPacketsLostPer30Seconds;

    /*!
     * \brief _countPacket counts the number of EEG packets
     */
    int _countPacket;

    /*!
     * \property DeviceManager::_currentTimestamp
     *
     * It keeps the currentTimestamp to be set to the received EEG samples.
     */
    unsigned long long _currentEEGTimestamp;

    /*!
     * \property DeviceManager::_firstEEGTimeStamp
     *
     * This variable keeps the time stamp of the first EEG sample.
     */
    unsigned long long _firstEEGSampleTimestamp;

    /*!
     * \property DeviceManager::_currentEEGStamp
     *
     * It keeps the current EEG stamp which is used to identify lost packets in
     * the EEG streaming.
     */
    unsigned long long _currentEEGStamp;


    // Last received packets

    /*!
     * \property DeviceManager::_lastEEGData
     *
     * It keeps the last processed EEG sample.
     */
    ChannelData _lastEEGData;

    /*!
     * \property DeviceManager::_lastAccelerometerData
     *
     * It keeps the last processed Accelerometer sample.
     */
    ChannelData _lastAccelData;

    /*!
     * \property DeviceManager::_lastStimData
     *
     * It keeps the last processed Stimulation data.
     */
    ChannelData _lastStimData;

    // FUNCTIONS
    // ----------------


    // Polling operations

    /*!
     * It processes the received data (EEG streaming, read configuration,
     * acknowledgements).
     *
     * \param nBytes Bytes to be processed from the rfcomm device.
     *
     * \return Negative number if there was any problem while processing the
     * information. Zero if the frame from the device has not been completely
     * processed yet. A Positive number if a data frame has been successfully
     * processed.
     */
    int _processData (int nBytes = MAX_LENGTH_RX_BUFFER);


    /*!
     * This method keeps alive the communication with the device and perform
     * periodic operations like processing the incoming streamings, requesting
     * the battery level or sending the online stimulation signal.
     *
     * \return Zero or a negative number according to success or failure due
     * to an error.
     */
    int _poll ();

    /*!
     * It calculates the battery state of charge from the measured voltage.
     *
     * \param measurement Measurement of the device battery in Volts.
     *
     * \return Stimation of the percentage (from 0 to 100) of battery that
     * still remains.
     */
    int _calculateBatteryLevel (int measurement);

    /*!
     * It determines whether there is an instrument device at the other
     * side of the rfcomm connection.
     *
     * \return True if an icognos3G/StarStim device is detected , false
     * otherwise.
     */
    bool _lookForStarStim ();

    // EEG Streaming

    /*!
     * This function analyses the timestamp from the last EEG frame received
     * Basically, two opeartions are performed (1) Determine the the firstEEGTimeStamp
     * when no data was still received, (2) Check whether there has been packet lost in
     * the communication with Dennis
     */
    int _timestampAnalysis(StarstimData * data);

    /*!
     * This functions analyses the EEG data from StarStimData and generates the signals corresponding
     * to the number of packets missing as per indicated in diff (difference with previous timestamp
     */
    void _eegProcessing(StarstimData * data, int diff);

public slots:

    /*!
     * \brief runPoll slot used to start the polling of the TCP socket
     */
    void runPoll();


signals:


    // Synchronisation signals
    /*!
     * \brief finished indicates that the pollThread was finished
     */
    void finished();

    /*!
     * \brief operationDone indicates that the requested operation was finished
     */
    void operationDone();


    // Received data signals

    /*!
     * Signal that is emitted whenever a new EEG data is received.
     *
     * \param data The new received sample data
     */
    void receivedEEGData(ChannelData data);


    /*!
     * Signal that is emitted reporting the new accelerometer data
     * received.
     *
     * \param data The new received sample data
     *        We use channels 1,2 and 3
     */
    void receivedAccelData(ChannelData data);

    /*!
     * Signal that is emitted reporting the firmware version of the
     * device.
     *
     * \param version Firmware version
     */
    void receivedFirmwareVersion(int firmwareVersion);



    /*!
     * Signal that is emitted reporting the remaining percentage of the
     * battery, firmware version and t1, t2 (ms from the moment where device is ON)
     *
     * \param deviceType type of device connected
     * \param n_channel number of channels of the device
     * \param level Remaining battery in percentage from 0 to 100%
     * \param firmwareVersion firmware version of the device
     * \param t1
     * \param t2
     */
    void receivedProfile(DeviceManagerTypes::DeviceType deviceType, int n_channel,
                         int batteryLevel, int firmwareVersion, int t1, int t2);

    /*!
     * Signal that is emitted whenever the icognos3G/StarStim device status
     * changes.
     *
     * \param deviceStatus
     */
    void receivedDeviceStatus(DeviceManagerTypes::DeviceStatus deviceStatus);

    /*!
     * Signal that is emitted whenever a new stimulation data is received.
     *
     * \param data The new received sample data
     */
    void receivedStimulationData(ChannelData data);

    /*!
     * Signal that is emitted whenever a new Impedance data is received.
     *
     * \param data The new received sample data
     *
     * \param timeStamp The timeStamp of the data
     */
    void receivedImpedanceData(ChannelData data);

};

#endif // DEVICEMANAGERPOLL_H
