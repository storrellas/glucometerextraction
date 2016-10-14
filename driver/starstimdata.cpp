#include "icognosdata.h"


const int StarstimData::NumEEGConfigReg           = EEG_NUM_REGS;
const int StarstimData::NumStimConfigReg          = STM_NUM_REGS;
const int StarstimData::NumAccelerometerConfigReg = ACCEL_NUM_REGS;
const int StarstimData::NumSDCardConfigReg        = SDCARD_NUM_REGS;



StarstimData::StarstimData()
{
    empty();
}

void StarstimData::empty()
{
    _isEEGDataPresent = false;
    _isRegConfigPresent = false;
    _isStimDataPresent = false;
    _isStimImpedancePresent = false;
    _isAccelDataPresent = false;
    _isFirmwareVersionPresent = false;
    _deviceStatus = 0;
    _deviceError = 0;
    _isCommandToggled = false;
    _isSDCardRecording = false;
    //_statusByte1 = 0;
    //_statusByte0 = 0;
    for (int i = 0; i < 32; i++)
    {
        _isEEGChPresent[i] = 0;
    }
    _eegStamp = 0;
    _eegStartAddress = 0;
    _eegNumRegs = 0;
    for (int i = 0; i < NumEEGConfigReg; i++)
    {
        _confReg[i] = 0;
    }
    //_stimChInfo = 0;
    _stimData.setChannelInfo(0);
    for (int i = 0; i < 8; i++)
    {
        //_stimData[i] = 0;
        _stimData.data()[i] = 0;
        _isStimChPresent[i] = 0;
    }
    _stimStartAddress = 0;
    _stimNumRegs = 0;
    for (int i = 0; i < NumStimConfigReg; i++)
    {
        _stimReg[i] = 0;
    }
    //_stimImpedanceChInfo = 0;
    _stimImpedance.setChannelInfo(0);
    for (int i = 0; i < 8; i++)
    {
        _stimImpedance.data()[i] = 0;
        _isStimImpedanceChPresent[i] = 0;
    }

    for (int i = 1; i < 4; i++)
        _accelerometer.data()[i] = 0;

    _firmwareVersion = 0;
    _battery = 0;

    _synchT1 = 0;
    _synchT2 = 0;
}

unsigned int StarstimData::eegChInfo()
{
    // NOTE: All elements in _eegDataArray.channelInfo() should be equal
    return _eegDataArray[0].channelInfo();

}

void StarstimData::isCommandToggled(bool value)
{
    _isCommandToggled = value;
}

void StarstimData::isSDCardRecording(bool value)
{
    _isSDCardRecording = value;

    if (_isSDCardRecording)
        _isSDCardRecording = value;

    //_statusByte1 |= (value ? 0x02 : 0x00);
}

void StarstimData::isEEGChPresent(int index, bool value)
{
    if (index >= 8) index = 7;
    _isEEGChPresent[index] = value;
}

void StarstimData::isStimChPresent(int index, bool value)
{
    if (index >= 8) index = 7;
    _isStimChPresent[index] = value;
}

void StarstimData::isStimImpedanceChPresent(int index, bool value)
{
    if (index >= 8) index = 7;
    _isStimImpedanceChPresent[index] = value;
}


void StarstimData::eegChInfo(unsigned int value)
{
    for(int i = 0; i < _eegDataArray.count(); i++)
    {
        _eegDataArray[i].setChannelInfo(value);
    }


    for (int i = 0; i < 32; i++)
    {
        _isEEGChPresent[i] = ((value & (1 << i)) > 0);
    }



}

void StarstimData::eegData(int index, int value, int sample, int eegCompressionType)
{
    if (index >= 32) index = 31;
    if (sample>_eegDataArray.size())
    {
        qDebug()<<"StarStimData::eegData Error in _eegDataArray size()";
        return;
    }
    _eegDataArray[sample].data()[index] = value;

    // These values indicated in the conditional statment "if" are
    // set by the FW when there is overflow in compression
    switch(eegCompressionType){
        case 0: // EEG_NO_COMPRESSION
            break;
        case 1: // EEG_16BIT_COMPRESSION
             if( value == 0x7FFF || value == 0x8000){

                //qDebug() << "marked Overflow -> sample[" << sample << "] index[" << index << "]";
                this->_eegDataArray[sample].compressionOverflow()[index] = true;
            }
             break;
        case 2: // EEG_12BIT_COMPRESSION
            if( value == 0x07FF || value == 0x0800){
                this->_eegDataArray[sample].compressionOverflow()[index] = true;
                //qDebug() << "marked Overflow -> sample[" << sample << "] index[" << index << "]";
            }

            break;
    }

}

void StarstimData::nSamples(int value){
    this->_nSamples = value;

    _eegDataArray.clear();
    ChannelData channelData;
    channelData.setChannelInfo(0);
    for (int i = 0; i < 32; i++)
    {
        channelData.data()[i] = 0;
        channelData.compressionOverflow()[i] = false;
    }


    for(int i = 0; i< _nSamples; i++){
        _eegDataArray.append(channelData);
    }

}



