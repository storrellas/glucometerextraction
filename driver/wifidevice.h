#ifndef WIFIDEVICE_H
#define WIFIDEVICE_H

// Qt includes
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QApplication>
#include <QTimer>

// Project includes
#include "commonparameters.h"

/*!
 * \class WifiDevice wifidevice.h
 *
 * \brief This class implements the driver that communicates with the
 * icognos3G/StarStim device through the Wifi/TCP protocol.
 */
class WifiDevice : public QObject
{
    Q_OBJECT
public:

    QTimer* timer;


    /*!
     * Default constructor.
     */
    explicit WifiDevice(QObject *parent = 0);
    
        /*!
     * Destructor.
     */
    ~WifiDevice();


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


    /*!
     * It opens the connection with the device identified by the provided
     * Bluetooth mac address.
     *
     * \param host IP address of the device to connect
     *
     * \param port TCP port to be connected
     *
     * \return It returns ERR_DEVICE_NOT_CONNECTED if the hardware can not be
     * opened.
     */
    errType open (const char *host, int port);

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
    int read (char *buffer, unsigned long numBytes);

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
    int write (const char * buffer, unsigned long numBytes);

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
     * \brief _socketStarstimThread socket used to run the _icognosSocket
     * This is necessary since polling operations are not permited in QTcpSocket
     * If polling operations are required you should use QEventLoop
     * See here: https://www.qtdeveloperdays.com/2013/sites/default/files/presentation_pdf/Qt_Event_Loop.pdf
     */
    QThread* _socketStarstimThread;


    // Reading variables
public:
    /*!
     * \brief icognosSocket socket to connect to icognos
     */
    QTcpSocket* _icognosSocket;

    /*!
     * \brief mutex mutex used to communicate thread accross _readArray variable.
     */
    QMutex mutex;

    /*!
     * \brief _readArray shared variable
     */
    QByteArray _readArray;

    /*!
     * \property _timeout
     *
     * This property holds the timeout for the connection.
     */
    unsigned int _timeout;


signals:
    /*!
     * \brief writeToSocket this signal is used to indicate to _icognosSocket, which
     * lives in an _socketStarstimThread thread, that array needs to be written
     * \param array
     */
    void writeToSocket(QByteArray array);

public slots:
    /*!
     * \brief writeToSocket this signal is used to indicate to _icognosSocket, which
     * lives in an _socketStarstimThread thread, that array needs to be written
     * \param array
     */
    void onWriteToSocket(QByteArray array);

    /*!
     * \brief readyRead public slot for reading
     */
    void readyRead();

    /*!
     * \brief error Raised when an error
     * \param error
     */
    void error(QAbstractSocket::SocketError error);

};

#endif // WIFIDEVICE_H
