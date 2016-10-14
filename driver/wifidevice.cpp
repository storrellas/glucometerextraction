#include "wifidevice.h"

WifiDevice::WifiDevice(QObject *parent) :
    QObject(parent)
{

    // Configure sockets
    _icognosSocket = new QTcpSocket( this );
/*
    connect( _icognosSocket, SIGNAL(readyRead()), SLOT(readyRead()) );
    connect( _icognosSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));

    connect( this, SIGNAL(writeToSocket(QByteArray)), SLOT(onWriteToSocket(QByteArray)) );

    // Create thread for firmware
    _socketStarstimThread = new QThread();
    connect(_socketStarstimThread, SIGNAL(finished()), _socketStarstimThread, SLOT(deleteLater()));
    _socketStarstimThread->start();
/**/

}

WifiDevice::~WifiDevice()
{

}

WifiDevice::errType WifiDevice::open (const char * host, int port){


    QString hostStr = QString::fromLocal8Bit((char*) host);
    loggerMacroDebug("IP from QString " + hostStr)

    // Connect to the specified host
    _icognosSocket->connectToHost(hostStr, port);

    loggerMacroDebug("Connected!")

    // Wait for the connection of the socket
    if( _icognosSocket->waitForConnected() ){
        loggerMacroDebug("Socket is connected! :)")
        return ERR_NO_ERROR;
    }else{
        loggerMacroDebug("Socket failure on connection :(")
        return ERR_DEVICE_NOT_CONNECTED;
    }

//    // Remove parent from _icognosSocket
//    _icognosSocket->setParent(0);
//    _icognosSocket->moveToThread(_socketStarstimThread);


}

WifiDevice::errType WifiDevice::close (){
    _icognosSocket->close();
    return ERR_NO_ERROR;
}


int WifiDevice::read (char * buffer, unsigned long numBytes){
/*
    int n_read;

    // If no bytes to process process signals&slots
    if(_readArray.size() == 0){
        // Process all signals&slots before entering here
        QApplication::processEvents(QEventLoop::AllEvents);
    }


    // Get the mutex
    mutex.lock();


    if( _readArray.size() == 0 ){
        mutex.unlock();
        //loggerMacroDebug("unlock")
        return _readArray.size();
    }



    if( _readArray.size() > numBytes ){
        memcpy(buffer, _readArray.constData(), numBytes);
        _readArray.remove(0, numBytes);
        n_read = numBytes;
    }else{
        memcpy(buffer, _readArray.constData(), _readArray.size());
        n_read = _readArray.size();
        _readArray.clear();

    }

    mutex.unlock();

//    loggerMacroDebug("unlock")
//    loggerMacroDebug("Current Thread->")
//    qDebug() << QThread::currentThreadId();

    return n_read;
/**/

    static int n_read_static = 0;
    //if( _icognosSocket->bytesAvailable() == 0 ) return 0;
/*
    timer = new QTimer();
    QEventLoop loop;
    connect(_icognosSocket, SIGNAL(readyRead()), &loop, SLOT(quit()));
    connect(timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer->start(100);
    loop.exec();
    timer->stop();
/**/

    bool available = _icognosSocket->waitForReadyRead(100);
    if ( !available ) return 0;


    int n_read = _icognosSocket->read(buffer, numBytes);
    n_read_static += n_read;
    //loggerMacroDebug("Total number of read bytes " + QString::number(n_read_static) + " bytes")


    return n_read;
    /**/
}

int WifiDevice::write (const char * buffer, unsigned long numBytes){

/*
    QByteArray array = QByteArray(buffer, numBytes);
    emit writeToSocket(array);

    // TODO: This is fake and we could make it better
    return numBytes;
/**/

    static int n_write_static = 0;
    // Write to socket
    qint64 n_write = _icognosSocket->write(buffer, numBytes);
    _icognosSocket->flush();
    // Wait until all bytes are written to host
    _icognosSocket->waitForBytesWritten(-1);
    n_write_static += n_write;
    //loggerMacroDebug("Total number of write bytes " + QString::number(n_write_static) + " bytes")

    return n_write;

}

void WifiDevice::onWriteToSocket(QByteArray array){
    //loggerMacroDebug("Write to socket")
    _icognosSocket->write(array);
    _icognosSocket->flush();
}

qint64 WifiDevice::pendingBytesOnReading ()
{
    return 0;
}


void WifiDevice::readyRead(){
    mutex.lock();

    _readArray.append( _icognosSocket->readAll() );
//    loggerMacroDebug("Read bytes " + QString::number(_readArray.size()))
//    loggerMacroDebug("Current Thread->")
//    qDebug() << QThread::currentThreadId();
    mutex.unlock();
}

void WifiDevice::error(QAbstractSocket::SocketError error){
    loggerMacroDebug("Error in socket " + QString::number(error))
}
