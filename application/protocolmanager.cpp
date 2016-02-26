#include "protocolmanager.h"


#define N_IMPEDANCE_AVERAGE                     (4)
#define SHAM_STIMULATION_TIME                   (10)
#define MANUAL_IMPEDANCE_CHECK_STIMULATION_TIME (5)

#define STIMULATION_AFTER_LOADING
    // Activates whether an stimulation is launched after loading the template
#define IMPEDANCE_MEASURMENENT
    // Enables or disables the activation of impedance measurement

ProtocolManager::ProtocolManager(DeviceManager *device) :
    _device(device),
    _currentState(STIM_STOPPED)
{

    // Timer for the stimulation protocol
    _timerProtocol = new QTimer(this);
    //connect(_timerProtocol, SIGNAL(timeout()), this, SLOT(timerProtocolTimeOut()) );

}

ProtocolManager::~ProtocolManager(){
    // Do nothing
}

void ProtocolManager::delay(int nSecs){
    QTime dieTime= QTime::currentTime().addSecs(nSecs);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

StimulationState ProtocolManager::getCurrentState(){
    return _currentState;
}

///////////////////////////////////////////
// Stimulation Control
///////////////////////////////////////////

void ProtocolManager::fillTACSorTRNS(ProtocolSessionConfiguration* currentConfiguration){
    currentConfiguration->isTACSorTRNS = false;
    for(int i = 0; i < NUM_STIM_CHANNELS; i++){
        if(currentConfiguration->electrode[i].Atacs != 0 ) currentConfiguration->isTACSorTRNS = true;
        if(currentConfiguration->electrode[i].Atrns != 0 ) currentConfiguration->isTACSorTRNS = true;
    }

}

bool ProtocolManager::startStimulationProtocol(StimSession stimSession, ProtocolSessionConfiguration currentConfiguration)
{
    loggerMacroDebug("Start the stimulation")

    // Transform template from basic to advanced
    if( currentConfiguration.protocolType == BASIC){
        StimProtocolSessionConfig::updateAdvancedStimFromBasic(currentConfiguration);

        loggerMacroDebug(currentConfiguration.toString())
    }

    // Store current template
    // -------------------------------------------
    _currentConfiguration = currentConfiguration;
    _stimSession = stimSession;
    // -------------------------------------------
    // Resets the impedance
    this->resetImpedance();

    // Finds out whether is tACS or tRNS
    this->fillTACSorTRNS( &_currentConfiguration );

    // Not checking impedance
    _isCheckingImpedance   = false;
    _isStoppingStimulation = false;

    qDebug()<<"ProtocolManager::startStimulation()";

    // Connect slots according to sham
    connectTimeOutSlot(_currentConfiguration.isSham);

    // Alter current configuration according to the percentage completed
    int remainingPercentage = (100 - stimSession.percentageCompleted);
#ifdef CALCULATE_PERCENTAGE_WITH_RAMPS
    float totalStimDuration = (_currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration) * ( (float) remainingPercentage/100 );
    float resumeStimDuration = totalStimDuration - _currentConfiguration.rampUpDuration - _currentConfiguration.rampDownDuration;
    _stimDurationElapsedFromResume = totalStimDuration * stimSession.percentageCompleted;
#else
    float resumeStimDuration = (_currentConfiguration.stimulationDuration) * ( (float) remainingPercentage/100 );
    _stimDurationElapsedFromResume = _currentConfiguration.stimulationDuration * ((float) stimSession.percentageCompleted /100);
#endif

    loggerMacroDebug(QString::number(remainingPercentage) + "% is still missing for stimulation out of " +  QString::number(currentConfiguration.stimulationDuration) )
    loggerMacroDebug("Calculated stimulation of " + QString::number(resumeStimDuration) + " seconds"  )
    _currentConfiguration.stimulationDuration = (int) round(resumeStimDuration);
    loggerMacroDebug("Applying stimulationDuration=" + QString::number( _currentConfiguration.stimulationDuration ) +
                            " RampUp=" + QString::number(_currentConfiguration.rampUpDuration) +
                            " RampDown=" + QString::number(_currentConfiguration.rampDownDuration) )

    // Check the correctness of the protocol
    StimProtocolSessionConfig::StimProtocolIssueInfo protocolIssueInfo;
    bool res = StimProtocolSessionConfig::protocolSessionConfigurationIsCorrect(_currentConfiguration, 0, false, protocolIssueInfo);
    if( res == false ){
        QString msg;
        StimProtocolSessionConfig::getIssueMessage(_currentConfiguration.protocolType, protocolIssueInfo, msg);
        loggerMacroDebug("Error in stimulation protocol " + msg)
        return false;
    }

    // Configure sham stimulation
    ProtocolSessionConfiguration protocolLoadedIntoDevice;
    if( _currentConfiguration.isSham ){
        loggerMacroDebug("Current configuration is Sham")

        // Check whether stimulation configured sham is possible
        int totalStim     = (_currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration);
        int totalShamStim = (_currentConfiguration.rampUpDuration + SHAM_STIMULATION_TIME + _currentConfiguration.rampSham + _currentConfiguration.rampSham + _currentConfiguration.rampDownDuration);
        if( totalStim < totalShamStim ){
            loggerMacroDebug("This sham stimulation is not possible. Aborting operation")

            delay(2); // Delay a couple of seconds

            emit stimulationStatus(_currentState, totalStim + _stimDurationElapsedFromResume, totalStim + _stimDurationElapsedFromResume);
            return false;
        }



        // Protocol to be loaded into device
        protocolLoadedIntoDevice = _currentConfiguration;
        protocolLoadedIntoDevice.stimulationDuration = SHAM_STIMULATION_TIME;
        protocolLoadedIntoDevice.rampDownDuration    = _currentConfiguration.rampSham;
        startStimSecondSham = loadStimSecondSham = false;

        _currentState = STIM_FIRSTSHAM_RAMPUP;


    }else{

        // Protocol to be loaded into device
        protocolLoadedIntoDevice = _currentConfiguration;

        // Next state
        _currentState = STIM_RAMPUP;
    }



    // Configure the device with the desired template
    res = this->loadStimulationConfiguration( protocolLoadedIntoDevice );
    if(!res) return false;
    // Start Stimulation
#ifdef STIMULATION_AFTER_LOADING
    _device->startStimulation();
#endif

    return true;
}

bool ProtocolManager::stopStimulationProtocol(){
    loggerMacroDebug("Aborting stimulation operation")

    // Stop the stimulation
    _isStoppingStimulation = true;
    if( _device->getDeviceStimulating() ){
        _device->stopStimulation();
    }

    // When standard stimulation
    if( _currentState == STIM_RAMPUP || _currentState == STIM_STIMULATION ){
        _currentState = STIM_RAMPDOWN;
    }

    // If we are waiting for second sham
    if( _currentState == STIM_WAITINGFORSECONDSHAM){
        // Delay the stop of the protocol for a couple of seconds
        QTimer::singleShot(2000, this, SLOT(stopStimulationProtocolDelayed()) );
    }
    // If we still are in the first sham them move to a fake state second sham
    if( _currentState == STIM_FIRSTSHAM_RAMPDOWN || _currentState == STIM_FIRSTSHAM_STIMULATION || _currentState == STIM_FIRSTSHAM_RAMPDOWN){
        _currentState = STIM_SECONDSHAM_RAMPDOWN;
    }



    return true;
}

void ProtocolManager::stopStimulationProtocolDelayed(){
    // Move to the following state
    _currentState = STIM_STOPPED;

    // Update sesion result
    emit updateSessionResult(getCurrentSession().sessionId,
                             getStimulationDurationElapsed() + getStimulationDurationElapsedFromResume(),
                             QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm") );
}

///////////////////////////////////////////
// Slot connection/disconnection
///////////////////////////////////////////
void ProtocolManager::connectTimeOutSlot(bool isSham){

    // Configure stimulation
    if( isSham ){
        loggerMacroDebug("Sham Configuration with ramp:" + QString::number(_currentConfiguration.rampSham))
        connect(_timerProtocol, SIGNAL(timeout()), this, SLOT(timerProtocolTimeOutSham()) );
    }else{
        loggerMacroDebug("Standard Configuration")
        connect(_timerProtocol, SIGNAL(timeout()), this, SLOT(timerProtocolTimeOutStandard()) );
    }

    // Prints out information about the object
    //_timerProtocol->dumpObjectInfo();
}

void ProtocolManager::disconnectTimeOutSlot(bool isSham){
    // Configure stimulation
    if( isSham ){
        loggerMacroDebug("Sham Configuration with ramp:" + QString::number(_currentConfiguration.rampSham))
        disconnect(_timerProtocol, SIGNAL(timeout()), this, SLOT(timerProtocolTimeOutSham()) );
    }else{
        loggerMacroDebug("Standard Configuration")
        disconnect(_timerProtocol, SIGNAL(timeout()), this, SLOT(timerProtocolTimeOutStandard()) );
    }

    // Prints out information about the object
    //_timerProtocol->dumpObjectInfo();

}

///////////////////////////////////////////
// Stimulation Monitoring
///////////////////////////////////////////

void ProtocolManager::timerProtocolTimeOutStandard(){

    StimulationState initState = _currentState;

    int threshold = 0;

    // Increase the duration ellapsed
    _stimDurationEllapsed++;

    // Signal with curent stimulation
    float totalStim = _currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration;

    QString str = QString::number( totalStim );
    loggerMacroDebug("-------------------------------------------");
    loggerMacroDebug( "Stimulation is " + QString::number(_stimDurationEllapsed ) + " out of " + str + " controlTimer:" + QString::number(controlTimer.elapsed()/1000.0+1.0));
    //qDebug()<<"ProtocolManager::timeOut() _stimIndex"<<_stimIndex<<"_currentState"<<_currentState<<controlTimer.elapsed()/1000.0+1.0<<"_currentState"<<_currentState;


    // If there is a delay we increment the _stimDurationEllapsed
    if (int(controlTimer.elapsed()/1000.0+1.0)>_stimDurationEllapsed)
    {
        qDebug()<<"ProtocolManager::timeOut(): There is a delay and I must increment the timer";
        qDebug()<<"ProtocolManager::timeOut()"<<_stimDurationEllapsed<<controlTimer.elapsed()/1000.0+1.0;
        _stimDurationEllapsed++;
    }


// [1]
#ifdef CALCULATE_PERCENTAGE_WITH_RAMPS
    int offset = 0;
    offset = _stimDurationElapsedFromResume;
#else
    int offset = 0;
    if( _stimDurationElapsedFromResume != 0 ) offset = _currentConfiguration.rampUpDuration + _stimDurationElapsedFromResume;
#endif
    // Notify with the current status
    emit stimulationStatus(_currentState, _stimDurationEllapsed + offset , ((int) totalStim + _stimDurationElapsedFromResume));


    switch (_currentState)
    {

        // RampUp State
        // -------------------
        case (STIM_RAMPUP):  // RampUp
            loggerMacroDebug("Current state -> STIM_RAMPUP")



            threshold = _currentConfiguration.rampUpDuration;
            if (_stimDurationEllapsed >= threshold)
            {
                // Start Impedance measurement
#ifdef IMPEDANCE_MEASURMENENT
                _device->startImpedanceMeasurement();
#endif

                _currentState = STIM_STIMULATION;
            }

            // Correct _stimDurationEllapsed if necessary
            if (_stimDurationEllapsed >= (_currentConfiguration.rampUpDuration )) _stimDurationEllapsed--;

        break;

        // Stimulating State
        // -------------------
        case (STIM_STIMULATION):
            loggerMacroDebug("Current state -> STIM_STIMULATION")

            threshold = _currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration;
            if (_stimDurationEllapsed >= threshold )
            {
                qDebug()<<"\tWe stop impedance _stimulationDuration"<<_currentConfiguration.stimulationDuration<<"_stimDurationElapsedFromResume"<<_stimDurationElapsedFromResume<<"_rampDownDuration"<<_currentConfiguration.rampDownDuration;
                //emit endFullStimulation();

                _currentState = STIM_RAMPDOWN;

                // Start Impedance measurement
#ifdef IMPEDANCE_MEASURMENENT
                _device->stopImpedanceMeasurement();
#endif
            }

            // Correct _stimDurationEllapsed if necessary
            if (_stimDurationEllapsed >= (_currentConfiguration.stimulationDuration + _currentConfiguration.rampUpDuration)) _stimDurationEllapsed--;

        break;

        // RampDown State
        // -------------------
        case (STIM_RAMPDOWN):
            loggerMacroDebug("Current state -> STIM_RAMPDOWN")
        break;

        // STOPPED State
        // -------------------
        case (STIM_STOPPED):
            loggerMacroDebug("Current state -> STIM_STOPPED ")
// [2]
            // NOTE: Synchronisation problem.
            // This happens because we have received a StimulationFinished between [1] and [2]
            if( initState != _currentState ) return;
            if( _stimDurationEllapsed >= totalStim || _isStoppingStimulation  ){
                _timerProtocol->stop();
                disconnectTimeOutSlot(_currentConfiguration.isSham);
                loggerMacroDebug("Stopping timer protocol")
            }
            break;


        default:
             loggerMacroDebug("ERROR: Reached default state")
             _timerProtocol->stop();

        break;
     }
}

void ProtocolManager::timerProtocolTimeOutSham(){
    StimulationState initState = _currentState;

    int   threshold = 0;

    // This is used for controlling the second sham
    int   totalStim;
    ProtocolSessionConfiguration currentConfiguration;



    // Increase the duration ellapsed
    _stimDurationEllapsed++;

    // Signal with curent stimulation
    totalStim = _currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration;

    QString str = QString::number( totalStim );
    loggerMacroDebug("-------------------------------------------");
    loggerMacroDebug("timeProtocolTimeOutSham")
    loggerMacroDebug( "Stimulation is " + QString::number(_stimDurationEllapsed ) + " out of " + str + " controlTimer:" + QString::number(controlTimer.elapsed()/1000.0+1.0));


    // If there is a delay we increment the _stimDurationEllapsed
    if (int(controlTimer.elapsed()/1000.0+1.0)>_stimDurationEllapsed)
    {
        qDebug()<<"ProtocolManager::timeOut(): There is a delay and I must increment the timer";
        qDebug()<<"ProtocolManager::timeOut()"<<_stimDurationEllapsed<<controlTimer.elapsed()/1000.0+1.0;
        _stimDurationEllapsed++;
    }
#ifdef CALCULATE_PERCENTAGE_WITH_RAMPS
    int offset = 0;
    offset = _stimDurationElapsedFromResume;
#else
    int offset = 0;
    if( _stimDurationElapsedFromResume != 0 ) offset = _currentConfiguration.rampUpDuration + _stimDurationElapsedFromResume;
#endif
    emit stimulationStatus(_currentState, _stimDurationEllapsed + offset, (int) totalStim + _stimDurationElapsedFromResume);




    switch (_currentState)
    {

        // STIM_FIRSTSHAM_RAMPUP State
        // ---------------------------
        case (STIM_FIRSTSHAM_RAMPUP):  // RampUp
            loggerMacroDebug("Current state -> STIM_FIRSTSHAM_RAMPUP")

            threshold = _currentConfiguration.rampUpDuration;
            if (_stimDurationEllapsed >= threshold)
            {
                // Start Impedance measurement
                _device->startImpedanceMeasurement();

                _currentState = STIM_FIRSTSHAM_STIMULATION;
            }
        break;

        // STIM_FIRSTSHAM_STIMULATION State
        // --------------------------------
        case (STIM_FIRSTSHAM_STIMULATION):
            loggerMacroDebug("Current state -> STIM_FIRSTSHAM_STIMULATION")

            threshold = _currentConfiguration.rampUpDuration + SHAM_STIMULATION_TIME;
            if (_stimDurationEllapsed >= threshold )
            {
                qDebug()<<"\tWe stop impedance _stimulationDuration"<<_currentConfiguration.stimulationDuration<<"_stimDurationElapsedFromResume"<<_stimDurationElapsedFromResume<<"_rampDownDuration"<<_currentConfiguration.rampDownDuration;
                //emit endFullStimulation();

                _currentState = STIM_FIRSTSHAM_RAMPDOWN;

                // Start Impedance measurement
                _device->stopImpedanceMeasurement();
                //if (_doImpedanceCheck) emit stopImpedanceCheck();
            }
        break;

        // STIM_FIRSTSHAM_RAMPDOWN State
        // -----------------------------
        case (STIM_FIRSTSHAM_RAMPDOWN):
            loggerMacroDebug("Current state -> STIM_RAMPDOWN")
        break;

        // STIM_WAITINGFORSECONDSHAM State
        // -----------------------------
        case STIM_WAITINGFORSECONDSHAM:
            loggerMacroDebug("Current state -> STIM_WAITINGFORSECONDSHAM")

            // Generate fake impedance info
            updateFakeImpedanceInfo();

            if( loadStimSecondSham == false){
                currentConfiguration = _currentConfiguration;
                currentConfiguration.rampUpDuration = currentConfiguration.rampSham;
                currentConfiguration.stimulationDuration = 0;
                loadStimulationConfiguration(currentConfiguration);
                loadStimSecondSham = true;
            }

            // Jump to STIM_SECONDSHAM_RAMPUP when elapsed
            totalStim = (_currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration );
            threshold = totalStim - ( _currentConfiguration.rampSham + _currentConfiguration.rampDownDuration );
            if( _stimDurationEllapsed >= threshold ){
                _currentState = STIM_SECONDSHAM_RAMPUP;
            }
            break;

        case STIM_SECONDSHAM_RAMPUP:
            loggerMacroDebug("Current state -> STIM_SECONDSHAM_RAMPUP")

            if( startStimSecondSham == false){
                // Start Stimulation
                _device->startStimulation();
                startStimSecondSham = true;
            }

            totalStim = (_currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration );
            threshold = totalStim - ( _currentConfiguration.rampDownDuration );

            if( _stimDurationEllapsed >= threshold ){
                _currentState = STIM_SECONDSHAM_RAMPDOWN;
            }
            break;

        case STIM_SECONDSHAM_RAMPDOWN:
            loggerMacroDebug("Current state -> STIM_SECONDSHAM_RAMPDOWN")

            // Second sham rampDown
            totalStim = (_currentConfiguration.rampUpDuration + _currentConfiguration.stimulationDuration + _currentConfiguration.rampDownDuration );
            threshold = totalStim ;

            break;

        // STOPPED State
        // -------------------
        case (STIM_STOPPED):
            loggerMacroDebug("Current state -> STIM_STOPPED")
            totalStim = _currentConfiguration.rampUpDuration + SHAM_STIMULATION_TIME + _currentConfiguration.rampSham;
// [2]
            // NOTE: Synchronisation problem.
            // This happens because we have received a StimulationFinished between [1] and [2]
            if( initState != _currentState ) return;
            if( _stimDurationEllapsed >= totalStim || _isStoppingStimulation ){
                loggerMacroDebug("Stopping timer protocol")
                _timerProtocol->stop();
                disconnectTimeOutSlot(_currentConfiguration.isSham);
            }
            break;

        default:
             loggerMacroDebug("ERROR: Reached default state")
             _timerProtocol->stop();

        break;
     }

}



bool ProtocolManager::isSessionFinished(){
    if( _isCheckingImpedance == true) return false;
    if( _currentState == STIM_WAITINGFORSECONDSHAM ) return false;

    return true;
}

void ProtocolManager::stimulationFinished(bool abortedbyNEC){
    // Stop Timer if active to avoid execution of timeout
    if(_timerProtocol->isActive()){
        _timerProtocol->stop();
    }

    //qDebug()<<"ProtocolManager::StimulationFinished Received, Current State"<<_currentState<<"_rampSham"<<_rampSham<<"abortedbyNEC"<<abortedbyNEC;
    loggerMacroDebug("Stimulation finished. Current state -> " + QString::number(_currentState) + " abortedByNEC -> " + QString::number(abortedbyNEC))

    if( _currentState == STIM_FIRSTSHAM_RAMPUP      ){}
    if( _currentState == STIM_FIRSTSHAM_STIMULATION ){}

    if( _currentState == STIM_FIRSTSHAM_RAMPDOWN){
        // Move to the following state
        _currentState = STIM_WAITINGFORSECONDSHAM;
        // Restart the timer
        _timerProtocol->start();
    }

    if( _currentState == STIM_SECONDSHAM_RAMPUP){}
    if( _currentState == STIM_SECONDSHAM_RAMPDOWN){
        _currentState = STIM_STOPPED;
        _timerProtocol->start();
    }


    // Standard protocol
    // -------------------
    if (_currentState == STIM_RAMPUP){
        // Do nothing here
    }

    if (_currentState == STIM_RAMPDOWN)
    {
        // Stopping timer
        _currentState = STIM_STOPPED;

        loggerMacroDebug("Starting timer " +  QString::number( _currentState ) )
        // Restart the timer
        _timerProtocol->start();
    }



    if(_currentState == STIM_STIMULATION)
    {

        // We were stimulating and we arrive here because STIM-OFF was received
        loggerMacroDebug("Setting states to stopped in STIM_STIMULATION")
        _currentState = STIM_STOPPED;
        _timerProtocol->start();
    }



    if(_currentState == STIM_PREEEG)
    {
         //We are doing the preEEG and we arrive here because STIM-OFF was received
    }


    if (_currentState == STIM_DOINGSECONDSHAM)
    {

        _currentState = STIM_POSTEEG;
        _timerProtocol->start();
    }

    if(_currentState == STIM_IMPEDANCECHECK) //MANUAL IMPEDANCE REQUEST
    {
        qDebug()<<"Current State Impedance Request";

    }


}

///////////////////////////////////////////
// New Impedance Data
///////////////////////////////////////////

void ProtocolManager::updateFakeImpedanceInfo()
{

    // Generate fake impedance for every channel
    for( int i = 0; i < NUM_STIM_CHANNELS; i++ ){

        // Only generate for those which are active
        if( _vlTestValueMovingAverage[i] == 0 && _isTestValueMovingAverage[i] == 0) continue;

        // We generate a number between 0 and 10 to decide if new impedances should  be calculated
        if ( (rand()%10) >5) continue;

        loggerMacroDebug("Generate fake impedance for channel " + QString::number(i))

        // Generate new impedance fake
        int percentageOfVariation = _impedanceTestValueMovingAverage[i]*0.05;
        int fakeImpedance = _impedanceTestValueMovingAverage[i]+rand()%(percentageOfVariation);

        // Emit signal
        emit reportImpedanceTest(i, fakeImpedance, _isTestValueMovingAverage[i], _vlTestValueMovingAverage[i], 0);

    }


}

void ProtocolManager::newImpedanceData(ChannelData data, unsigned long long timeStamp){

    int aux;
    int Cm;
    int Ci;
    float Is;
    float Vl;
    int Impedance;

    // Log the result
    //loggerMacroDebug( "ProtocolManager::newImpedanceData:" + QString::number( timeStamp ) + " " + QString::number( _currentState ) );

    // Only perform operations when STIM_STIMULATION
    bool condition = false;
    switch( _currentState ){
        case STIM_FIRSTSHAM_STIMULATION:
        case STIM_STIMULATION:
        case STIM_FIRSTSHAM_RAMPDOWN:
        case STIM_WAITINGFORSECONDSHAM:
        case STIM_SECONDSHAM_RAMPUP:
            condition = false;
            break;
        default:
            condition = true;
    }
    //if ( _currentState != STIM_STIMULATION || _currentState != STIM_FIRSTSHAM_STIMULATION ){
    if ( condition ){
        loggerMacroDebug("Current state is not Stimulating (Sham/Standard). Aborting impedance measurement")
        return;
    }


    // Iterate for every channel
    for (int i = 0; i < NUM_STIM_CHANNELS; i++){

        // If this channel is not
        if (data.data()[i] == 0) continue;

        aux=data.data()[i];
        Cm=aux&0x0000FFFF;
        Ci=(aux&0xFFFF0000)>>16;

        Is = (2.5/1024.0*(double)Cm-1.25)/504.0;


        //Vl = (2.5/1024.0*(double)Ci-1.25)*(16)+1.25;
        //For new HW use the following formula
        Vl = (2.5/1024.0*(double)Ci-1.25)*(23)+1.25-1.49;

        if (_isTestValueMovingAverage[i]==0.0) //To avoid wrong averages
            _isTestValueMovingAverage[i]=Is;

        if (_vlTestValueMovingAverage[i]==0.0) //To avoid wrong averages
            _vlTestValueMovingAverage[i]=Vl;

        //qDebug()<<"_isTestValue[i]"<<i+1<<_isTestValueMovingAverage[i]*1000000<<Is*1000000;
        //qDebug()<<"_vlTestValue[i]"<<i+1<<_vlTestValueMovingAverage[i]<<Vl;


        // Discard the impedance
        if ((fabs(Is*1000000)<200.0)&&(fabs(Vl)<3.0))
        {
            qDebug()<<"We discard impedance in electrode"<<i+1<<fabs(Is*1000000)<<fabs(Vl);
            continue;
        }

        //If current=0 then impedance calculation fails
        if (Is!=0.0)
        {
            Impedance = Vl/Is;

            if(Impedance<0)
                Impedance=(-1)*Impedance;

            if (Impedance>25000)
                Impedance=25000;
        }
        else
        {
            qDebug()<<"**************ERROR: If current=0 then impedance calculation fails Is"<<Is<<"Vl"<<Vl<<"_isTestValue[i]"<<_isTestValueMovingAverage[i]*1000000<<"_vlTestValue[i]"<<_vlTestValueMovingAverage[i];
            Impedance=25000;
        }


        // If we are in impedance check procedure
        //if ( _currentState == STIM_IMPEDANCECHECK ) // Checking impedance
        if( _isCheckingImpedance )
        {
            _isTestValueMovingAverage[i] = _isTestValueMovingAverage[i]*0.9 + 0.1*Is;
            _vlTestValueMovingAverage[i] = _vlTestValueMovingAverage[i]*0.9 + 0.1*Vl;

            _impedanceTestValueMovingAverage[i] = _impedanceTestValueMovingAverage[i]*0.9 + 0.1*Impedance;

            // emit the signal
            emit reportImpedanceTest(i, _impedanceTestValueMovingAverage[i],_isTestValueMovingAverage[i],_vlTestValueMovingAverage[i],timeStamp);
        }
        // Otherwise
        else
        {
            //Now we add the values to calculate the minimum of each 8 impedance values
            _isTestValueBuffer[i].push_back(Is);
            _vlTestValueBuffer[i].push_back(Vl);
            _impedanceTestValueBuffer[i].push_back(Impedance);

            // Make the average before emitting signal
            if (_impedanceTestValueBuffer[i].size() == N_IMPEDANCE_AVERAGE)
            {
                float minIs=_isTestValueBuffer[i].at(0);
                float minVl=_vlTestValueBuffer[i].at(0);
                int minImpedance=_impedanceTestValueBuffer[i].at(0);

                for (int j=0;j<_impedanceTestValueBuffer[i].size();j++)
                {
                    if (minImpedance>_impedanceTestValueBuffer[i].at(j))
                    {
                        minImpedance=_impedanceTestValueBuffer[i].at(j);
                        minVl=_vlTestValueBuffer[i].at(j);
                        minIs=_isTestValueBuffer[i].at(j);
                    }
                } // End for

                //If it is TACSorTRNS, we use tha abs value to change the moving average
                if (_currentConfiguration.isTACSorTRNS)
                {
                    _isTestValueMovingAverage[i] = _isTestValueMovingAverage[i]*0.7 + 0.3*fabs(minIs);
                    _vlTestValueMovingAverage[i] = _vlTestValueMovingAverage[i]*0.7 + 0.3*fabs(minVl);
                }
                else
                {
                    _isTestValueMovingAverage[i] = _isTestValueMovingAverage[i]*0.7 + 0.3*minIs;
                    _vlTestValueMovingAverage[i] = _vlTestValueMovingAverage[i]*0.7 + 0.3*minVl;
                } // End : if (_isTACSorTRNS)

                _impedanceTestValueMovingAverage[i] = _impedanceTestValueMovingAverage[i]*0.7 + 0.3*minImpedance;

                // emit the signal
                emit reportImpedanceTest(i, _impedanceTestValueMovingAverage[i],_isTestValueMovingAverage[i],_vlTestValueMovingAverage[i],timeStamp);

                _impedanceTestValueBuffer[i].clear();
                _isTestValueBuffer[i].clear();
                _vlTestValueBuffer[i].clear();
            } // END: if (_impedanceTestValueBuffer[i].size()==4)

        } // END: if ( _currentState == STIM_IMPEDANCECHECK )


        // Check if impedance measurement is failing
        if( _impedanceTestValueMovingAverage[i] > MAX_IC_RANGE ){_impedanceWrongValues[i] ++;
        }else{ _impedanceWrongValues[i] = 0; }
        if ( _impedanceWrongValues[i]>2 && _currentState == STIM_STIMULATION ){
            loggerMacroDebug("Impedance check has failed on channel " + QString::number(i))
            emit impedanceCheckFailed(i);
        }


//            qDebug()<<"***"<<i<<"Impedance: "<<Impedance<<_impedanceTestValue[i];
//            qDebug()<<"***"<<i<<"Current: "<<Is<<Is*1000000;
//            qDebug()<<"***"<<i<<"Voltage: "<<Vl<<_vlTestValue[i];
//            qDebug()<<"***"<<i<<"Cm: "<<Cm;
//            qDebug()<<"***"<<i<<"Ci: "<<Ci;
    } // END: main for
/**/
}

///////////////////////////////////////////
// Impedance Check
///////////////////////////////////////////
bool ProtocolManager::isStimulationReturnElectrode(StimulationElectrodeConfig electrode ){
    return (electrode.electrodeTypeAdv == STIMULATION_ADV || electrode.electrodeTypeAdv == RETURN_ADV);
}


void ProtocolManager::resetImpedance(){
    // Initialise impedance buffers
    for (int i = 0; i < NUM_STIM_CHANNELS; i++)
    {
        _impedanceTestValueMovingAverage[i] = 5000;
        _vlTestValueMovingAverage[i]        = 0.0;
        _isTestValueMovingAverage[i]        = 0.0;

        _impedanceTestValueBuffer[i].clear();
        _isTestValueBuffer[i].clear();
        _vlTestValueBuffer[i].clear();

        _impedanceWrongValues[i] = 0;
    }
}

void ProtocolManager::manualImpedanceCheckProtocol(ProtocolSessionConfiguration currentConfiguration){

    loggerMacroDebug("Impedance check starting")

    // Transform template from basic to advanced
    if( currentConfiguration.protocolType == BASIC){
        StimProtocolSessionConfig::updateAdvancedStimFromBasic(currentConfiguration);
        loggerMacroDebug(currentConfiguration.toString())
    }

    // Store current template
    // -------------------------------------------
    _currentConfiguration = currentConfiguration;
    // -------------------------------------------

    // Reset impedance values
    this->resetImpedance();

    // Finds out whether is tACS or tRNS
    this->fillTACSorTRNS( &_currentConfiguration );


    // Create the template for the impedance check
    _currentConfiguration.rampUpDuration      = 1;
    _currentConfiguration.stimulationDuration = MANUAL_IMPEDANCE_CHECK_STIMULATION_TIME;
    _currentConfiguration.rampDownDuration    = 1;
    _currentConfiguration.isSham = false;


    int stimulatingElectrodes = 0;
    for(int i = 0; i < NUM_STIM_CHANNELS; i++){
        if( !isStimulationReturnElectrode( _currentConfiguration.electrode[i] ) ) continue;
        stimulatingElectrodes++;

        loggerMacroDebug("Configuring electrode -> " + QString::number(i))

        // Make all the elctrodes to be stimulating
        _currentConfiguration.electrode[i].electrodeTypeAdv = STIMULATION_ADV;

        // Select gain for each electrode
        int gain = (i%2==0)?1:-1;
        _currentConfiguration.electrode[i].Atdcs = gain*1000;
        _currentConfiguration.electrode[i].Atacs = 0;
        _currentConfiguration.electrode[i].Ftacs = 0;
        _currentConfiguration.electrode[i].Ptacs = 0;
        _currentConfiguration.electrode[i].Atrns = 0;

    }
    loggerMacroDebug("Number of stimulating electrodes -> " + QString::number(stimulatingElectrodes))


    if( (stimulatingElectrodes%2) == 0 ){
        // Do nothing
    }else{
        _currentConfiguration.electrode[stimulatingElectrodes-3].Atdcs = 1000/2;
        _currentConfiguration.electrode[stimulatingElectrodes-1].Atdcs = 1000/2;
    }

    loggerMacroDebug(" -- Stimulation Manual Impedance Check template --")
    loggerMacroDebug(_currentConfiguration.toString())


    // Set that we are checking impedance
    _isCheckingImpedance = true;

    // Configure timeOutSlot
    connectTimeOutSlot( _currentConfiguration.isSham );
    _currentState = STIM_RAMPUP;
    // Load Stimulation on device
    bool res = loadStimulationConfiguration( _currentConfiguration );
    if( !res ){ loggerMacroDebug("Failed to load template"); return;}

    // Start stimulation
    _device->startStimulation();

}

///////////////////////////////////////////
// Start Stimulation
///////////////////////////////////////////

void ProtocolManager::stimulationStarted(){
    loggerMacroDebug("Stimulation has started")

    if( _currentState == STIM_RAMPUP || _currentState == STIM_FIRSTSHAM_RAMPUP ){
        _stimDurationEllapsed = 0;

    }


    // Stimulation Started
    loggerMacroDebug( "Stimulation Started, Current State : " + QString::number(_currentState) );


    controlTimer.start();
    _timerProtocol->start(1000);
    qDebug()<<"Stimulation Started, Current State"<<_currentState;


    return;

}


bool ProtocolManager::loadStimulationConfiguration( ProtocolSessionConfiguration currentConfiguration ){


    QString auxStr;
    QString auxSprintf;
    int retriesPerformed;
    unsigned char aux;
    int val=0;
    QElapsedTimer myTimer;
    int errorCount=0;
    int numRegistersWritten=0;
    int numTotalRegistersToWrite = 88; // RampUp, RampDown, StimTime(6)
                                       // Atdcs(8*2)
                                       // Ftacs,Ptacs,Atacs(8*3*2)
                                       // Trns(8*2)
                                       // CH_INFO, CH_FREE (2)

    // Check pre-conditions
    // --------------------------
    if( _device->getDeviceStatus().STIM == true ){
        qDebug() << "Manager::startStimulation() STM-ON we cannot do another stimulation so fast";
        //stopStimulation();
        //emit abortCurrentStimulationProtocol();
        return false;
    }


    // Set the frequency to check impedance
    unsigned char freqImp=238;
    QByteArray freqImpArray;
    freqImpArray.append((char) freqImp);
    _device->writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_IMP_FREQ_DEC_ADDR, freqImpArray);

    isTACSorTRNS=false;

    myTimer.start();


    // Calculate REG_CH_INFO
    // ---------------------
    unsigned int REG_CH_INFO   = 0;
    unsigned int REG_CH_RETURN = 0;
    auxStr.clear();
    for (int i = 0; i < _device->getNumOfChannels(); i++)
    {
        // Limit the number of channels
        if( i >= _device->getNumOfChannels() ) continue;

        auxStr += "CH[" + QString::number(i) + "]=";
        if(currentConfiguration.electrode[i].electrodeTypeAdv == STIMULATION_ADV){
            auxStr += "STM ";
            REG_CH_INFO=REG_CH_INFO|(1<<(i));
        }else if(currentConfiguration.electrode[i].electrodeTypeAdv == RETURN_ADV){
            auxStr += "RET ";
            REG_CH_INFO=REG_CH_INFO|(1<<(i));
            REG_CH_RETURN=REG_CH_RETURN|(1<<(i));
        }else if(currentConfiguration.electrode[i].electrodeTypeAdv == NOT_USED_ADV){
            auxStr += "NOT ";
        }else{
            auxStr += "EEG ";
        }

     } // END: for (int i = 0; i <_device->getNumOfChannels(); i++)

    loggerMacroDebug("Channel configuration " + auxStr)

    loggerMacroDebug("Calculated CH_INFO "   + auxStr.sprintf("0x%08X", REG_CH_INFO))
    loggerMacroDebug("Calculated CH_RETURN " + auxStr.sprintf("0x%08X", REG_CH_RETURN))

    // Write EEG_REGISTERS, CH_INFO
    // ----------------------------

    // If working with devices with less than 32 channel, force other EEG channels to be OFF.
    unsigned int  mask = 0x00;

    for(int i = 0; i < NUM_STIM_CHANNELS; i++){
        if( i >= _device->getNumOfChannels()) mask = mask | (1 << i);
    }


    QByteArray chInfoArray;
    chInfoArray.clear();

    unsigned int EEG_REG_CH_INFO = mask |(REG_CH_INFO&0xFFFFFFFF);
    loggerMacroDebug("Configured EEG_REGISTERS - CH_INFO " + auxStr.sprintf("0x%02X", EEG_REG_CH_INFO ))
    chInfoArray.append( (char) ((EEG_REG_CH_INFO & 0x000000FF) >> 8*0) );
    chInfoArray.append( (char) ((EEG_REG_CH_INFO & 0x0000FF00) >> 8*1) );
    chInfoArray.append( (char) ((EEG_REG_CH_INFO & 0x00FF0000) >> 8*2) );
    chInfoArray.append( (char) ((EEG_REG_CH_INFO & 0xFF000000) >> 8*3) );
    _device->writeRegister(DeviceManagerTypes::EEG_REGISTERS, EEG_CH_INFO_ADDR,chInfoArray);
    retriesPerformed = _device->checkRegWritten(chInfoArray, DeviceManagerTypes::EEG_REGISTERS, EEG_CH_INFO_ADDR);
    if( retriesPerformed < 5 ) loggerMacroDebug("EEG_CH_INFO_ADDR : Read registers OK")
    else{ loggerMacroDebug("EEG_CH_INFO_ADDR : Read registers FAIL") return false;}
    errorCount += retriesPerformed;


    loggerMacroDebug ( "Elapsed time " +  QString::number(myTimer.elapsed()/1000.0) );
    numRegistersWritten ++;

    // Write OPERATION_REGISTERS
    // ----------------------------------------
    QByteArray regArray;
    regArray.clear();
    for(int i = 0; i < 17; i++) regArray.append((char) 0x00);
    regArray[0] = (char) ((REG_CH_INFO & 0x000000FF) >> 8*0);
    regArray[1] = (char) ((REG_CH_INFO & 0x0000FF00) >> 8*1);
    regArray[2] = (char) ((REG_CH_INFO & 0x00FF0000) >> 8*2);
    regArray[3] = (char) ((REG_CH_INFO & 0xFF000000) >> 8*3);


    regArray[6] = (char) ((REG_CH_RETURN & 0x000000FF) >> 8*0);
    regArray[7] = (char) ((REG_CH_RETURN & 0x0000FF00) >> 8*1);
    regArray[8] = (char) ((REG_CH_RETURN & 0x00FF0000) >> 8*2);
    regArray[9] = (char) ((REG_CH_RETURN & 0xFF000000) >> 8*3);

    regArray[10] = STM_REGS_RATE_DEC_DEF;

    ProtocolSessionConfiguration* psc = &(currentConfiguration);
    regArray[11] = (unsigned char)((psc->rampUpDuration      & 0x000000FF) >> 8*0);
    regArray[12] = (unsigned char)((psc->rampUpDuration      & 0x0000FF00) >> 8*1);
    regArray[13] = (unsigned char)((psc->rampDownDuration    & 0x000000FF) >> 8*0);
    regArray[14] = (unsigned char)((psc->rampDownDuration    & 0x0000FF00) >> 8*1);
    regArray[15] = (unsigned char)((psc->stimulationDuration & 0x000000FF) >> 8*0);
    regArray[16] = (unsigned char)((psc->stimulationDuration & 0x0000FF00) >> 8*1);


    _device->writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH_INFO_ADDR, regArray);
    retriesPerformed = _device->checkRegWritten(regArray, DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH_INFO_ADDR);
    if( retriesPerformed < 5 ) loggerMacroDebug("STM_REGS_CH_INFO_ADDR : Read registers OK")
    else{ loggerMacroDebug("STM_REGS_CH_INFO_ADDR : Read registers FAIL"); return false;}


    // Write Atdcs Register Set (DC WAVEFORM GENERATOR)
    // ------------------------------------------------
    auxStr.clear();
    loggerMacroDebug("Write Atdcs Register set (DC WAVEFORM GENERATOR)")
    unsigned char regAdcs[_device->getNumOfChannels()*2];
    for( int i = 0; i < _device->getNumOfChannels(); i++){

        auxStr += "CH[" + QString::number(i) + "]=";
        // Atdcs Register
        if(currentConfiguration.electrode[i].electrodeTypeAdv == STIMULATION_ADV){
            val = (65536/4)*(double)((currentConfiguration.electrode[i].Atdcs/1000.0));
        }

        regAdcs[i*2+0] = (unsigned char)((val & 0x00FF) >> 8*0);
        regAdcs[i*2+1] = (unsigned char)((val & 0xFF00) >> 8*1);

        auxStr += QString::number(currentConfiguration.electrode[i].Atdcs);
        auxStr += " LSB:" + auxSprintf.sprintf("0x%02X", regAdcs[i*2+0]) + " MSB:" + auxSprintf.sprintf("0x%02X", regAdcs[i*2+0]) + " ";
        qDebug() << auxStr;
        auxStr.clear();
    }

    QByteArray regAdcsArray;
    for( int i = 0; i < _device->getNumOfChannels()*2; i++) regAdcsArray.append( regAdcs[i] );
    _device->writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH0_DC_OFF_0_ADDR, regAdcsArray);
    retriesPerformed = _device->checkRegWritten(regAdcsArray, DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH0_DC_OFF_0_ADDR);
    if( retriesPerformed < 5 ) loggerMacroDebug("stimConfig[i].Atdcs : Read registers OK")
    else{ loggerMacroDebug("stimConfig[i].Atdcs : Read registers FAIL"); return false;}
    errorCount += retriesPerformed;
    loggerMacroDebug ( "Elapsed time " +  QString::number(myTimer.elapsed()/1000.0) );
    numRegistersWritten += _device->getNumOfChannels()*2;


    // Write Ftacs,Ptacs,Atacs Register set (Sinusoidal Waveform Generator)
    // --------------------------------------------------------------------
    auxStr.clear();
    loggerMacroDebug("Write Ftacs,Ptacs,Atacs Register set (Sinusoidal Waveform Generator)")
    unsigned char regSinusoidal[_device->getNumOfChannels()*6];
    for( int i = 0; i < _device->getNumOfChannels(); i++){

        auxStr += "CH[" + QString::number(i) + "]=";
        // Ftacs Register
        // FLOOR(Fsin(Hz)*65536/1000)
        val = floor(currentConfiguration.electrode[i].Ftacs*65536/1000+0.5);
//        regSinusoidal[0+i*6] = (unsigned char)(val%256);
//        regSinusoidal[1+i*6] = (unsigned char)(val/256);

        regSinusoidal[0+i*6] = (unsigned char)((val & 0x00FF) >> 8*0);
        regSinusoidal[1+i*6] = (unsigned char)((val & 0xFF00) >> 8*1);

        auxStr += " Ftacs:" + QString::number(currentConfiguration.electrode[i].Ftacs);
        auxStr += " (LSB:" + auxSprintf.sprintf("0x%02X", regSinusoidal[0+i*6]) + " MSB:" + auxSprintf.sprintf("0x%02X", regSinusoidal[1+i*6]) + ") ";

        // Ptacs Register (input in degrees)
        int originalValue;
        float valFloat;
        originalValue = currentConfiguration.electrode[i].Ptacs;
        valFloat = originalValue;
        valFloat = (valFloat*PI)/180; //We convert to radians

        val = floor((valFloat/PI)*32768+0.5);
        regSinusoidal[2+i*6] = (unsigned char)(val/256);
        regSinusoidal[3+i*6] = (unsigned char)(val%256);

        auxStr += " Ptacs:" + QString::number(currentConfiguration.electrode[i].Ptacs);
        auxStr += " (LSB:" + auxSprintf.sprintf("0x%02X", regSinusoidal[3+i*6]) + " MSB:" + auxSprintf.sprintf("0x%02X", regSinusoidal[2+i*6]) + ") ";

        // Atacs Register
        double aux2 = 0;
        if(currentConfiguration.electrode[i].Atacs==0){
            regSinusoidal[4+i*6]=0;
            regSinusoidal[5+i*6]=1;        
        }else{
            isTACSorTRNS=true;
            val = currentConfiguration.electrode[i].Atacs;
            aux2 = val/2000.0;

            // Added dec2frac function from Antonio
            dec2frac(aux2,&regSinusoidal[4+i*6],&regSinusoidal[5+i*6], DEC2FRAC_8BIT);
        }

        auxStr += " Atacs:" + QString::number(currentConfiguration.electrode[i].Atacs);
        auxStr += " (NUM:" + auxSprintf.sprintf("0x%02X", regSinusoidal[4+i*6]) + " DEN:" + auxSprintf.sprintf("0x%02X", regSinusoidal[5+i*6]) + ") ";
        qDebug() << auxStr;
        auxStr.clear();

    }


    // Write Registers
    QByteArray regSinusoidalArray;
    for( int i = 0; i < _device->getNumOfChannels()*6; i++) regSinusoidalArray.append( regSinusoidal[i] );
    _device->writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH0_SIN_FREQ_0_ADDR, regSinusoidalArray);


    retriesPerformed = _device->checkRegWritten(regSinusoidalArray, DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH0_SIN_FREQ_0_ADDR);
    if( retriesPerformed < 5 ) loggerMacroDebug("stimConfig[i].Ftacs,Ptacs,Atacs : Read registers OK")
    else{ loggerMacroDebug("stimConfig[i].Ftacs,Ptacs,Atacs : Read registers FAIL"); return false;}
    errorCount += retriesPerformed;
    loggerMacroDebug ( "Elapsed time " +  QString::number(myTimer.elapsed()/1000.0) );
    numRegistersWritten += _device->getNumOfChannels()*6;



    // Write Trns (GAUSSIAN WAVEFORM GENERATOR)
    // --------------------------------------------------------------------
    auxStr.clear();
    unsigned char regGaussian[_device->getNumOfChannels()*2];
    for( int i = 0; i < _device->getNumOfChannels(); i++){

        auxStr += "CH[" + QString::number(i) + "]=";
        // Atrns Register
        if(currentConfiguration.electrode[i].Atrns==0){
            regGaussian[i*2+0]=0;
            regGaussian[i*2+1]=1;
        }else{
           isTACSorTRNS=true;
           double aux2 = currentConfiguration.electrode[i].Atrns/640.0;

           if( _device->getFirmwareVersion() >= FWVERSION_TRNS_PAIRS){
               loggerMacroDebug("Configuring DEC2FRAC for 7 bit")
               //Added dec2frac function from Antonio
               dec2frac(aux2,&regGaussian[i*2+0],&regGaussian[i*2+1], DEC2FRAC_7BIT);
           }else{
               loggerMacroDebug("Configuring DEC2FRAC for 8 bit")
               //Added dec2frac function from Antonio
               dec2frac(aux2,&regGaussian[i*2+0],&regGaussian[i*2+1], DEC2FRAC_8BIT);
           } // END: FWVERSION_TRNS_PAIRS

        }
        auxStr += " Atrns:" + QString::number(currentConfiguration.electrode[i].Atrns);
        auxStr += " (NUM:" + auxSprintf.sprintf("0x%02X", regGaussian[i*2+0]) + " DEN:" + auxSprintf.sprintf("0x%02X", regGaussian[i*2+0]) + ") ";
        qDebug() << auxStr;
        auxStr.clear();
    }

    // Write Registers
    QByteArray regGaussianArray;
    for( int i = 0; i < _device->getNumOfChannels()*2; i++) regGaussianArray.append( regGaussian[i] );
    _device->writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH0_GAUSS_GAIN_N_ADDR, regGaussianArray);
    retriesPerformed = _device->checkRegWritten(regGaussianArray, DeviceManagerTypes::STIM_REGISTERS, STM_REGS_CH0_GAUSS_GAIN_N_ADDR);
    if( retriesPerformed < 5 ) loggerMacroDebug("stimConfig[i].Ftacs,Ptacs,Atacs : Read registers OK")
    else{ loggerMacroDebug("stimConfig[i].Ftacs,Ptacs,Atacs : Read registers FAIL"); return false;}
    errorCount += retriesPerformed;

    loggerMacroDebug ( "Elapsed time " +  QString::number(myTimer.elapsed()/1000.0) );
    numRegistersWritten += _device->getNumOfChannels()*2;


    qDebug()<<"Elapsed time"<<myTimer.elapsed()/1000.0;

    // Deactive filtering for TRNS
    if( _device->getFirmwareVersion() >= FWVERSION_FIR_TRNS ){
        unsigned char ch_infoFIRREG = 0x00;
        QByteArray firREGArray;
        firREGArray.append( (char) ch_infoFIRREG );
        _device->requestSync(DeviceManagerTypes::WRITE_REGISTER_REQUEST,
                         DeviceManagerTypes::STIM_REGISTERS, STM_FLT_CH_INFO_ADDR, firREGArray);

        loggerMacroDebug("Deactivating FIR filtering for TRNS");
    }


    qDebug()<<"TOTAL Elapsed time"<<myTimer.elapsed()/1000.0;
    qDebug()<<"TOTAL Write Register Errors"<<errorCount;

    return true;
}