void StarstimData::confReg(int index, unsigned char value)
{
    if (index >= NumEEGConfigReg) index = NumEEGConfigReg - 1;
    _confReg[index] = value;
}

void StarstimData::stimChInfo(unsigned int value)
{
    //_stimChInfo = value;
    _stimData.setChannelInfo(value);
    for (int i = 0; i < 8; i++)
    {
        _isStimChPresent[i] = ((value & (1 << i)) > 0);
    }
}

void StarstimData::stimImpedanceChInfo(unsigned int value)
{
    //_stimImpedanceChInfo = value;
    _stimImpedance.setChannelInfo(value);
    for (int i = 0; i < 8; i++)
    {
        _isStimImpedanceChPresent[i] = ((value & (1 << i)) > 0);
    }
}

void StarstimData::stimData(int index, int value)
{
    if (index >= NUM_STIM_CHANNELS) index = NUM_STIM_CHANNELS-1;
    //_stimData[index] = value;
    _stimData.setData(index, value);
}

void StarstimData::stimImpedance(int index, int value)
{    
    if (index >= NUM_STIM_CHANNELS) index = NUM_STIM_CHANNELS-1;
 //   _stimImpedance.data()[index] = value;
    _stimImpedance.setData(index, value);
}

void StarstimData::stimReg(int index, unsigned char value)
{
    if (index >= NumStimConfigReg) index = NumStimConfigReg - 1;
    _stimReg[index] = value;
}

void StarstimData::accelerometer(int index, int value)
{
    //Conversion to mg
    int convertedValue=value*3.9;

    //qDebug()<<"StarStimData::accelerometer"<<value<<convertedValue<<convertedValue*9.80665;

    //Conversion to mm/s^2
    convertedValue=convertedValue*9.80665;

    _accelerometer.data()[index] = convertedValue;
}


bool StarstimData::compare (StarstimData& ssd1, StarstimData& ssd2)
{
//    if (ssd1.isCommandToggled() != ssd2.isCommandToggled())
//        return false;
//    if (ssd1.deviceStatus() != ssd2.deviceStatus())
//        return false;
//    if (ssd1.deviceError() != ssd2.deviceError())
//        return false;
//    if (ssd1.isEEGDataPresent() != ssd2.isEEGDataPresent())
//        return false;
//    if (ssd1.isEEGConfigPresent() != ssd2.isEEGConfigPresent())
//        return false;
//    if (ssd1.isStimDataPresent() != ssd2.isStimDataPresent())
//        return false;
//    if (ssd1.isStimConfigPresent() != ssd2.isStimConfigPresent())
//        return false;
//    if (ssd1.isStimImpedancePresent() != ssd2.isStimImpedancePresent())
//        return false;
//    if (ssd1.isAccelDataPresent() != ssd2.isAccelDataPresent())
//        return false;
//    if (ssd1.isProfilePresent() != ssd2.isProfilePresent())
//        return false;
//    if (ssd1.eegChInfo() != ssd2.eegChInfo())
//        return false;
//    for (int i = 0; i < 8; i++)
//    {
//        if (ssd1.isEEGChPresent()[i] != ssd2.isEEGChPresent()[i])
//            return false;
//    }

//    if(ssd1._eegDataArray.count() != ssd2._eegDataArray.count()){
//        return false;
//    }else{
//        for( int j = 0; j < ssd1._eegDataArray.count(); j++){

//            for (int i = 0; i < 8; i++)
//            {
//                if (ssd1.eegDataArray()[j].data()[i] != ssd2.eegDataArray()[j].data()[i])
//                    return false;
//            }

//        }
//    }

//    if (ssd1.eegStamp() != ssd2.eegStamp())
//        return false;
//    if (ssd1.eegStartAddress() != ssd2.eegStartAddress())
//        return false;
//    if (ssd1.eegNumRegs() != ssd2.eegNumRegs())
//        return false;
//    for (int i = 0; i < NumEEGConfigReg; i++)
//    {
//        if (ssd1.eegReg()[i] != ssd2.eegReg()[i])
//            return false;
//    }
//    if (ssd1.stimChInfo() != ssd2.stimChInfo())
//        return false;
//    for (int i = 0; i < 8; i++)
//    {
//        if (ssd1.isStimChPresent()[i] != ssd2.isStimChPresent()[i])
//            return false;
//    }
//    for (int i = 0; i < 8; i++)
//    {
//        //if (ssd1.pointerToStimData()[i] != ssd2.pointerToStimData()[i])
//        if (ssd1.stimulationData().data()[i] != ssd2.stimulationData().data()[i])
//            return false;
//    }
//    if (ssd1.stimStartAddress() != ssd2.stimStartAddress())
//        return false;
//    if (ssd1.stimNumRegs() != ssd2.stimNumRegs())
//        return false;
//    for (int i = 0; i < NumStimConfigReg; i++)
//    {
//        if (ssd1.stimReg()[i] != ssd2.stimReg()[i])
//            return false;
//    }
//    for (int i = 0; i < 8; i++)
//    {
//        if (ssd1.isStimImpedanceChPresent()[i] !=
//                                        ssd2.isStimImpedanceChPresent()[i])
//            return false;
//    }
//    for (int i = 0; i < 8; i++)
//    {
//        //if (ssd1.pointerToImpedanceData()[i] !=
//        //                                    ssd2.pointerToImpedanceData()[i])
//        if (ssd1.stimImpedanceData().data()[i] !=
//                                            ssd2.stimImpedanceData().data()[i])
//            return false;
//    }
//    for (int i = 1; i < 4; i++)
//    {
//        if (ssd1.accelerometer().data()[i] != ssd2.accelerometer().data()[i])
//            return false;
//    }

//    if (ssd1.firmwareVersion() != ssd2.firmwareVersion())
//        return false;
//    if (ssd1.battery() != ssd2.battery())
//        return false;
    return true;
}

