#ifndef DEVICEMANAGERTYPES_H
#define DEVICEMANAGERTYPES_H

#include <QString>

namespace DeviceManagerTypes {


    /*!
     * \enum SampleRate
     *
     * Sets the sample rate for EEG measuring
     */
    typedef enum SampleRate {
        _500_SPS_ = 0x01,
        _250_SPS_ = 0x02,
        _125_SPS_ = 0x04,
        _75_SPS_  = 0x08,
        _37_5_SPS_  = 0x0F
    }SampleRate;

    /*!
     * \enum DeviceType
     *
     * Indicates the type of device
     */
    typedef enum DeviceType {
        ENOBIO = 0x00,
        STARSTIM = 0x01
    }DeviceType;

    /*!
     * \enum StarStimRegisterFamily
     *
     * Description of the different register families.
     */
    typedef enum StarstimRegisterFamily {
        STIM_REGISTERS,
        EEG_REGISTERS,
        ACCEL_REGISTERS,
        SDCARD_REGISTERS
    }StarStimRegisterFamily;

    /*!
     * \enum StatusByteBits
     *
     * Enumeration of the bit positions in the status byte.
     */
    typedef enum StatusByteBits{
        EEG_BIT = 0x04,
        STM_BIT = 0x08,
        IMP_BIT = 0x02,
        SDCARD_BIT = 0x01
    }StatusByteBits;

    /*!
     * \enum StatusByteBits
     *
     * Struct containing
     */
    class DeviceStatus
    {
    public:
        static const int DEVICESTATUS_UNKNOWN = 0x00;

        /*!
         * \brief ESTABLISHED indicates whether the communication is established with device
         */
        bool ESTABLISHED;

        /*!
         * \brief CONNECTED indicates whether the device is connected
         */
        bool CONNECTED;
        /*!
         * \brief EEG indicates whether EEG is currently active
         */
        bool EEG;

        /*!
         * \brief ACCEL indicates whether ACCEL is currently active
         */
        bool ACCEL;

        /*!
         * \brief STIM indicates whether STIM is currently active
         */
        bool STIM;

        /*!
         * \brief IMP indicates whether IMP is currently active
         */
        bool IMP;


        /*!
         * \brief bool2string Converts a boolean to its QString representation
         * \param val
         * \return
         */
        QString bool2string(bool val){
            return QString(val?"TRUE":"FALSE");
        }

    public:


        /*!
         * \brief set sets the current status according to indicated
         * in 'status'
         * \param status
         */
        void set(int status, bool _connected, bool _established){
            this->CONNECTED   = _connected;
            this->ESTABLISHED = _established;
            this->EEG = this->ACCEL = this->STIM = this->IMP = false;
            if( this->CONNECTED == false ) return;

            // Only alter this when is connected
            if( status & DeviceManagerTypes::EEG_BIT ) this->EEG   = true;
            this->ACCEL; // Accel is not set via this method
            if( status & DeviceManagerTypes::STM_BIT ) this->STIM  = true;
            if( status & DeviceManagerTypes::IMP_BIT ) this->IMP   = true;
        }

        /*!
         * \brief toString Converts the current status to QString
         */
        QString toString(){
            QString aux;
            aux += "CONNECTED:"   + bool2string(this->CONNECTED) + ";";
            aux += "ESTABLISHED:" + bool2string(this->ESTABLISHED) + ";";
            aux += "EEG:"         + bool2string(this->EEG) + ";";
            aux += "ACCEL:"       + bool2string(this->ACCEL) + ";";
            aux += "STIM:"        + bool2string(this->STIM) + ";";
            aux += "IMP:"         + bool2string(this->IMP) + ";";
            return aux;
        }
    };

    /*!
     * \enum StarstimRequest
     *
     * Enumeration of the potentials request made by DeviceManager
     */
    typedef enum StarstimRequest{
        FIRMWARE_REQUEST,
        PROFILE_REQUEST,
        NULL_REQUEST,
        START_STREAMING_REQUEST,
        STOP_STREAMING_REQUEST,
        WRITE_REGISTER_REQUEST,
        READ_REGISTER_REQUEST,
        START_STIMULATION_REQUEST,
        STOP_STIMULATION_REQUEST,
        START_IMPEDANCE_REQUEST,
        STOP_IMPEDANCE_REQUEST
    }StarstimRequest;

    /*!
     * \brief DEVICESTATUS_UNKNOWN Device status indicator when the current status of the device is undefined
     */
    static const int DEVICESTATUS_UNKNOWN = 0xFF;

}

#endif // DEVICEMANAGERTYPES_H
