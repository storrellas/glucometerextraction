#ifndef STARSTIMPROTOCOL_H
#define STARSTIMPROTOCOL_H

#include "starstimdata.h"

/*!
 * \class StarStimProtocol starstimprotocol.h
 *
 * \brief This the class implements the Enobio3G/StarStim protocol, so it
 * provides the mechanism to parse the messages received from the
 * Enobio3G/StarStim device as well as to build messsages to be sent to it.
 */
class StarStimProtocol
{
public:

    /*!
     * Default constructor
     */
    StarStimProtocol();
    ~StarStimProtocol ();

    /*!
     * \brief getFirmwareVersion returns the firmware version
     * \return
     */
    int getFirmwareVersion(){ return _firmwareVersion; }

    /*!
     * It performs the parse of the protocol byte by byte.
     *
     * \param byte Byte to be parsed according to the StarStim protocol.
     *
     * \return It returns true when the processing of the byte leads to a
     * completion of some statement of the protocol. It returns false
     * otherwise.
     */
    bool parseByte (unsigned char byte);

    /*!
     * It returns the last received StarStim data.
     *
     * \return pointer to the last received StarStim data.
     */
    StarstimData * getStarStimData ();

    /*!
     * It resets the protocol status so a new frame is ready to be parsed.
     */
    void reset ();

    /*!
     *It builds an Enobio3G/StarStim frame request for starting the EEG
     *streaming.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStartEEGFrame ();

    /*!
     *It builds an Enobio3G/ StarStim frame request for stopping the EEG
     *streaming.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStopEEGFrame ();

    /*!
     * It builds a StarStim frame request for starting the stimtulation.
     *
     * \return Byte array that contains the request
     */
    static QByteArray buildStartStimulationFrame ();

    /*!
     * It builds a StarStim frame request for stopping the stimtulation.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStopStimulationFrame ();

    /*!
     * It builds a StarStim frame request for starting the impedance
     * measurements.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStartImpedanceFrame ();

    /*!
     * It builds a StarStim frame request for stopping the impedance
     * measurements.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStopImpedanceFrame ();

    /*!
     * It builds an Enobio3G/StarStim frame request for writing a EEG
     * configuration register.
     *
     * \param address Address of the EEG register.
     *
     * \param value Pointer to the set of values to be written.
     *
     * \param length Number of register to be updated with the provided values.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildWriteRegisterFrame (int address,
                                                  unsigned char *value,
                                                  int length);

    /*!
     * It builds an Enobio3G/StarStim frame request for reading a EEG
     * configuration register.
     *
     * \param address Address of the EEG register.
     *
     * \param length Number of consecutive register to be read
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildReadRegisterFrame (int address, int length = 1);

    /*!
     * It builds an Enobio3G/StarStim frame request for the battery
     * measurement.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildProfileRequest ();

    /*!
     * It builds an Enobio3G/StarStim frame request for keeping the
     * communication alive.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildNullRequest ();

    /*!
     * It builds an Enobio3G/StartStim frame that requests starting the sending
     * of beacons.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStartBeaconRequest ();

    /*!
     * It builds an Enobio3G/StartStim frame that requests stopping the sending
     * of beacons.
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildStopBeaconRequest ();

    /*!
     * It builds an Enobio3G/StartStim frame that requests the device to enter in bootloader mode
     *
     * \param nPages number of pages that shape the bootloader
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildBootloaderModeRequest(int nPages);

    /*!
     * It builds an Enobio3G/StartStim frame that downloads the calibration paramters for the stimulation
     *
     * \param nPages number of pages that shape the bootloader
     *
     * \return Byte array that contains the request.
     */
    static QByteArray buildDownloadCalibrationParamtersRequest(int* offsetArray, int* gain_n_Array, int* gain_d_Array);

    /*!
     * \property StarStimProtocol::SFD_0
     *
     * First byte of the Start of Frame Delimiter.
     */
    static const int SFD_0;

    /*!
     * \property StarStimProtocol::SFD_1
     *
     * Second byte of the Start of Frame Delimiter.
     */
    static const int SFD_1;

    /*!
     * \property StarStimProtocol::SFD_2
     *
     * Third byte of the Start of Frame Delimiter.
     */
    static const int SFD_2;

    /*!
     * \property StarStimProtocol::EFD_0
     *
     * First byte of the End of Frame Delimiter.
     */
    static const int EFD_0;

    /*!
     * \property StarStimProtocol::EFD_1
     *
     * Second byte of the End of Frame Delimiter.
     */
    static const int EFD_1;

