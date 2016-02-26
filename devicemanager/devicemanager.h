#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H


// Qt includes
#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include <QElapsedTimer>
#include <QDateTime>

// Project includes
#include "commonparameters.h"
#include "starstimcom.h"
#include "devicemanagertypes.h"
#include "sleeper.h"


#define BATTERY_REQUEST_PERIOD  60*1000 // Number of [ms] before batter is requested
#define ENABLE_NULL_REQUEST



class DeviceManager : public QObject
{
    Q_OBJECT
public:




    // FUNCTIONS
    // ----------------

    /*!
     * \brief DeviceManager public constructor
     * \param parent
     */
    explicit DeviceManager(QObject *parent = 0);


    /*!
     * \brief DeviceManager Desctructor
     */
    ~DeviceManager();

    // Getters/Setters

    /*!
     * It returns the firmware version.
     */
    int getFirmwareVersion (){ return _starstimCom->getFirmwareVersion(); }

    /*!
     * \brief setDeviceType getter/setter for DeviceType
     * \param type
     */         
    DeviceManagerTypes::DeviceType getDeviceType(){ return _starstimCom->getDeviceType(); }
    QString deviceType2String(){ return _starstimCom->deviceType2String(); }

    /*!
     * \brief getDeviceStatus returns the device status
     * \return
     */
    DeviceManagerTypes::DeviceStatus getDeviceStatus(){ return _starstimCom->getDeviceStatus(); }

    /*!
     * Indicates whether the device is stimulating.
     */
    void setDeviceStimulating(bool val ){ _deviceStimulating = val; }
    bool getDeviceStimulating(){ return _deviceStimulating; }

    /*!
     * returns the number of channels
     */
    int getNumOfChannels(){ return _starstimCom->getNumOfChannels(); }

    /*!
     * \brief setSampleRate setter and getter for sample rate
     * \param sampleRate
     */
    void setSampleRate(DeviceManagerTypes::SampleRate sampleRate);
    DeviceManagerTypes::SampleRate getSampleRate(){ return _starstimCom->getSampleRate(); }


    // Open/Close operations

    /*!
     * It performs the required operations to open and initialize any device
     * and performs the firmware/battery request
     *_lookForStarStim
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

    // Register initialisation

    /*!
     * \brief initRegisters initialises the registers of the EEG bank depending on whether
     * the device is starstim or enobio. Set the EEG registers for 500Hz, G=6, Vref=2.4
     *
     * \param partialInit writes only the minimal set of registers. Used when initEEGRegisters already called
     */
    void initRegisters(bool partialInit = false);

    /**
     * @brief checkRegWritten Checks whether the registers were well written in the device and retry if necessary
     * @param reg registers written to the device
     * @param size number of registers written
     * @param regName registers written (used for logging)
     * @param nRetries number of retries
     * @return Returns number of tries
     */
    #define DEFAULT_N_RETRIES 5
    int checkRegWritten(QByteArray regArray, DeviceManagerTypes::StarstimRegisterFamily family, int address, int nRetries = DEFAULT_N_RETRIES);

    // General Requests

    /*!
     * It writes a set of configuration register with consecutive addresses.
     *
     * \param family Identification of the family of registers to be writen.
     *
     * \param address Address of the register within the family.
     *
     * \param value Vector with the values to be written.
     *
     * \param length Length of the value vector.
     *
     * \return True if the register has been written, false otherwise.
     */
    bool writeRegister (DeviceManagerTypes::StarstimRegisterFamily family, int address,
                       QByteArray& regArray);


    /*!
     * It reads a configuration register.
     *
     * \param family Identification of the family of registers to be read.
     *
     * \param address Address of the register within the family.
     *
     * \param reg Read value. If the fucntion returns false this value is not.
     *
     * \param length Number of consecutive register to be read
     *
     * \return True if the register has been read, false otherwise.
     */
    bool readRegister (DeviceManagerTypes::StarstimRegisterFamily family, int address,
                       QByteArray& regArray, int length = 1);

    /*!
     * This method does the operations for requesting the firmware
     * version
     *
     * \return True if the operations has been successfully performed, false
     * otherwise.
     */
    bool firmwareVersionRequest();

