#include "starstimcom.h"


StarstimCom::StarstimCom(QObject* parent) :
    _samplesPerBeacon(1)
{
    _deviceType   = DeviceManagerTypes::ENOBIO;
    _numOfChannels = 8;
    _deviceStatus = DeviceManagerTypes::DEVICESTATUS_UNKNOWN; // unknown value

    _sampleRate = DeviceManagerTypes::_500_SPS_;

    // Create WifiDevice instance
    _wifiDevice = new WifiDevice();
    //requestBlock = true;

}

//////////////////////////////////////////
// Open/Close Operations
//////////////////////////////////////////

bool StarstimCom::openDevice(const char * ipAddress, int port)
{

    // Check pre-conditions
    if( _deviceStatusStruct.CONNECTED == true ){
        loggerMacroDebug("Device already connected")
        return false;
    }
    // Open WifiDevice
    loggerMacroDebug("Connecting to " + QString(ipAddress) + ":" + QString::number(port))
    WifiDevice::errType errType = _wifiDevice->open(ipAddress, port);
    if( errType == WifiDevice::ERR_NO_ERROR){
        // Do nothing
    }else{
        return false;
    }


    // Reset the protocol
    _protocol.reset();
    _protocol.setMultipleSample( false );
    _protocol.setEEGCompressionType(StarStimProtocol::EEG_NO_COMPRESSION);

    // Look for starstim
    bool isOpen = _lookForStarStim();
    _deviceStatusStruct.set( _deviceStatus, isOpen, false );
    if ( isOpen == false ){
        loggerMacroDebug("ERROR _lookForStarStim() failed");
        _wifiDevice->close();
        return isOpen;
    }

    loggerMacroDebug("Starting poll thread")
    startPollThread();


    return true;
}


bool StarstimCom::_lookForStarStim ()
{

    loggerMacroDebug("Searching for device");
    bool isDevicePresent = false;

    QByteArray txBuffer = StarStimProtocol::buildStartBeaconRequest();
    if (_wifiDevice->write((char*)txBuffer.data(), txBuffer.size()) < 0){
        return false;
    }

    QElapsedTimer timer;
    timer.start();
    while ((!isDevicePresent) && (timer.elapsed() < 3000))
    {
        int retValue = _processData();
        if( retValue > 0){
            isDevicePresent = true;
            break;
        }else if (retValue < 0){
            loggerMacroDebug("Returned from <0")
            break;
        }
    }

    return (isDevicePresent);
}


bool StarstimCom::closeDevice (){
    loggerMacroDebug("Closing device ...")

    loggerMacroDebug("Stop poll thread")
    stopPollThread();

    // Closing device
    QByteArray txBuffer = StarStimProtocol::buildStopBeaconRequest();
    _wifiDevice->write((char*)txBuffer.data(), txBuffer.size());

    loggerMacroDebug("Close socket")
    _wifiDevice->close();
    _deviceStatusStruct.set( _deviceStatus, false, false);
    emit receivedDeviceStatus(_deviceStatusStruct);

    loggerMacroDebug("DONE!")

}

//////////////////////////////////////////
// Polling operations
//////////////////////////////////////////


void StarstimCom::startPollThread ()
{
    // Store the thread that created the Poll Thread
    formerThread = QThread::currentThread();

    loggerMacroDebug("Starting here")
    qDebug() << QThread::currentThreadId();
    pollThread = new QThread();
    moveToThread(pollThread);
    _wifiDevice->moveToThread(pollThread);
    connect(pollThread, SIGNAL(started()), this, SLOT(runPoll()));

    //connect(this, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(this, SIGNAL(finished()), pollThread, SLOT(quit()));
    //connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    //connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    pollThread->start();
}

