#include "starstimprotocol.h"

// Qt Includes
#include <QDebug>

// Project includes
#include "commonparameters.h"

const int StarStimProtocol::SFD_0 = 'S';
const int StarStimProtocol::SFD_1 = 'O';
const int StarStimProtocol::SFD_2 = 'F';
const int StarStimProtocol::EFD_0 = 'E';
const int StarStimProtocol::EFD_1 = 'O';
const int StarStimProtocol::EFD_2 = 'F';

#define SCP_RQST_LENGTH_B0_OFF         (3)
#define SCP_RQST_LENGTH_B1_OFF         (4)
#define SCP_RQST_ACTION_OFF            (5)
#define SCP_RQST_RDWR_OFF_B0           (6)
#define SCP_RQST_DATA_OFF              (7)

#define SFD_LENGTH 3

//#define __DEBUGARTIFACTENOBIO20PROTOCOL__

#ifdef __DEBUGARTIFACTENOBIO20PROTOCOL__
#include <QFile>
#include <QDateTime>
QFile debugProtocolFile;
#endif

StarStimProtocol::StarStimProtocol ()
{
    // Default configuration
    _isStimulating       = false;
    _multipleSample      = false;
    _eegCompresssionType = EEG_NO_COMPRESSION;

    reset();
#ifdef __DEBUGARTIFACTENOBIO20PROTOCOL__
    debugProtocolFile.setFileName(QDateTime::currentDateTime().toString("yyyyMMddhhmss") +
                                               "_debugArtifact_protocol.txt");
    debugProtocolFile.open(QIODevice::WriteOnly | QIODevice::Text);
#endif
}

StarStimProtocol::~StarStimProtocol ()
{
#ifdef __DEBUGARTIFACTENOBIO20PROTOCOL__
    debugProtocolFile.close();
#endif
}

////////////////////////////////////////
// Incoming operations
////////////////////////////////////////
void StarStimProtocol::reset ()
{
    _status = 255;

    _firmwareVersion=0;

    _debugErrorFrameIndex = 0;
    _lastDeviceStatus = 0;
    _nBytes = 0;
    _sfd[0] = 'x';
    _sfd[1] = 'x';
    _sfd[2] = 'x';
    _sfd[3] = '\0';
    _counter = 0;
/*
    _eof[0] = 'x';
    _eof[1] = 'x';
    _eof[2] = 'x';
    _eof[3] = 'x';
    _eof[4] = '\0';
/**/
    _eof[0] = 'x';
    _eof[1] = 'x';
    _eof[2] = 'x';
    _eof[4] = '\0';


    _previousCommandToggle = 0;
    _lastArtifactCheckerCounter = -1;
    for (int i = 0; i < 32; i++)
    {
        _artifactLastValues[i] = 0;
    }

    _isLength2Bytes = false;

    _resetStateMachine();
}

void StarStimProtocol::_resetStateMachine ()
{
    _state = ST_IDLE;
    _isEEGDataProcessed = 0;
    _isRegConfigProcessed = 0;
    _isStimDataProcessed = 0;
    _isStimConfigProcessed = 0;
    _isStimImpedanceProcessed = 0;
    _isBatteryProcessed = 0;
    _isAccelDataProcessed = 0;
    _isFirmwareProcessed = 0;
    _currentChannel = 0;
    _currentConfigAddress = 0;
    _dataLength = 0;
    _artifactCheckerCounter = 0;
}

void StarStimProtocol::_processStatusByte0(unsigned char status)
{
    _starStimData.isSDCardRecording((status & 0x08) > 0);

    if  (_status != ((status&0x70)>>4))
    {
        //qDebug() << "status = " << ((status&0x70)>>4) << " (" << _status_ << ")";
        _status = ((status&0x70)>>4);
    }

    bool toggled = ((status & 0x80) != _previousCommandToggle);
    //if (toggled) qDebug() << "toggled!! (status = " << status << ", previous = " << _previousCommandToggle << ")";
    _starStimData.isCommandToggled(toggled);
    _previousCommandToggle = (status & 0x80);
    //_starStimData.deviceStatus((status >> 4) & 0x07);
    _starStimData.deviceStatus((status >> 3) & 0x0f);
    _starStimData.deviceError((status & 0x0f));
}

void StarStimProtocol::_processStatusByte1 (unsigned char content_byte0)
{
    _starStimData.isRegConfigPresent(         (content_byte0 & 0x01) > 0);
    _starStimData.isEEGDataPresent(           (content_byte0 & 0x02) > 0);
    _starStimData.isAccelDataPresent(         (content_byte0 & 0x04) > 0);
    _starStimData.isStimDataPresent(          (content_byte0 & 0x08) > 0);
    _starStimData.isStimImpedancePresent(     (content_byte0 & 0x10) > 0);
    _starStimData.isProfilePresent(           (content_byte0 & 0x40) > 0);
    _starStimData.isFirmwareVersionPresent(   (content_byte0 & 0x80) > 0);
}

