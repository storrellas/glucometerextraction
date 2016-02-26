#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ENABLE_EASY_FILE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup user interface
    ui->setupUi(this);

    // Create objects
    deviceManager    = new DeviceManager();
    protocolManager  = new ProtocolManager(deviceManager);
    deviceStatus     = new DeviceStatus(deviceManager, protocolManager);
    fileWriter       = new FileWriter();

    // Set window title
    this->setWindowTitle(APP_NAME);


    // Connect signals and slots
    // ----------------------------------------

    // Signals&slot from application
    connect(this, SIGNAL(consoleWriteSignal(QString)),
                        this, SLOT(consoleWriteSlot(QString)));


    // Register MetaTypes
    qRegisterMetaType<ChannelData>("ChannelData");
    qRegisterMetaType<DeviceManagerTypes::DeviceStatus>("DeviceManagerTypes::DeviceStatus");
    qRegisterMetaType<StimulationState>("StimulationState");
    qRegisterMetaType<DeviceManagerTypes::DeviceType>("DeviceManagerTypes::DeviceType");

    // Connect signals&slots from deviceManager
    connect(deviceManager, SIGNAL(receivedDeviceStatus(DeviceManagerTypes::DeviceStatus)),            deviceStatus, SLOT(receivedDeviceStatus(DeviceManagerTypes::DeviceStatus)));
    connect(deviceManager, SIGNAL(receivedEEGData(ChannelData)),         this, SLOT(receivedEEGData(ChannelData)));
    connect(deviceManager, SIGNAL(receivedAccelData(ChannelData)),       this, SLOT(receivedAccelData(ChannelData)));
    connect(deviceManager, SIGNAL(receivedStimulationData(ChannelData)), this, SLOT(receivedStimulationData(ChannelData)));
    connect(deviceManager, SIGNAL(receivedImpedanceData(ChannelData)),   this, SLOT(receivedImpedanceData(ChannelData)));

    connect(deviceManager, SIGNAL(receivedFirmwareVersion(int)),         this, SLOT(receivedFirmwareVersion(int)));
    connect(deviceManager, SIGNAL(receivedProfile(DeviceManagerTypes::DeviceType,int, int,int,int,int)),
            this,SLOT(receivedProfile(DeviceManagerTypes::DeviceType,int,int,int,int,int)));

    connect(deviceManager, SIGNAL(receivedEEGData(ChannelData)),         fileWriter, SLOT(onNewData(ChannelData)));
    connect(deviceManager, SIGNAL(receivedAccelData(ChannelData)),       fileWriter, SLOT(onNewAccelerometerData(ChannelData)));
    connect(deviceManager, SIGNAL(receivedStimulationData(ChannelData)), fileWriter, SLOT(onNewStimData(ChannelData)));

    // Connect signals from protocol Manager
    connect(deviceStatus,    SIGNAL(stimulationStarted()),                        protocolManager, SLOT(stimulationStarted()));
    connect(deviceStatus,    SIGNAL(stimulationFinished(bool)),                   protocolManager, SLOT(stimulationFinished(bool)));
    connect(protocolManager, SIGNAL(stimulationStatus(StimulationState,int,int)), this, SLOT(stimulationStatus(StimulationState,int,int)) );


    // Cheating section
    // --------------------------
    //ui->ipAddressLine->setText("127.0.0.1");
    ui->ipAddressLine->setText("192.168.1.1");
    ui->netmaskLine->setText("255.255.255.0");
    ui->gatewayLine->setText("192.168.1.1");
//    ui->ipAddressLine->setText("150.150.1.1");
//    ui->bdAddressLine->setText("150.150.1.255");

    //ui->accelerometerCheckBox->setChecked(true);
    ui->ssidLine->setText("belkin54g");
    ui->passwordLine->setText("df2starlabdf2");
    ui->macLine->setText("00:07:80:27:77:77");
    // --------------------------

    // Initialise _monitorThread
    monitorThread = new QThread();
    protocolManager->moveToThread(monitorThread);
    deviceManager->moveToThread(monitorThread);
    monitorThread->start();

    // NOTE: It is better to put DeviceStatus class
    // in a separated thread
    statusThread = new QThread();
    deviceStatus->moveToThread(statusThread);
    statusThread->start();

}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    loggerMacroDebug("Close event")

}