void StarstimCom::stopPollThread (){

    // Check if thread is currently running
    if( pollThread->isRunning() == false ) return;

    loggerMacroDebug("Stopping poll thread")
    _isPollThreadRunning = false;

    /*// Wait for finished signal
    QEventLoop loop;
    connect(this,SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    */

    // Wait for finished signal
//#ifdef TEST_DEVICEMANAGER
    //loggerMacroDebug("*** TESTING MODE ***");
    stopSync.lock();
    //bool timeout = waitStopSync.wait(&stopSync, 10000);
        waitStopSync.wait(&stopSync, 10000);
    stopSync.unlock();
/*#else
    // Wait for finished signal
    QEventLoop loop;
    connect(this,SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
#endif*/

    loggerMacroDebug("Thread was stopped")
}


void StarstimCom::runPoll(){

    loggerMacroDebug("Running poll thread ...")

    // Run the poll function
    _poll();

    // Return _wifiDevice to its original thread
    _wifiDevice->moveToThread(formerThread);
    moveToThread(formerThread);

    // Signal that poll was finished
//#ifdef TEST_DEVICEMANAGER
    waitStopSync.wakeAll();
/*#else
    emit finished();
#endif*/

    /*// Signal that poll was finished
    emit finished();
    */

    loggerMacroDebug("Stopped poll thread");
}

int StarstimCom::_poll(){

    // QElapsedTimer to monitor the time from which instrument was lost
    QElapsedTimer monitorTimer;
    // QElapsedTimer to count number of ms from the last battery request
    QElapsedTimer batteryTimer;
    // QElapsedTimer to count number of ms from the last null request
    QElapsedTimer nullRequestTimer;

    monitorTimer.start();
    batteryTimer.start();
    nullRequestTimer.start();

    bool isLostSent = false;
    qint64 isLostLastLog;
    qint64 lastNullRequest = nullRequestTimer.elapsed();

    loggerMacroDebug("Starting poll")
    loggerMacroDebug("Current thread ->");
    qDebug() << QThread::currentThreadId();


    // Initialise to no operation pending
    sharedTxBuffer.clear();

    _beaconCounterStayAlive = 0;

    // Polling loop
    _isPollThreadRunning = true; // Indicates that poll thread is running
    while( _isPollThreadRunning ){


        // Prevents from reading the socket when writing is necessary
        QApplication::processEvents();
        sync.lock();
        if( !sharedTxBuffer.isEmpty() ){
            //loggerMacroDebug("Pending operation")
//            QEventLoop loop;
//            QTimer::singleShot(100, &loop, SLOT(quit()));
//            loop.exec();

            // Write operation into the device
            if( sharedTxBuffer.size() > 0 ){
                //loggerMacroDebug("Writing command")
                _wifiDevice->write((char*)sharedTxBuffer.data(), sharedTxBuffer.size());
                sharedTxBuffer.clear();
            }


            sync.unlock();
            continue;
        }
        sync.unlock();


//        // Battery measurement (every 60 seconds)
//        if ( batteryTimer.elapsed() > BATTERY_REQUEST_PERIOD ){
//            requestBlock = false;
//            request(DeviceManagerTypes::PROFILE_REQUEST);
//            requestBlock = true;
//            batteryTimer.restart();
//        }else{

//        // Send NULL Request every 5000 ms for Starstim
//        if (_deviceType == DeviceManagerTypes::STARSTIM ){
//#ifdef ENABLE_NULL_REQUEST
//            if ((nullRequestTimer.elapsed())> 5000) {
//                loggerMacroDebug("Performing NULL Request")

//                requestBlock = false;
//                request(DeviceManagerTypes::NULL_REQUEST);
//                requestBlock = true;

//                // Restart the NULL Timer
//                nullRequestTimer.restart();
//            }
//#endif
//        }
//        }

        // Evaluate wheter data was received
        int processDataResult = _processData();
        if (processDataResult > 0){
            // Instrument is still there!
            if (monitorTimer.elapsed()>=4000){
                loggerMacroDebug("Instrument was recovered from a disconnection")
                _deviceStatusStruct.set( _deviceStatus, true, true );
            }
            // Restart operations for signalling operations
            monitorTimer.restart();
            isLostSent = false;
            isLostLastLog = monitorTimer.elapsed();

        }
        else
        {
            // Signalling operations
            if( monitorTimer.elapsed() > 2000){
                if ((monitorTimer.elapsed() - isLostLastLog) > 1000) {
                    loggerMacroDebug("Device was lost " + QString::number(monitorTimer.elapsed()/1000) + " secs ago")
                    isLostLastLog = monitorTimer.elapsed();
                }
                if (monitorTimer.elapsed() > 4000 && isLostSent == false){
                    isLostSent = true;
                    _deviceStatusStruct.set( _deviceStatus, true, false );
                    emit receivedDeviceStatus( _deviceStatusStruct );
                }
            }

            if ((processDataResult < 0) || (monitorTimer.elapsed() > 15000)){
                loggerMacroDebug("Closed device after " + QString::number(monitorTimer.elapsed()) +" ms without response")
                _wifiDevice->close();
                _deviceStatus = DeviceManagerTypes::DEVICESTATUS_UNKNOWN; // unknown value
                _deviceStatusStruct.set( _deviceStatus, false, false );
                emit receivedDeviceStatus( _deviceStatusStruct );
                loggerMacroDebug("Device lost. Finishing _poll thread. Where is your device?")
                return -1;
            }

        }

    }

    loggerMacroDebug("Stopped poll thread")
    return 0;
}


