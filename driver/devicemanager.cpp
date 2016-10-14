#include "devicemanager.h"



DeviceManager::DeviceManager(QObject *parent) :
    QObject(parent)
{
    // Create objects
    _icognosCom = new StarstimCom(this);


    qRegisterMetaType<DeviceManagerTypes::StarstimRegisterFamily>("DeviceManagerTypes::StarstimRegisterFamily");
    qRegisterMetaType<DeviceManagerTypes::StarstimRequest>("DeviceManagerTypes::StarstimRequest");
    qRegisterMetaType<DeviceManagerTypes::DeviceType>("DeviceManagerTypes::DeviceType");
    qRegisterMetaType<ChannelData>("ChannelData");
    qRegisterMetaType<DeviceManagerTypes::DeviceStatus>("DeviceManagerTypes::DeviceStatus");


    // Signal notifiers
    connect(_icognosCom, SIGNAL(receivedFirmwareVersion(int)),                           this, SIGNAL(receivedFirmwareVersion(int)));
    connect(_icognosCom, SIGNAL(receivedProfile(DeviceManagerTypes::DeviceType,int,int,int,int,int)),
            this,         SIGNAL(receivedProfile(DeviceManagerTypes::DeviceType,int,int,int,int,int)));
    connect(_icognosCom, SIGNAL(receivedEEGData(ChannelData)),                           this, SIGNAL(receivedEEGData(ChannelData)));
    connect(_icognosCom, SIGNAL(receivedAccelData(ChannelData)),                         this, SIGNAL(receivedAccelData(ChannelData)));
    connect(_icognosCom, SIGNAL(receivedStimulationData(ChannelData)),                   this, SIGNAL(receivedStimulationData(ChannelData)));
    connect(_icognosCom, SIGNAL(receivedDeviceStatus(DeviceManagerTypes::DeviceStatus)), this, SIGNAL(receivedDeviceStatus(DeviceManagerTypes::DeviceStatus)));
    connect(_icognosCom, SIGNAL(receivedImpedanceData(ChannelData)),                     this, SIGNAL(receivedImpedanceData(ChannelData)));

}


DeviceManager::~DeviceManager(){
    // Terminate the polling thread
    _icognosCom->stopPollThread();
}

/////////////////////////////////////
// Open/Close operations
/////////////////////////////////////

bool DeviceManager::openDevice(const char * ipAddress, int port)
{
    // Open the device
    bool res = _icognosCom->openDevice(ipAddress, port);
    if( !res ){
        loggerMacroDebug("Error while opening")
        return res;
    }

    // Wait some time before placing the ProfileRequest
    Sleeper::msleep(100);

    // Get battery level
    loggerMacroDebug("Perform battery request")
    profileRequest();

#ifdef ENABLE_NULL_REQUEST
    if (_icognosCom->getDeviceType() == DeviceManagerTypes::STARSTIM) {
        connect(&_nullRequestTimer, SIGNAL(timeout()),
                    this, SLOT(nullRequest()) );
        _nullRequestTimer.start(5000);
    }
#endif

    connect(&_profileRequestTimer, SIGNAL(timeout()),
                this, SLOT(profileRequest()) );
    _profileRequestTimer.start(BATTERY_REQUEST_PERIOD);


    return true;
}


bool DeviceManager::closeDevice (){

#ifdef ENABLE_NULL_REQUEST
    if (_icognosCom->getDeviceType() == DeviceManagerTypes::STARSTIM) {
        disconnect(&_nullRequestTimer, SIGNAL(timeout()),
                    this, SLOT(nullRequest()) );
        _nullRequestTimer.stop();
    }
#endif

    disconnect(&_profileRequestTimer, SIGNAL(timeout()),
                this, SLOT(profileRequest()) );
    _profileRequestTimer.stop();

    return _icognosCom->closeDevice();
}


/////////////////////////////////////
// Initialise Registers
/////////////////////////////////////