void MainWindow::initialize(){
    loggerMacroDebug("Log from Initialize")

    loggerMacroDebug("Current thread->");
    qDebug() << QThread::currentThreadId();


    // Configure File Writer
    loggerMacroDebug("Configure easy file")
    if ( fileWriter->setOutDir(".") == false){
        loggerMacroDebug("Error setting out dir")
    }
    fileWriter->setRecordingEASYFile(true);
    fileWriter->setRecordingSTIMFile(true);
    fileWriter->setRecordingNEDFFile(false);

    // Value of address
    QHostAddress mask = QHostAddress("255.255.255.0");
    QHostAddress addressHost(ui->ipAddressLine->text());
    qint32 broadcastAddressInt = addressHost.toIPv4Address() | (~mask.toIPv4Address());
    loggerMacroDebug("Current broadcast addresss ->" + QHostAddress(broadcastAddressInt).toString())

}

void MainWindow::on_clearConsoleBtn_clicked()
{
    ui->consoleTextEdit->clear();
}

void MainWindow::consoleWrite(const QString &line){
    emit consoleWriteSignal(line);
}

void MainWindow::consoleWriteSlot(const QString &line){
    QCoreApplication::processEvents();
    ui->consoleTextEdit->appendPlainText(line);
}


////////////////////////////////////////////
// Device Configuration Operations
////////////////////////////////////////////

void MainWindow::on_searchStarstimBtn_clicked()
{
    // Calculating broadcast address
    QHostAddress mask = QHostAddress(ui->netmaskLine->text());
    QHostAddress addressHost(ui->ipAddressLine->text());
    qint32 broadcastAddressInt = addressHost.toIPv4Address() | (~mask.toIPv4Address());
    loggerMacroDebug("Current broadcast addresss ->" + QHostAddress(broadcastAddressInt).toString())
    QHostAddress bdAddress = QHostAddress(broadcastAddressInt);

    loggerMacroDebug("Searching device ...")
    QHostAddress deviceAddress;
    bool found = deviceConfiguration.find( bdAddress, deviceAddress );
    loggerMacroDebug("Done!")

    if( found ){
        loggerMacroDebug("Device found at " + deviceAddress.toString())
        ui->ipAddressLine->setText(deviceAddress.toString());
    }else{
        loggerMacroDebug("Device was NOT found")
    }


}

void MainWindow::on_configureInfrasBtn_clicked()
{
    loggerMacroDebug("Configure infrastructure")

    // Configure infrastructure
    QHostAddress deviceAddress = QHostAddress(ui->ipAddressLine->text());
    QString ssid = ui->ssidLine->text();
    QString password = ui->passwordLine->text();
    bool result = deviceConfiguration.setWifiConfiguration(deviceAddress, ssid, password);

    // Log result
    if( result ){
        loggerMacroDebug("Network configuration OK!")
    }else{
        loggerMacroDebug("Network configuration FAIL!")
    }
}


void MainWindow::on_setModeBtn_clicked()
{
    bool result = false;
    QString mode = ui->modeComboBox->currentText();
    QHostAddress deviceAddress = QHostAddress(ui->ipAddressLine->text());
    if( mode == "Access Point" ){
        loggerMacroDebug("Selecting access point mode")
        result = deviceConfiguration.setMode(deviceAddress, DeviceConfiguration::ACCESS_POINT);
    }
    if( mode == "Infrastructure DHCP" ){
        loggerMacroDebug("Selecting infrastructure DHCP")
        result = deviceConfiguration.setMode(deviceAddress, DeviceConfiguration::INFRASTRUCTURE_DHCP);
    }

    if( mode == "Infrastructure Static" ){
        loggerMacroDebug("Selecting infrastructure Static")
        result = deviceConfiguration.setMode(deviceAddress, DeviceConfiguration::INFRASTRUCTURE_STATIC);
    }

    // Log result
    if( result ) loggerMacroDebug("Set mode OK!")
    else loggerMacroDebug("Set mode Fail!")


}