int StarstimCom::_processData (int nBytes)
{
    int ret = -1;
    int nBytesToRead = nBytes;
    int nBytesRead;

    static qint64 n_frame = 0;
    const qint64 n_frame_indicator = 200;

    //qDebug()<<"_processData()"<<QThread::currentThreadId() << nBytesToRead;

    while (nBytesToRead > 0)
    {
        nBytesToRead = (nBytesToRead > MAX_LENGTH_RX_BUFFER) ?
                            MAX_LENGTH_RX_BUFFER : nBytesToRead;

        // Read From wifi device
        nBytesRead = _wifiDevice->read(_rxBuffer, MAX_LENGTH_RX_BUFFER);
        if (nBytesRead < 0){
            // There was an error
            break;
        }
        if( nBytesRead == 0 ){
            //break;
        }


//        // ---------------------------------

        // NOTE: Keep it for debug purposes
        QString aux = "Frame contents: ";
        QString str;
        for (int i = 0; i < nBytesRead; i++){

            if( nBytesRead > 2 ){
                // Detect SOF
                if( _rxBuffer[i] == 83 && _rxBuffer[i+1] == 79 && _rxBuffer[i+2] == 70 )
                    aux += "| ";
            }

            //aux+= QString::number(_rxBuffer[i]) + " ";
            unsigned char   value = (_rxBuffer[i] & 0xFF);
            aux+= str.sprintf("0x%02X ", value );

        }
/**/
//        if( _waitingFirstEEGSample == false)
//          if( nBytesRead != 0 )
//            qDebug()<<aux;
//        // ---------------------------------

        // Indicate that operation was ok!
        ret = 0;



        // Iterate over the number of bytes read
        for (int i = 0; i < nBytesRead; i++)
        {
            // Do not process;
            //continue;

            // MAIN IF (parseByte) - Check if a full packet was received
            if (_protocol.parseByte((unsigned char) _rxBuffer[i]))
            {
                //loggerMacroDebug("Found packet!")
                _beaconCounterStayAlive++;
                StarstimData * data = _protocol.getStarStimData();
                if (data->isCommandToggled()){
                    QtMessageHandler message = qInstallMessageHandler(0);
                    loggerMacroDebug("Got acknowledge")
                    qInstallMessageHandler(message);
                    _beaconCounterStayAlive = 0;

                    // Operation is done
                    emit operationDone();
                    this->reqSyncCond.wakeAll();
                } // END: data->isCommandToggled()

                if (data->isRegConfigPresent())
                {
                    //loggerMacroDebug("Read registers")
                    int numReg = data->eegNumRegs();
                    int starAdd = data->eegStartAddress();
                    unsigned char * reg = data->confReg();
                    memcpy(_regContent, &(reg[starAdd]), numReg);
                } // END: data->isEEGConfigPresent()


                if (data->isFirmwareVersionPresent())
                {

#ifdef USE_APPLICATION_TIME
                    _synch_t3 = ApplicationTime::currentTime();
                    _synch_t3 *= 1000;
                    _synch_t1 = data->synchT1();
                    _synch_t2 = data->synchT2();
#endif

                    loggerMacroDebug("Got firmware version ! " + QString::number(_protocol.getFirmwareVersion()))
                    emit receivedFirmwareVersion(data->firmwareVersion());
                } // END: data->isFirmwareVersionPresent()

                if ( data->isProfilePresent() ){
                    loggerMacroDebug("Profile present")
                    int batteryLevel = _calculateBatteryLevel(data->battery());
                    //loggerMacroDebug("Received battery level " + QString::number(batteryLevel) + "% (" + QString::number(data->battery()) +")" )

                    _numOfChannels = data->numOfChannels();
                    _deviceType = data->deviceType() ;
                    emit receivedProfile(data->deviceType(), data->numOfChannels(),
                                        batteryLevel, data->firmwareVersion(), data->synchT1(), data->synchT2());


                } // END: data->isProfilePresent()


                // data->isEEGDataPresent()
                if (data->isEEGDataPresent()){
                    //loggerMacroDebug("EEG is present")

                    int diff = _timestampAnalysis(data);
                    if( diff == -1 ){
                        loggerMacroDebug("Error in DIFF")
                    }else{
                        // Processes the EEG data given the fact that between packets has been diff
                        this->_eegProcessing(data, diff);
                        if( diff != _sampleRate ) loggerMacroDebug("Some packets were lost diff:" + QString::number(diff))
                    }

                } // END: data->isEEGDataPresent()


                if (_deviceStatus != data->deviceStatus())
                {


                    _deviceStatus = data->deviceStatus();
                    _deviceStatusStruct.set( _deviceStatus, true, true );
//                    QtMessageHandler handler = qInstallMessageHandler(0);
//                    QString aux;
//                    loggerMacroDebug("Status changed. Status:" + aux.sprintf("0x%02X", _deviceStatus))
//                    loggerMacroDebug("New device status " + _deviceStatusStruct.toString())
//                    qInstallMessageHandler(handler);

                    emit receivedDeviceStatus( _deviceStatusStruct );


                }

                if (data->isAccelDataPresent())
                {
//                    int * acclData= data->accelerometer();

//                    ChannelData aux;
//                    aux.setTimestamp(_currentTimestamp);
//                    aux.setChannelInfo(14);
//                    aux.setData(1,acclData[0]);
//                    aux.setData(2,acclData[1]);
//                    aux.setData(3,acclData[2]);

                    // Configure _last accel data
                    _lastAccelData = data->accelerometer();
                    _lastAccelData.setChannelInfo(7);
                    _lastAccelData.setTimestamp(_currentEEGTimestamp);

                    //qDebug() << _lastAccelerometerData.timestamp();
                    // ST: I think this is not necessary
                    _deviceStatusStruct.ACCEL = true;
                    emit receivedAccelData(_lastAccelData);
                } // END: data->isAccelerometerPresent()



                if (data->isStimDataPresent())
                {
                    //loggerMacroDebug("New stim data")
                    if (!_firstStimSampleReceived)
                    {
                        _firstStimSampleReceived = true;
#ifdef USE_APPLICATION_TIME
                        qint64 timeFirstSample = ApplicationTime::currentTimeSinceEpoch();
#else
                        qint64 timeFirstSample = QDateTime::currentMSecsSinceEpoch();
#endif
                        qint64 latency = timeFirstSample - _timeRequestFirstStimSample;
                        // we assume a symetric radio link
                        _currentStimTimestamp = timeFirstSample - (latency / 2);
                        _currentStimTimestamp -=1;
                    }

                     _currentStimTimestamp++;

                    _lastStimData=data->stimulationData();
                    _lastStimData.setTimestamp(_currentStimTimestamp);

                    //loggerMacroDebug("Emitting stimulation data")
                    emit receivedStimulationData(_lastStimData);
                } // END: data->isStimDataPresent

                if (data->isStimImpedancePresent()){
                    //loggerMacroDebug("New impedance data" + QString::number(impedanceData.timestamp()))
                    ChannelData impedanceData = data->stimImpedanceData();
                    impedanceData.setTimestamp( _currentStimTimestamp );
                    emit receivedImpedanceData( impedanceData );
                } // END: data->isStimDataPresent


                // Device is present and return 1
                ret = 1;


                // Packet counter
                n_frame ++;
                //loggerMacroDebug("New packet " + QString::number(n_frame))
                if( (n_frame % n_frame_indicator == 0) ){

                    //loggerMacroDebug("Received " + QString::number(n_frame_indicator) + " packets")
                }

            } // END MAIN IF (parseByte)
        }



        // TODO: Remove this
        nBytesToRead = 0;
    }



    return ret;
}


