#include "rfcommdevice.h"

#include "bluetooth/BluetoothAPI.h"

int RFCOMMDevice::_numberOfInstances = 0;


RFCOMMDevice::RFCOMMDevice() :
        _lastError(ERR_NO_ERROR),
        _handle(0),
        _rfcommTimeout(100000)
{
    if (_numberOfInstances++ == 0)
    {
        int ret = 0;
        ret = iniBTSockets();
    }

}

RFCOMMDevice::~RFCOMMDevice()
{
    if(--_numberOfInstances == 0)
    {
        closeBTSockets();
    }
}

#ifdef Q_OS_LINUX
bool RFCOMMDevice::checkcommandExists(QString command){
    QProcess process;
    process.start( command );
    process.waitForFinished(-1);

    if( process.errorString() == "No such file or directory"){
        loggerMacroDebug("Command Failed -> " + command)
        return false;
    }else return true;
}
#endif

//QFile debugBluetoothFile("debugBluetoothFile.txt");

void RFCOMMDevice::logDebugBluetooth (QString string)
{
//    writeLogDebugBluetooth(string.toAscii().data());
//    qDebug()<<string;
}

RFCOMMDevice::errType RFCOMMDevice::open (const unsigned char * macAddress)
{
#ifdef Q_OS_LINUX
    bool cond1 = this->checkcommandExists("bluez-test-device");
    bool cond2 = this->checkcommandExists("hcitool");
    bool cond3 = this->checkcommandExists("bluetooth-agent");
    if( cond1 || cond2 || cond3){
        loggerMacroDebug("WARNING: Some commands were not found in the system. Scan, pair and unpair operation may not work")
    }
#endif

#ifdef __DEBUGBLUETOOTH__
  logDebugBluetooth(QString("[RFD] Calling open device"));
#endif

  int returnValue;

  returnValue=openRFCOMM(macAddress, &_handle);

    //if (openRFCOMM(macAddress, &_handle) <= 0)
    if (returnValue!= 1)
    {
        if (returnValue==10022)
            _lastError = ERR_DEVICE_NOT_CONNECTED_WSAEINVAL;
        else
            _lastError = ERR_DEVICE_NOT_CONNECTED;

        qDebug()<<"ERROR IN RFCOMMDevice::open"<<returnValue;
        return _lastError;
    }
    return ERR_NO_ERROR;
}

RFCOMMDevice::errType RFCOMMDevice::close ()
{
  logDebugBluetooth(QString("[RFD] Calling close device"));
    if (closeRFCOMM(_handle) <= 0)
    {
        _lastError = ERR_CLOSING_DEVICE;
        return _lastError;
    }
    return ERR_NO_ERROR;
}

void RFCOMMDevice::setReadTimeout (int timeout)
{
    _rfcommTimeout = timeout;
}

int RFCOMMDevice::read (unsigned char * buffer, unsigned long numBytes)
{
    //qDebug()<<"RFCOMMDevice::read";
    int nBytes = -1;
    nBytes = readRFCOMM(_handle, (char *)buffer, numBytes, _rfcommTimeout);
    //qDebug()<<"RFCOMMDevice::read"<<nBytes;
    if (nBytes < 0)
    {
        _lastError = ERR_READING_DEVICE;
    }
    return nBytes;
}

int RFCOMMDevice::write (unsigned char * buffer, unsigned long numBytes)
{

    int ret = -1;
    ret = writeRFCOMM(_handle, (char *) buffer, numBytes);
    if (ret < 0)
    {
        _lastError = ERR_WRITING_DEVICE;
    }
    return ret;
}

RFCOMMDevice::errType RFCOMMDevice::getLastError ()
{
    return _lastError;
}

qint64 RFCOMMDevice::pendingBytesOnReading ()
{
    return 0;
}
