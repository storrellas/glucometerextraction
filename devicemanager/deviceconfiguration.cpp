#include "deviceconfiguration.h"

#define UDP_PORT    (20000) // Port used for searching device

DeviceConfiguration::DeviceConfiguration(QObject *parent) :
    QObject(parent)
{

}

bool DeviceConfiguration::find(QHostAddress bdAddress, QHostAddress& deviceAddress){
    loggerMacroDebug("Searching for devices")


    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    loggerMacroDebug("Done! ")

    // UDP socket
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'echo:' to address '" + bdAddress.toString() + "' ...")
    udpSocket->writeDatagram(QByteArray("echo:"), bdAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

//    loggerMacroDebug("Current device address is " + deviceAddress.toString())

    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();

    // Return value for DeviceAddress
    deviceAddress = _deviceAddress;

    return _receivedAck;
}



bool DeviceConfiguration::setMode(QHostAddress deviceAddress, WifiType wifiType){

    // Determine mode
    QString mode = "accpoint";
    if( wifiType == INFRASTRUCTURE_DHCP   ) mode = "infras-d";
    if( wifiType == INFRASTRUCTURE_STATIC ) mode = "infras-s";

    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    loggerMacroDebug("Done! ")

    // UDP socket
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'set_mode:" + mode + "' to address '" + deviceAddress.toString() + "' ...")
    QString command = "set_mode:" + mode;
    udpSocket->writeDatagram(QByteArray(command.toUtf8()), deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

//    loggerMacroDebug("Current device address is " + deviceAddress.toString())

    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();

    return true;
}

bool DeviceConfiguration::setWifiConfiguration(QHostAddress deviceAddress, QString SSID, QString password){


    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    // Set SSID
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'set_ssid:" + SSID + "' to address '" + deviceAddress.toString() + "' ...")
    QString command = "set_ssid:" + SSID;
    udpSocket->writeDatagram(QByteArray(command.toUtf8()), deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

    if( _receivedAck == false ){
        loggerMacroDebug("Error while setting SSID")
        goto exit;
    }

    // Set Password
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'set_password:" + password + "' to address '" + deviceAddress.toString() + "' ...")
    command = "set_password:" + password;
    udpSocket->writeDatagram(QByteArray(command.toUtf8()), deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();


    if( _receivedAck == false ){
        loggerMacroDebug("Error while setting password")
        goto exit;
    }

exit:
    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();


    return _receivedAck;
}


bool DeviceConfiguration::setStatic(QHostAddress deviceAddress, QHostAddress staticIP, QHostAddress mask, QHostAddress gateway){


    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    // Set Device Address
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'set_static_ip:" + deviceAddress.toString() + "' to address '" + deviceAddress.toString() + "' ...")
    QString command = "set_static_ip:";
    QByteArray commandByteArray = QByteArray(command.toUtf8());
    commandByteArray.append( (staticIP.toIPv4Address() & 0x000000FF) );
    commandByteArray.append( (staticIP.toIPv4Address() & 0x0000FF00) >> 8 );
    commandByteArray.append( (staticIP.toIPv4Address() & 0x00FF0000) >> 16);
    commandByteArray.append( (staticIP.toIPv4Address() & 0xFF000000) >> 24);
    udpSocket->writeDatagram(commandByteArray, deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

    if( _receivedAck == false ){
        loggerMacroDebug("Error while setting Device Address")
        goto exit;
    }

    // Set Mask
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'set_static_mask:" + mask.toString() + "' to address '" + deviceAddress.toString() + "' ...")
    command = "set_static_mask:";
    commandByteArray = QByteArray(command.toUtf8());
    commandByteArray.append( (mask.toIPv4Address() & 0x000000FF) );
    commandByteArray.append( (mask.toIPv4Address() & 0x0000FF00) >> 8 );
    commandByteArray.append( (mask.toIPv4Address() & 0x00FF0000) >> 16);
    commandByteArray.append( (mask.toIPv4Address() & 0xFF000000) >> 24);
    udpSocket->writeDatagram(commandByteArray, deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();


    if( _receivedAck == false ){
        loggerMacroDebug("Error while setting password")
        goto exit;
    }

    // Set Gateway
    // ----------------------
    _receivedCR = _receivedAck = false;
    loggerMacroDebug("Sending 'set_static_gw:" + mask.toString() + "' to address '" + deviceAddress.toString() + "' ...")
    command = "set_static_gw:";
    commandByteArray = QByteArray(command.toUtf8());
    commandByteArray.append( (gateway.toIPv4Address() & 0x000000FF) );
    commandByteArray.append( (gateway.toIPv4Address() & 0x0000FF00) >> 8 );
    commandByteArray.append( (gateway.toIPv4Address() & 0x00FF0000) >> 16);
    commandByteArray.append( (gateway.toIPv4Address() & 0xFF000000) >> 24);
    udpSocket->writeDatagram(commandByteArray, deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();


    if( _receivedAck == false ){
        loggerMacroDebug("Error while setting password")
        goto exit;
    }
/**/
exit:
    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();


    return _receivedAck;
}

bool DeviceConfiguration::getMACAddress(QHostAddress deviceAddress, QString& macAddress){
    loggerMacroDebug("Searching for devices")


    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    loggerMacroDebug("Done! ")

    // UDP socket
    // ----------------------
    _receivedCR = _receivedAck = false;
    _receivedData = "";
    loggerMacroDebug("Sending 'get_mac:' to address '" + deviceAddress.toString() + "' ...")
    udpSocket->writeDatagram(QByteArray("get_mac:"), deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

    macAddress = _receivedData.mid(sizeof("ACK:")-1, sizeof("XX:XX:XX:XX:XX:XX")-1);
    //loggerMacroDebug("Captured MAC Address <" + serialNumber + ">")

    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();


    return _receivedAck;
}

bool DeviceConfiguration::setMACAddress(QHostAddress deviceAddress, QString macAddress){
    loggerMacroDebug("Searching for devices")

    bool result;
    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    loggerMacroDebug("Done! ")

    // UDP socket
    // ----------------------
    _receivedCR = _receivedAck = false;
    _receivedData = "";
    loggerMacroDebug("Sending 'set_mac:" + macAddress + "' to address '" + deviceAddress.toString() + "' ...")
    QString command = "set_mac:";
    QByteArray commandByteArray = QByteArray(command.toUtf8());
    commandByteArray.append( macAddress.mid(0,2).toUInt(&result,16) );
    commandByteArray.append( macAddress.mid(3,2).toUInt(&result,16) );
    commandByteArray.append( macAddress.mid(6,2).toUInt(&result,16) );
    commandByteArray.append( macAddress.mid(9,2).toUInt(&result,16) );
    commandByteArray.append( macAddress.mid(12,2).toUInt(&result,16) );
    commandByteArray.append( macAddress.mid(15,2).toUInt(&result,16) );
    udpSocket->writeDatagram(commandByteArray, deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();

    // We return true since Wifi is OFF after this command
    return true;
}

bool DeviceConfiguration::getWIFIChannel(QHostAddress deviceAddress, QString& WIFIChannel){
    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    loggerMacroDebug("Done! ")

    // UDP socket
    // ----------------------
    _receivedCR = _receivedAck = false;
    _receivedData = "";
    loggerMacroDebug("Sending 'get_wifi_channel:' to address '" + deviceAddress.toString() + "' ...")
    udpSocket->writeDatagram(QByteArray("get_wifi_channel:"), deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

    WIFIChannel = _receivedData.mid(sizeof("ACK:")-1, 2);

    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();


    return _receivedAck;
}

bool DeviceConfiguration::setWIFIChannel(QHostAddress deviceAddress, QString WIFIChannel){
    loggerMacroDebug("Searching for devices")

    bool result;
    // Open UDP socket
    // ----------------------
    loggerMacroDebug("Opening UDP socket ... ")
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    udpSocket->bind(QHostAddress::LocalHost);

    loggerMacroDebug("Done! ")

    // UDP socket
    // ----------------------
    _receivedCR = _receivedAck = false;
    _receivedData = "";
    loggerMacroDebug("Sending 'set_wifi_channel:" + WIFIChannel + "' to address '" + deviceAddress.toString() + "' ...")
    QString command = "set_wifi_channel:" + WIFIChannel;
    QByteArray commandByteArray = QByteArray(command.toUtf8());
    udpSocket->writeDatagram(commandByteArray, deviceAddress, UDP_PORT);
    udpSocket->flush();
    loggerMacroDebug("Done! ")

    // Synchronize with timeout
    QEventLoop loop;
    connect(this, SIGNAL(receivedAckSignal()), &loop, SLOT(quit()));
    QTimer::singleShot(1000, &loop, SLOT(quit()) );
    loop.exec();

    // Close UDP Socket
    // -------------------------
    loggerMacroDebug("Closing UDP socket ...")
    udpSocket->close();
    loggerMacroDebug("Done! ")

    // Delete socket
    udpSocket->deleteLater();

    // We return true since Wifi is OFF after this command
    return true;
}

void DeviceConfiguration::handleReadyRead(){

    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        // Read the datagram
        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        _deviceAddress = sender;

        QString receivedStr = QString(datagram);
        if( receivedStr.contains("ACK") ) _receivedAck = true;
        if( receivedStr.contains("\n"))   _receivedCR = true;
        loggerMacroDebug("Received " + QString(datagram) + " from " + sender.toString())

        // Accumulate the received data
        _receivedData += receivedStr;
    }

    // Emit signal when we received and ACK
    if( _receivedAck && _receivedCR ) emit receivedAckSignal();
}
