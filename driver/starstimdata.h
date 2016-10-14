#ifndef STARSTIMDATA_H
#define STARSTIMDATA_H

// Mingw includes
#include <stdint.h>

// Qt includes
#include <QString>
#include <QVector>
#include <QDebug>

// Project includes
#include "channeldata.h"
#include "commonparameters.h"
#include "devicemanagertypes.h"

#include "fw/stim_mgr.h"
#include "fw/eeg_mgr.h"
#include "fw/sdcard_mgr.h"
#include "fw/accel_mgr.h"

// Defines the maximum number of registers for every bank
#define MAX_CONF_REGISTERS  (4096)


/*!
 * \class StarStimData icognosdata.h
 *
 * \brief This class holds all the information from the icognos3G/StarStim
 * beacon frames.
 */
class StarstimData
{
public:

    /*!
     * Default constructor.
     */
    StarstimData();

    /*!
     * This function resets all the fields so the object can be reused.
     */
    void empty();

    /*!
     * It compares the two StarStimData instances.
     *
     * \param ssd1 First StarStimData instance to be compared.
     * \param ssd2 Second StarStimData instance to be compared.
     *
     * \return True if all the fields of both StarStimData instances have the
     * same values.
     */
    static bool compare (StarstimData& ssd1, StarstimData& ssd2);

    /*!
     * It returns the value of all the filds into a QString.
     *
     * \return Formatted QString with all StarStim field values.
     */
    QString toString ();


    /*!
     * It returns the device status value.
     *
     * \return The device status.
     */
    int deviceStatus() { return _deviceStatus; }

    /*!
     * It returns the device error.
     *
     * \return The device error.
     */
    int deviceError(){ return _deviceError; }

    /*!
     * It returns if the acknowledge command bit is toggled.
     *
     * \return True if the acknowledge command bit has toggled regarding the
     * previous data.
     */
    bool isCommandToggled(){ return _isCommandToggled; }

    /*!
     * It returns if the SD card is being recorded.
     *
     * \return True if the SD card is being recorded.
     */
    bool isSDCardRecording(){ return _isSDCardRecording; }

    /*!
     * It returns whether the current StarStim data has information regarding
     * EEG.
     *
     * \return True if EEG data is present in the current icognos3G/StarStim
     * data, false otherwise.
     */
    bool isEEGDataPresent(){ return _isEEGDataPresent; }

    /*!
     * It returns whether the current icognos3G/StarStim data has information
     * regarding EEG configuration registers.
     *
     * \return True if EEG configuration data is present in the current
     * icognos3G/StarStim data, false otherwise.
     */
    bool isRegConfigPresent(){ return _isRegConfigPresent; }

    /*!
     * It returns whether the current StarStim data has information regarding
     * stimulation data.
     *
     * \return True if stimulation data is present in the current StarStim
     * data, false otherwise.
     */
    bool isStimDataPresent(){ return _isStimDataPresent; }

    /*!
     * It returns whether the current StarStim data has information regarding
     * Impedance measurements.
     *
     * \return True if Impedance data is present in the current StarStim data,
     * false otherwise.
     */
    bool isStimImpedancePresent(){ return _isStimImpedancePresent; }


    /*!
     * It returns whether the current icognos3G/StarStim data has information
     * regarding the Accelerometer.
     *
     * \return True if the data from the Accelerometer is present in the
     * current icognos3G/StarStim data, false otherwise.
     */
    bool isAccelDataPresent(){ return _isAccelDataPresent; }


    /*!
     * It returns whether the current icognos3G/StarStim data has information
     * regarding the firmware version.
     *
     * \return True if the data from the firmware version is present in the
     * current icognos3G/StarStim data, false otherwise.
     */
    bool isFirmwareVersionPresent(){ return _isFirmwareVersionPresent; }

    /*!
     * It returns whether the current icognos3G/StarStim data has information
     * regarding the battery charging level.
     *
     * \return True if the data from the battery is present in the current
     * icognos3G/StarStim data, false otherwise.
     */
    bool isProfilePresent(){ return _isProfilePresent; }

