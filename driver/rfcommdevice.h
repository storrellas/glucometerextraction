#ifndef COMDEVICE_H
#define COMDEVICE_H

// Qt project includes
#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <QProcess>

// Project includes
#include "commonparameters.h"



/*!
 * \class RFCOMMDevice rfcommdevice.h
 *
 * \brief This class implements the driver that communicates with the
 * Enobio3G/StarStim device through the Bluetooth rfcomm protocol.
 */
class RFCOMMDevice : public QObject
{
    Q_OBJECT
public:
    /*!
     * Default constructor.
     */
    RFCOMMDevice();

    /*!
     * Destructor.
     */
    ~RFCOMMDevice();

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
        ERR_CONFIGURATION_VALUE,
        ERR_DEVICE_NOT_CONNECTED_WSAEINVAL
    };

    void logDebugBluetooth (QString string);

    /*!
     * It opens the connection with the device identified by the provided
     * Bluetooth mac address.
     *
     * \param macAddress Bluetooth mac address of the device.
     *
     * \return It returns ERR_DEVICE_NOT_CONNECTED if the hardware can not be
     * opened.
     */
    errType open (const unsigned char * macAddress);

    /*!
     * It performs the operations for closing the hardware device.
     *
     * \return It returns ERR_CLOSING_DEVICE if the hardware could not be
     * closed.
     */
    errType close ();

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
    int read (unsigned char * buffer, unsigned long numBytes);

    /*!
     * It writes to the hardware device the number of bytes specified in the
     * parameters.
     *
     * \param buffer Pointer to the buffer where the byte to be sen are placed.
     *
     * \param numBytes Number of bytes placed in buffer to be sent to the
     * device.
     *
     * \return Number of byte written to the device.
     */
    int write (unsigned char * buffer, unsigned long numBytes);

    /*!
     * It returns the last error that might have happened when accessing the
     * hardware device.
     *
     * \return Error type identifier.
     */
    errType getLastError ();

    /*!
     * It sets the timeout for reading operations.
     *
     * \param timeout time in microseconds before returning from a reading
     * operation
     */
    void setReadTimeout (int timeout);

    /*!
     * It returns the pending bytes in the reading buffer.
     *
     * \return Number of byte pending to be read.
     */
    qint64 pendingBytesOnReading ();

private:

    /*!
     * \property RFCOMMDevice::_lastError
     *
     * Last error that might have happened
     */
    errType _lastError;

    /*!
     * \property RFCOMMDevice::_handle
     *
     * Internal handle used in the hardwre operations.
     */
    unsigned int _handle;

    /*!
     * \property RFCOMMDevice::_rfcommTimeout
     *
     * This property holds the timeout for the reading operations.
     */
    unsigned int _rfcommTimeout;

    /*!
     * \property RFCOMMDevice::_numberOfInstances
     *
     * Counter of the number of instances of the class. It is used for the
     * Bluetooh initializations and closing operations.
     */
    static int _numberOfInstances;

#ifdef Q_OS_LINUX

    /*!
     * Checks whether the command indicated by string is existing
     *
     * \param command command to be checked
     * \return true on existence, false otherwise
     */
    bool checkcommandExists(QString command);
#endif
};

#endif // COMDEVICE_H