    /*!
     * \property StarStimProtocol::EFD_2
     *
     * Third byte of the End of Frame Delimiter.
     */
    static const int EFD_2;

    /*!
     * It sets the firmware version of the Enobio3G/StartStim.
     *
     * \param firmwareVersion.
     */
    void setFirmwareVersion(int firmwareVersion);

    /*!
     * It indicates whether the device is doing stimulation.
     *
     * \param value.
     */
    void setIsStimulating(bool value);

    /*!
     * Sets whether StarstimProtocol is working in multisample
     *
     * \param value.
     */
    void setMultipleSample( int value ){ _multipleSample = value; }
    int getMultiSample(){ return _multipleSample; }


    /*!
     * \enum EEGCompressionType
     *
     * Different configurations for compression
     */
    typedef enum {EEG_NO_COMPRESSION = 0, EEG_16BIT_COMPRESSION = 1, EEG_12BIT_COMPRESSION = 2} EEGCompressionType;

    /*!
     * Sets whether StarstimProtocol is working with compressed EEG samples
     *
     * \param value.
     */
    void setEEGCompressionType( EEGCompressionType value ){ _eegCompresssionType = value; }
    EEGCompressionType getEEGCompressionType( void ){ return _eegCompresssionType; }


private:

    // ATTRIBUTES
    // ----------------

    /*!
     * \property _firmwareVersion
     *
     * Firmware version of the connected device
     */
    int _firmwareVersion;

    /*!
     * \property _isStimulating
     *
     * Booleand indicating if the device is stimulating
     */
    bool _isStimulating;

    /*!
     * \enum StatusProtocol
     *
     * State machine values
     */
    typedef enum
    {
        ST_IDLE,
        ST_LENGTH0,
        ST_LENGTH1,
        ST_STATUS_0,
        ST_CONTENT_0,
        ST_EEG_DATA,
        ST_EEG_CH_INFO_0,
        ST_EEG_CH_INFO_1,
        ST_EEG_CH_INFO_2,
        ST_EEG_CH_INFO_3,
        ST_EEG_DATA_RAW_MSB,
        ST_EEG_DATA_RAW_MSBLSB,
        ST_EEG_DATA_RAW_LSB,
        ST_EEG_COMPRESSED_16BIT_MSB,
        ST_EEG_COMPRESSED_16BIT_LSB,
        ST_EEG_COMPRESSED_12BIT_S1,
        ST_EEG_COMPRESSED_12BIT_S1S2,
        ST_EEG_COMPRESSED_12BIT_S2,
        ST_EEG_STAMP,
        ST_EEG_STAMP_2,
        ST_EEG_STAMP_3,
        ST_EEG_STAMP_4,
        ST_CONFIG_REGS_0,
        ST_CONFIG_REGS_1,
        ST_CONFIG_REGS_NUM,
        ST_CONFIG_REGS_DATA,
        ST_STIM_CH_INFO_0,
        ST_STIM_CH_INFO_1,
        ST_STIM_CH_INFO_2,
        ST_STIM_CH_INFO_3,
        ST_STIM_DATA_MSB,
        ST_STIM_DATA_LSB,
        ST_ACCEL_CONFIG,
        ST_ACCEL_CONFIG_NUM_REGS,
        ST_ACCEL_CONFIG_REGS,
        ST_STIM_IMPEDANCE_CH_INFO_0,
        ST_STIM_IMPEDANCE_CH_INFO_1,
        ST_STIM_IMPEDANCE_CH_INFO_2,
        ST_STIM_IMPEDANCE_CH_INFO_3,
        ST_STIM_IMPEDANCE_DATA_MSB,
        ST_STIM_IMPEDANCE_DATA_MSBLSB,
        ST_STIM_IMPEDANCE_DATA_LSBMSB,
        ST_STIM_IMPEDANCE_DATA_LSB,
        ST_PROFILE_BATTERY,
        ST_PROFILE_BATTERY_1,
        ST_PROFILE_BATTERY_2,
        ST_PROFILE_BATTERY_3,
        ST_PROFILE_FIRMWARE,
        ST_PROFILE_FIRMWARE_1,
        ST_PROFILE_SYNCH,
        ST_PROFILE_SYNCH_1,
        ST_PROFILE_SYNCH_2,
        ST_PROFILE_SYNCH_3,
        ST_PROFILE_SYNCH_4,
        ST_PROFILE_SYNCH_5,
        ST_PROFILE_SYNCH_6,
        ST_PROFILE_SYNCH_7,
        ST_PROFILE_DEV_TYPE,
        ST_PROFILE_N_CHANNELS,
        ST_ACCELEROMETER,
        ST_ACCELEROMETER_1,
        ST_ACCELEROMETER_2,
        ST_ACCELEROMETER_3,
        ST_ACCELEROMETER_4,
        ST_ACCELEROMETER_5,
        ST_EOF,
        ST_EOF_1,
        ST_EOF_2
    } StatusProtocol;