unsigned char StarStimProtocol::_findNextChannel (unsigned int channelInfo)
{

    int i = 1;
    i = i << _currentChannel;
    while (_currentChannel < 32)
    {
        if (channelInfo & i)
        {
            break;
        }
        _currentChannel++;
        i = i << 1;
    }
    return _currentChannel;
}

bool StarStimProtocol::_isLastEEGChannel ()
{
    _currentChannel++;
    return (_findNextChannel(_starStimData.eegChInfo()) >= 32);

}

bool StarStimProtocol::_processEEGChannelInfo (unsigned int eegChInfo)
{
    eegChInfo = ~eegChInfo; // '0' means EEG and '1' STM

    _starStimData.eegChInfo(eegChInfo);
    if (eegChInfo == 0)
    {
        return false;
    }
    _currentChannel = 0;
    _findNextChannel (eegChInfo);
    return true;
}

bool StarStimProtocol::_isLastEEGReg ()
{
    _currentConfigAddress++;
    return ((_currentConfigAddress - _starStimData.eegStartAddress()) >=
                                                _starStimData.eegNumRegs());
}

bool StarStimProtocol::_isLastStimChannel ()
{
    _currentChannel++;
    return (_findNextChannel(_starStimData.stimChInfo()) >= NUM_STIM_CHANNELS);
}

bool StarStimProtocol::_isLastStimImpedanceChannel ()
{
    _currentChannel++;
    return (_findNextChannel(_starStimData.stimImpedanceChInfo()) >= NUM_STIM_CHANNELS);
}

bool StarStimProtocol::_processStimChannelInfo (unsigned int stimChInfo)
{
    _starStimData.stimChInfo(stimChInfo);
    if (stimChInfo == 0)
    {
        return false;
    }
    _currentChannel = 0;
    _findNextChannel (stimChInfo);
    return true;
}

bool StarStimProtocol::_processStimImpedanceChannelInfo (unsigned int chInfo)
{
    _starStimData.stimImpedanceChInfo(chInfo);
    if (chInfo == 0)
    {
        return false;
    }
    _currentChannel = 0;
    _findNextChannel (chInfo);
    return true;
}

bool StarStimProtocol::_isLastStimReg ()
{
    _currentConfigAddress++;
    return ((_currentConfigAddress - _starStimData.stimStartAddress()) >=
                                                _starStimData.stimNumRegs());
}

bool StarStimProtocol::_transitionToNextBlock ()
{
    if (_starStimData.isEEGDataPresent() && !_isEEGDataProcessed)
    {
        _isEEGDataProcessed = true;
        _statusTransition(ST_EEG_DATA);
    }
    else if (_starStimData.isRegConfigPresent() && !_isRegConfigProcessed)
    {
        _isRegConfigProcessed = true;
        _statusTransition(ST_CONFIG_REGS_0);
    }
    else if (_starStimData.isStimDataPresent() && !_isStimDataProcessed)
    {
        _isStimDataProcessed = true;
        _statusTransition(ST_STIM_CH_INFO_0);
    }
    else if (_starStimData.isStimImpedancePresent() && !_isStimImpedanceProcessed)
    {
        _isStimImpedanceProcessed = true;
        _statusTransition(ST_STIM_IMPEDANCE_CH_INFO_0);
    }
    else if (_starStimData.isProfilePresent() && !_isBatteryProcessed)
    {
        _isBatteryProcessed = true;
        _statusTransition(ST_PROFILE_BATTERY);
    }
    else if (_starStimData.isAccelDataPresent() && !_isAccelDataProcessed)
    {
        _isAccelDataProcessed = true;
        _statusTransition(ST_ACCELEROMETER);
    }
    else // no data
    {
        return false;
    }
    return true;
}