    /*!
     * It returns a pointer to a vector of booleans. The values inform whether
     * the corresponding channel is present in the EEG data. The first value in
     * the vector corresponds to channel 0.
     *
     * \return Pointer to the boolean vector with the channels present in the
     * EEG data.
     */
    bool * isEEGChPresent(){ return _isEEGChPresent; }

    /*!
     * It returns a pointer to a vector of booleans. The values inform whether
     * the corresponding channel is present in the stimulation data. The first
     * value in the vector corresponds to channel 0.
     *
     * \return Pointer to the boolean vector with the channels present in the
     * stimulation data.
     */
    bool * isStimChPresent(){ return _isEEGChPresent; }

    /*!
     * It returns a pointer to a vector of booleans. The values inform whether
     * the corresponding channel is present in the Impedance data. The first
     * value in the vector corresponds to channel 0.
     *
     * \return Pointer to the boolean vector with the channels present in the
     * impedance data.
     */
    bool * isStimImpedanceChPresent(){ return _isStimImpedanceChPresent; }

    /*!
     * It returns the EEG channel info byte that is sent before the EEG data to
     * know which channels are reported.
     *
     * \return The EEG channel info byte.
     */
    unsigned int eegChInfo();

    /*!
     * It returns the EEG stamp byte that is sent after the EEG data to check
     * if there have been lost packets.
     *
     * \return The EEG stamp.
     */
    unsigned int eegStamp (){ return _eegStamp; }

    /*!
     * It returns the start address of the EEG configuration registers that are
     * send in the icognos3G/StarStim beacon frame.
     *
     * \return EEG start address byte.
     */
    int eegStartAddress(){ return _eegStartAddress; }

    /*!
     * It returns the number of EEG configuration registers that are sent in
     * the icognos3G/StarStim beacon frame.
     *
     * \return Number of EEG configuration registers in the icognos3G/StarStim
     * beacon frame.
     */
    unsigned char eegNumRegs(){ return _eegNumRegs; }

    /*!
     * It returns the pointer to the EEG configuration registers that are sent
     * in the icognos3G/StarStim beacon frame.
     *
     * \return Pointer to the EEG configuration registers in the
     * icognos3G/StarStim beacon frame.
     */
    unsigned char * confReg(){ return _confReg; }

    /*!
     * It returns the stimulation channel info byte that is sent before the
     * stimulation data to know which channels are reported.
     *
     * \return The stimulation channel info byte.
     */
    unsigned int stimChInfo(){ return _stimData.channelInfo(); }

    /*!
     * It returns the impedance channel info byte that is sent before the
     * impedance data to know which channels are reported.
     *
     * \return The impedance channel info byte.
     */
    unsigned int stimImpedanceChInfo(){ return _stimImpedance.channelInfo(); }

    /*!
     * It returns the start address of the stimulation configuration registers
     * that are send in the StarStim beacon frame.
     *
     * \return Stimulation start address byte.
     */
    unsigned char stimStartAddress(){ return _stimStartAddress; }

    /*!
     * It returns the number of stimulation configuration registers that are
     * send in the StarStim beacon frame.
     *
     * \return Number of stimulation configuration registers in the StarStim
     * beacon frame.
     */
    unsigned char stimNumRegs(){ return _stimNumRegs; }

    /*!
     * It returns the pointer to the stimulation configuration registers that
     * are send in the StarStim beacon frame.
     *
     * \return Pointer to the stimulation configuration registers in the
     * StarStim beacon frame.
     */
    unsigned char * stimReg(){ return _stimReg; }

    /*!
     * It returns the acceleroemter value sent in the icognos3G/StarStim beacon
     * frame.
     *
     * \return Acceleremoter value in the icognos3G/StarStim beacon frame.
     */
    ChannelData & accelerometer(){ return _accelerometer; }

