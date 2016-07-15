#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

// Qt includes
#include <QObject>
#include <QUdpSocket>
#include <QEventLoop>
#include <QTimer>

// Project includes
#include "commonparameters.h"

class DeviceConfiguration : public QObject
{
    Q_OBJECT
public:

    /*!
     * \enum WifiType
     *
     * Type of wifi to be configured
     */
    typedef enum WifiType{ ACCESS_POINT, INFRASTRUCTURE_DHCP, INFRASTRUCTURE_STATIC } WifiType;

    // ATTRIBUTES
    // ----------------

    /*!
     * \brief getIPAdddress returns the IP Address where the device was found
     * \return
     */
    QHostAddress getIPAdddress(){ return _deviceAddress; }


    // FUNCTIONS
    // ----------------

    /*!
     * Public constructor
     */
    explicit DeviceConfiguration(QObject *parent = 0);
    
    /*!
     * \brief find Attemps to find a device
     *
     * \return true if operation was successful
     */
    bool find(QHostAddress bdAddress, QHostAddress &deviceAddress);

    /*!
     * \brief setMode sets the mode of the WiFi network
     * \param wifiType
     * \return true if operation was successful
     */
    bool setMode(QHostAddress deviceAddress, WifiType wifiType);

    /*!
     * \brief setWifiConfiguration
     * \param SSID
     * \param password
     * \return true if operation was successful
     */
    bool setWifiConfiguration(QHostAddress deviceAddress, QString SSID, QString password);

    /*!
     * \brief setStatic configures the paramters to run the WiFi radio under a
     * infrastructure static scheme
     * \param deviceAddress
     * \param mask
     * \param gateway
     * \return
     */
    bool setStatic(QHostAddress deviceAddress, QHostAddress staticIP, QHostAddress mask, QHostAddress gateway);


    /*!
     * \brief getSerialNumber queries the device to get the MAC Address
     * \param deviceAddress
     * \param serialNumber
     * \return
     */
    bool getMACAddress(QHostAddress deviceAddress, QString &macAddress);

    /*!
     * \brief setMACAddress sets the specified MAC Address into the device
     * \param deviceAddress
     * \param macAddress
     * \return
     */
    bool setMACAddress(QHostAddress deviceAddress, QString macAddress);

    /*!
     * \brief getWIFIChannel queries the device to get the WIFI channel
     * \param deviceAddress
     * \param WIFIChannel
     * \return
     */
    bool getWIFIChannel(QHostAddress deviceAddress, QString &WIFIChannel);

    /*!
     * \brief setWIFIChannel sets the specified WIFI channel into the device
     * \param deviceAddress
     * \param WIFIChannel
     * \return
     */
    bool setWIFIChannel(QHostAddress deviceAddress, QString WIFIChannel);
private:

    // ATTRIBUTES
    // ----------------

    /*!
     * Private UDP socket to perform the search
     */
    QUdpSocket* udpSocket;


    /*!
     * \brief deviceAddress represents the address of the device found
     */
    QHostAddress _deviceAddress;


    /*!
     * \brief receivedAck indicates whether an ack was received from last
     * find procedure
     */
    bool _receivedAck;

    /*!
     * \brief _receivedCR indicates whether CR - LF are returned
     */
    bool _receivedCR;

    /*!
     * \brief _receivedData Holds the data received from the host
     */
    QString _receivedData;

    // FUNCTIONS
    // ----------------

signals:
    
    /*!
     * \brief receivedAck signal emitted whenever a new ACK has been received
     */
    void receivedAckSignal();

public slots:
    
    /*!
     * \brief handleReadyRead slot reached when a new UDP data has been received
     */
    void handleReadyRead();

};

#endif // STARSTIMSEARCH_H