bool StarStimProtocol::parseByte (unsigned char byte)
{

    QString debugString = "";

    int auxArtifactCorrector;

    /* JUST WHILE DEVELOPING ****/
    //qDebug() << nBytes;
    //if (++nBytes >= 10)
    //{
    //    nBytes = 0;
    //    return true;
    //}
    //return false;
    /****************************/
    //qDebug() << "." << byte;


    _debugErrorFrame[_debugErrorFrameIndex++] = byte;
    if (_debugErrorFrameIndex >= 300)
    {
        _debugErrorFrameIndex = 0;
    }
    _nBytes++;

    switch (_state)
    {
    case ST_IDLE:
        if (_isStartOfFrame(byte)) // start frame delimiter
        {
            _resetStateMachine();
            _starStimData.empty();
            _nBytes = SFD_LENGTH;
            _statusTransition(ST_LENGTH0);
        }
        break;

    case ST_LENGTH0:
        _dataLength = byte << 8*1;
        _statusTransition(ST_LENGTH1);
        break;
    case ST_LENGTH1:
        _dataLength += byte;
        _statusTransition(ST_STATUS_0);
        break;
    case ST_STATUS_0:
        _processStatusByte0(byte);
        _statusTransition(ST_CONTENT_0);
        break;
    case ST_CONTENT_0:
        _processStatusByte1( byte );
        if (!_transitionToNextBlock()) // no more data
        {
            _statusTransition(ST_EOF);
        }
        break;
    case ST_EEG_DATA:

        if( _multipleSample ){ // Multiple Sample Mode ON!
            _statusTransition(ST_EEG_CH_INFO_0);
            // Store number of samples per byte
            _starStimData.nSamples( byte );
            _currentSample = 0;

        }else{
            _temp = (byte << 24);
            _statusTransition(ST_EEG_CH_INFO_1);

            // Force to have 1 Sample per beacon
            _starStimData.nSamples( 1 );
            _currentSample = 0;

        }
        break;

    case ST_EEG_CH_INFO_0:
        _temp = (byte << 24);
        _statusTransition(ST_EEG_CH_INFO_1);

        break;

    case ST_EEG_CH_INFO_1:
        _temp += (byte << 16);
        _statusTransition(ST_EEG_CH_INFO_2);
        break;
    case ST_EEG_CH_INFO_2:
        _temp += (byte << 8);
        _statusTransition(ST_EEG_CH_INFO_3);
        break;
    case ST_EEG_CH_INFO_3:
        _temp += byte;
        if (!_processEEGChannelInfo(_temp))
        {
            qDebug()<<"_processEEGChannelInfo Error";

            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_EEG_DATA_RAW_MSB);
        }

        break;
    case ST_EEG_DATA_RAW_MSB:
        //_starStimData.pointerToEEGData()[_currentChannel] = byte << 16;
        _temp = byte << 8*2;
        _statusTransition(ST_EEG_DATA_RAW_MSBLSB);
        break;
    case ST_EEG_DATA_RAW_MSBLSB:
        //_starStimData.pointerToEEGData()[_currentChannel] += byte << 8;
        _temp += byte << 8*1;
        auxArtifactCorrector = _temp>>8;
        if (_lastArtifactCheckerCounter >= 0)
        {
            _artifactCheckerCounter += abs(auxArtifactCorrector - _artifactLastValues[_currentChannel]);
        }
        _artifactLastValues[_currentChannel] = auxArtifactCorrector;
        _lastChannelWithEEG = _currentChannel + 1;
        _statusTransition(ST_EEG_DATA_RAW_LSB);
        break;
    case ST_EEG_DATA_RAW_LSB:
        //_starStimData.pointerToEEGData()[_currentChannel] += byte;
        _temp += byte;
        _starStimData.eegData( _currentChannel, _temp, _currentSample, _eegCompresssionType  );

        if (_isLastEEGChannel())
        {
            _artifactCheckerCounter = _artifactCheckerCounter / _lastChannelWithEEG;


            _currentSample ++;
            if( _currentSample == _starStimData.nSamples() ){
                _statusTransition( ST_EEG_STAMP );
            }else{
                _currentChannel = 0;
                _findNextChannel ( _starStimData.eegChInfo() );

                // NOTE: First sample is not compressed
                // When compression enabled => first sample is 3 byte/channel, next ones are 2 byte/channel
                if( _eegCompresssionType == StarStimProtocol::EEG_16BIT_COMPRESSION && _currentSample > 0){
                    _statusTransition( ST_EEG_COMPRESSED_16BIT_MSB );
                }else if( _eegCompresssionType == StarStimProtocol::EEG_12BIT_COMPRESSION && _currentSample > 0){
                      _statusTransition( ST_EEG_COMPRESSED_12BIT_S1 );
                }else{
                    _statusTransition( ST_EEG_DATA_RAW_MSB );
                }
            }

        }
        else
        {
            _statusTransition(ST_EEG_DATA_RAW_MSB);
        }

        break;
    case ST_EEG_COMPRESSED_16BIT_MSB:
        _temp = byte << 8*1;
        _statusTransition( ST_EEG_COMPRESSED_16BIT_LSB );
        break;
    case ST_EEG_COMPRESSED_16BIT_LSB:

        _temp += byte;
        _starStimData.eegData( _currentChannel, _temp, _currentSample, _eegCompresssionType );
        if (_isLastEEGChannel())
        {
            _artifactCheckerCounter = _artifactCheckerCounter / _lastChannelWithEEG;

            _currentSample ++;
            if( _currentSample == _starStimData.nSamples() ){
                _statusTransition( ST_EEG_STAMP );
            }else{
                _currentChannel = 0;
                _findNextChannel ( _starStimData.eegChInfo() );

                // NOTE: First sample is not compressed
                // When compression enabled => first sample is 3 byte/channel, next ones are 2 byte/channel
                _statusTransition( ST_EEG_COMPRESSED_16BIT_MSB );
            }

        }
        else
        {
            _statusTransition( ST_EEG_COMPRESSED_16BIT_MSB );
        }

        break;
    case ST_EEG_COMPRESSED_12BIT_S1:
        _temp = byte << 4;
        _statusTransition( ST_EEG_COMPRESSED_12BIT_S1S2 );
        break;
    case ST_EEG_COMPRESSED_12BIT_S1S2:

        // First EEG channel received
        _temp = _temp + (byte >> 4);
        _starStimData.eegData( _currentChannel, _temp, _currentSample, _eegCompresssionType );
        _currentChannel++;

        // Prepare for next sample
        _temp = (byte&0x0F) << 8;

        _statusTransition( ST_EEG_COMPRESSED_12BIT_S2 );
        break;
    case ST_EEG_COMPRESSED_12BIT_S2:

        // Second EEG channel received
        _temp = _temp + byte;
        _starStimData.eegData( _currentChannel, _temp, _currentSample, _eegCompresssionType );
        _currentChannel++;
        if ( _findNextChannel(_starStimData.eegChInfo()) >= 32 )
        {
            // Number of samples received
            _currentSample ++;
            if( _currentSample == _starStimData.nSamples() ){
                _statusTransition( ST_EEG_STAMP );
            }else{
                _currentChannel = 0;
                _findNextChannel ( _starStimData.eegChInfo() );
                //_currentChannel ++;
                _statusTransition( ST_EEG_COMPRESSED_12BIT_S1 );

            }
        }
        else // If not last channel, redo procedure
        {
            _statusTransition( ST_EEG_COMPRESSED_12BIT_S1 );
        }

        break;

    case ST_EEG_STAMP:
        _stamp++;
        _temp = (byte << 24);
        _statusTransition(ST_EEG_STAMP_2);
        break;
    case ST_EEG_STAMP_2:
        _temp += (byte << 16);
        _statusTransition(ST_EEG_STAMP_3);
        break;
    case ST_EEG_STAMP_3:
        _temp += (byte << 8);
        _statusTransition(ST_EEG_STAMP_4);
        break;
    case ST_EEG_STAMP_4:
        _temp += byte;
        _starStimData.eegStamp(_temp);
        if (!_transitionToNextBlock()) // no more data
        {
            _statusTransition(ST_EOF);
        }
        break;
    case ST_CONFIG_REGS_0:
        _temp = (byte << 8*1);
        _statusTransition(ST_CONFIG_REGS_1);
        break;
    case ST_CONFIG_REGS_1:
        _temp += (byte << 8*0);
        _starStimData.eegStartAddress(_temp);
        //_starStimData.eegStartAddress(byte);
        _currentConfigAddress = _starStimData.eegStartAddress();
        _statusTransition(ST_CONFIG_REGS_NUM);
        break;
    case ST_CONFIG_REGS_NUM:
        _starStimData.eegNumRegs(byte);
        if (_starStimData.eegNumRegs() == 0)
        {
            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_CONFIG_REGS_DATA);
        }
        break;
    case ST_CONFIG_REGS_DATA:
        _starStimData.confReg()[_currentConfigAddress] = byte;
        if (_isLastEEGReg())
        {
            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_CONFIG_REGS_DATA);
        }
        break;
    case ST_STIM_CH_INFO_0:
        _temp = (byte << 24);
        _statusTransition(ST_STIM_CH_INFO_1);
        break;
    case ST_STIM_CH_INFO_1:
        _temp += (byte << 16);
        _statusTransition(ST_STIM_CH_INFO_2);
        break;
    case ST_STIM_CH_INFO_2:
        _temp += (byte << 8);
        _statusTransition(ST_STIM_CH_INFO_3);
        break;
    case ST_STIM_CH_INFO_3:
        _temp += byte;
        if (!_processStimChannelInfo(_temp))
        {
            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_STIM_DATA_MSB);
        }
        break;
    case ST_STIM_DATA_MSB:
        //_starStimData.pointerToStimData()[_currentChannel] = (byte << 8);
        _temp = (byte << 8);
        _statusTransition(ST_STIM_DATA_LSB);
        break;
    case ST_STIM_DATA_LSB:
        //_starStimData.pointerToStimData()[_currentChannel] += byte;
        _temp += byte;
        _starStimData.stimData(_currentChannel, _temp);
        if (_isLastStimChannel())
        {
            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_STIM_DATA_MSB);
        }
        break;
    case ST_STIM_IMPEDANCE_CH_INFO_0:
        _temp = (byte << 24);
        _statusTransition(ST_STIM_IMPEDANCE_CH_INFO_1);
        break;
    case ST_STIM_IMPEDANCE_CH_INFO_1:
        _temp += (byte << 16);
        _statusTransition(ST_STIM_IMPEDANCE_CH_INFO_2);
        break;
    case ST_STIM_IMPEDANCE_CH_INFO_2:
        _temp += (byte << 8);
        _statusTransition(ST_STIM_IMPEDANCE_CH_INFO_3);
        break;
    case ST_STIM_IMPEDANCE_CH_INFO_3:
        _temp += (byte);
        if (!_processStimImpedanceChannelInfo(_temp))
        {
            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_STIM_IMPEDANCE_DATA_MSB);
        }
        break;
    case ST_STIM_IMPEDANCE_DATA_MSB:
        //_starStimData.pointerToImpedanceData()[_currentChannel] = byte << 24;
        _temp = byte << 24;
        _statusTransition(ST_STIM_IMPEDANCE_DATA_MSBLSB);
        break;
    case ST_STIM_IMPEDANCE_DATA_MSBLSB:
        //_starStimData.pointerToImpedanceData()[_currentChannel] += byte << 16;
        _temp += byte << 16;
        _statusTransition(ST_STIM_IMPEDANCE_DATA_LSBMSB);
        break;
    case ST_STIM_IMPEDANCE_DATA_LSBMSB:
        //_starStimData.pointerToImpedanceData()[_currentChannel] += byte << 8;
        _temp += byte << 8;
        _statusTransition(ST_STIM_IMPEDANCE_DATA_LSB);
        break;
    case ST_STIM_IMPEDANCE_DATA_LSB:
        //_starStimData.pointerToImpedanceData()[_currentChannel] += byte;
        _temp += byte;
        _starStimData.stimImpedance(_currentChannel, _temp);
        if (_isLastStimImpedanceChannel())
        {
            if (!_transitionToNextBlock()) // no more data
            {
                _statusTransition(ST_EOF);
            }
        }
        else
        {
            _statusTransition(ST_STIM_IMPEDANCE_DATA_MSB);
        }
        break;
    case ST_PROFILE_BATTERY:
        _temp = byte;
        _statusTransition(ST_PROFILE_BATTERY_1);
        break;
    case ST_PROFILE_BATTERY_1:
        _temp += (byte << 24);
        _statusTransition(ST_PROFILE_BATTERY_2);
        break;
    case ST_PROFILE_BATTERY_2:
        _temp += (byte << 16);
        _statusTransition(ST_PROFILE_BATTERY_3);
        break;
    case ST_PROFILE_BATTERY_3:
        _temp += (byte << 8);
        _starStimData.battery(_temp);
        _statusTransition(ST_PROFILE_FIRMWARE);
        break;
    case ST_PROFILE_FIRMWARE:
        _temp=byte<<8;
        _statusTransition(ST_PROFILE_FIRMWARE_1);
        break;
    case ST_PROFILE_FIRMWARE_1:
        _temp += (byte);
        _starStimData.firmwareVersion( (_temp & 0xFFFF) );
        _firmwareVersion = (_temp);
        _statusTransition(ST_PROFILE_SYNCH);
        break;
    case ST_PROFILE_SYNCH:
        _temp = (byte << 24);
        _statusTransition(ST_PROFILE_SYNCH_1);
        break;
    case ST_PROFILE_SYNCH_1:
        _temp += (byte << 16);
        _statusTransition(ST_PROFILE_SYNCH_2);
        break;
    case ST_PROFILE_SYNCH_2:
        _temp += (byte << 8);
        _statusTransition(ST_PROFILE_SYNCH_3);
        break;
    case ST_PROFILE_SYNCH_3:
        _temp += byte;
        _starStimData.synchT1(_temp);
        _statusTransition(ST_PROFILE_SYNCH_4);
        break;
    case ST_PROFILE_SYNCH_4:
        _temp = (byte << 24);
        _statusTransition(ST_PROFILE_SYNCH_5);
        break;
    case ST_PROFILE_SYNCH_5:
        _temp += (byte << 16);
        _statusTransition(ST_PROFILE_SYNCH_6);
        break;
    case ST_PROFILE_SYNCH_6:
        _temp += (byte << 8);
        _statusTransition(ST_PROFILE_SYNCH_7);
        break;
    case ST_PROFILE_SYNCH_7:
        _temp += byte;
        _starStimData.synchT2(_temp);