    /*!
     * It returns the firmware version value sent in the icognos3G/StarStim beacon
     * frame.
     *
     * \return firmwareVersion value in the icognos3G/StarStim beacon frame.
     */
    int firmwareVersion(){ return _firmwareVersion; }

    /*!
     * It returns the battery value sent in the icognos3G/StarStim beacon
     * frame.
     *
     * \return Battery value in the icognos3G/StarStim beacon frame.
     */
    unsigned int battery(){ return _battery; }

    /*!
     * \return It returns the synchronization time T1
     */
    unsigned int synchT1(){ return _synchT1; }

    /*!
     * \return It returns the synchronization time T2
     */
    unsigned int synchT2(){ return _synchT2; }

    /*!
     * It returns the stimulation data sample.
     *
     * \return Stimulation data of the channels that are sent in the beacon
     * frame.
     */
    ChannelData & stimulationData(){ return _stimData; }

    /*!
     * It returns the EEG Data Sample Array
     *
     * \return EEG Data Array of the channels
     */
    QVector<ChannelData>& eegDataArray(){ return _eegDataArray; }

    /*!
     * It returns the number of samples present in the beacon
     *
     * \return Number of samples present in the beacon
     */
    int nSamples(){ return _nSamples; }

    /*!
     * It returns the impedance data sample.
     *
     * \return Impedance data of the channels that are sent in the beacon
     * frame.
     */
    ChannelData & stimImpedanceData(){ return _stimImpedance; }

    /*!
     * It sets the device status value.
     *
     * \param deviceStatus The device status value.
     */
    void deviceStatus(int deviceStatus){ _deviceStatus = deviceStatus; }

    /*!
     * It sets the device error value.
     *
     * \param deviceError The device error value.
     */
    void deviceError(int deviceError){ _deviceError = deviceError; }

    /*!
     * It sets whether the current icognos3G/StarStim beacon frame has toggled
     * the acknowledge command bit.
     *
     * \param status True if the acknowledge command bit is toggled, false
     * otherwise.
     */
    void isCommandToggled(bool status);

    /*!
     * It sets whether the SD Card is recording.
     *
     * \param status True if the SD card is recording, false otherwise.
     */
    void isSDCardRecording(bool status);

    /*!
     * It sets whether the EEG data is present in the current icognos3G/StarStim
     * beacon frame.
     *
     * \param status True if the EEG data is present, false otherwise.
     */
    void isEEGDataPresent(bool status){ _isEEGDataPresent = status; }

    /*!
     * It sets whether the EEG configuration data is present in the current
     * icognos3G/StarStim beacon frame.
     *
     * \param status True if the EEG configuration data is present, false
     * otherwise.
     */
    void isRegConfigPresent(bool status){ _isRegConfigPresent = status; }

    /*!
     * It sets whether the stimulation data is present in the current StarStim
     * beacon frame.
     *
     * \param status True if the stimulation data is present, false otherwise.
     */
    void isStimDataPresent(bool status){ _isStimDataPresent = status; }

    /*!
     * It sets whether the impedance data is present in the current StarStim
     * beacon frame.
     *
     * \param status True if the impedance data is present, false otherwise.
     */
    void isStimImpedancePresent(bool status){ _isStimImpedancePresent = status; }


    /*!
     * It sets whether the accelerometer data is present in the current
     * icognos3G/StarStim beacon frame.
     *
     * \param status True if the accelerometer data is present, false
     * otherwise.
     */
    void isAccelDataPresent(bool status){ _isAccelDataPresent = status; }


    /*!
     * It sets whether the firmware version data is present in the current
     * icognos3G/StarStim beacon frame.
     *
     * \param status True if the firmware version data is present, false
     * otherwise.
     */
    void isFirmwareVersionPresent(bool status){ _isFirmwareVersionPresent = status; }

    /*!
     * It sets whether the battery data is present in the current
     * icognos3G/StarStim beacon frame.
     *
     * \param status True if the battery data is present, false otherwise.
     */
    void isProfilePresent (bool status) { _isProfilePresent = status; }