int StarstimCom::_calculateBatteryLevel(int batteryMeasurement)
{
    unsigned char b1 = (batteryMeasurement&0x0000ff00) >> 8;
    unsigned char b0 = (batteryMeasurement&0x000000ff);
    float stateOfCharge = 0;

    float vCell = (((b0 << 8) + b1) >> 4) * 1.25;

    // 4200 mv = 100%
    // 3630 mv = 6.25%
    // 3040 mv = 0%
    if (vCell > 3630)
    {
        stateOfCharge = 6.25 + (100 - 6.25) / (4200 - 3630) * (vCell - 3630);
        if (stateOfCharge > 100)
        {
            stateOfCharge = 100;
        }
    }
    else
    {
        stateOfCharge = 6.25 / (3630 - 3040) * (vCell - 3040);
        if (stateOfCharge < 0)
        {
            stateOfCharge = 0;
        }

    }
    return (int)stateOfCharge;
}

/////////////////////////////////////
// EEG Processing
/////////////////////////////////////

int StarstimCom::_timestampAnalysis(StarstimData * data){

    // with EEG the beacon rate is half the regular one
    int diff = 1;
    if (_firstEEGSampleReceived == false){
        _firstEEGSampleReceived = true;

        // Initialise variables
        _countPacket = 0;
        _countPacketsPer30Seconds=0;
        _countPacketsLostPer30Seconds=0;


        // Calculate first sample timestamp
        qint64 timeFirstSample = QDateTime::currentMSecsSinceEpoch();
        qint64 latency = timeFirstSample - _firstTimestampRequest;

        // Assume a symetric radio link
        _currentEEGTimestamp = timeFirstSample - (latency / 2);
        _firstEEGSampleTimestamp   =_currentEEGTimestamp;

        _currentEEGStamp = 0;
    }else{

        diff = (data->eegStamp() - _currentEEGStamp)/2 - (_samplesPerBeacon-1);

        //To avoid problems with packets from the future
        if ((diff>50000)||(diff==0))
        {
            qDebug()<<"ERROR IN SOF --> diff is"<<diff<<"_currentEEGStamp"<<_currentEEGStamp<<"data->eegStamp()" << data->eegStamp();
            QString aux;
            qDebug() << aux.sprintf("0x%02X", data->eegStamp() );
            return -1;
        }
        _countPacket+=diff*_samplesPerBeacon;

        _countPacketsPer30Seconds+=diff*_samplesPerBeacon;
        _countPacketsLostPer30Seconds+=(diff-1)*_samplesPerBeacon;

        if (_countPacketsPer30Seconds>=SAMPLES_PER_SECOND*30) //We check packet loss each 15000 packets (30 seconds)
        {
            if (_countPacketsLostPer30Seconds>0)
                qDebug()<<"DeviceManager percentage of packets lost"<<_countPacketsLostPer30Seconds<<_countPacketsPer30Seconds<<_countPacketsLostPer30Seconds*100.0/_countPacketsPer30Seconds;
//            if (_countPacketsOverflowPer30Seconds>0)
//            qDebug()<<"DeviceManager percentage of packets overflow"<<_countPacketsOverflowPer30Seconds<<_countPacketsPer30Seconds<<_countPacketsOverflowPer30Seconds*100.0/_countPacketsPer30Seconds;

//            emit newPacketLossOverflowData(_countPacketsLostPer30Seconds*100.0/_countPacketsPer30Seconds,_countPacketsOverflowPer30Seconds*100.0/_countPacketsPer30Seconds);

            _countPacketsPer30Seconds=0;
            _countPacketsLostPer30Seconds=0;
//            _countPacketsOverflowPer30Seconds=0;
        }

    } // END _waitingFirstEEGSample

    // Set current time stamp
    _currentEEGStamp = data->eegStamp();

    return diff;
}