//        loggerMacroDebug("Got synchro T1:" + QString::number(_starStimData.synchT1()) +
//                                    " T2:" + QString::number(_starStimData.synchT2()))
        _statusTransition(ST_PROFILE_DEV_TYPE);
        break;
    case ST_PROFILE_DEV_TYPE:
        _starStimData.deviceType( byte );
        _statusTransition(ST_PROFILE_N_CHANNELS);
        break;
    case ST_PROFILE_N_CHANNELS:
        _starStimData.numOfChannels( byte );
        if (!_transitionToNextBlock()) // no more data
        {
            _statusTransition(ST_EOF);
        }
        break;
    case ST_ACCELEROMETER:
        _temp=byte<<8;
        _statusTransition(ST_ACCELEROMETER_1);
        break;
    case ST_ACCELEROMETER_1:
        _temp += (byte);
        if (_temp >= 32768)
          _temp -= 65536;
        _starStimData.accelerometer(0,_temp);
        _statusTransition(ST_ACCELEROMETER_2);
        break;
    case ST_ACCELEROMETER_2:
        _temp=byte<<8;
        _statusTransition(ST_ACCELEROMETER_3);
        break;
    case ST_ACCELEROMETER_3:
        _temp += (byte);
        if (_temp >= 32768)
          _temp -= 65536;
        _starStimData.accelerometer(1,_temp);
        _statusTransition(ST_ACCELEROMETER_4);
        break;
    case ST_ACCELEROMETER_4:
        _temp=byte<<8;
        _statusTransition(ST_ACCELEROMETER_5);
        break;
    case ST_ACCELEROMETER_5:
        _temp += (byte);
        if (_temp >= 32768)
          _temp -= 65536;
        _starStimData.accelerometer(2,_temp);
        if (!_transitionToNextBlock()) // no more data
        {
            _statusTransition(ST_EOF);
        }
        break;
    case ST_EOF:
        _isEndOfFrame(byte);
        _statusTransition(ST_EOF_1);
        break;
    case ST_EOF_1:
        _isEndOfFrame(byte);
        _statusTransition(ST_EOF_2);
        break;
    case ST_EOF_2:
        _statusTransition(ST_IDLE);
        if (_isEndOfFrame(byte) && _nBytes == _dataLength)
        {
#ifdef __DEBUGARTIFACTENOBIO20PROTOCOL__
            if (_starStimData.deviceStatus() & 0x02)
            {
                for (unsigned int k = 0; k < _debugErrorFrameIndex; k++)
                {
                    debugString.append(" ");
                    debugString.append(QString::number(_debugErrorFrame[k]));
                }
                debugString.append("\t" + QString::number(QDateTime::currentMSecsSinceEpoch()) +
                                   "\n");
                debugProtocolFile.write(debugString.toAscii());
            }
#endif

            _debugErrorFrameIndex = 0;
            if ((_firmwareVersion<593)&&(!_isStimulating))
            {
                if (_lastArtifactCheckerCounter >= 0)
                {
                    if (_artifactCheckerCounter > 1000)
                    {
                        _lastArtifactCheckerCounter = -1;
                        break;
                    }
                }
                if (_starStimData.deviceStatus() & 0x02)
                {
                    int i;
                    for (i = 0; i < 32; i++)
                    {
                        if (_artifactLastValues[i] != 0)
                            break;
                    }
                    if (i < 32)
                    {
                        _lastArtifactCheckerCounter = _artifactCheckerCounter;
                    }
                }
                else if (_lastDeviceStatus & 0x02)
                {
                    _lastArtifactCheckerCounter = -1;
                    for (int i = 0; i < 32; i++)
                    {
                        _artifactLastValues[i] = 0;
                    }
                }
            }
            _lastDeviceStatus = _starStimData.deviceStatus();
            return true;
        }


        for (unsigned int k = 0; k < _debugErrorFrameIndex; k++)
        {
            QString aux;
            debugString.append(aux.sprintf(" 0x%02X", _debugErrorFrame[k]));
        }
        qDebug() << "Frame with errors: byte"<<byte<<"nBytes" << _nBytes << "_dataLength"<<_dataLength << "_state"<<_state << "\n" << debugString;
        _debugErrorFrameIndex = 0;
        break;
    }
    return false;
}