    /*!
     * It sets whether the EEG data from an specific channel is present in the
     * icognos3G/StarStim beacon frame.
     *
     * \param index Zero-based index of the channel which its presence status
     * is set.
     *
     * \param status True if the channel is present in the EEG data, false
     * otherwise.
     */
    void isEEGChPresent(int index, bool status);

    /*!
     * It sets whether the stimulation data from an specific channel is present
     * in the StarStim beacon frame.
     *
     * \param index Zero-based index of the channel which its presence status
     * is set.
     *
     * \param status True if the channel is present in the stimulation data,
     * false otherwise.
     */
    void isStimChPresent(int index, bool status);

    /*!
     * It sets whether the impedance data from an specific channel is present
     * in the StarStim beacon frame.
     *
     * \param index Zero-based index of the channel which its presence status
     * is set.
     *
     * \param status True if the channel is present in the impedance data,
     * false otherwise.
     */
    void isStimImpedanceChPresent(int index, bool status);

    /*!
     * It sets the EEG channel info.
     *
     * \param chInfo The EEG channel info.
     */
    void eegChInfo(unsigned int chInfo);

    /*!
     * It sets the EEG data corresponding to a specific channel.
     *
     * \param index Zero-based index of the channel.
     *
     * \param value EEG data value of the channel.
     *
     * \param sample number of sample to be stored
     */
    void eegData(int index, int value, int sample, int eegCompressionType);



    /*!
     * Sets the number of samples in the current Beacon
     *
     * \param nSamples number of samples in the current beacon
     */
    void nSamples( int value );

    /*!
     * It sets the EEG stamp.
     *
     * \param value The EEG stamp.
     */
    void eegStamp (unsigned int value){ _eegStamp = value; }

    /*!
     * It sets the start address of the EEG configuration registers that are
     * present in the icognos3G/StarStim beacon frame.
     *
     * \param startAddress The EEG start address.
     */
    void eegStartAddress(int startAddress){ _eegStartAddress = startAddress; }

    /*!
     * It sets the number of EEG configuration registers that are present in
     * the icognos3G/StarStim beacon frame.
     *
     * \param numRegs The number of EEG configuration registers.
     */
    void eegNumRegs(unsigned char numRegs){ _eegNumRegs = numRegs; }

    /*!
     * It sets the value of the EEG configuration registers that are present in
     * the icognos3G/StarStim beacon frame.
     *
     * \param address The EEG register address.
     *
     * \param value The EEG register value.
     */
    void confReg(int address, unsigned char value);

    /*!
     * It sets the stimulation channel info.
     *
     * \param chInfo The stimulation channel info.
     */
    void stimChInfo(unsigned int chInfo);

    /*!
     * It sets the impedance channel info.
     *
     * \param chInfo The impedance channel info.
     */
    void stimImpedanceChInfo(unsigned int chInfo);

    /*!
     * It sets the stimulation data corresponding to a specific channel.
     *
     * \param index Zero-based index of the channel.
     *
     * \param value The stimulation data value of the channel.
     */
    void stimData(int index, int value);

    /*!
     * It sets the impedance data corresponding to a specific channel.
     *
     * \param index Zero-based index of the channel.
     *
     * \param value The impedance data value of the channel.
     */
    void stimImpedance(int index, int value);

    /*!
     * It sets the start address of the stimulation configuration registers
     * that are present in the StarStim beacon frame.
     *
     * \param startAddress The stimulation start address.
     */
    void stimStartAddress(unsigned char startAddress){ _stimStartAddress = startAddress; }

    /*!
     * It sets the number of stimulation configuration registers that are
     * present in the StarStim beacon frame.
     *
     * \param numRegs The number of stimulation configuration registers.
     */
    void stimNumRegs(unsigned char numRegs){ _stimNumRegs = numRegs; }

    /*!
     * It sets the value of the stimulation configuration registers that are
     * present in the StarStim beacon frame.
     *
     * \param address The stimulation register address.
     *
     * \param value The stimulation register value.
     */
    void stimReg(int address, unsigned char value);