    // Streaming Requests

    /*!
     * It sends a request to the Enobio3G/StarStim to start the EEG streaming.
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool startStreaming ();

    /*!
     * It sends a request to the Enobio3G/StarStim to stop the EEG streaming.
     *
     * \param doStopAccelerometer In case that the Accelerometer is on, indicates if it should be stopped or not (useful for SDCard recording)
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool stopStreaming (bool doStopAccelerometer=true);

    /*!
     * It sends a request to the device to start streaming accelerometer data.
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool startAccelerometer();

    /*!
     * It sends a request to the device to stop streaming accelerometer data..
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool stopAccelerometer();

    // Stimulation Requests

    /*!
     * It sends a request to the StarStim to start the stimulation.
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool startStimulation ();

    /*!
     * It sends a request to the StarStim to stop the stimulation.
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool stopStimulation ();

        /*!
     * It sends a request to the StarStim to start sending the impedance
     * measurements.
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool startImpedanceMeasurement ();

    /*!
     * It sends a request to the StarStim to stop sending the impedance
     * measurements.
     *
     * \return True if the request has been correctly received by the device,
     * false otherwise.
     */
    bool stopImpedanceMeasurement ();



    /*!
     * \brief request slot for performing a SYNCHROUNOUS request in the device. Parameters are
     * only to be used for WRITE_REGISTER_REQUEST and READ_REGISTER_REQUEST
     * \param request
     * \param familiy
     * \param address
     * \param frame
     */
    bool requestSync(DeviceManagerTypes::StarstimRequest request_type,
                    DeviceManagerTypes::StarstimRegisterFamily family, int address, QByteArray& frame, int length = 0);

    /*!
     * \brief request slot for performing a SYNCHROUNOUS request for the rest of operations
     * \param request
     * \param familiy
     * \param address
     * \param frame
     */
    bool requestSync(DeviceManagerTypes::StarstimRequest request_type);

private:

    // ATTRIBUTES
    // ----------------

    /*!
     * \brief _mutex prevents two operations to be run at the same time
     */
    QMutex _mutex;

    /*!
    * \brief nullRequestTimer controls the ms to send a new null request
    */
   QTimer _nullRequestTimer;

   /*!
    * \brief _profileRequestTimer controls the ms to send a new profile request
    */
   QTimer _profileRequestTimer;


    /*!
     * \brief _starstimCom instance to DeviceManagerPoll
     */
    StarstimCom* _starstimCom;

    // Device Profile



    /*!
     * \property DeviceManager::_isStimulationRunning
     *
     * This is a boolean that informs if an stimulation is going on.
     */
    bool _deviceStimulating;


    // FUNCTIONS
    // ----------------




signals:
    

    // Streaming signals

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
     * Signal that is emitted whenever the Enobio3G/StarStim device status
     * changes.
     *
     * \param deviceStatus New Enobio3G/Starstim status. The value 0xFF means
     * that the device does not repond. For different values the meaning is at
     * bit level as it follows: Bit 6: Stimulation ON/OFF, Bit 5: EEG Streaming
     * ON/OFF, Impedance measurement ON/OFF. Bit set to one means ON.
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

    /*!
     * \brief request slot for performing a request in the device. Note that parameters are
     * only to be used with WRITE_REGISTER_REQUEST and READ_REGISTER_REQUEST
     * \param request_type
     * \param familiy
     * \param address
     * \param frame
     */
    void request(DeviceManagerTypes::StarstimRequest request_type,
                 DeviceManagerTypes::StarstimRegisterFamily family = DeviceManagerTypes::EEG_REGISTERS,
                 int address = 0, QByteArray frame = QByteArray());

private slots:

    /*!
     * \brief onReceivedDeviceStatus slot raised when received DeviceStatus
     * \param deviceStatus
     */
    void onReceivedDeviceStatus(int deviceStatus);

    /*!
     * This method does the operations for requesting the battery
     * measurement
     *
     * \return True if the operations has been successfully performed, false
     * otherwise.
     */
    bool profileRequest();

    /*!
     * This method sends a null request which keeps the communication alive
     * with the device.
     */
    bool nullRequest ();


};

#endif // DEVICEMANAGER_H