bool StarStimProtocol::_isStartOfFrame (unsigned char byte)
{
    _sfd[0] = _sfd[1];
    _sfd[1] = _sfd[2];
    _sfd[2] = byte;
    bool b = (QString::compare("SOF", QString::fromLatin1((const char*)_sfd)) == 0);
    _counter++;
    if (b)
    {
        if (_counter > 3)
        {
            qDebug() << "SOF not found in first byte, but in byte" << _counter;
        }
        _counter = 0;
    }

//    QString aux;
//    QString str = aux.sprintf("0x%02X 0x%02X 0x%02X", _sfd[0], _sfd[1], _sfd[2]);
//    loggerMacroDebug("Value -> " + str + " " + QString(b?"TRUE":"FALSE"))

    return (QString::compare("SOF", QString::fromLatin1((const char*)_sfd)) == 0);
}

bool StarStimProtocol::_isEndOfFrame (unsigned char byte)
{
/*
    _eof[0] = _eof[1];
    _eof[1] = _eof[2];
    _eof[2] = _eof[3];
    _eof[3] = byte;
/**/
    _eof[0] = _eof[1];
    _eof[1] = _eof[2];
    _eof[2] = byte;

//    QString aux;
//    QString str = aux.sprintf("0x%02X 0x%02X 0x%02X 0x%02X", _eof[0], _eof[1], _eof[2], _eof[3]);
//    loggerMacroDebug("Value -> " + str )


    //return (QString::compare("EOF\n", QString::fromLatin1((const char*)_eof)) == 0);
    return (QString::compare("EOF", QString::fromLatin1((const char*)_eof)) == 0);

    //return (byte == '\n');
}