    // FUNCTIONS
    // ----------------

    /*!
     * \brief _addSOF adds SOF field to the frame
     *  indicated and a space for length field
     * \param frame
     */
    static void _addSOF(QByteArray& frame);

    /*!
     * \brief _addHeader adds SOF, length, action, RD/WR fields to the frame
     *  indicated and a space for length field
     * \param frame
     */
    static void _addHeader(QByteArray& frame);

    /*!
     * \brief _addEOF adds EOF together with length to the frame
     * \param frame
     */
    static void _addTail(QByteArray& frame);

    /*!
     * It performs the operations that are needed to pass from the current
     * states to the next one
     *
     * \param status New status to which the state machine performs the
     * transition
     */
    void _statusTransition (StatusProtocol status);

    /*!
     * It processes the status byte #0 that contents information regarding the
     * rest of the frame and the status of the device.
     *
     * \param status byte that contents the information of both the rest of the
     * frame and the device at bit-level.
     */
    void _processStatusByte0 (unsigned char status);

    /*!
     * It processes the status byte #1 that contents information regarding the
     * rest of the frame and the status of the device.
     *
     * \param status byte that contents the information of both the rest of the
     * frame and the device at bit-level.
     */
    void _processStatusByte1 (unsigned char content_byte0);

    /*!
     * It returns the next channel present in the frame according to the
     * information channel byte passed as parameter and the channel currently
     * been processed.
     *
     * \param channelInfo Byte that contains the channels that are present in
     * the received frame at bit level.
     *
     * \return number of the next channel that is going to be processed. If
     * there is no more channels to be processed an out of range value is
     * returned.
     *
     * \post The channel currently been processed is updated accordingly.
     */
    unsigned char _findNextChannel (unsigned int channelInfo);

    /*!
     * \return True if there is no more EEG channels to be processed in the
     * current frame.
     */
    bool _isLastEEGChannel ();

    /*!
     * It processes the byte that contains the informaion regarding the EEG
     * channels that are present in the current frame.
     *
     * \paragraph eegChInfo Byte that contains the EEG channels that are
     * present in the received frame at bit level.
     *
     * \return True if there is at least one channel present in the information
     * byte, false otherwise.
     */
    bool _processEEGChannelInfo (unsigned int eegChInfo);

    /*!
     * It informs whether the current EEG register address is the last one
     * present in the frame.
     *
     * \return True if the current EEG register address that
     * has been processed is the last one present in the frame
     */
    bool _isLastEEGReg ();

    /*!
     * It informs whether the current stimulation channel is the last one
     * present in the frame.
     *
     * \return True if there is no more stimulation channels to be processed in
     * the current frame
     */
    bool _isLastStimChannel ();

    /*!
     * It informs whether the current impedance channel is the last one present
     * in the frame.
     *
     * \return True if there is no more channel where to read impedance from.
     */
    bool _isLastStimImpedanceChannel ();

    /*!
     * It processes the byte that contains the informaion regarding the
     * stimulation channels that are present in the current frame.
     *
     * \param stimChInfo Byte that contains the stimulation channels that are
     * present in the received frame at bit level.
     *
     * \return True if there is at least one channel present in the information
     * byte, false otherwise.
     */
    bool _processStimChannelInfo (unsigned int stimChInfo);

    /*!
     * It processes the byte that contains the informaion regarding the
     * impedance of the stimulation channels that are present in the current
     * frame.
     *
     * \param chInfo Byte that contains the impedance of the stimulation
     * channels that are present in the received frame at bit level.
     *
     * \return True if there is at least one channel present in the information
     * byte, false otherwise.
     */
    bool _processStimImpedanceChannelInfo (unsigned int chInfo);

    /*!
     * It informs whether the current stimulation register address is the last
     * one present in the frame.
     *
     * \return True if the current stimulation register addres been processed
     * is the last one present in the frame
     */
    bool _isLastStimReg ();

