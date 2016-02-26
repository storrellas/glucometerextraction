#include <QDebug>
#include <QRegExp>
#include "comdevice.h"
#include "qextserialenumerator.h"

COMDevice::COMDevice()
{
    _port = new QextSerialPort();
    QObject::connect(_port, SIGNAL(readyRead()),
                     this, SLOT(onDataAvailable()));
    QObject::connect(_port, SIGNAL(dsrChanged(bool)),
                     this, SLOT(onDSRChanged(bool)));
    QObject::connect(_port, SIGNAL(bytesWritten(qint64)),
                     this, SLOT(onBytesWritten(qint64)));
    _lastError = ERR_NO_ERROR;
    _pendingBytesInWriting = 0;
}

COMDevice::~COMDevice()
{
    delete _port;
}

qint64 COMDevice::pendingBytesOnWriting ()
{
    return _pendingBytesInWriting;
}

unsigned int COMDevice::getNumDevices ()
{
    unsigned int FTDIDevices = 0;
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    int serialDevices = ports.size();
    for (int i = 0; i < serialDevices; i++)
    {
        qDebug() << ports.at(i).enumName << "|"
                 << ports.at(i).friendName << "|"
                 << ports.at(i).physName << "|"
                 << ports.at(i).portName << "|"
                 << ports.at(i).productID << "|"
                 << ports.at(i).vendorID ;

        //if (ports.at(i).enumName.compare("FTDIBUS") == 0)
#ifdef Q_OS_WIN
        //if (ports.at(i).enumName.compare("USB") == 0)
        if (_isValidPort(ports.at(i).physName, "\\Device\\BthModem"))
#else
        //if (ports.at(i).portName.compare("ttyACM0") == 0)
        if (_isValidPort(ports.at(i).portName, "ttyACM"))
#endif
        {
            FTDIDevices++;
        }
    }
    return FTDIDevices;
}

HWErrType COMDevice::open ()
{
    if(_port->isOpen())
    {
        return ERR_NO_ERROR;
    }
    return open(0);

}

bool COMDevice::_isValidPort (QString portName, QString validName)
{
    int i = 0;
    QChar c = portName.data()[portName.size() - 1];
    while (c >= '0' && c <= '9' && i < portName.size())
    {
        i++;
        c = portName.data()[portName.size() - 1 - i];
    }
    QRegExp re("[0-9]");
    portName = portName.section(re, 0, 0);
    return (portName.compare(validName) == 0);
}

HWErrType COMDevice::open (QString COMName)
{
    if(_port->isOpen())
    {
        _port->close();
    }
    _port->setPortName(COMName);
    configure();
    if (_port->open(QIODevice::ReadWrite))
    {
        _port->flush();
        return ERR_NO_ERROR;
    }
    _lastError = ERR_DEVICE_NOT_CONNECTED;
    return _lastError;
}

HWErrType COMDevice::open (unsigned int index)
{
    if(_port->isOpen())
    {
        _port->close();
    }
    // Searching the device
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    unsigned int indexFTDI = 0;
    for (int i = 0; i < ports.size(); i++)
    {
        // only FTDI based Serial ports
        //if (ports.at(i).enumName.compare("FTDIBUS") == 0)
#ifdef Q_OS_WIN
        //if (ports.at(i).enumName.compare("USB") == 0)
        if (_isValidPort(ports.at(i).physName, "\\Device\\BthModem"))
#else
        //if (ports.at(i).portName.compare("ttyACM0") == 0)
        if (_isValidPort(ports.at(i).portName, "ttyACM"))
#endif
        {
            if (indexFTDI++ == index)
            {
#ifdef Q_OS_WIN
                _port->setPortName(ports.at(i).portName);
#else
                _port->setPortName(ports.at(i).physName);
#endif
                configure();
                if (_port->open(QIODevice::ReadWrite))
                {
                   return ERR_NO_ERROR;
                }
            }
        }
    }
    _lastError = ERR_DEVICE_NOT_CONNECTED;
    return _lastError;
}

void COMDevice::onDataAvailable ()
{
    emit dataAvailable(_port->bytesAvailable());
}

void COMDevice::onDSRChanged (bool status)
{
    qDebug() << "onDSRChanged =" << status;
    emit statusConnectionChanged(status);
}

void COMDevice::onBytesWritten (qint64 bytesWritten)
{
    _pendingBytesInWriting -= bytesWritten;
}

qint64 COMDevice::pendingBytesOnReading ()
{
    return _port->bytesAvailable();
}

HWErrType COMDevice::close ()
{
    _port->close();
    return ERR_NO_ERROR;
}

HWErrType COMDevice::reset ()
{

    if (!_port->isOpen())
    {
        _lastError = ERR_DEVICE_NOT_OPENED;
    }
    else if (!_port->reset())
    {
        _lastError = ERR_RESETING_DEVICE;
    }
    else
    {
        return ERR_NO_ERROR;
    }
    return _lastError;
}

bool COMDevice::configure ()
{
    //port->setBaudRate(BAUD115200);
    _port->setBaudRate(BAUD921600);
    _port->setDataBits(DATA_8);
    _port->setFlowControl(FLOW_OFF);
    _port->setParity(PAR_NONE);
    _port->setStopBits(STOP_1);
    _port->setTimeout(100);
    //_port->setQueryMode(QextSerialPort::EventDriven);
    _port->setQueryMode(QextSerialPort::Polling);
    return true;
}

void COMDevice::setReadTimeout (int timeout)
{
    //_port->setQueryMode(QextSerialPort::Polling);
    _port->setTimeout(timeout);
    //_port->setQueryMode(QextSerialPort::EventDriven);
}

int COMDevice::read (unsigned char * buffer, unsigned long numBytes)
{
    int nBytes = -1;
    if (_port->isOpen())
    {
        nBytes = _port->read((char*)buffer, numBytes);
        if (nBytes < 0)
        {
            _lastError = ERR_READING_DEVICE;
        }
    }
    else
    {
        _lastError = ERR_DEVICE_NOT_OPENED;
    }
    if ((nBytes == 0) && (_port->lineStatus() == 0)) // device disconnected?
    {
        nBytes = -1;
    }
    return nBytes;
}

int COMDevice::write (unsigned char * buffer, unsigned long numBytes)
{
    int nBytes = -1;
    if (_port->isOpen())
    {
        nBytes = _port->write((char*)buffer, numBytes);
        if (nBytes < 0)
        {
            _lastError = ERR_WRITING_DEVICE;
        }
        else if (nBytes == 0)
        {
            _pendingBytesInWriting += numBytes;
            nBytes = numBytes;
        }
    }
    else
    {
        _lastError = ERR_DEVICE_NOT_OPENED;
    }
    return nBytes;
}

HWErrType COMDevice::getLastError ()
{
    return _lastError;
}