void StarStimProtocol::_statusTransition (StatusProtocol status)
{
    // nothig to do but to update the status property
    _state = status;
}

StarstimData * StarStimProtocol::getStarStimData ()
{
    return &_starStimData;
}


//////////////////////////////////////////////////
// Outgoing operations
//////////////////////////////////////////////////


void StarStimProtocol::_addSOF(QByteArray &frame){
    frame.append( StarStimProtocol::SFD_0 );
    frame.append( StarStimProtocol::SFD_1 );
    frame.append( StarStimProtocol::SFD_2 );
    frame.append( (char) 0 );    // length
    frame.append( (char) 0 );    // length
}

void StarStimProtocol::_addHeader(QByteArray &frame){
    frame.append( StarStimProtocol::SFD_0 );
    frame.append( StarStimProtocol::SFD_1 );
    frame.append( StarStimProtocol::SFD_2 );
    frame.append( (char) 0x00 ); // length_B0
    frame.append( (char) 0x00 ); // length_B1
    frame.append( (char) 0x00 ); // ACTION
    frame.append( (char) 0x00 ); // CONTENT
}

void StarStimProtocol::_addTail(QByteArray &frame){

    frame.append( StarStimProtocol::EFD_0 );
    frame.append( StarStimProtocol::EFD_1 );
    frame.append( StarStimProtocol::EFD_2 );

    // If this operation fails, a fatal failure should occur
    if( frame.size() > SCP_RQST_LENGTH_B1_OFF ){
        frame[SCP_RQST_LENGTH_B0_OFF] = frame.count() % 256; // Set length
        frame[SCP_RQST_LENGTH_B1_OFF] = frame.count() / 256; // Set length
    }

}