void StarstimCom::_eegProcessing(StarstimData * data, int nLostPacket){

    // Convert rawEEG to nV
    int rawEEG[32];
    for(int j = 0; j < data->eegDataArray().count(); j++){
        int* dataEEG = data->eegDataArray()[j].data();
        for (int i=0; i<32;i++)
        {
            if (dataEEG[i] >=0x800000 )
            {
                dataEEG[i] = dataEEG[i] - 0x1000000;
            }

            // Conversion
            rawEEG[i] = dataEEG[i];
            dataEEG[i] = (dataEEG[i] * 2.4 * 1000000000) /
                                                8388607.0 / 6.0;

        }

//        QString str = "decom dataEEG" + QString::number(j) + " ";
//        QString aux;
//        for(int i = 0; i < 8; i++){
//                str += aux.sprintf(" [%d]=0x%06X ", i , rawEEG[i]);
//        }
//        qDebug() << str << "ch_info:" << aux.sprintf("0x%2X", data->eegChInfo() );

    }

    // Emit nLostPacket packets
    while(nLostPacket > 1){

        int iterations = _samplesPerBeacon;
        for(int i = 0; i < iterations; i++){

            _currentEEGTimestamp += 2;
            _lastEEGData.setTimestamp(_currentEEGTimestamp);
            _lastEEGData.setRepeated(true);
            emit receivedEEGData(_lastEEGData);
        }
        nLostPacket--;
    }


    // Iterate over the samples stored in StarStimData
    for( int i = 0 ; i < data->eegDataArray().count(); i ++){

        _currentEEGTimestamp += 2;
        _lastEEGData = data->eegDataArray()[i];
        _lastEEGData.setTimestamp(_currentEEGTimestamp);
        _lastEEGData.setRepeated(false);
        emit receivedEEGData(_lastEEGData);
    }


    if( _lastEEGData.isRepeated() ){
        loggerMacroDebug("Packet is repeated!")
    }
}