void MainWindow::on_staticBtn_clicked()
{
    loggerMacroDebug("Configuring static paramters")
    QHostAddress deviceAddress = QHostAddress(ui->ipAddressLine->text());
    QHostAddress maskAddress   = QHostAddress(ui->netmaskLine->text());
    QHostAddress gwAddress   = QHostAddress(ui->gatewayLine->text());
    bool result = deviceConfiguration.setStatic(deviceAddress,
                                                QHostAddress("192.168.2.100"), maskAddress, gwAddress);

    // Log result
    if( result ) loggerMacroDebug("Set static params OK!")
    else loggerMacroDebug("Set static params Fail!")
}

void MainWindow::on_getMACBtn_clicked()
{

    loggerMacroDebug("Getting Serial Number from device ...")

    QHostAddress deviceAddress = QHostAddress(ui->ipAddressLine->text());
    QString macAddress = "";
    bool result = deviceConfiguration.getMACAddress(deviceAddress, macAddress);

    // Log result
    if( result ){
        loggerMacroDebug("MAC captured " + macAddress)
    }else{
        loggerMacroDebug("MAC capture FAIL!")
    }

}

void MainWindow::on_setMACBtn_clicked()
{
    QString macAddressStr = ui->macLine->text();
    loggerMacroDebug("Setting MAC to device " + macAddressStr + "...")

    QHostAddress deviceAddress = QHostAddress(ui->ipAddressLine->text());
    bool result = deviceConfiguration.setMACAddress(deviceAddress, macAddressStr);

    // Log result
    if( result ){
        loggerMacroDebug("MAC set was OK " + macAddressStr)
    }else{
        loggerMacroDebug("MAC set FAIL!")
    }

}

////////////////////////////////////////////
// Starstim Operations
////////////////////////////////////////////

void MainWindow::on_executeOperation_btn_clicked()
{
    // Call specific methods
    QString operation = ui->commandComboBox->currentText();
    if( operation == "Open Device")            this->openDevice();
    if( operation == "Launch Stimulation" )    this->launchStimulation();
    if( operation == "Start Streaming" )       this->startStreaming();
    if( operation == "Stop Streaming" )        this->stopStreaming();
    if( operation == "Close Device" )          this->closeDevice();
    if( operation == "Enable SDCard" )         this->enableSDCard();

}

void MainWindow::openDevice(){
    loggerMacroDebug("clicked OpenDevice")


    int port = 10000;
    QString ip = ui->ipAddressLine->text();
    bool res = deviceManager->openDevice((const char*) ip.toLocal8Bit().data(), port);
    if( ! res ){
        loggerMacroDebug("Device was not opened")
        return;
    }


    // Leave some time to allow receivedProfile to print out result to log
    QEventLoop loop;
    QTimer::singleShot(100, &loop, SLOT(quit()));
    loop.exec();

/////////////////////////////////////
//    return;

/////////////////////////////
    // Initialise registers
    deviceManager->initRegisters(false);

    // Configure number of channels in fileWriter
    fileWriter->setNumOfChannels(deviceManager->getNumOfChannels());

    // Configure reporting for STM_DATA
    if( deviceManager->getDeviceType() == DeviceManagerTypes::STARSTIM ){

        unsigned char one = 1;
        loggerMacroDebug("Warning: Sending STIM data")
        QByteArray regArray;
        regArray.append((char) one);
        bool res = deviceManager->writeRegister(DeviceManagerTypes::STIM_REGISTERS, STM_REPORT_DATA_ADDR, regArray);
        if( ! res ){
            loggerMacroDebug("Error writing register")
        }
    }

    // Modify sample rate
    loggerMacroDebug("Decreasing sample rate to 500SPS")
    deviceManager->setSampleRate(DeviceManagerTypes::_500_SPS_);

}

void MainWindow::closeDevice(){
    loggerMacroDebug("clicked closeDevice")

    // Pre-conditions
    if( deviceManager->getDeviceStatus().CONNECTED == false ){
        loggerMacroDebug("Device is not connected!")
        return;
    }

#ifdef ENABLE_EASY_FILE
    // Starwriting
    loggerMacroDebug("Stop Writing easy file")
    fileWriter->stopWriting();
#endif

    // Closes the current device
    deviceManager->closeDevice();



}