QByteArray StarStimProtocol::buildStartBeaconRequest (){

    QByteArray txBuffer;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF]  = (char) 0x7F; // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0] = (char) 0x7F; // CONTENT_B1

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    txBuffer[3] = txBuffer.count(); // Set length

    return txBuffer;

}

QByteArray StarStimProtocol::buildStopBeaconRequest (){

    QByteArray txBuffer;

    // Add EOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF]  = (char) 0x00; // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0] = (char) 0x00; // CONTENT_B1

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}

QByteArray StarStimProtocol::buildProfileRequest (){

    QByteArray txBuffer;
    unsigned char addProfile = 0x01;

    _addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF]  = (addProfile << 6); // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}

QByteArray StarStimProtocol::buildNullRequest (){
    QByteArray txBuffer;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF]  = (char) 0x80; // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0] = (char) 0x00; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}


QByteArray StarStimProtocol::buildStartEEGFrame (){

    QByteArray txBuffer;
    unsigned char addStartEEG = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF] = (addStartEEG << 0);  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;

}

QByteArray StarStimProtocol::buildStopEEGFrame ()
{

    QByteArray txBuffer;
    unsigned char addStopEEG = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF] = (addStopEEG << 1);  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}

QByteArray StarStimProtocol::buildStartStimulationFrame ()
{
    QByteArray txBuffer;
    unsigned char addStartStim = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF] = (addStartStim << 2);  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}

QByteArray StarStimProtocol::buildStopStimulationFrame ()
{
    QByteArray txBuffer;
    unsigned char addStopStim = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF] = (addStopStim << 3);  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}

QByteArray StarStimProtocol::buildStartImpedanceFrame ()
{

    QByteArray txBuffer;
    unsigned char addStartImp = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF] = (addStartImp << 4);  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;
}

QByteArray StarStimProtocol::buildStopImpedanceFrame ()
{
    QByteArray txBuffer;
    unsigned char addStopImp = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF] = (addStopImp << 5);  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0]; // CONTENT_B0

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;

}



QByteArray StarStimProtocol::buildReadRegisterFrame (int address,
                                                        int length)
{

    QByteArray txBuffer;
    unsigned char addReadEEGRegs = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF];  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0] = (addReadEEGRegs << 0); // CONTENT_B0

    txBuffer.append( (char) ((address >> 8*1) & 0xFF) );
    txBuffer.append( (char) ((address >> 8*0) & 0xFF) );
    txBuffer.append( (char) length );

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;

}