void DeviceManager::initRegisters( bool partialInit ){

    QElapsedTimer initTimer;
    initTimer.start();

    QByteArray regArray;
    loggerMacroDebug("Launching initRegisters for " +  deviceType2String() + " with partialInit:" + QString(partialInit?"TRUE":"FALSE"))

    // Configure EEG Registers for ENOBIO
    if ( _icognosCom->getDeviceType() == DeviceManagerTypes::ENOBIO)
    {
        loggerMacroDebug("Initialising Registers for icognos")

        // Configure INITIAL
        regArray.clear();
        regArray.append( 0x86 );
        regArray.append( 0x10 );
        regArray.append( 0xCC );
        writeRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_REGS_CONFIG_1_ADDR, regArray);

        int retriesPerformed = checkRegWritten(regArray, DeviceManagerTypes::EEG_REGISTERS, EEG_REGS_CONFIG_1_ADDR);
        if( retriesPerformed < 5 ) loggerMacroDebug("EEG INITIAL registers : Read registers OK")
        else loggerMacroDebug("EEG INITIAL registers : Read registers FAIL")


    }

    // Configure EEG Registers for STARSTIM
    if( _icognosCom->getDeviceType() == DeviceManagerTypes::STARSTIM ){

        loggerMacroDebug("Initialising Registers for Starstim")

        // Configure LOFF and CHANNEL
        regArray.clear();
        regArray.append( (char) 0x00 );
        writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH_INFO_ADDR, regArray);
        // Check correctness
        int retriesPerformed = checkRegWritten(regArray, DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH_INFO_ADDR);
        if( retriesPerformed < 5 ) loggerMacroDebug("Stim Writing registers for Startim : Read registers OK")
        else loggerMacroDebug("Stim Writing registers for Startim : Read registers FAIL")


        regArray.clear();
        for( int i = 0; i < 25; i++) regArray.append( (char) 0x00 );
        regArray[0] = 0x86;
        regArray[2] = 0xCC;
        requestSync(DeviceManagerTypes::WRITE_REGISTER_REQUEST,
                    DeviceManagerTypes::EEG_REGISTERS, EEG_REGS_CONFIG_1_ADDR, regArray);

        // Check correctness
        retriesPerformed = checkRegWritten(regArray, DeviceManagerTypes::EEG_REGISTERS, EEG_REGS_CONFIG_1_ADDR);
        if( retriesPerformed < 5 ) loggerMacroDebug("EEG Writing registers for Startim : Read registers OK")
        else loggerMacroDebug("EEG Writing registers for Startim : Read registers FAIL")

    }


    // Configure EEG Registers - both Starstim/icognos
    if ( partialInit == false ){

        // Configure LOFF and CHANNEL
        regArray.clear();
        regArray.append( (char) 0x00 );
        regArray.append( (char) 0x00 );
        writeRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_REGS_LOFF_ADDR, regArray);

        regArray.clear();
        for( int i = 0; i < 13; i++ ) regArray.append( (char) 0x00 );
        writeRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_REGS_RLD_SENS_P_ADDR, regArray);

        // Calculate CH_INFO to be set to EEG_CH_INFO_ADDR
        int shift=1;
        int aux=0;

        for (int i=0;i<getNumOfChannels();i++)
            aux = aux | (shift <<i);
        aux = ~ aux;

        regArray.clear();
        regArray.append( (char) ((aux & 0x000000FF) >> 8*0) );
        regArray.append( (char) ((aux & 0x0000FF00) >> 8*1) );
        regArray.append( (char) ((aux & 0x00FF0000) >> 8*2) );
        regArray.append( (char) ((aux & 0xFF000000) >> 8*3) );
        writeRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_CH_INFO_ADDR, regArray);

        int retriesPerformed = checkRegWritten(regArray, DeviceManagerTypes::EEG_REGISTERS, EEG_CH_INFO_ADDR);
        if( retriesPerformed < 5 ) loggerMacroDebug("EEG CH_INFO registers : Read registers OK")
        else loggerMacroDebug("EEG CH_INFO registers : Read registers FAIL")
    }

    loggerMacroDebug("DONE! initRegisters took " + QString::number(initTimer.elapsed()) + " ms")

}