QString StarstimData::toString ()
{
//    QString ret = "Status:\n";
//    ret += "isCommandToggled: " + QString::number(isCommandToggled()) + "\n";
//    ret += "deviceStatus: " + QString::number(deviceStatus()) + "\n";
//    ret += "deviceError:" + QString::number(deviceError()) + "\n";

//    ret += "StarStimData:\n";
//    ret += "isEEGDataPresent:" + QString::number(isEEGDataPresent()) + "\n";
//    ret += "eegChInfo:" + QString::number(eegChInfo()) + "\n";
//    for (int i = 0; i < 32; i++)
//    {
//        ret += "\tisEEGChPresent[" + QString::number(i) + "]:" +
//               QString::number(isEEGChPresent()[i]) + "\n";
//    }

//    foreach(ChannelData channelData , _eegDataArray)
//    {
//        for (int i = 0; i < 8; i++)
//        {
//            ret += "\teegDataSample2[" + QString::number(i) + "]:" +
//                   QString::number(channelData.data()[i]) + "\n";
//        }
//    }

//    ret += "eegStamp:" + QString::number(eegStamp()) + "\n";
//    ret += "isEEGConfigPresent:" + QString::number(isEEGConfigPresent()) + "\n";
//    ret += "\teegStartAddress:" + QString::number(eegStartAddress()) + "\n";
//    ret += "\teegNumRegs:" + QString::number(eegNumRegs()) + "\n";
//    for (int i = 0; i < NumEEGConfigReg; i++)
//    {
//        ret += "\teegReg[" + QString::number(i) + "]:" +
//               QString::number(eegReg()[i]) + "\n";
//    }
//    ret += "isStimDataPresent:" + QString::number(isStimDataPresent()) + "\n";
//    ret += "stimChInfo:" + QString::number(stimChInfo()) + "\n";
//    for (int i = 0; i < 8; i++)
//    {
//        ret += "\tisStimChPresent[" + QString::number(i) + "]:" +
//               QString::number(isStimChPresent()[i]) + "\n";
//    }
//    for (int i = 0; i < 8; i++)
//    {
//        ret += "\tstimData[" + QString::number(i) + "]:" +
//               //QString::number(pointerToStimData()[i]) + "\n";
//               QString::number(this->stimulationData().data()[i]) + "\n";
//    }
//    ret += "isStimConfigPresent:" +
//                    QString::number(isStimConfigPresent()) + "\n";
//    ret += "\tstimStartAddress:" + QString::number(stimStartAddress()) + "\n";
//    ret += "\tstimNumRegs:" + QString::number(stimNumRegs()) + "\n";
//    for (int i = 0; i < NumStimConfigReg; i++)
//    {
//        ret += "\tstimReg[" + QString::number(i) + "]:" +
//               QString::number(stimReg()[i]) + "\n";
//    }
//    ret += "isStimImpedancePresent:"
//                + QString::number(isStimImpedancePresent()) + "\n";
//    ret += "stimImpedanceChInfo:"
//                + QString::number(stimImpedanceChInfo()) + "\n";
//    for (int i = 0; i < 8; i++)
//    {
//        ret += "\tisStimImpedanceChPresent[" + QString::number(i) + "]:" +
//               QString::number(isStimImpedanceChPresent()[i]) + "\n";
//    }
//    for (int i = 0; i < 8; i++)
//    {
//        ret += "\tstimImpedance[" + QString::number(i) + "]:" +
//               //QString::number(pointerToImpedanceData()[i]) + "\n";
//               QString::number(stimImpedanceData().data()[i]) + "\n";
//    }
//    ret += "isAccelerometerPresent:" +
//           QString::number(isAccelDataPresent()) + "\n";
//    for (int i = 1; i < 4; i++)
//    {
//        ret += "\taccelerometer[" + QString::number(i) + "]:" +
//               //QString::number(pointerToImpedanceData()[i]) + "\n";
//               QString::number(accelerometer().data()[i]) + "\n";
//    }
//    ret += "isFirmwareVersionPresent:" +
//           QString::number(isFirmwareVersionPresent()) + "\n";
//    ret += "firmwareVersion:" + QString::number(firmwareVersion()) + "\n";
//    ret += "battery:" + QString::number(battery());
//    return ret;
    return "";
}