    /*!
     * It sets the value of the accelerometer data that is present in the
     * icognos3G/StarStim beacon frame.
     *
     * \param index The index of the 3-dimension vector
     *
     * \param value The accelerometer value.
     */
    void accelerometer(int index, int value);

    /*!
     * It sets the value of the firmware version data that is present in the
     * icognos3G/StarStim firmware version frame.
     *
     * \param value The firmwareVersion value.
     */
    void firmwareVersion(int value){ _firmwareVersion = value; }

    /*!
     * It sets the value of the battery data that is present in the
     * icognos3G/StarStim beacon frame.
     *
     * \param value The battery value.
     */
    void battery(unsigned int value){ _battery = value; }

    /*!
     * It sets the value of the synchronization time T1.
     *
     * \param value The T1 value.
     */
    void synchT1(unsigned int value){ _synchT1 = value; }

    /*!
     * It sets the value of the synchronization time T2.
     *
     * \param value The T2 value.
     */
    void synchT2(unsigned int value){ _synchT2 = value; }

    /*!
     * \brief deviceType sets the type of device
     *
     * \param type value of the device type
     */
    void deviceType(unsigned char type ){ _deviceType = (DeviceManagerTypes::DeviceType) type; }

    /*!
     * \brief deviceType Returns the type of device currently hold
     * \return
     */
    DeviceManagerTypes::DeviceType deviceType(){ return _deviceType; }

    /*!
     * \brief numOfChannels sets the number of channels
     * \param byte
     */
    void numOfChannels(unsigned char byte ){ _numOfChannels = byte; }

    /*!
     * \brief numOfChannels getter for the number of channels
     * \return
     */
    unsigned char numOfChannels(){ return _numOfChannels; }

    /*!
     * \property StarStimData::NumEEGConfigReg
     *
     * Number of EEG configuration registers.
     */
    static const int NumEEGConfigReg;

    /*!
     * \property StarStimData::NumStimConfigReg
     *
     * Number of stimulation configuration registers.
     */
    static const int NumStimConfigReg;

    /*!
     * \property StarStimData::NumAccelerometerConfigReg
     *
     * Number of accelerometer configuration registers.
     */
    static const int NumAccelerometerConfigReg;

    /*!
     * \property StarStimData::NumSDCardConfigReg
     *
     * Number of SDCard configuration registers.
     */
    static const int NumSDCardConfigReg;

private:
    /*!
     * \property StarStimData::_deviceStatus
     *
     * It stores the received device's status.
     */
    int _deviceStatus;

    /*!
     * \property StarStimData::_deviceError
     *
     * It stores the received device's error status.
     */
    int _deviceError;

    /*!
     * \property StarStimData::_isCommandToggled
     *
     * It stores the boolean that indicates whether the command bit has been
     * toggled.
     */
    bool _isCommandToggled;

    /*!
     * \property StarStimData::_sdCardRecording
     *
     * It stores the boolean that indicates whether the SD card is being written.
     */
    bool _isSDCardRecording;

    /*!
     * \property StarStimData::_isEEGDataPresent
     *
     * It informs whether the current frame contains the EEG data block.
     */
    bool _isEEGDataPresent;

    /*!
     * \property StarStimData::_isEEGConfigPresent
     *
     * It informs whether the current frame contains the Configuration EEG
     * register data block.
     */
    bool _isRegConfigPresent;

    /*!
     * \property StarStimData::_isStimDataPresent
     *
     * It informs whether the current frame contains the Stimulation data
     * block.
     */
    bool _isStimDataPresent;

    /*!
     * \propertu StarStimData::_isStimImpedance
     *
     * It informs whether the current frame contains the Impedance data.
     */
    bool _isStimImpedancePresent;


    /*!
     * \property StarStimData::_isAccelerometerPresent
     *
     * It informs whether the accelerometer data block is present in the
     * current frame.
     */
    bool _isAccelDataPresent;