void MainWindow::startStreaming(){

/*
    loggerMacroDebug("Starting impedance measurement")
    deviceManager->startImpedanceMeasurement();
    return;
/**/
/*
// Example of other requests
    deviceManager->profileRequest();
    deviceManager->doNullRequest();
    return;
/**/
/*
// EXAMPLE OF WRITE ARRAY
    loggerMacroDebug("Writing EEG register")
    unsigned char value[] = {0x01, 0x20};
    QByteArray array = QByteArray::fromRawData((const char*) value, 2);
    deviceManager->writeRegister(DeviceManagerTypes::EEG_REGISTERS, 0, array);
//    deviceManager->writeRegister(DeviceManagerTypes::EEG_REGISTERS, 0, array);
//    deviceManager->writeRegister(DeviceManagerTypes::STIM_REGISTERS, 0x00, array);
//    deviceManager->writeRegister(DeviceManagerTypes::ACCEL_REGISTERS, 0x00, array);
//    deviceManager->writeRegister(DeviceManagerTypes::SDCARD_REGISTERS, 280, array);
    return;
/**/
    // Start Writing
#ifdef ENABLE_EASY_FILE
    loggerMacroDebug("Start Writing easy file")
    fileWriter->startWriting();
#endif

    loggerMacroDebug("Launching startStreaming")
    bool res = deviceManager->startStreaming();

    // Start accelerometer if required
    if( ui->accelerometerCheckBox->isChecked() ){
        loggerMacroDebug("Starting accelerometer")
        deviceManager->startAccelerometer();
        fileWriter->setRecordingAccelerometer(true);
    }
}

void MainWindow::stopStreaming(){
    loggerMacroDebug("clicked stopStreaming")
/*
    loggerMacroDebug("Stop impedance measurement")
    deviceManager->stopImpedanceMeasurement();
    return;
/**/

/*
// EXAMPLE OF READ ARRAY
    QByteArray read_array;
    deviceManager->readRegister(DeviceManagerTypes::EEG_REGISTERS, 0, read_array, 2);
//    deviceManager->readRegister(DeviceManagerTypes::EEG_REGISTERS, 0, read_array, 2);
//    deviceManager->readRegister(DeviceManagerTypes::STIM_REGISTERS, 0x00, read_array, 2);
//    deviceManager->readRegister(DeviceManagerTypes::ACCEL_REGISTERS, 0x00, read_array, 2);
//    deviceManager->readRegister(DeviceManagerTypes::SDCARD_REGISTERS, 280, read_array, 2);
    QString str, aux;
    for (int i = 0; i < read_array.size(); i++ ){
        int test = read_array[i];
        str += aux.sprintf("0x%02X ", test);
    }
    loggerMacroDebug("Read array-> "  + str);
    return;
/**/

#ifdef ENABLE_EASY_FILE
    // Starwriting
    loggerMacroDebug("Stop Writing easy file")
    fileWriter->stopWriting();
#endif


    // Stops accelerometer Streaming
    if( deviceManager->getDeviceStatus().ACCEL == true ){

        bool res = deviceManager->stopAccelerometer();
        if( res ){ loggerMacroDebug("Stop Streaming successful");
        }else{ loggerMacroDebug("Error Stopping streaming");}
        fileWriter->setRecordingAccelerometer (false);
    }

    loggerMacroDebug("Launching stopStreaming")
    bool res = deviceManager->stopStreaming();



}