int DeviceManager::checkRegWritten(QByteArray regArray, DeviceManagerTypes::StarstimRegisterFamily family, int address, int nRetries){

    int i = 0;
    QString aux;

    QByteArray regArrayRead;
    regArrayRead.clear();
    requestSync(DeviceManagerTypes::READ_REGISTER_REQUEST, family, address, regArrayRead, regArray.size());

    QString regReadStr = QString("regRead: [") +  QString::number(address) + QString("] = ");
    QString regArrayStr = QString("regArray: [") +  QString::number(address) + QString("] = ");
    for(int i = 0; i < regArray.size(); i ++){
        int value = (regArrayRead[i] & 0xFF);
        regReadStr += aux.sprintf("0x%02X ", value);
        value = (regArray[i] & 0xFF);
        regArrayStr += aux.sprintf("0x%02X ", value);
    }
    //qDebug() << regReadStr;
    qDebug() << regArrayStr;


    unsigned char * regRead = new unsigned char[regArray.size()];

    // Compare whether retry is necessary
    if(memcmp(regArray.data(), regArrayRead.data(), regArray.size()) != 0 ){
        loggerMacroDebug("Read registers FAIL")
        i++;
        while ( i< nRetries ){ // We make up to 5 retries of writing registers

            // Write register again
            writeRegister( family, address, regArray);

            // Read register
            regArrayRead.clear();
            readRegister( family, address, regArrayRead, regArray.size());

            // Print regRead
            int j = 0;
            regReadStr = QString("regRead: [") +  QString::number(address) + QString("] = ");
            for(j = 0; j < regArray.size(); j ++){
                regReadStr += aux.sprintf("0x%02X ", regRead[j]);
            }
            qDebug() << "Iteration: "<< i << regReadStr;

            if( memcmp(regArray.data(), regArrayRead.data(), regArray.size()) == 0 ) break;
            i++;
        }

//        stopStimulation();
//        emit abortCurrentStimulationProtocol();
//        emit newMessage(5, "Error writing registers.");
    }
    delete regRead;
    return i;

}


/////////////////////////////////////
// General Requets
/////////////////////////////////////

bool DeviceManager::writeRegister (DeviceManagerTypes::StarstimRegisterFamily family, int address, QByteArray &regArray){
    bool res = this->requestSync(DeviceManagerTypes::WRITE_REGISTER_REQUEST, family, address, regArray);
    return res;
}

bool DeviceManager::readRegister (DeviceManagerTypes::StarstimRegisterFamily family, int address,
                                  QByteArray &regArray, int length)
{

    bool res = this->requestSync(DeviceManagerTypes::READ_REGISTER_REQUEST, family, address, regArray, length);
    return res;
}


bool DeviceManager::firmwareVersionRequest()
{
    bool res = this->requestSync(DeviceManagerTypes::FIRMWARE_REQUEST);
    return res ;
}

bool DeviceManager::profileRequest(){
    bool res = this->requestSync(DeviceManagerTypes::PROFILE_REQUEST);
    return res;
}

bool DeviceManager::nullRequest(){
    bool res = this->requestSync(DeviceManagerTypes::NULL_REQUEST);
    return res;
}

bool DeviceManager::requestSync(DeviceManagerTypes::StarstimRequest request_type,
                                   DeviceManagerTypes::StarstimRegisterFamily family, int address,  QByteArray& frame, int length){
    QMutexLocker locker(&_mutex);

    // Create a QByteArray with the desired size
    if( request_type == DeviceManagerTypes::READ_REGISTER_REQUEST ){
        for( int i = 0; i < length; i++) frame.append('\0');
    }

    loggerMacroDebug("Perform a request ")
    bool result = _icognosCom->request(request_type, family, address, frame);
    if( result == false ){
        loggerMacroDebug("Request was not completed")
        return false;
    }

    // Capture content of read register
    if( request_type == DeviceManagerTypes::READ_REGISTER_REQUEST ){
        frame.clear();
        for(int i = 0; i < length; i++){
            frame.append( (0xFF & _icognosCom->_regContent[i]) );
        }
    }

    return true;
}