    /*!
     * It updates the state machine to be receiving the nex block of data
     * according to the information that the status bytes carried and the
     * current processed block.
     *
     * \return True if there is any other data block to process, false
     * otherwise.
     */
    bool _transitionToNextBlock ();

    /*!
     * It resets the state machine so a new frame can be received.
     */
    void _resetStateMachine ();

    /*!
     * It checks if the Start Of Frame delimites has been received.
     *
     * \param byte Last received byte.
     *
     * \return True if the Start Of Frame has been detected, false otherwise.
     */
    bool _isStartOfFrame (unsigned char byte);

    /*!
     * It checks if the EndOfFrame has been received.
     *
     * \param byte Last received byte.
     *
     * \return True if the EndOfDelimiter has been detected, false otherwise.
     */
    bool _isEndOfFrame (unsigned char byte);

    /*!
     * \\property StarStimProtocol::StarStimData
     *
     * The parsed data is stored in that object.
     */
    StarstimData _starStimData;

    /*!
     * \property StarStimProtocol::_dataLength
     *
     * It stores the length of the data frame.
     */
    int _dataLength;

    /*!
     * \property StarStimProtocol::_state
     *
     * It keeps the value of the current state of the parser's state machine.
     */
    StatusProtocol _state;

    /*!
     * \property StarStimProtocol::_previousCommandToggle
     *
     * It keeps track of the bit that indicates if the last command sent was
     * processed.
     */
    unsigned char _previousCommandToggle;

    /*!
     * \property StarStimProtocol::_isEEGDataProcessed
     *
     * It marks whether the EEG data block has been processed or not.
     */
    bool _isEEGDataProcessed;

    /*!
     * \property StarStimProtocol::_isEEGConfigProcessed
     *
     * It marks whether the EEG configuration block has been processed or not.
     */
    bool _isRegConfigProcessed;

    /*!
     * \property StarStimProtocol::_isStimDataProcessed
     *
     * It marks whether the StarStim data block has been processed or not.
     */
    bool _isStimDataProcessed;

    /*!
     * \property StarStimProtocol::_isStimConfigProcessed
     *
     * It marks whether the StarStim configuration block has been processed or
     * not.
     */
    bool _isStimConfigProcessed;

    /*!
     * \property StarStimProtocol::_isStimImpedanceProcessed
     *
     * It marks whether the StarStim Impedance configuration block has been
     * processed or not.
     */
    bool _isStimImpedanceProcessed;

    /*!
     * \property StarStimProtocol::_isBatteryProcessed
     *
     * It marks whether the battery data block has been processed or not.
     */
    bool _isBatteryProcessed;

    /*!
     * \property StarStimProtocol::_isAccelerometerProcessed
     *
     * It marks whether the accelerometer block has been processed or
     * not.
     */
    bool _isAccelDataProcessed;

    /*!
     * \property StarStimProtocol::_isFirmWareProcessed
     *
     * It marks whether the firmware version block has been processed or
     * not.
     */
    bool _isFirmwareProcessed;

    /*!
     * \property StarStimProtocol::_currentChannel
     *
     * It stores the channel whose data is currently being parsed.
     */
    unsigned char _currentChannel;

    /*!
     * \property StarStimProtocol::_currentSample
     *
     * It stores the sample samples are collected.
     */
    unsigned char _currentSample;

    /*!
     * \property StarStimProtocol::_multipleSample
     *
     * Configures the StarStimProtocol to work in multiple sample mode
     */
    int _multipleSample;

    /*!
     * \property StarStimProtocol::_compressionEnabled
     *
     * Configures the StarStimProtocol to handle compressed EEG Samples
     */
    EEGCompressionType _eegCompresssionType;

    /*!
     * \property StarStimProtocol::_currentConfigAddress
     *
     * It stores the address of the eeg/stimulation register that is currently
     * being processed.
     */
    int _currentConfigAddress;

    /*!
     * \brief isLength2Bytes Holds whether the current device has length of two bytes
     */
    bool _isLength2Bytes;

    int _artifactCheckerCounter;
    int _lastArtifactCheckerCounter;
    int _artifactLastValues[32];

    int _status;

    unsigned char _debugErrorFrame[3000];
    unsigned int _debugErrorFrameIndex;
    int _lastChannelWithEEG;
    unsigned char _lastDeviceStatus;

    int _nBytes;



    unsigned int _temp;
    unsigned char _stamp;
    unsigned char _eof[4];
    unsigned char _sfd[4];
    unsigned int _counter;

    int _sdcardRecording;

};

#endif // STARSTIMPROTOCOL_H