void MainWindow::launchStimulation(){
    loggerMacroDebug("clicked launchStimulation")

    // Start Writing
#ifdef ENABLE_EASY_FILE
    loggerMacroDebug("Start Writing easy file")
    fileWriter->startWriting();
    fileWriter->startWritingStimData();
#endif

// -----------------------------------------------
// NEW SIMPLEMANAGER Current Configuration
// -----------------------------------------------

    loggerMacroDebug("Configuring stimulation ...")

    // TEMPLATE - PA001	Stimulation Anodal tACS 1000mA - 1 electrodes
    // --------------------------------------------------------------------------
    ProtocolSessionConfiguration currentConfiguration;

    currentConfiguration.isSham = false;
    currentConfiguration.protocolType = ADVANCED;

    // Configure stimulation duration
    currentConfiguration.stimulationDuration = 7;
    currentConfiguration.rampDownDuration = 2;
    currentConfiguration.rampUpDuration = 3;


    for(int i = 0; i < deviceManager->getNumOfChannels(); i++){
        currentConfiguration.electrode[i].electrodeTypeAdv = EEG_RECORDING_ADV;
    }

    // Channel 0
    currentConfiguration.electrode[0].electrodeTypeAdv = STIMULATION_ADV;  // Return Electrode - Cathodal
    currentConfiguration.electrode[0].Atdcs = 0;
    currentConfiguration.electrode[0].Atacs = 1000;
    currentConfiguration.electrode[0].Ftacs = 1;
    currentConfiguration.electrode[0].Ptacs = 0;
    currentConfiguration.electrode[0].Atrns = 0;


    // Channel 1
    currentConfiguration.electrode[1].electrodeTypeAdv = RETURN_ADV;  // Stimulation Electrode - Anodal
    currentConfiguration.electrode[1].Atdcs = 0;
    currentConfiguration.electrode[1].Atacs = 0;
    currentConfiguration.electrode[1].Ftacs = 0;
    currentConfiguration.electrode[1].Ptacs = 0;
    currentConfiguration.electrode[1].Atrns = 0;


    // Channel 2
    currentConfiguration.electrode[2].electrodeTypeAdv = NOT_USED_ADV;  // EEG Measuring
    currentConfiguration.electrode[2].Atdcs = 0;
    currentConfiguration.electrode[2].Atacs = 0;
    currentConfiguration.electrode[2].Ftacs = 0;
    currentConfiguration.electrode[2].Ptacs = 0;
    currentConfiguration.electrode[2].Atrns = 0;


    // Channel 3
    currentConfiguration.electrode[3].electrodeTypeAdv = NOT_USED_ADV;  // EEG Measuring
    currentConfiguration.electrode[3].Atdcs = 0;
    currentConfiguration.electrode[3].Atacs = 0;
    currentConfiguration.electrode[3].Ftacs = 0;
    currentConfiguration.electrode[3].Ptacs = 0;
    currentConfiguration.electrode[3].Atrns = 0;


    // Channel 4
    currentConfiguration.electrode[4].electrodeTypeAdv = NOT_USED_ADV;  // EEG Measuring
    currentConfiguration.electrode[4].Atdcs = 0;
    currentConfiguration.electrode[4].Atacs = 1000;
    currentConfiguration.electrode[4].Ftacs = 1;
    currentConfiguration.electrode[4].Ptacs = 0;
    currentConfiguration.electrode[4].Atrns = 0;


    // Channel 5
    currentConfiguration.electrode[5].electrodeTypeAdv = NOT_USED_ADV;  // EEG Measuring
    currentConfiguration.electrode[5].Atdcs = 0;
    currentConfiguration.electrode[5].Atacs = 0;
    currentConfiguration.electrode[5].Ftacs = 0;
    currentConfiguration.electrode[5].Ptacs = 0;
    currentConfiguration.electrode[5].Atrns = 0;


    // Channel 6
    currentConfiguration.electrode[6].electrodeTypeAdv = NOT_USED_ADV;  // EEG Measuring
    currentConfiguration.electrode[6].Atdcs = 0;
    currentConfiguration.electrode[6].Atacs = 0;
    currentConfiguration.electrode[6].Ftacs = 0;
    currentConfiguration.electrode[6].Ptacs = 0;
    currentConfiguration.electrode[6].Atrns = 0;


    // Channel 7
    currentConfiguration.electrode[7].electrodeTypeAdv = NOT_USED_ADV;  // EEG Measuring
    currentConfiguration.electrode[7].Atdcs = 0;
    currentConfiguration.electrode[7].Atacs = 0;
    currentConfiguration.electrode[7].Ftacs = 0;
    currentConfiguration.electrode[7].Ptacs = 0;
    currentConfiguration.electrode[7].Atrns = 0;
/**/
/*
    // Channel 30
    currentConfiguration.electrode[30].electrodeTypeAdv = STIMULATION_ADV;  // Return Electrode - Cathodal
    currentConfiguration.electrode[30].Atdcs = 500;
    currentConfiguration.electrode[30].Atacs = 0;
    currentConfiguration.electrode[30].Ftacs = 0;
    currentConfiguration.electrode[30].Ptacs = 0;
    currentConfiguration.electrode[30].Atrns = 0;

    // Channel 31
    currentConfiguration.electrode[31].electrodeTypeAdv = RETURN_ADV;  // EEG Measuring
    currentConfiguration.electrode[31].Atdcs = 0;
    currentConfiguration.electrode[31].Atacs = 0;
    currentConfiguration.electrode[31].Ftacs = 0;
    currentConfiguration.electrode[31].Ptacs = 0;
    currentConfiguration.electrode[31].Atrns = 0;
/**/
// ---------------------------------------


    // Create Fake StimSession for compatibility with NICHome
    StimSession stimSession;
    stimSession.sessionId = 0;
    stimSession.percentageCompleted = 0;




    // Start the stimulation
    bool res = protocolManager->startStimulationProtocol(stimSession, currentConfiguration);
    if( !res ) loggerMacroDebug("Error starting stimulation")

}