//////////////////////////////////////////
// Request operations
//////////////////////////////////////////

bool StarstimCom::request(DeviceManagerTypes::StarstimRequest request, DeviceManagerTypes::StarstimRegisterFamily family, int address, QByteArray frame){

    // Convert frame to suitable variables
    int length           = frame.size();
    unsigned char* value = (unsigned char*) frame.data();

    // General requests
    QByteArray txBuffer;
    if (request == DeviceManagerTypes::PROFILE_REQUEST)  txBuffer = StarStimProtocol::buildProfileRequest();
    if (request == DeviceManagerTypes::NULL_REQUEST)     txBuffer = StarStimProtocol::buildNullRequest();

    // EEG Streaming requests
    if (request == DeviceManagerTypes::START_STREAMING_REQUEST){
        // Initialise variables before requesting for EEG Streaming
        _firstEEGSampleReceived = false;
        _firstTimestampRequest = QDateTime::currentMSecsSinceEpoch();
        txBuffer = StarStimProtocol::buildStartEEGFrame();
    }
    if (request == DeviceManagerTypes::STOP_STREAMING_REQUEST)  txBuffer = StarStimProtocol::buildStopEEGFrame();


    // RW Fields request
    if (request == DeviceManagerTypes::WRITE_REGISTER_REQUEST ){
        // Decide the offset to apply
        int offset = -1;
        if( family == DeviceManagerTypes::EEG_REGISTERS    ) offset = EEG_REGS_OFFSET;
        if( family == DeviceManagerTypes::STIM_REGISTERS   ) offset = STM_REGS_OFFSET;
        if( family == DeviceManagerTypes::ACCEL_REGISTERS  ) offset = ACCEL_REGS_OFFSET;
        if( family == DeviceManagerTypes::SDCARD_REGISTERS ) offset = SDCARD_REGS_OFFSET;

        if( offset == -1 ){
            qDebug() << "invalid address";
        }else{
            txBuffer = StarStimProtocol::buildWriteRegisterFrame(offset | address, value, length);
        }
    }
    if (request == DeviceManagerTypes::READ_REGISTER_REQUEST ){

        // Decide the offset to apply
        int offset = -1;
        if( family == DeviceManagerTypes::EEG_REGISTERS    ) offset = EEG_REGS_OFFSET;
        if( family == DeviceManagerTypes::STIM_REGISTERS   ) offset = STM_REGS_OFFSET;
        if( family == DeviceManagerTypes::ACCEL_REGISTERS  ) offset = ACCEL_REGS_OFFSET;
        if( family == DeviceManagerTypes::SDCARD_REGISTERS ) offset = SDCARD_REGS_OFFSET;

        if( offset == -1 ){
            qDebug() << "invalid address";
        }else{
            txBuffer = StarStimProtocol::buildReadRegisterFrame(offset | address, length);
        }

    }


    // Stimulation requests
    if( request == DeviceManagerTypes::START_STIMULATION_REQUEST ){
        _firstStimSampleReceived = false;
#ifdef USE_APPLICATION_TIME
        _timeRequestFirstStimSample = ApplicationTime::currentTimeSinceEpoch();
#else
        _timeRequestFirstStimSample = QDateTime::currentMSecsSinceEpoch();
#endif
        txBuffer = StarStimProtocol::buildStartStimulationFrame();
    }
    if( request == DeviceManagerTypes::STOP_STIMULATION_REQUEST ){
        txBuffer = StarStimProtocol::buildStopStimulationFrame();
    }

    if( request == DeviceManagerTypes::START_IMPEDANCE_REQUEST ) txBuffer = StarStimProtocol::buildStartImpedanceFrame();
    if( request == DeviceManagerTypes::STOP_IMPEDANCE_REQUEST )  txBuffer = StarStimProtocol::buildStopImpedanceFrame();

//    //
//    QString aux, str;
//    for(int i = 0; i < txBuffer.size(); i++ ){
//        str += aux.sprintf("0x%02X ", (0xFF & txBuffer[i]) );
//    }
//    loggerMacroDebug("txBuffer[size:" + aux.sprintf("0x%04X",txBuffer.size() )+ "] -> " + str)
//    //



    // Check whether txBuffer was filled with something
    if( txBuffer.size() == 0 ){
        loggerMacroDebug("Error requesting " + QString::number(request))
        return false;
    }

    // Protect shared variables (sharedTxBuffer)
    // --------------------------------------------------------------
    sync.lock();
    if( !sharedTxBuffer.isEmpty() ){
        sync.unlock();
        return false;
    }

    // Add new command to sharedTxBuffer
    sharedTxBuffer.clear();
    sharedTxBuffer.append(txBuffer);
    sync.unlock();
    // --------------------------------------------------------------

    // Waits until ack for command is received
    // --------------------------------------------------------------
//    if( requestBlock ){
#define ACKWNOLEDGE_TIMEOUT 10000
        reqSyncMutex.lock();
        //loggerMacroDebug("Waiting for ack")
        bool timeout = reqSyncCond.wait(&reqSyncMutex, ACKWNOLEDGE_TIMEOUT);
        reqSyncMutex.unlock();
        if( timeout == false ){
            loggerMacroDebug("Command has timed out")
            return false;
        }
        loggerMacroDebug("Request done, ACK has been received!")
//    }

    return true;
}