bool DeviceManager::requestSync(DeviceManagerTypes::StarstimRequest request_type){
    QMutexLocker locker(&_mutex);


    loggerMacroDebug("Perform a request")
    bool result = _icognosCom->request(request_type);
    if( result == false ){
        loggerMacroDebug("Request was not completed")
        return false;
    }

    return true;
}

void DeviceManager::setSampleRate(DeviceManagerTypes::SampleRate sampleRate){

    QByteArray regArray;
    regArray.append(sampleRate);
    writeRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_STREAMING_RATE, regArray);

    // Read operations
    regArray.clear();
    readRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_STREAMING_RATE, regArray, 1);

    if( regArray.size() == 1 && regArray[0] == sampleRate ){
        loggerMacroDebug("SampleRate OK!")
    }else{
        loggerMacroDebug("SampleRate FAILED")
    }

    // Indicate to Starstimcom
    _icognosCom->setSampleRate(sampleRate);
}

/////////////////////////////////////
// Streaming Requests
/////////////////////////////////////

bool DeviceManager::startStreaming (){

    bool res = this->requestSync(DeviceManagerTypes::START_STREAMING_REQUEST);
    return res ;
}

bool DeviceManager::stopStreaming (bool doStopAccelerometer){
    bool res = this->requestSync(DeviceManagerTypes::STOP_STREAMING_REQUEST);
    return res ;
}

bool DeviceManager::startAccelerometer(){

    unsigned char REG_ACC=0x01;
    bool result;
    loggerMacroDebug("Starting accelerometer")
    if( _icognosCom->getDeviceStatus().ACCEL == false )
    {
        QByteArray regAccArray;
        regAccArray.append( (char) REG_ACC );
        result = writeRegister(DeviceManagerTypes::ACCEL_REGISTERS, 0x00, regAccArray);

        loggerMacroDebug("Result: " + QString(result?"TRUE":"FALSE"))
        _icognosCom->setStreamingAccel(true);

    }
    else
        qDebug()<<"startAccelerometer() not done. It was already streamming accelerometer";

    return result;

}

bool DeviceManager::stopAccelerometer(){

    loggerMacroDebug("Stopping accelerometer")

    unsigned char REG_ACC=0x00;
    QByteArray regAccArray;
    regAccArray.append( (char) REG_ACC );
    bool result = writeRegister(DeviceManagerTypes::ACCEL_REGISTERS, 0x00, regAccArray);

    loggerMacroDebug("Result: " + QString(result?"TRUE":"FALSE"))
    _icognosCom->setStreamingAccel(false);
    return result;

}

void DeviceManager::onReceivedDeviceStatus(int deviceStatus){
    QString aux;
    //loggerMacroDebug("Current status " + aux.sprintf("0x%02X", deviceStatus))
}

/////////////////////////////////////
// Stimulation Requests
/////////////////////////////////////

bool DeviceManager::startStimulation (){
    loggerMacroDebug("Starting stimulation!")
    bool res = this->requestSync(DeviceManagerTypes::START_STIMULATION_REQUEST);
    return res;
}


bool DeviceManager::stopStimulation (){
    bool res = this->requestSync(DeviceManagerTypes::STOP_STIMULATION_REQUEST);
    return res;
}


bool DeviceManager::startImpedanceMeasurement (){
    loggerMacroDebug("start impedance measurement")
    bool res = this->requestSync(DeviceManagerTypes::START_IMPEDANCE_REQUEST);
    return res;
}


bool DeviceManager::stopImpedanceMeasurement (){
    loggerMacroDebug("stop impedance measurement")
    bool res = this->requestSync(DeviceManagerTypes::STOP_IMPEDANCE_REQUEST);
    return res;
}