void MainWindow::enableSDCard(){
    loggerMacroDebug("clicked enableSDCard")
}

////////////////////////////////////////////
// Starstim Slots
////////////////////////////////////////////


void MainWindow::receivedProfile(DeviceManagerTypes::DeviceType deviceType, int n_channel,
                         int batteryLevel, int firmwareVersion, int t1, int t2){

    loggerMacroDebug("Received DeviceType  -> " + QString((deviceType==DeviceManagerTypes::ENOBIO)?"ENOBIO":"STARSTIM"));
    loggerMacroDebug("Received Channel Number  -> " + QString::number(n_channel));
    loggerMacroDebug("Received Battery Level -> "+ QString::number(batteryLevel))
    loggerMacroDebug("Received Firmware version -> "+ QString::number(firmwareVersion))
    loggerMacroDebug("Received synchro -> "+ QString::number(t1) + " " + QString::number(t2))
}



void MainWindow::receivedEEGData(ChannelData data){
    QMutexLocker locker(&_mutex);
//    QtMessageHandler msgHandler = qInstallMessageHandler(0);
    qInstallMessageHandler(nicMessageHandler);

    static int eeg_counter = 0;    
    if( (eeg_counter % 300) == 0){
        loggerMacroDebug("New EEG Data with timestamp " + QString::number(data.timestamp()) + " Total EEG Data:" + QString::number(eeg_counter) + " samples")
    }
    eeg_counter ++;


//    qInstallMessageHandler(msgHandler);
    qInstallMessageHandler(nicMessageHandlerVisual);
}

void MainWindow::receivedAccelData(ChannelData data){
    QMutexLocker locker(&_mutex);
    //QtMessageHandler msgHandler = qInstallMessageHandler(0);
    qInstallMessageHandler(nicMessageHandler);

    static int accel_counter = 0;
    if( (accel_counter % 300) == 0){
        loggerMacroDebug("New AccelData with timestamp " + QString::number(data.timestamp()) + " Total ACCEL Data:" + QString::number(accel_counter) + " samples")
    }
    accel_counter ++;


//    qInstallMessageHandler(msgHandler);
    qInstallMessageHandler(nicMessageHandlerVisual);
}

void MainWindow::receivedStimulationData(ChannelData data){
    QMutexLocker locker(&_mutex);
    //QtMessageHandler msgHandler = qInstallMessageHandler(0);
    qInstallMessageHandler(nicMessageHandler);
    //loggerMacroDebug("new stimulation data at " + QString::number(data.timestamp()))
    //loggerMacroDebug("Sample[0]" + QString::number(data.data()[0]))
    static int stim_counter = 0;
    if( (stim_counter % 1000) == 0){
        loggerMacroDebug("New STM Data with timestamp " + QString::number(data.timestamp()) + " Total STM Data:" + QString::number(stim_counter) + " samples")
    }
    stim_counter ++;
//    qInstallMessageHandler(msgHandler);
    qInstallMessageHandler(nicMessageHandlerVisual);
}