    /*!
     * \property StarStimData::_isFirmwareVersionPresent
     *
     * It informs whether the firmware version data block is present in the
     * current frame.
     */
    bool _isFirmwareVersionPresent;

    /*!
     * \property StarStimData::isProfilePresent
     *
     * It informs whether the battery data block is present in the
     * current frame.
     */
    bool _isProfilePresent;

    /*!
     * \property StarStimData::_isEEGChPresent
     *
     * It keeps if each of the 8 channels are present in the EEG data block.
     */
    bool _isEEGChPresent[32];

    /*!
     * \property StarStimData::_isStimChPresent
     *
     * It keeps if each of the 8 channels are present in the StarStim data
     * block.
     */
    bool _isStimChPresent[8];

    /*!
     * \property StarStimData::_isStimChPresent
     *
     * It keeps if each of the 8 channels are present in the StarStim data
     * block.
     */
    bool _isStimImpedanceChPresent[8];

    /*!
     * \property StarStimData::eegDataArray
     *
     * It contains the EEG Samples in the present beacon
     */
    QVector<ChannelData> _eegDataArray;

    /*!
     * \property StarStimData::_eegStamp
     *
     * It keeps the EEG stamp.
     */
     unsigned int   _eegStamp;

    /*!
     * \property StarStimData::_eegStartAddress
     *
     * It stores the address of the first EEG register that appears in the
     * received frame.
     */
    int _eegStartAddress;

    /*!
     * \property StarStimData::_eegNumRegs
     *
     * It stores the number of EEG registers that are present in the received
     * frame.
     */
    unsigned char _eegNumRegs;

    /*!
     * \property StarStimData::_eegReg
     *
     * It is a vector of bytes that contains the value of the EEG configuration
     * registers that are present in the received frame
     */
    unsigned char _confReg[MAX_CONF_REGISTERS];

    /*!
     * \property StarStimData::_stimData
     *
     * It contains the Stimulation samples of the received frame.
     */
    ChannelData _stimData;

    /*!
     * \property StarStimData::_stimImpedance
     *
     * It contains the Impedance samples of the received frame.
     */
    ChannelData _stimImpedance;

    /*!
     * \property StarStimData::_stimStartAddress
     *
     * It stores the address of the first stimulation register that appears in
     * the received frame.
     */
    unsigned char _stimStartAddress;

    /*!
     * \property StarStimData::_stimNumRegs
     *
     * It stores the number of stimulation registers that are present in the
     * received frame.
     */
    unsigned char _stimNumRegs;

    /*!
     * \property StarStimData::_stimReg
     *
     * It is a vector of bytes that contains the value of the stimulation
     * configuration registers that are present in the received frame
     */
    unsigned char _stimReg[STM_NUM_REGS];

    /*!
     * \property StarStimData::_accelerometer
     *
     * It stores the accelerometer value present in the received frame.
     */
    ChannelData _accelerometer;

    /*!
     * \property StarStimData::_firmwareVersion
     *
     * It stores the firmware version present in the received frame.
     */
    int _firmwareVersion;


    /*!
     * \property StarStimData::nSamples
     *
     * It stores the number of samples in the current beacon
     */
    int _nSamples;

    /*!
     * \property StarStimData::_battery
     *
     * It stores the battery value present in the received frame.
     */
    unsigned int _battery;

    /*!
     * \property StarStimData::_synchT1
     *
     * It stores the synchronization time T1.
     */
    unsigned int _synchT1;

    /*!
     * \property StarStimData::_synchT1
     *
     * It stores the synchronization time T2.
     */
    unsigned int _synchT2;

    /*!
     * \property DeviceManagerTypes::DeviceType
     *
     * It stores the type of device connected
     */
    DeviceManagerTypes::DeviceType _deviceType;


    /*!
     * \property _numOfChannels
     *
     * It stores the number of channels of the device
     */
    unsigned char _numOfChannels;

};

#endif // STARSTIMDATA_H
