#include "devicestatus.h"

DeviceStatus::DeviceStatus(DeviceManager* device, ProtocolManager* protocolManager) :
    _protocolManager(protocolManager),
    _device(device)
{
}


DeviceStatus::~DeviceStatus(){
}

void DeviceStatus::receivedDeviceStatus(DeviceManagerTypes::DeviceStatus status){

    // This variable is initialised once and keeps its value during Manager lifetime
    static bool _hasFirstStimulationBeenStarted = false;
    static bool _stimulationStarted = false;

    // Avoids collision with others when logging
    QEventLoop loop;
    QTimer::singleShot(10, &loop, SLOT(quit()));
    loop.exec();


    loggerMacroDebug("EEG-"   + QString(status.EEG?"ON":"OFF"))
    loggerMacroDebug("ACCEL-" + QString(status.ACCEL?"ON":"OFF"))
    loggerMacroDebug("STIM-"  + QString(status.STIM?"ON":"OFF"))
    loggerMacroDebug("IMP-"   + QString(status.IMP?"ON":"OFF"))

    QtMessageHandler msgHandler = qInstallMessageHandler(0);    // Actions to be committed if stimulating
    if ( status.STIM ){

        if( _stimulationStarted == false){
            loggerMacroDebug("Stimulation started!")
            emit stimulationStarted();
            _stimulationStarted = true;
        }

    }else{

        _protocolManager->setCheckingImpedance(false);
        if( _stimulationStarted == true){
            loggerMacroDebug("Stimulation finished!")
            emit stimulationFinished(false);
            _stimulationStarted = false;
        }

    }
/**/

    qInstallMessageHandler(msgHandler);
}

void DeviceStatus::onDeviceStatus(int status){
/*
    // This variable is initialised once and keeps its value during Manager lifetime
    static bool _hasFirstStimulationBeenStarted = false;
    static bool _stimulationStarted = false;

    if (status == DeviceManagerTypes::DEVICESTATUS_UNKNOWN){
        loggerMacroDebug("Device status UNKNOWN")
        //_isReceiverPresent = false;
        //emit DeviceIsON(false);
    }


    // Check whether device is Streaming
    if (status & DeviceManagerTypes::EEG_BIT){ qDebug() << "EEG-ON";
    }else{ qDebug() << "EEG-OFF"; }

    // Check for impedance
    if (status & DeviceManagerTypes::IMP_BIT) qDebug() << "IMP-ON";
    else qDebug() << "IMP-OFF";


    // Check for SDCard
    if (status & DeviceManagerTypes::SDCARD_BIT) qDebug() << "SDCARD-ON";
    else qDebug() << "SDCARD-OFF";


    if (status & DeviceManagerTypes::STM_BIT){
        qDebug() << "STM-ON";

        if( _stimulationStarted == false){
            loggerMacroDebug("Stimulation has started!")
            _protocolManager->stimulationStarted();
            _stimulationStarted = true;
        }

        _hasFirstStimulationBeenStarted = true;
    }
    else
    {
        qDebug() << "STM-OFF";


        bool aborted=false;

        if( _hasFirstStimulationBeenStarted ){

            if (_device->getDeviceStimulating())
            {
                //We read register 114 to check the reason for the stop
                unsigned char stimResult=0;
                _device->readRegister(DeviceManagerTypes::STIM_REGISTERS, STM_ERROR_ADDR, &stimResult);
                qDebug()<<"Register 114 is"<<stimResult;
                if (stimResult==1)
                {
                    //qDebug()<<"Stimulation was aborted due to an impedance problem.";
//                    emit newMessage(5, "Stimulation was aborted by the NECBOX due to an impedance problem.");
                    aborted=true;
                }
                else if (stimResult==2)
                {
                    //qDebug()<<"Stimulation was aborted by StarStim due to a communication problem.";
//                    emit newMessage(5, "Stimulation was aborted by the NECBOX due to a communication problem.");
                    aborted=true;
                }
//                else if (stimResult==3)
//                {
//                    //qDebug()<<"Stimulation was aborted due to a stop command sent from NIC.";
//                    emit newMessage(5, "Stimulation was aborted due to a stop command sent from NIC.");
//                }

//                emit stimulationFinishedStatus(stimResult);
            }

        }



        // Operations after stimulation is finished
        // ----------------------------------------
        if (_device->getDeviceStimulating()){
            _protocolManager->setCheckingImpedance(false);
            _protocolManager->stimulationFinished(aborted);
            _device->setDeviceStimulating(false);
            _stimulationStarted = false;
        }


    }


/**/


}

