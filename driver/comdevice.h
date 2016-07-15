#ifndef COMDEVICE_H
#define COMDEVICE_H

#include <QObject>
#include "HWDevice.h"
#include "qextserialport.h"

/*!
 * \class COMDevice comdevice.h
 *
 * \brief This class implements the driver that communicates with the StarStim
 * device through a serial COM device
 */
class COMDevice : public QObject, HWDevice
{
    Q_OBJECT
public:
    COMDevice();
    ~COMDevice();

public:
    /*!
     * \enum errType
     *
     * Description of the possible error types.
     */
    enum errType
    {
        ERR_NO_ERROR = 0,
        ERR_DEVICE_NOT_CONNECTED,
        ERR_CLOSING_DEVICE,
        ERR_RESETING_DEVICE,
        ERR_DEVICE_NOT_OPENED,
        ERR_READING_DEVICE,
        ERR_WRITING_DEVICE,
        ERR_CONFIGURING_DEVICE,
        ERR_CONFIGURATION_VALUE
    };
    /*!
     * It performs the operations for openning the hardware device
     *
     * \return It returns negative number if the hardware can not be opened.
     */
    virtual HWErrType open ();

    /*!
     * It opens the device that is listed in the system in the position
     * indicated by index
     *
     * \param index Zero-based index of the device to be opened
     *
     * \return It returns negative number if the hardware can not be opened.
     */
    HWErrType open (unsigned int index);


    HWErrType open (QString COMName);

    /*!
     * It provides the number of devices that potentially could be a StarStim
     * device plugged to the system
     *
     * \return number of devices using the USB COM serial
     */
    unsigned int getNumDevices ();
    /*!
     * It performs the operations for closing the hardware device.
     *
     * \return It returns negative number if the hardware can not be opened.
     */
    virtual HWErrType close ();

    /*!
     * It performs the operations for resetting the hardwre device.
     */
    virtual HWErrType reset ();

    /*!
     * It performs the operations for configuring the hardware device according
     * with the parameters previously configured.
     */
    virtual bool configure ();

    /*!
     * It reads from the hardware device an specific number of bytes.
     *
     * \param buffer Pointer to buffer that receives the data from the device.
     *
     * \param numBytes Number of bytes to be read from the hardware device.
     *
     * \return Number of actual bytes read. If this number does not match with
     * the numBytes parameter, then check the getLastError method.
     */
    virtual int read (unsigned char * buffer, unsigned long numBytes);

    /*!
     * It writes to the hardware device the number of bytes specified in the
     * parameters.
     */
    virtual int write (unsigned char * buffer, unsigned long numBytes);

    /*!
     * It returns the last error that might have happened when accessing the
     * hardware device.
     *
     * \return Error type identifier.
     */
    virtual HWErrType getLastError ();

    /*!
     * It sets the timeout for reeading operation
     *
     * \param timeout time in milliseconds before returning from a reading
     * operation
     */
    void setReadTimeout (int timeout);

    /*!
     * It returns the pending bytes to be written
     */
    qint64 pendingBytesOnWriting ();

    /*!
     * It returns the pending bytes in the reading buffer
     */
    qint64 pendingBytesOnReading ();

signals:
    /*!
     * Signal that indicates there is data availabe to be read.
     *
     * \param numBytes Number of bytes available
     */
    void dataAvailable(int numBytes);

    /*!
     * Signal that announces that the device has been connected/disconnected
     *
     * \param status True if the device is connected, false otherwise
     */
    void statusConnectionChanged(bool status);

private slots:
    /*!
     * Slot to be internally called when the hardware has available data
     */
    void onDataAvailable ();

    /*!
     * Slot to be internally called when the hardware reports a change in the
     * DSR line
     *
     * \param status True if the DSR line is up, false otherwise
     */
    void onDSRChanged (bool status);

    /*!
     * Slot to be internally called when the hardware reports that some bytes
     * have been successfully written
     *
     * \param bytesWritten Number of bytes alredy written
     */
    void onBytesWritten (qint64 bytesWritten);

private:

    /*!
     * checks if the portName corresponds to a valid device name
     *
     * \param portName Port name wich might include the enumerator number
     *
     * \param validName Genreric name that does not include the enumerator
     * number
     *
     * \return True if portName corresponds to a valid device name
     */
    bool _isValidPort (QString portName, QString validName);

    /*!
     * Pointer to an instance of an external serial port
     */
    QextSerialPort * _port;

    /*!
     * Last error that might be happened
     */
    HWErrType _lastError;

    /*!
     * Bytes pending to be written
     */
    qint64 _pendingBytesInWriting;

    /*!
     * Bytes pending in the reception buffer of the device
     */
    qint64 _pendingBytesInReading;
};

#endif // COMDEVICE_H