void MainWindow::receivedFirmwareVersion(int firmwareVersion){

    loggerMacroDebug("Current firmware version is " + QString::number(firmwareVersion))

    loggerMacroDebug("Device type ->" + deviceManager->deviceType2String())
}

void MainWindow::receivedImpedanceData(ChannelData data){
    QMutexLocker locker(&_mutex);
    //QtMessageHandler msgHandler = qInstallMessageHandler(0);
    qInstallMessageHandler(nicMessageHandler);
    loggerMacroDebug("New impedance data at " + QString::number(data.timestamp()))

//    qInstallMessageHandler(msgHandler);
    qInstallMessageHandler(nicMessageHandlerVisual);
}


//////////////////////////////////
// Protocol Manager Slots
//////////////////////////////////

void MainWindow::stimulationStatus(StimulationState state, int stimulationProgress, int stimulationDuration){

    loggerMacroDebug("Stimulation CurrentState -> " + QString::number(state) + " "
                     + QString::number(stimulationProgress) + " elapsed from " + QString::number(stimulationDuration))

    switch(state){
        case STIM_RAMPUP:
            // Do nothing
            break;

        case STIM_STIMULATION:
//            if( deviceManager->getDeviceStatus().EEG == false ){
//                loggerMacroDebug("Entering in STIM_STIMULATION. Starting streaming")
//                deviceManager->startStreaming();
//            }
            break;
        case STIM_RAMPDOWN:
//            if( deviceManager->getDeviceStatus().EEG == true ){
//                loggerMacroDebug("Entering in STIM_STOPPED. Stopping streaming")
//                deviceManager->stopStreaming();
//            }
            break;

        case STIM_STOPPED:
            loggerMacroDebug("Stimulation is stopped!")
            fileWriter->stopWriting();
            break;
        default:
            break;
    }

}

//////////////////////////////////
// Production test environment
//////////////////////////////////


void MainWindow::on_flashWF121Button_clicked()
{
    loggerMacroDebug("Flashing firmware into WF121 ...")

    // 1. Read WF121 firmware
    // ------------------------------
    loggerMacroDebug("Reading firmware from device ...")
    QString fwFilename = "wf121-firmware-read.hex";
    QProcess* process = new QProcess(this);
    QString icd3Command = "\"C:\\Program Files (x86)\\Microchip\\MPLAB IDE\\Programmer Utilities\\ICD3\\ICD3CMD.EXE\" ";
    QString command = icd3Command  + " " + QString("/P32MX695F512H /GF") + QDir::currentPath() + "/" + fwFilename;

    loggerMacroDebug( command );
    process->start( command );
    if( !process->waitForStarted(-1) ){
        loggerMacroDebug("process did not start")
    }

    process->waitForFinished();
    QString strOut = process->readAllStandardOutput();
    QString strErr = process->readAllStandardError();
    loggerMacroDebug("output -> " + strOut)
    if( strOut.contains("Read Complete") ){
        loggerMacroDebug("Successfully read fw!")
    }else{
        loggerMacroDebug("Failed to read fw")
        return;
    }


    // 2. Read WF121 to search MAC Address
    // ------------------------------
    // Followed : https://bluegiga.zendesk.com/entries/94187278--HOW-TO-Retain-MAC-address-on-WF121-module-when-reflashing-via-PICkit3-ICSP
    QFile file(fwFilename);
    if(!file.open(QIODevice::ReadOnly)) {
        loggerMacroDebug("Error while opening file")
    }

    int line_count = 1;
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        // We need to get to line 32266 and extract MAC Address
        if( line_count == 32266 ){
            QString content = line.mid(9, line.length() - 9);
            QString macAddressRaw = content.mid(8, 12);
            QString macAddress = macAddressRaw.mid(0,2) + ":" +
                                 macAddressRaw.mid(2,2) + ":" +
                                 macAddressRaw.mid(4,2) + ":" +
                                 macAddressRaw.mid(6,2) + ":" +
                                 macAddressRaw.mid(8,2) + ":" +
                                 macAddressRaw.mid(10,2);
            loggerMacroDebug("MAC Address " + macAddress)
            break;
        }
        line_count ++;
    }

    file.close();
/**/

}