QByteArray StarStimProtocol::buildWriteRegisterFrame (int address,
                                                         unsigned char *value,
                                                         int length)
{
    QByteArray txBuffer;
    unsigned char addWriteEEGRegs = 0x01;

    // Add SOF
    StarStimProtocol::_addHeader(txBuffer);

    txBuffer[SCP_RQST_ACTION_OFF];  // ACTION
    txBuffer[SCP_RQST_RDWR_OFF_B0] = (addWriteEEGRegs << 1); // CONTENT_B0

    txBuffer.append( (char) ((address >> 8*1) & 0xFF) );
    txBuffer.append( (char) ((address >> 8*0) & 0xFF) );
    txBuffer.append( (char) length );
    txBuffer.append((const char*) value, length);

    // Add EOF
    StarStimProtocol::_addTail( txBuffer );

    return txBuffer;

}

QByteArray StarStimProtocol::buildBootloaderModeRequest( int nPages )
{
//    unsigned char nPages_MSB    = ((nPages>>8*2)&0xFF);
//    unsigned char nPages_MSBLSB = ((nPages>>8*1)&0xFF);
//    unsigned char nPages_LSB    = ((nPages>>8*0)&0xFF);

    QByteArray txBuffer;
//    txBuffer.append( StarStimProtocol::SFD_0 );
//    txBuffer.append( StarStimProtocol::SFD_1 );
//    txBuffer.append( StarStimProtocol::SFD_2 );

//    txBuffer.append( ((char) 0) );  // length
//    txBuffer.append( ((char) 0) );  // ST_B0
//    txBuffer.append( ((char) 0) );  // ST_B1


//    txBuffer.append( (char) 0x01); // Command Indicator 0x01 : Enter bootloader mode

//    // Add the pages which conform the new firmware
//    txBuffer.append( nPages_MSB    );
//    txBuffer.append( nPages_MSBLSB );
//    txBuffer.append( nPages_LSB    );

//    txBuffer.append( StarStimProtocol::EFD_0 );
//    txBuffer.append( StarStimProtocol::EFD_1 );
//    txBuffer.append( StarStimProtocol::EFD_2 );

//    txBuffer[3] = txBuffer.count(); // Set length

    return txBuffer;
}

QByteArray StarStimProtocol::buildDownloadCalibrationParamtersRequest(int* offsetArray, int* gain_n_Array, int* gain_d_Array){

    // NOTE: We use QByteArray since we do not know
    // the length of name beforehand
    QByteArray txBuffer;
//    txBuffer.append( StarStimProtocol::SFD_0 );
//    txBuffer.append( StarStimProtocol::SFD_1 );
//    txBuffer.append( StarStimProtocol::SFD_2 );

//    txBuffer.append( ((char) 0) );  // length
//    txBuffer.append( ((char) 0) );  // ST_B0
//    txBuffer.append( ((char) 0) );  // ST_B1

//    // Add cal_offset
//    for(int i = 0; i < NUM_STIM_CHANNELS; i++){
//        unsigned char msb = (offsetArray[i] & 0xFF00) >> 8*1;
//        unsigned char lsb = (offsetArray[i] & 0x00FF) >> 8*0;
//        txBuffer.append( msb );
//        txBuffer.append( lsb );
//    }

//    // Add cal_gain_n
//    for(int i = 0; i < NUM_STIM_CHANNELS; i++){
//        unsigned char msb = (gain_n_Array[i] & 0xFF00) >> 8*1;
//        unsigned char lsb = (gain_n_Array[i] & 0x00FF) >> 8*0;
//        txBuffer.append( msb );
//        txBuffer.append( lsb );
//    }

//    // Add cal_gain_d
//    for(int i = 0; i < NUM_STIM_CHANNELS; i++){
//        unsigned char msb = (gain_d_Array[i] & 0xFF00) >> 8*1;
//        unsigned char lsb = (gain_d_Array[i] & 0x00FF) >> 8*0;
//        txBuffer.append( msb );
//        txBuffer.append( lsb );
//    }

//    txBuffer.append( StarStimProtocol::EFD_0 );
//    txBuffer.append( StarStimProtocol::EFD_1 );
//    txBuffer.append( StarStimProtocol::EFD_2 );

//    txBuffer[3] = txBuffer.count(); // Set length

    return txBuffer;


}

///////////////////////////////////
// Setters and getters
///////////////////////////////////
void StarStimProtocol::setFirmwareVersion(int firmwareVersion){
    _firmwareVersion=firmwareVersion;
}

void StarStimProtocol::setIsStimulating(bool value){
    _isStimulating=value;
}