void ProtocolManager::dec2frac(double x, uint8_t *num, uint8_t *den, VariableLength_t variable_length)
{
  uint8_t sign;
  double  xa;
  double  z;

  double  prev_den;
  double  frac_den;
  double  frac_num;
  double  scratch_value;

  uint8_t ind;


  // Depending on variable length
  double resolution;
  if ( variable_length == DEC2FRAC_8BIT ){ resolution = 0.00392156862745098; // 1/255 = 0.00392156862745098 ( num and den should fit into 8-bit variable )
  }else{ resolution = 0.00787401574803149; }// 1/127 = 0.00787401574803149 ( num and den should fit into 7-bit variable )

  uint8_t top = (variable_length == DEC2FRAC_8BIT)?255:127;

  /* Check sign */
  sign = (x < 0.0) ? -1 : 1;

  xa   = fabs(x);

  //printf("xa = %lf   -   floor(xa) = %lf\n", xa, floor(xa));


  if (xa == floor(xa)) {
    /* Integer number */
    *num = ((uint8_t) xa)*sign;
    *den = 1;
  } else if (xa < resolution) {
    /* Too low number */
    *num = sign;
    *den = top;
  } else if (xa > top) {
    *num = top*sign;
    *den = 1;
  } else {
    z = xa;

    prev_den = 0.0;
    frac_den = 1.0;

    ind = 0;
    do {
      z             = 1.0/(z - floor(z));
      scratch_value = frac_den;
      frac_den      = frac_den*floor(z) + prev_den;
      prev_den      = scratch_value;
      frac_num      = floor(xa*frac_den + 0.5);

      ind++;
    } while ((( fabs(xa - (frac_num/frac_den)) > resolution) && !(z == floor(z))) && (ind < 20));

    *num = ((uint8_t) frac_num)*sign;
    *den = (uint8_t)  frac_den;
  }
}

///////////////////////////////////////////
// Check Correctness of the stimulation
///////////////////////////////////////////

bool ProtocolManager::isProtocolSessionConfigurationCorrectWrapper(ProtocolSessionConfiguration& currentConfiguration, QString *reason){
    //bool res = isProtocolSessionConfigurationCorrect(&currentConfiguration,reason);
    StimProtocolSessionConfig::StimProtocolIssueInfo protocolIssueInfo;
    bool res = StimProtocolSessionConfig::protocolSessionConfigurationIsCorrect(currentConfiguration, 0, false, protocolIssueInfo);
    if (res == false){
        emit stimulationStatus(_currentState, 0, currentConfiguration.stimulationDuration + _stimDurationElapsedFromResume);
    }

    return res;
}


