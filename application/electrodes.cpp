#include "electrodes.h"

namespace Electrodes
{
ElectrodePlacement &setup = ElectrodePlacement::getInstance();

void ElectrodePlacement::sortElectrodes(const ElectrodeSort sort)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    _currentSort = sort;
    switch(sort)
    {
    case SortedByName:
        _edb.sortByName(_dev/*, _icognos20Setup*/, _eogEnabled);
        break;
    case SortedByScalp:
        _edb.sortByScalp(_dev/*, _icognos20Setup*/, _eogEnabled);
        break;
    }
}

void sortElectrodes(const ElectrodeSort sort)
{
    setup.sortElectrodes(sort);
}

ElectrodeSort currentSort()
{
    return setup.currentSort();
}

ElectrodeSort ElectrodePlacement::currentSort()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return _currentSort;
}

void setDevice(DeviceType::SupportedNICDevice dev)
{
    {
        QMutexLocker locker(&setup._electrodeSetupMutex);
        setup._dev = dev;
        setup._icognos20Setup = icognosStd;
        switch(dev)
        {
        case DeviceType::ENOBIO8:
            setup.setupDevice(8);
            setup._deviceScalpTotal = 8;
            break;
        case DeviceType::ENOBIO20:
            setup.setupDevice(20);
            setup._deviceScalpTotal = 19;
            setup.setupicognos20Standard();
            break;
        case DeviceType::ENOBIO32:
            setup.setupDevice(32);
            setup._deviceScalpTotal = 32;
            setup.setupicognos32();
            break;
        case DeviceType::STARSTIM:
            setup.setupDevice(8,8);
            setup._deviceScalpTotal = 8;
            break;
            /*
        case Device::STARSTIM20:
            setupDevice(20,20);
            break;
        case Device::STARSTIM32:
            setupDevice(32,32);
            break;
        */
        default: //unknown device?
            setup.setupDevice(0,0);
            break;
        }
        setup._deviceSpecialTotal = setup._deviceChannelTotal - setup._deviceScalpTotal;
    }
    setup.sortElectrodes(currentSort());
}

DeviceType::SupportedNICDevice deviceType()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._dev;
}

void ElectrodePlacement::setupDevice(unsigned int eegChannels, unsigned int stimChannels)
{
    _eogEnabled = false;
    _deviceScalpTotal = 0;
    _deviceEEGChannelTotal = eegChannels;
    _deviceStimulationChannelTotal = stimChannels;
    _deviceChannelTotal = eegChannels < stimChannels ? stimChannels : eegChannels;
    _edb.currentElectrodePlacement.clear();
    _edb.currentElectrodePlacement.resize(_deviceChannelTotal);
    _edb.deviceElectrodeChannelNames = _edb.electrodeChannelNames.mid(0, _deviceChannelTotal);
    _edb.deviceElectrodeChannelIDs = _edb.electrodeChannelIDsSorted.mid(0, _deviceChannelTotal);
    for(unsigned int i = 0; i < setup._deviceChannelTotal; i++)
    {
        resetElectrodePlacement(i);
    }
}

unsigned int deviceStimulationChannels()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._deviceStimulationChannelTotal;
}

unsigned int deviceEEGChannels()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._deviceEEGChannelTotal;
}

unsigned int deviceStandardChannels()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._deviceScalpTotal;
}

void ElectrodePlacement::setBipolarLongitudinalPlacement(unsigned int channelID, ElectrodePosition::BipolarLongitudinal::BipolarLongitudinalPairEnum pairID)
{
    _edb.currentElectrodePlacement[channelID] = pairID;
}

void ElectrodePlacement::setBipolarTransversalPlacement(unsigned int channelID, ElectrodePosition::BipolarTransversal::TransBipPairEnum pairID)
{
    _edb.currentElectrodePlacement[channelID] = pairID;
}

inline int ElectrodePlacement::ElectrodePlacementsDict::setElectrodePlacement(unsigned int c, unsigned int p)
{
    if(!currentElectrodePlacement.contains(p))
    {
        currentElectrodePlacement[c] = p;
        return c;
    }
    return currentElectrodePlacement.indexOf(p);
}

int ElectrodePlacement::setElectrodePlacement(unsigned int channelID, unsigned int positionID)
{    
    if(deviceHasChannel(channelID))
    {
        if(ElectrodePosition::isPosition(positionID))
        {
            return _edb.setElectrodePlacement(channelID, positionID);
        }
        resetElectrodePlacement(channelID);
    }
    return INVALID_ELECTRODE;
}

int setElectrodePlacement(unsigned int channelID, unsigned int positionID)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup.setElectrodePlacement(channelID, positionID);
}

int setElectrodePlacement(unsigned int channelID, const QString& positionName)
{
    if(getChannelID(positionName) != INVALID_ELECTRODE)//if name is a channel
    {
        resetElectrodePlacement(channelID);//it can only be itself
        return channelID;
    }
    unsigned int positionID = getPositionID(positionName);
    return setElectrodePlacement(channelID, positionID);
}

void resetElectrodePlacement()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    switch(setup._dev)
    {
    case DeviceType::ENOBIO20:
        setup.setupicognos20Standard();
        break;
    case DeviceType::ENOBIO32:;
        setup.setupicognos32();
        break;
        /*
    case DeviceType::ENOBIO8:
    case DeviceType::STARSTIM:
        /*
        case Device::STARSTIM20:
            setupDevice(20,20);
            break;
        case Device::STARSTIM32:
            setupDevice(32,32);
            break;
        */
    default: //other devices
        setup.resetElectrodePlacement();
        break;
    }
}

void resetElectrodePlacement(unsigned int channelID)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    setup.resetElectrodePlacement(channelID);
}

void ElectrodePlacement::resetElectrodePlacement(unsigned int channelID)
{
    _edb.currentElectrodePlacement[channelID] = channelID;
}

void ElectrodePlacement::resetElectrodePlacement()
{
    for(unsigned int i = 0; i < setup._deviceChannelTotal; i++)
        _edb.currentElectrodePlacement[i] = i;
}

int getPositionChannel(const QString& positionName)
{
    int posID = getPositionID(positionName);
    {
        return getPositionChannel(posID);
    }
}

int getPositionChannel(unsigned int posID)
{    
    QMutexLocker locker(&setup._electrodeSetupMutex);
    if(int(posID) != INVALID_ELECTRODE)
        return setup._edb.currentElectrodePlacement.indexOf(posID);
    return INVALID_ELECTRODE;
}

void seticognosSetup(icognosSetup s)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    if(setup._dev != DeviceType::ENOBIO20 || setup._icognos20Setup == s)
        return;
    switch(s)
    {
    case icognosStd:
        setup._deviceChannelTotal = 20;
        setup.setupicognos20Standard();
        break;
    case E20BipolarLong:
        setup._eogEnabled = false;
        setup._deviceChannelTotal = Electrodes::ElectrodePosition::BipolarLongitudinal::supportedBipolarLongitudinalPairs;
        setup.setupicognos20BipolarLongitudinal();        
        break;
    case E20BipolarTrans:
        setup._eogEnabled = false;
        setup._deviceChannelTotal = Electrodes::ElectrodePosition::BipolarTransversal::supportedBipolarTransversalPairs;
        setup.setupicognos20BipolarTransversal();
        break;
    }
    setup._icognos20Setup = s;
}

icognosSetup currenticognosSetup()
{    
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._icognos20Setup;
}

void ElectrodePlacement::setupicognos20Standard()
{
    _edb.setupicognos20Standard();
}

void ElectrodePlacement::setupicognos20BipolarLongitudinal()
{
    _edb.setupicognos20BipolarLongitudinal();
}

void ElectrodePlacement::setupicognos20BipolarTransversal()
{
    _edb.setupicognos20BipolarTransversal();
}

void ElectrodePlacement::setupicognos32()
{
    _edb.setupicognos32();
}

void ElectrodePlacement::ElectrodePlacementsDict::setEOGPlacement(bool enabled)
{
    if(enabled)
    {
        setElectrodePlacement(ElectrodeChannel::CH17, ElectrodePosition::EOG1);
        setElectrodePlacement(ElectrodeChannel::CH13, ElectrodePosition::EOG2);
    }
    else
    {
        setElectrodePlacement(ElectrodeChannel::CH17, ElectrodePosition::Fp1);
        setElectrodePlacement(ElectrodeChannel::CH13, ElectrodePosition::Fp2);
    }
}

void setEOGPlacement(bool enabled)
{    
    QMutexLocker locker(&setup._electrodeSetupMutex);
    switch(setup._dev)
    {
    case DeviceType::ENOBIO20:
    case DeviceType::ENOBIO32:
    {
        setup._eogEnabled = enabled;
        setup._edb.setEOGPlacement(enabled);
    }
    default:
        break;
    }
}

bool isEOGEnabled()
{
    bool found = (getPositionChannel(ElectrodePosition::EOG1) != ElectrodeChannel::invalidChannel)
            || (getPositionChannel(ElectrodePosition::EOG2) != ElectrodeChannel::invalidChannel);
    {
        QMutexLocker locker(&setup._electrodeSetupMutex);
        setup._eogEnabled = found;
        return setup._eogEnabled;
    }
}

const QString& getChannelName(unsigned int channelID)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return ElectrodeChannel::isChannel(channelID) ? setup._edb.electrodeChannelNames[channelID] : ElectrodeChannel::invalidStr;
}

int getChannelID(const QString& e)
{
    QString eUpper = e.toUpper();
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._edb.electrodeChannelIDs.contains(eUpper) ? setup._edb.electrodeChannelIDs.value(eUpper) : INVALID_ELECTRODE;
}


const QVector<QString>& getChannelNames()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._edb.deviceElectrodeChannelNames;
}

const QVector<QString> getChannelNames(unsigned int total)
{
    {
        QMutexLocker locker(&setup._electrodeSetupMutex);
        if(int(total) < setup._edb.electrodeChannelNames.size())
        {
            return setup._edb.electrodeChannelNames.mid(0, total);
        }
    }
    return getChannelNames();
}

const QVector<unsigned int>&    getChannelIDs()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._edb.deviceElectrodeChannelIDs;
}

const QVector<unsigned int>  getChannelIDs(unsigned int total)
{
    {
        QMutexLocker locker(&setup._electrodeSetupMutex);
        if(int(total) < setup._edb.electrodeChannelIDsSorted.size())
        {
            return setup._edb.electrodeChannelIDsSorted.mid(0, total);
        }
    }
    return getChannelIDs();
}

const QString& getOldElectrodeName(unsigned int oldElectrodePosition)
{
    return OldElectrodePosition::isOldPosition(oldElectrodePosition) ? setup._edb.oldElectrodePositionNames[oldElectrodePosition] : OldElectrodePosition::invalidStr;
}
/* not allowed
int ElectrodePlacement::getOldElectrodeID(const QString &e) const
{
    QString eUpper = e.toUpper();
    return _edb.oldElectrodePositionIDs.contains(eUpper) ? _edb.oldElectrodePositionIDs.value(eUpper) : -1;
}
*/
const QString& getPositionName(unsigned int positionID)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return ElectrodePosition::isPosition(positionID) ? setup._edb.electrodePositionNames[positionID] : ElectrodePosition::invalidStr;
}

int getPositionID(const QString& positionName)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    QString eUpper = positionName.toUpper();
    if(setup._edb.electrodePositionIDs.contains(eUpper))
    {
        return setup._edb.electrodePositionIDs.value(eUpper);
    }
    return  INVALID_ELECTRODE;
}

const QVector<QString>&  getPositionNames()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._edb.electrodePositionNamesSorted;
}

const QVector<unsigned int>& getPositionIDs()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._edb.electrodePositionIDsSorted;
}

const QVector<QString>&  getPositionNames(DeviceType::SupportedNICDevice dev,/* icognosSetup config,*/ ElectrodeSort s, bool withEOG)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    switch(s)
    {
    case SortedByName:
        switch(dev)
        {
        case DeviceType::ENOBIO20:
            //switch(config)
            if(withEOG)
            {
                return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20StdEOG];
            }
            else
            {
                //case icognosStd:
                return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20Std];
            //    break;
                /*
                case E20BipolarLong:
                return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20BipLong];
                break;
                case E20BipolarTrans:
                return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20BipTrans];
                break;
                */
            }
            break;
        case DeviceType::ENOBIO32:
            if(withEOG)
            {
                return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32EOG];
            }
            else
            {
                return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32];
            }
            break;
        default:
        //case DeviceType::STARSTIM:
        //case DeviceType::ENOBIO8:
            return setup._edb.devicePositionNamesByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigFree];
            break;
        }
        break;
    case SortedByScalp:
        switch(dev)
        {
        case DeviceType::ENOBIO20:
            if(withEOG)//switch(config)
            {
                //case icognosStd:
                return setup._edb.devicePositionNamesByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20StdEOG];
            }
            else
            {
                return setup._edb.devicePositionNamesByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20Std];
            }
            break;
        case DeviceType::ENOBIO32:
            if(withEOG)
            {
                return setup._edb.devicePositionNamesByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32EOG];
            }
            else
            {
                return setup._edb.devicePositionNamesByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32];
            }
            break;
        default:
        //case DeviceType::STARSTIM:
        //case DeviceType::ENOBIO8:
            return setup._edb.devicePositionNamesByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigFree];
            break;
        }
        break;
    }
}

const QVector<unsigned int>& getPositionIDs(DeviceType::SupportedNICDevice dev,/* icognosSetup config,*/ ElectrodeSort s, bool withEOG)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    switch(s)
    {
    case SortedByName:
        switch(dev)
        {
        case DeviceType::ENOBIO20:
            //switch(config)
            if(withEOG)
            {
                //case icognosStd:
                return setup._edb.devicePositionIDsByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20StdEOG];
            }
            else
            {
                return setup._edb.devicePositionIDsByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20Std];
            }
            break;
        case DeviceType::ENOBIO32:
            if(withEOG)
            {
                return setup._edb.devicePositionIDsByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32EOG];
            }
            else
            {
                return setup._edb.devicePositionIDsByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32];
            }
            break;
        default:
        //case DeviceType::STARSTIM:
        //case DeviceType::ENOBIO8:
            return setup._edb.devicePositionIDsByName[ElectrodePlacement::ElectrodePlacementsDict::ConfigFree];
            break;
        }
        break;
    case SortedByScalp:
        switch(dev)
        {
        case DeviceType::ENOBIO20:
            //switch(config)
            if(withEOG)
            {
                //case icognosStd:
                return setup._edb.devicePositionIDsByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20StdEOG];
            }
            else
            {
                return setup._edb.devicePositionIDsByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE20Std];
            }
            break;
        case DeviceType::ENOBIO32:
            if(withEOG)
            {
                return setup._edb.devicePositionIDsByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32EOG];
            }
            else
            {
                return setup._edb.devicePositionIDsByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigE32];
            }
            break;
        default:
        //case DeviceType::STARSTIM:
        //case DeviceType::ENOBIO8:
            return setup._edb.devicePositionIDsByScalp[ElectrodePlacement::ElectrodePlacementsDict::ConfigFree];
            break;
        }
        break;
    }
}
/*
const QVector<QString>&  getBipolarLongitudinalNames()
{
    return setup._edb.bipolarLongPairNames;
}

const QVector<unsigned int>& getBipolarLongitudinalIDs()
{
    return getChannelIDs(ElectrodePosition::BipolarLongitudinal::supportedBipolarLongitudinalPairs);
}

const QVector<QString>&  getBipolarTransversalNames()
{
    return setup._edb.bipolarTransPairNames;
}

const QVector<unsigned int>& getBipolarTransversalIDs()
{
    return getChannelIDs(ElectrodePosition::BipolarTransversal::supportedBipolarTransversalPairs);
}
*/
const QVector<QString>  getPlacementNames()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    QVector<QString> names;
    switch(setup._icognos20Setup)
    {
    case icognosStd:
        names.resize(setup._edb.currentElectrodePlacement.size());
        for(int e = 0; e < setup._edb.currentElectrodePlacement.size(); e++)
        {
            if(ElectrodeChannel::isChannel(setup._edb.currentElectrodePlacement[e]))
            {
                names[e] = setup._edb.electrodeChannelNames[e];
            }
            else
            {    // if(ElectrodePosition::isPosition(setup._edb.currentElectrodePlacement[e]))
                names[e] = setup._edb.electrodePositionNames[setup._edb.currentElectrodePlacement[e]];
            }
        }
        break;
    case E20BipolarLong:
        {
            names = setup._edb.bipolarLongPairNames.values().toVector();
        }
        break;
    case E20BipolarTrans:
        {
            names = setup._edb.bipolarTransPairNames.values().toVector();
        }
        break;
    }
    return names;
}

const QVector<unsigned int>&  getPlacementIDs()
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    return setup._edb.currentElectrodePlacement;
}

QString getPlacementName(unsigned int channel)
{
    QMutexLocker locker(&setup._electrodeSetupMutex);
    QString name;
    if(setup.deviceHasChannel(channel))
    {
        if(ElectrodeChannel::isChannel(setup._edb.currentElectrodePlacement[channel]))
        {
            name = setup._edb.electrodeChannelNames[channel];
        }
        else if(setup._icognos20Setup == icognosStd)
        {    // if(ElectrodePosition::isPosition(setup._edb.currentElectrodePlacement[e]))
            name = setup._edb.electrodePositionNames[setup._edb.currentElectrodePlacement[channel]];
        }
        else //Special icognos20 configuration
        {
            if(setup._icognos20Setup == E20BipolarLong)
            {// if(ElectrodePosition::isBipolarLongitudinal(setup._edb.currentElectrodePlacement[e]))
                name = setup._edb.bipolarLongPairNames[setup._edb.currentElectrodePlacement[channel]];
            }
            else// if(setup._icognos20 == E20BipolarTransversal)
            {// if(ElectrodePosition::isTransBip(setup._edb.currentElectrodePlacement[e]))
                name = setup._edb.bipolarTransPairNames[setup._edb.currentElectrodePlacement[channel]];
            }
        }
    }
    return name;
}

int getPlacementID(const QString& channelName)
{
    QString eUpper = channelName.toUpper();
    int chan = getChannelID(eUpper);
    if(chan != INVALID_ELECTRODE)
    {
        QMutexLocker locker(&setup._electrodeSetupMutex);
        return setup._edb.currentElectrodePlacement[chan];
    }
    return chan;
}

void ElectrodePlacement::ElectrodePlacementsDict::sortByName(DeviceType::SupportedNICDevice dev
                                                             /*, icognosSetup s*/, bool eog)
{
    //electrodePositionIDsSorted = electrodePositionIDsByName;
    //electrodePositionNamesSorted = electrodePositionNamesByName;
    switch(dev)
    {
    case DeviceType::ENOBIO8:
    case DeviceType::STARSTIM:
        electrodePositionIDsSorted = devicePositionIDsByName[ConfigFree];
        electrodePositionNamesSorted = devicePositionNamesByName[ConfigFree];
        break;
    case DeviceType::ENOBIO20:
        /*switch(s)
        {
        case icognosStd:
        */
            if(eog)
            {
                electrodePositionIDsSorted = devicePositionIDsByName[ConfigE20StdEOG];
                electrodePositionNamesSorted = devicePositionNamesByName[ConfigE20StdEOG];
            }
            else
            {
                electrodePositionIDsSorted = devicePositionIDsByName[ConfigE20Std];
                electrodePositionNamesSorted = devicePositionNamesByName[ConfigE20Std];
            }
        break;
    case DeviceType::ENOBIO32:
        if(eog)
        {
            electrodePositionIDsSorted = devicePositionIDsByName[ConfigE32EOG];
            electrodePositionNamesSorted = devicePositionNamesByName[ConfigE32EOG];
        }
        else
        {
            electrodePositionIDsSorted = devicePositionIDsByName[ConfigE32];
            electrodePositionNamesSorted = devicePositionNamesByName[ConfigE32];
        }
        break;
    default:
        break;
    }
}

void ElectrodePlacement::ElectrodePlacementsDict::sortByScalp(DeviceType::SupportedNICDevice dev
                                                              /*, icognosSetup s*/, bool eog)
{
    //electrodePositionIDsSorted = electrodePositionIDsByScalp;
    //electrodePositionNamesSorted = electrodePositionNamesByScalp;
    switch(dev)
    {
    case DeviceType::ENOBIO8:
    case DeviceType::STARSTIM:
        electrodePositionIDsSorted = devicePositionIDsByScalp[ConfigFree];
        electrodePositionNamesSorted = devicePositionNamesByScalp[ConfigFree];
        break;
    case DeviceType::ENOBIO20:
        /*
        switch(s)
        {
        case icognosStd:
        */  if(eog)
            {
                electrodePositionIDsSorted = devicePositionIDsByScalp[ConfigE20StdEOG];
                electrodePositionNamesSorted = devicePositionNamesByScalp[ConfigE20StdEOG];
            }
            else
            {
                electrodePositionIDsSorted = devicePositionIDsByScalp[ConfigE20Std];
                electrodePositionNamesSorted = devicePositionNamesByScalp[ConfigE20Std];
            }
        break;
    case DeviceType::ENOBIO32:
        if(eog)
        {
            electrodePositionIDsSorted = devicePositionIDsByScalp[ConfigE32EOG];
            electrodePositionNamesSorted = devicePositionNamesByScalp[ConfigE32EOG];
        }
        else
        {
            electrodePositionIDsSorted = devicePositionIDsByScalp[ConfigE32];
            electrodePositionNamesSorted = devicePositionNamesByScalp[ConfigE32];
        }
        break;
    default:
        break;
    }
}

ElectrodePlacement::ElectrodePlacementsDict::ElectrodePlacementsDict()
    //: electrodePositionIDsSorted(electrodeChannelIDsSorted)
    //, electrodePositionNamesSorted(electrodeChannelNames)
{
    buildDictionaries();
}

void ElectrodePlacement::ElectrodePlacementsDict::buildChannelDict()
{
    electrodeChannelNames.resize(ElectrodeChannel::supportedChannels);
    electrodeChannelNames[ElectrodeChannel::CH1 ] =  "Ch1" ;
    electrodeChannelNames[ElectrodeChannel::CH2 ] =  "Ch2" ;
    electrodeChannelNames[ElectrodeChannel::CH3 ] =  "Ch3" ;
    electrodeChannelNames[ElectrodeChannel::CH4 ] =  "Ch4" ;
    electrodeChannelNames[ElectrodeChannel::CH5 ] =  "Ch5" ;
    electrodeChannelNames[ElectrodeChannel::CH6 ] =  "Ch6" ;
    electrodeChannelNames[ElectrodeChannel::CH7 ] =  "Ch7" ;
    electrodeChannelNames[ElectrodeChannel::CH8 ] =  "Ch8" ;
    electrodeChannelNames[ElectrodeChannel::CH9 ] =  "Ch9" ;
    electrodeChannelNames[ElectrodeChannel::CH10] =  "Ch10";
    electrodeChannelNames[ElectrodeChannel::CH11] =  "Ch11";
    electrodeChannelNames[ElectrodeChannel::CH12] =  "Ch12";
    electrodeChannelNames[ElectrodeChannel::CH13] =  "Ch13";
    electrodeChannelNames[ElectrodeChannel::CH14] =  "Ch14";
    electrodeChannelNames[ElectrodeChannel::CH15] =  "Ch15";
    electrodeChannelNames[ElectrodeChannel::CH16] =  "Ch16";
    electrodeChannelNames[ElectrodeChannel::CH17] =  "Ch17";
    electrodeChannelNames[ElectrodeChannel::CH18] =  "Ch18";
    electrodeChannelNames[ElectrodeChannel::CH19] =  "Ch19";
    electrodeChannelNames[ElectrodeChannel::CH20] =  "Ch20";
    electrodeChannelNames[ElectrodeChannel::CH21] =  "Ch21";
    electrodeChannelNames[ElectrodeChannel::CH22] =  "Ch22";
    electrodeChannelNames[ElectrodeChannel::CH23] =  "Ch23";
    electrodeChannelNames[ElectrodeChannel::CH24] =  "Ch24";
    electrodeChannelNames[ElectrodeChannel::CH25] =  "Ch25";
    electrodeChannelNames[ElectrodeChannel::CH26] =  "Ch26";
    electrodeChannelNames[ElectrodeChannel::CH27] =  "Ch27";
    electrodeChannelNames[ElectrodeChannel::CH28] =  "Ch28";
    electrodeChannelNames[ElectrodeChannel::CH29] =  "Ch29";
    electrodeChannelNames[ElectrodeChannel::CH30] =  "Ch30";
    electrodeChannelNames[ElectrodeChannel::CH31] =  "Ch31";
    electrodeChannelNames[ElectrodeChannel::CH32] =  "Ch32";

    electrodeChannelIDs.insert("CH1" , ElectrodeChannel::CH1);
    electrodeChannelIDs.insert("CH2" , ElectrodeChannel::CH2);
    electrodeChannelIDs.insert("CH3" , ElectrodeChannel::CH3);
    electrodeChannelIDs.insert("CH4" , ElectrodeChannel::CH4);
    electrodeChannelIDs.insert("CH5" , ElectrodeChannel::CH5);
    electrodeChannelIDs.insert("CH6" , ElectrodeChannel::CH6);
    electrodeChannelIDs.insert("CH7" , ElectrodeChannel::CH7);
    electrodeChannelIDs.insert("CH8" , ElectrodeChannel::CH8);
    electrodeChannelIDs.insert("CH9" , ElectrodeChannel::CH9);
    electrodeChannelIDs.insert("CH10",ElectrodeChannel::CH10);
    electrodeChannelIDs.insert("CH12",ElectrodeChannel::CH11);
    electrodeChannelIDs.insert("CH12",ElectrodeChannel::CH12);
    electrodeChannelIDs.insert("CH13",ElectrodeChannel::CH13);
    electrodeChannelIDs.insert("CH14",ElectrodeChannel::CH14);
    electrodeChannelIDs.insert("CH15",ElectrodeChannel::CH15);
    electrodeChannelIDs.insert("CH16",ElectrodeChannel::CH16);
    electrodeChannelIDs.insert("CH17",ElectrodeChannel::CH17);
    electrodeChannelIDs.insert("CH18",ElectrodeChannel::CH18);
    electrodeChannelIDs.insert("CH19",ElectrodeChannel::CH19);
    electrodeChannelIDs.insert("CH20",ElectrodeChannel::CH20);
    electrodeChannelIDs.insert("CH21",ElectrodeChannel::CH21);
    electrodeChannelIDs.insert("CH22",ElectrodeChannel::CH22);
    electrodeChannelIDs.insert("CH23",ElectrodeChannel::CH23);
    electrodeChannelIDs.insert("CH24",ElectrodeChannel::CH24);
    electrodeChannelIDs.insert("CH25",ElectrodeChannel::CH25);
    electrodeChannelIDs.insert("CH26",ElectrodeChannel::CH26);
    electrodeChannelIDs.insert("CH27",ElectrodeChannel::CH27);
    electrodeChannelIDs.insert("CH28",ElectrodeChannel::CH28);
    electrodeChannelIDs.insert("CH29",ElectrodeChannel::CH29);
    electrodeChannelIDs.insert("CH30",ElectrodeChannel::CH30);
    electrodeChannelIDs.insert("CH31",ElectrodeChannel::CH31);
    electrodeChannelIDs.insert("CH32",ElectrodeChannel::CH32);

    electrodeChannelIDsSorted.resize(ElectrodeChannel::supportedChannels);
    for(unsigned int i = 0; i < ElectrodeChannel::supportedChannels; i++)
    {
        electrodeChannelIDsSorted[i] = (unsigned int)(ElectrodeChannel::CH1)+i;
    }

}

void ElectrodePlacement::ElectrodePlacementsDict::buildLegacyPositionDict()
{
    oldElectrodePositionNames[OldElectrodePosition::Fpz ]  =  "Fpz";
    oldElectrodePositionNames[OldElectrodePosition::Fp1 ]  =  "Fp1";
    oldElectrodePositionNames[OldElectrodePosition::Fp2 ]  =  "Fp2";
    oldElectrodePositionNames[OldElectrodePosition::AF7 ]  =  "AF7";
    oldElectrodePositionNames[OldElectrodePosition::AF8 ]  =  "AF8";
    oldElectrodePositionNames[OldElectrodePosition::F7  ]  =   "F7";
    oldElectrodePositionNames[OldElectrodePosition::F3  ]  =   "F3";
    oldElectrodePositionNames[OldElectrodePosition::Fz  ]  =   "Fz";
    oldElectrodePositionNames[OldElectrodePosition::F4  ]  =   "F4";
    oldElectrodePositionNames[OldElectrodePosition::F8  ]  =   "F8";
    oldElectrodePositionNames[OldElectrodePosition::T7  ]  =   "T7";
    oldElectrodePositionNames[OldElectrodePosition::C3  ]  =   "C3";
    oldElectrodePositionNames[OldElectrodePosition::C1  ]  =   "C1";
    oldElectrodePositionNames[OldElectrodePosition::Cz  ]  =   "Cz";
    oldElectrodePositionNames[OldElectrodePosition::C2  ]  =   "C2";
    oldElectrodePositionNames[OldElectrodePosition::C4  ]  =   "C4";
    oldElectrodePositionNames[OldElectrodePosition::T8  ]  =   "T8";
    oldElectrodePositionNames[OldElectrodePosition::P7  ]  =   "P7";
    oldElectrodePositionNames[OldElectrodePosition::P3  ]  =   "P3";
    oldElectrodePositionNames[OldElectrodePosition::Pz  ]  =   "Pz";
    oldElectrodePositionNames[OldElectrodePosition::P4  ]  =   "P4";
    oldElectrodePositionNames[OldElectrodePosition::P8  ]  =   "P8";
    oldElectrodePositionNames[OldElectrodePosition::PO7 ]  =  "PO7";
    oldElectrodePositionNames[OldElectrodePosition::PO8 ]  =  "PO8";
    oldElectrodePositionNames[OldElectrodePosition::O1  ]  =   "O1";
    oldElectrodePositionNames[OldElectrodePosition::Oz  ]  =   "Oz";
    oldElectrodePositionNames[OldElectrodePosition::O2  ]  =   "O2";
    oldElectrodePositionNames[OldElectrodePosition::EOG1]  = "EOG1";
    oldElectrodePositionNames[OldElectrodePosition::EOG2]  = "EOG2";

    //Setup old electrode order (can not be changed as it is used only
    //for compatibility with old stimulation templates, which did not allow
    //different orderings. This is removed as it should not be needed, since
    //we will convert old IDs to new IDs when working
    /*
    oldElectrodePositionIDs.insert("FPZ" , OldElectrodePosition::Fpz);
    oldElectrodePositionIDs.insert("FP1" , OldElectrodePosition::Fp1);
    oldElectrodePositionIDs.insert("FP2" , OldElectrodePosition::Fp2);
    oldElectrodePositionIDs.insert("AF7" , OldElectrodePosition::AF7);
    oldElectrodePositionIDs.insert("AF8" , OldElectrodePosition::AF8);
    oldElectrodePositionIDs.insert("F7"  ,  OldElectrodePosition::F7);
    oldElectrodePositionIDs.insert("F3"  ,  OldElectrodePosition::F3);
    oldElectrodePositionIDs.insert("FZ"  ,  OldElectrodePosition::Fz);
    oldElectrodePositionIDs.insert("F4"  ,  OldElectrodePosition::F4);
    oldElectrodePositionIDs.insert("F8"  ,  OldElectrodePosition::F8);
    oldElectrodePositionIDs.insert("T7"  ,  OldElectrodePosition::T7);
    oldElectrodePositionIDs.insert("C3"  ,  OldElectrodePosition::C3);
    oldElectrodePositionIDs.insert("C1"  ,  OldElectrodePosition::C1);
    oldElectrodePositionIDs.insert("CZ"  ,  OldElectrodePosition::Cz);
    oldElectrodePositionIDs.insert("C2"  ,  OldElectrodePosition::C2);
    oldElectrodePositionIDs.insert("C4"  ,  OldElectrodePosition::C4);
    oldElectrodePositionIDs.insert("T8"  ,  OldElectrodePosition::T8);
    oldElectrodePositionIDs.insert("P7"  ,  OldElectrodePosition::P7);
    oldElectrodePositionIDs.insert("P3"  ,  OldElectrodePosition::P3);
    oldElectrodePositionIDs.insert("PZ"  ,  OldElectrodePosition::Pz);
    oldElectrodePositionIDs.insert("P4"  ,  OldElectrodePosition::P4);
    oldElectrodePositionIDs.insert("P8"  ,  OldElectrodePosition::P8);
    oldElectrodePositionIDs.insert("PO7" , OldElectrodePosition::PO7);
    oldElectrodePositionIDs.insert("PO8" , OldElectrodePosition::PO8);
    oldElectrodePositionIDs.insert("O1"  ,  OldElectrodePosition::O1);
    oldElectrodePositionIDs.insert("OZ"  ,  OldElectrodePosition::Oz);
    oldElectrodePositionIDs.insert("O2"  ,  OldElectrodePosition::O2);
    oldElectrodePositionIDs.insert("EOG1",OldElectrodePosition::EOG1);
    oldElectrodePositionIDs.insert("EOG2",OldElectrodePosition::EOG2);
    */
}

void ElectrodePlacement::ElectrodePlacementsDict::buildPositionDict()
{
    electrodePositionNames[ElectrodePosition::Fp1 ]= "Fp1";
    electrodePositionNames[ElectrodePosition::Fpz ]= "Fpz";
    electrodePositionNames[ElectrodePosition::Fp2 ]= "Fp2";
    electrodePositionNames[ElectrodePosition::AF7 ]= "AF7";
    electrodePositionNames[ElectrodePosition::AF3 ]= "AF3";
    electrodePositionNames[ElectrodePosition::AF4 ]= "AF4";
    electrodePositionNames[ElectrodePosition::AF8 ]= "AF8";
    electrodePositionNames[ElectrodePosition::F7  ]=  "F7";
    electrodePositionNames[ElectrodePosition::F3  ]=  "F3";
    electrodePositionNames[ElectrodePosition::Fz  ]=  "Fz";
    electrodePositionNames[ElectrodePosition::F4  ]=  "F4";
    electrodePositionNames[ElectrodePosition::F8  ]=  "F8";
    electrodePositionNames[ElectrodePosition::FC5 ]= "FC5";
    electrodePositionNames[ElectrodePosition::FC1 ]= "FC1";
    electrodePositionNames[ElectrodePosition::FC2 ]= "FC2";
    electrodePositionNames[ElectrodePosition::FC6 ]= "FC6";
    electrodePositionNames[ElectrodePosition::T7  ]=  "T7";
    electrodePositionNames[ElectrodePosition::C3  ]=  "C3";
    electrodePositionNames[ElectrodePosition::C1  ]=  "C1";
    electrodePositionNames[ElectrodePosition::Cz  ]=  "Cz";
    electrodePositionNames[ElectrodePosition::C2  ]=  "C2";
    electrodePositionNames[ElectrodePosition::C4  ]=  "C4";
    electrodePositionNames[ElectrodePosition::T8  ]=  "T8";
    electrodePositionNames[ElectrodePosition::CP5 ]= "CP5";
    electrodePositionNames[ElectrodePosition::CP1 ]= "CP1";
    electrodePositionNames[ElectrodePosition::CP2 ]= "CP2";
    electrodePositionNames[ElectrodePosition::CP6 ]= "CP6";
    electrodePositionNames[ElectrodePosition::P7  ]=  "P7";
    electrodePositionNames[ElectrodePosition::P3  ]=  "P3";
    electrodePositionNames[ElectrodePosition::Pz  ]=  "Pz";
    electrodePositionNames[ElectrodePosition::P4  ]=  "P4";
    electrodePositionNames[ElectrodePosition::P8  ]=  "P8";
    electrodePositionNames[ElectrodePosition::PO7 ]= "PO7";
    electrodePositionNames[ElectrodePosition::PO3 ]= "PO3";
    electrodePositionNames[ElectrodePosition::PO4 ]= "PO4";
    electrodePositionNames[ElectrodePosition::PO8 ]= "PO8";
    electrodePositionNames[ElectrodePosition::O1  ]=  "O1";
    electrodePositionNames[ElectrodePosition::Oz  ]=  "Oz";
    electrodePositionNames[ElectrodePosition::O2  ]=  "O2";

    electrodePositionNames[ElectrodePosition::EOG1]="EOG1";
    electrodePositionNames[ElectrodePosition::EOG2]="EOG2";
    electrodePositionNames[ElectrodePosition::ECG ]= "ECG";
    electrodePositionNames[ElectrodePosition::EXT ]= "EXT";

    electrodePositionIDs["FP1"] = ElectrodePosition::Fp1;
    electrodePositionIDs["FPZ"] = ElectrodePosition::Fpz;
    electrodePositionIDs["FP2"] = ElectrodePosition::Fp2;
    electrodePositionIDs["AF7"] = ElectrodePosition::AF7;
    electrodePositionIDs["AF3"] = ElectrodePosition::AF3;
    electrodePositionIDs["AF4"] = ElectrodePosition::AF4;
    electrodePositionIDs["AF8"] = ElectrodePosition::AF8;
    electrodePositionIDs["F7"]  =  ElectrodePosition::F7;
    electrodePositionIDs["F3"]  =  ElectrodePosition::F3;
    electrodePositionIDs["FZ"]  =  ElectrodePosition::Fz;
    electrodePositionIDs["F4"]  =  ElectrodePosition::F4;
    electrodePositionIDs["F8"]  =  ElectrodePosition::F8;
    electrodePositionIDs["FC5"] = ElectrodePosition::FC5;
    electrodePositionIDs["FC1"] = ElectrodePosition::FC1;
    electrodePositionIDs["FC2"] = ElectrodePosition::FC2;
    electrodePositionIDs["FC6"] = ElectrodePosition::FC6;
    electrodePositionIDs["T7"]  =  ElectrodePosition::T7;
    electrodePositionIDs["C3"]  =  ElectrodePosition::C3;
    electrodePositionIDs["C1"]  =  ElectrodePosition::C1;
    electrodePositionIDs["CZ"]  =  ElectrodePosition::Cz;
    electrodePositionIDs["C2"]  =  ElectrodePosition::C2;
    electrodePositionIDs["C4"]  =  ElectrodePosition::C4;
    electrodePositionIDs["T8"]  =  ElectrodePosition::T8;
    electrodePositionIDs["CP5"] = ElectrodePosition::CP5;
    electrodePositionIDs["CP1"] = ElectrodePosition::CP1;
    electrodePositionIDs["CP2"] = ElectrodePosition::CP2;
    electrodePositionIDs["CP6"] = ElectrodePosition::CP6;
    electrodePositionIDs["P7"]  =  ElectrodePosition::P7;
    electrodePositionIDs["P3"]  =  ElectrodePosition::P3;
    electrodePositionIDs["PZ"]  =  ElectrodePosition::Pz;
    electrodePositionIDs["P4"]  =  ElectrodePosition::P4;
    electrodePositionIDs["P8"]  =  ElectrodePosition::P8;
    electrodePositionIDs["PO7"] = ElectrodePosition::PO7;
    electrodePositionIDs["PO3"] = ElectrodePosition::PO3;
    electrodePositionIDs["PO4"] = ElectrodePosition::PO4;
    electrodePositionIDs["PO8"] = ElectrodePosition::PO8;
    electrodePositionIDs["O1"]  =  ElectrodePosition::O1;
    electrodePositionIDs["OZ"]  =  ElectrodePosition::Oz;
    electrodePositionIDs["O2"]  =  ElectrodePosition::O2;

    electrodePositionIDs["EOG1"]=ElectrodePosition::EOG1;
    electrodePositionIDs["EOG2"]=ElectrodePosition::EOG2;
    electrodePositionIDs["ECG"] = ElectrodePosition::ECG;
    electrodePositionIDs["EXT"] = ElectrodePosition::EXT;
}

void ElectrodePlacement::ElectrodePlacementsDict::buildPositionByScalp()
{
    //For the keys on the Electrode dict, we will only use capital letters
    //so searches will be performed with toUpper, ignoring variations
    QVector<unsigned int> electrodePositionIDsByScalp(ElectrodePosition::supportedPositions);
    electrodePositionIDsByScalp[0]  = ElectrodePosition::Fp1;
    electrodePositionIDsByScalp[1]  = ElectrodePosition::Fpz;
    electrodePositionIDsByScalp[2]  = ElectrodePosition::Fp2;
    electrodePositionIDsByScalp[3]  = ElectrodePosition::AF7;
    electrodePositionIDsByScalp[4]  = ElectrodePosition::AF3;
    electrodePositionIDsByScalp[5]  = ElectrodePosition::AF4;
    electrodePositionIDsByScalp[6]  = ElectrodePosition::AF8;
    electrodePositionIDsByScalp[7]  =  ElectrodePosition::F7;
    electrodePositionIDsByScalp[8]  =  ElectrodePosition::F3;
    electrodePositionIDsByScalp[9]  =  ElectrodePosition::Fz;
    electrodePositionIDsByScalp[10] =  ElectrodePosition::F4;
    electrodePositionIDsByScalp[11] =  ElectrodePosition::F8;
    electrodePositionIDsByScalp[12] = ElectrodePosition::FC5;
    electrodePositionIDsByScalp[13] = ElectrodePosition::FC1;
    electrodePositionIDsByScalp[14] = ElectrodePosition::FC2;
    electrodePositionIDsByScalp[15] = ElectrodePosition::FC6;
    electrodePositionIDsByScalp[16] =  ElectrodePosition::T7;
    electrodePositionIDsByScalp[17] =  ElectrodePosition::C3;
    electrodePositionIDsByScalp[18] =  ElectrodePosition::C1;
    electrodePositionIDsByScalp[19] =  ElectrodePosition::Cz;
    electrodePositionIDsByScalp[20] =  ElectrodePosition::C2;
    electrodePositionIDsByScalp[21] =  ElectrodePosition::C4;
    electrodePositionIDsByScalp[22] =  ElectrodePosition::T8;
    electrodePositionIDsByScalp[23] = ElectrodePosition::CP5;
    electrodePositionIDsByScalp[24] = ElectrodePosition::CP1;
    electrodePositionIDsByScalp[25] = ElectrodePosition::CP2;
    electrodePositionIDsByScalp[26] = ElectrodePosition::CP6;
    electrodePositionIDsByScalp[27] =  ElectrodePosition::P7;
    electrodePositionIDsByScalp[28] =  ElectrodePosition::P3;
    electrodePositionIDsByScalp[29] =  ElectrodePosition::Pz;
    electrodePositionIDsByScalp[30] =  ElectrodePosition::P4;
    electrodePositionIDsByScalp[31] =  ElectrodePosition::P8;
    electrodePositionIDsByScalp[32] = ElectrodePosition::PO7;
    electrodePositionIDsByScalp[33] = ElectrodePosition::PO3;
    electrodePositionIDsByScalp[34] = ElectrodePosition::PO4;
    electrodePositionIDsByScalp[35] = ElectrodePosition::PO8;
    electrodePositionIDsByScalp[36] =  ElectrodePosition::O1;
    electrodePositionIDsByScalp[37] =  ElectrodePosition::Oz;
    electrodePositionIDsByScalp[38] =  ElectrodePosition::O2;

    electrodePositionIDsByScalp[39] =ElectrodePosition::EOG1;
    electrodePositionIDsByScalp[40] =ElectrodePosition::EOG2;
    electrodePositionIDsByScalp[41] = ElectrodePosition::ECG;
    electrodePositionIDsByScalp[42] = ElectrodePosition::EXT;

    QVector<QString> electrodePositionNamesByScalp(electrodePositionIDsByScalp.size());
    for(int i = 0; i < electrodePositionIDsByScalp.size(); i++)
        electrodePositionNamesByScalp[i] = electrodePositionNames.value(electrodePositionIDsByScalp[i]);
    devicePositionIDsByScalp[ConfigFree] = electrodePositionIDsByScalp;
    devicePositionNamesByScalp[ConfigFree] = electrodePositionNamesByScalp;
}

void ElectrodePlacement::ElectrodePlacementsDict::buildPositionByName()
{
    QVector<unsigned int> electrodePositionIDsByName(ElectrodePosition::supportedPositions);
    electrodePositionIDsByName[0]  = ElectrodePosition::AF3;
    electrodePositionIDsByName[1]  = ElectrodePosition::AF4;
    electrodePositionIDsByName[2]  = ElectrodePosition::AF7;
    electrodePositionIDsByName[3]  = ElectrodePosition::AF8;
    electrodePositionIDsByName[4]  =  ElectrodePosition::C1;
    electrodePositionIDsByName[5]  =  ElectrodePosition::C2;
    electrodePositionIDsByName[6]  =  ElectrodePosition::C3;
    electrodePositionIDsByName[7]  =  ElectrodePosition::C4;
    electrodePositionIDsByName[8]  = ElectrodePosition::CP1;
    electrodePositionIDsByName[9]  = ElectrodePosition::CP2;
    electrodePositionIDsByName[10] = ElectrodePosition::CP5;
    electrodePositionIDsByName[11] = ElectrodePosition::CP6;
    electrodePositionIDsByName[12] =  ElectrodePosition::Cz;
    electrodePositionIDsByName[13] =  ElectrodePosition::F3;
    electrodePositionIDsByName[14] =  ElectrodePosition::F4;
    electrodePositionIDsByName[15] =  ElectrodePosition::F7;
    electrodePositionIDsByName[16] =  ElectrodePosition::F8;
    electrodePositionIDsByName[17] = ElectrodePosition::FC1;
    electrodePositionIDsByName[18] = ElectrodePosition::FC2;
    electrodePositionIDsByName[19] = ElectrodePosition::FC5;
    electrodePositionIDsByName[20] = ElectrodePosition::FC6;
    electrodePositionIDsByName[21] = ElectrodePosition::Fp1;
    electrodePositionIDsByName[22] = ElectrodePosition::Fp2;
    electrodePositionIDsByName[23] = ElectrodePosition::Fpz;
    electrodePositionIDsByName[24] =  ElectrodePosition::Fz;
    electrodePositionIDsByName[25] =  ElectrodePosition::O1;
    electrodePositionIDsByName[26] =  ElectrodePosition::O2;
    electrodePositionIDsByName[27] =  ElectrodePosition::Oz;
    electrodePositionIDsByName[28] =  ElectrodePosition::P3;
    electrodePositionIDsByName[29] =  ElectrodePosition::P4;
    electrodePositionIDsByName[30] =  ElectrodePosition::P7;
    electrodePositionIDsByName[31] =  ElectrodePosition::P8;
    electrodePositionIDsByName[32] = ElectrodePosition::PO3;
    electrodePositionIDsByName[33] = ElectrodePosition::PO4;
    electrodePositionIDsByName[34] = ElectrodePosition::PO7;
    electrodePositionIDsByName[35] = ElectrodePosition::PO8;
    electrodePositionIDsByName[36] =  ElectrodePosition::Pz;
    electrodePositionIDsByName[37] =  ElectrodePosition::T7;
    electrodePositionIDsByName[38] =  ElectrodePosition::T8;

    electrodePositionIDsByName[39] = ElectrodePosition::ECG;
    electrodePositionIDsByName[40] =ElectrodePosition::EOG1;
    electrodePositionIDsByName[41] =ElectrodePosition::EOG2;
    electrodePositionIDsByName[42] = ElectrodePosition::EXT;

    QVector<QString> electrodePositionNamesByName(electrodePositionIDsByName.size());
    for(int i = 0; i < electrodePositionIDsByName.size(); i++)
        electrodePositionNamesByName[i] = electrodePositionNames.value(electrodePositionIDsByName[i]);
    devicePositionIDsByName[ConfigFree] = electrodePositionIDsByName;
    devicePositionNamesByName[ConfigFree] = electrodePositionNamesByName;
}

void ElectrodePlacement::ElectrodePlacementsDict::buildDevicePositionDict(ElectrodeConfiguration econf)
{
    const QVector<unsigned int>   &posIDsByName = devicePositionIDsByName[ConfigFree];
    const QVector<QString>   &posNamesByName = devicePositionNamesByName[ConfigFree];

    QVector<unsigned int>   eids = currentElectrodePlacement;

    QVector<unsigned int>   eidsSorted;
    QVector<QString>        enamesSorted;

    for(int i = 0; i < posIDsByName.size(); i++)
    {
        if(eids.contains(posIDsByName[i]))
        {
            eidsSorted.append(posIDsByName[i]);
            enamesSorted.append(posNamesByName[i]);
        }
    }
    devicePositionIDsByName[econf] = eidsSorted;
    devicePositionNamesByName[econf] = enamesSorted;

    eidsSorted.clear();
    enamesSorted.clear();

    const QVector<unsigned int>   &posIDsByScalp = devicePositionIDsByScalp[ConfigFree];
    const QVector<QString>   &posNamesByScalp = devicePositionNamesByScalp[ConfigFree];

    for(int i = 0; i < posIDsByScalp.size(); i++)
    {
        if(eids.contains(posIDsByScalp[i]))
        {
            eidsSorted.append(posIDsByScalp[i]);
            enamesSorted.append(posNamesByScalp[i]);
        }
    }
    devicePositionIDsByScalp[econf] = eidsSorted;
    devicePositionNamesByScalp[econf] = enamesSorted;
}

void ElectrodePlacement::ElectrodePlacementsDict::setupicognos20Standard()
{
    setElectrodePlacement(ElectrodeChannel::CH1 , ElectrodePosition::P7 );
    setElectrodePlacement(ElectrodeChannel::CH2 , ElectrodePosition::P4 );
    setElectrodePlacement(ElectrodeChannel::CH3 , ElectrodePosition::Cz );
    setElectrodePlacement(ElectrodeChannel::CH4 , ElectrodePosition::Pz );
    setElectrodePlacement(ElectrodeChannel::CH5 , ElectrodePosition::P3 );
    setElectrodePlacement(ElectrodeChannel::CH6 , ElectrodePosition::P8 );
    setElectrodePlacement(ElectrodeChannel::CH7 , ElectrodePosition::O1 );
    setElectrodePlacement(ElectrodeChannel::CH8 , ElectrodePosition::O2 );
    setElectrodePlacement(ElectrodeChannel::CH9 , ElectrodePosition::T8 );
    setElectrodePlacement(ElectrodeChannel::CH10, ElectrodePosition::F8 );
    setElectrodePlacement(ElectrodeChannel::CH11, ElectrodePosition::C4 );
    setElectrodePlacement(ElectrodeChannel::CH12, ElectrodePosition::F4 );
    setElectrodePlacement(ElectrodeChannel::CH13, ElectrodePosition::Fp2);
    setElectrodePlacement(ElectrodeChannel::CH14, ElectrodePosition::Fz );
    setElectrodePlacement(ElectrodeChannel::CH15, ElectrodePosition::C3 );
    setElectrodePlacement(ElectrodeChannel::CH16, ElectrodePosition::F3 );
    setElectrodePlacement(ElectrodeChannel::CH17, ElectrodePosition::Fp1);
    setElectrodePlacement(ElectrodeChannel::CH18, ElectrodePosition::T7 );
    setElectrodePlacement(ElectrodeChannel::CH19, ElectrodePosition::F7 );
    setElectrodePlacement(ElectrodeChannel::CH20, ElectrodePosition::EXT);
}

void ElectrodePlacement::ElectrodePlacementsDict::setupicognos20BipolarLongitudinal()
{
    setElectrodePlacement(ElectrodeChannel::CH1 , ElectrodePosition::BipolarLongitudinal::FP1_F7 );
    setElectrodePlacement(ElectrodeChannel::CH2 , ElectrodePosition::BipolarLongitudinal::F7_T7  );
    setElectrodePlacement(ElectrodeChannel::CH3 , ElectrodePosition::BipolarLongitudinal::T7_P7  );
    setElectrodePlacement(ElectrodeChannel::CH4 , ElectrodePosition::BipolarLongitudinal::P7_O1  );
    setElectrodePlacement(ElectrodeChannel::CH5 , ElectrodePosition::BipolarLongitudinal::FP2_F8 );
    setElectrodePlacement(ElectrodeChannel::CH6 , ElectrodePosition::BipolarLongitudinal::F8_T8  );
    setElectrodePlacement(ElectrodeChannel::CH7 , ElectrodePosition::BipolarLongitudinal::T8_P8  );
    setElectrodePlacement(ElectrodeChannel::CH8 , ElectrodePosition::BipolarLongitudinal::P8_O2  );
    setElectrodePlacement(ElectrodeChannel::CH9 , ElectrodePosition::BipolarLongitudinal::FP1_F3 );
    setElectrodePlacement(ElectrodeChannel::CH10, ElectrodePosition::BipolarLongitudinal::F3_C3  );
    setElectrodePlacement(ElectrodeChannel::CH11, ElectrodePosition::BipolarLongitudinal::C3_P3  );
    setElectrodePlacement(ElectrodeChannel::CH12, ElectrodePosition::BipolarLongitudinal::P3_O1  );
    setElectrodePlacement(ElectrodeChannel::CH13, ElectrodePosition::BipolarLongitudinal::FP2_F4 );
    setElectrodePlacement(ElectrodeChannel::CH14, ElectrodePosition::BipolarLongitudinal::F4_C4  );
    setElectrodePlacement(ElectrodeChannel::CH15, ElectrodePosition::BipolarLongitudinal::C4_P4  );
    setElectrodePlacement(ElectrodeChannel::CH16, ElectrodePosition::BipolarLongitudinal::P4_O2  );
    setElectrodePlacement(ElectrodeChannel::CH17, ElectrodePosition::BipolarLongitudinal::FZ_CZ  );
    setElectrodePlacement(ElectrodeChannel::CH18, ElectrodePosition::BipolarLongitudinal::CZ_PZ  );
    //setElectrodePlacement(ElectrodeChannel::CH19, ElectrodePosition::invalidElectrodePosition   );
    //setElectrodePlacement(ElectrodeChannel::CH20, ElectrodePosition::invalidElectrodePosition   );
}

void ElectrodePlacement::ElectrodePlacementsDict::setupicognos20BipolarTransversal()
{
    setElectrodePlacement(ElectrodeChannel::CH1 , ElectrodePosition::BipolarTransversal::F7_FP1  );
    setElectrodePlacement(ElectrodeChannel::CH2 , ElectrodePosition::BipolarTransversal::FP1_FP2 );
    setElectrodePlacement(ElectrodeChannel::CH3 , ElectrodePosition::BipolarTransversal::FP2_F8  );
    setElectrodePlacement(ElectrodeChannel::CH4 , ElectrodePosition::BipolarTransversal::F7_F3   );
    setElectrodePlacement(ElectrodeChannel::CH5 , ElectrodePosition::BipolarTransversal::F3_FZ   );
    setElectrodePlacement(ElectrodeChannel::CH6 , ElectrodePosition::BipolarTransversal::FZ_F4   );
    setElectrodePlacement(ElectrodeChannel::CH7 , ElectrodePosition::BipolarTransversal::F4_F8   );
    setElectrodePlacement(ElectrodeChannel::CH8 , ElectrodePosition::BipolarTransversal::T7_C3   );
    setElectrodePlacement(ElectrodeChannel::CH9 , ElectrodePosition::BipolarTransversal::C3_CZ   );
    setElectrodePlacement(ElectrodeChannel::CH10, ElectrodePosition::BipolarTransversal::CZ_C4   );
    setElectrodePlacement(ElectrodeChannel::CH11, ElectrodePosition::BipolarTransversal::C4_T8   );
    setElectrodePlacement(ElectrodeChannel::CH12, ElectrodePosition::BipolarTransversal::P5_P3   );
    setElectrodePlacement(ElectrodeChannel::CH13, ElectrodePosition::BipolarTransversal::P3_PZ   );
    setElectrodePlacement(ElectrodeChannel::CH14, ElectrodePosition::BipolarTransversal::PZ_P4   );
    setElectrodePlacement(ElectrodeChannel::CH15, ElectrodePosition::BipolarTransversal::P4_P8   );
    setElectrodePlacement(ElectrodeChannel::CH16, ElectrodePosition::BipolarTransversal::P7_01   );
    setElectrodePlacement(ElectrodeChannel::CH17, ElectrodePosition::BipolarTransversal::O1_O2   );
    setElectrodePlacement(ElectrodeChannel::CH18, ElectrodePosition::BipolarTransversal::O2_P8   );
    //setElectrodePlacement(ElectrodeChannel::CH19, ElectrodePosition::invalidElectrodePosition   );
    //setElectrodePlacement(ElectrodeChannel::CH20, ElectrodePosition::invalidElectrodePosition   );
}

void ElectrodePlacement::ElectrodePlacementsDict::setupicognos32()
{
    setElectrodePlacement(ElectrodeChannel::CH1 , ElectrodePosition::P7 );
    setElectrodePlacement(ElectrodeChannel::CH2 , ElectrodePosition::P4 );
    setElectrodePlacement(ElectrodeChannel::CH3 , ElectrodePosition::Cz );
    setElectrodePlacement(ElectrodeChannel::CH4 , ElectrodePosition::Pz );
    setElectrodePlacement(ElectrodeChannel::CH5 , ElectrodePosition::P3 );
    setElectrodePlacement(ElectrodeChannel::CH6 , ElectrodePosition::P8 );
    setElectrodePlacement(ElectrodeChannel::CH7 , ElectrodePosition::O1 );
    setElectrodePlacement(ElectrodeChannel::CH8 , ElectrodePosition::O2 );
    setElectrodePlacement(ElectrodeChannel::CH9 , ElectrodePosition::T8 );
    setElectrodePlacement(ElectrodeChannel::CH10, ElectrodePosition::F8 );
    setElectrodePlacement(ElectrodeChannel::CH11, ElectrodePosition::C4 );
    setElectrodePlacement(ElectrodeChannel::CH12, ElectrodePosition::F4 );
    setElectrodePlacement(ElectrodeChannel::CH13, ElectrodePosition::Fp2);
    setElectrodePlacement(ElectrodeChannel::CH14, ElectrodePosition::Fz );
    setElectrodePlacement(ElectrodeChannel::CH15, ElectrodePosition::C3 );
    setElectrodePlacement(ElectrodeChannel::CH16, ElectrodePosition::F3 );
    setElectrodePlacement(ElectrodeChannel::CH17, ElectrodePosition::Fp1);
    setElectrodePlacement(ElectrodeChannel::CH18, ElectrodePosition::T7 );
    setElectrodePlacement(ElectrodeChannel::CH19, ElectrodePosition::F7 );
    setElectrodePlacement(ElectrodeChannel::CH20, ElectrodePosition::Oz );
    setElectrodePlacement(ElectrodeChannel::CH21, ElectrodePosition::PO3);
    setElectrodePlacement(ElectrodeChannel::CH22, ElectrodePosition::AF3);
    setElectrodePlacement(ElectrodeChannel::CH23, ElectrodePosition::FC5);
    setElectrodePlacement(ElectrodeChannel::CH24, ElectrodePosition::FC1);
    setElectrodePlacement(ElectrodeChannel::CH25, ElectrodePosition::CP5);
    setElectrodePlacement(ElectrodeChannel::CH26, ElectrodePosition::CP1);
    setElectrodePlacement(ElectrodeChannel::CH27, ElectrodePosition::CP2);
    setElectrodePlacement(ElectrodeChannel::CH28, ElectrodePosition::CP6);
    setElectrodePlacement(ElectrodeChannel::CH29, ElectrodePosition::AF4);
    setElectrodePlacement(ElectrodeChannel::CH30, ElectrodePosition::FC2);
    setElectrodePlacement(ElectrodeChannel::CH31, ElectrodePosition::FC6);
    setElectrodePlacement(ElectrodeChannel::CH32, ElectrodePosition::PO4);
}

void ElectrodePlacement::ElectrodePlacementsDict::buildDevicePositionDict()
{
    currentElectrodePlacement.clear();
    currentElectrodePlacement.resize(20);

    setupicognos20Standard();
    buildDevicePositionDict(ConfigE20Std);

    //setupicognos20BipolarLongitudinal();
    //buildDevicePositionDict(ConfigE20BipLong);

    //setupicognos20BipolarTransversal();
    //buildDevicePositionDict(ConfigE20BipTrans);

    setEOGPlacement(true);
    buildDevicePositionDict(ConfigE20StdEOG);

    currentElectrodePlacement.clear();
    currentElectrodePlacement.resize(32);
    setupicognos32();
    buildDevicePositionDict(ConfigE32);

    setEOGPlacement(true);
    buildDevicePositionDict(ConfigE32EOG);
    setEOGPlacement(false);
}

void ElectrodePlacement::ElectrodePlacementsDict::buildBipolarLongitudinalDict()
{
    //bipolarLongPairNames.resize(ElectrodePosition::BipolarLongitudinal::supportedBipolarLongitudinalPairs);
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::FP1_F7]= "FP1-F7";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::F7_T7 ]=  "F7-T7";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::T7_P7 ]=  "T7-P7";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::P7_O1 ]=  "P7-O1";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::FP2_F8]= "FP2-F8";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::F8_T8 ]=  "F8-T8";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::T8_P8 ]=  "T8-P8";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::P8_O2 ]=  "P8-O2";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::FP1_F3]= "FP1-F3";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::F3_C3 ]=  "F3-C3";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::C3_P3 ]=  "C3-P3";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::P3_O1 ]=  "P3-O1";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::FP2_F4]= "FP2-F4";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::F4_C4 ]=  "F4-C4";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::C4_P4 ]=  "C4-P4";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::P4_O2 ]=  "P4-O2";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::FZ_CZ ]=  "FZ-CZ";
    bipolarLongPairNames[ElectrodePosition::BipolarLongitudinal::CZ_PZ ]=  "CZ-PZ";

    bipolarLongPairIDs["FP1-F7"] = ElectrodePosition::BipolarLongitudinal::FP1_F7;
    bipolarLongPairIDs["F7-T7" ] =  ElectrodePosition::BipolarLongitudinal::F7_T7;
    bipolarLongPairIDs["T7-P7" ] =  ElectrodePosition::BipolarLongitudinal::T7_P7;
    bipolarLongPairIDs["P7-O1" ] =  ElectrodePosition::BipolarLongitudinal::P7_O1;
    bipolarLongPairIDs["FP2-F8"] = ElectrodePosition::BipolarLongitudinal::FP2_F8;
    bipolarLongPairIDs["F8-T8" ] =  ElectrodePosition::BipolarLongitudinal::F8_T8;
    bipolarLongPairIDs["T8-P8" ] =  ElectrodePosition::BipolarLongitudinal::T8_P8;
    bipolarLongPairIDs["P8-O2" ] =  ElectrodePosition::BipolarLongitudinal::P8_O2;
    bipolarLongPairIDs["FP1-F3"] = ElectrodePosition::BipolarLongitudinal::FP1_F3;
    bipolarLongPairIDs["F3-C3" ] =  ElectrodePosition::BipolarLongitudinal::F3_C3;
    bipolarLongPairIDs["C3-P3" ] =  ElectrodePosition::BipolarLongitudinal::C3_P3;
    bipolarLongPairIDs["P3-O1" ] =  ElectrodePosition::BipolarLongitudinal::P3_O1;
    bipolarLongPairIDs["FP2-F4"] = ElectrodePosition::BipolarLongitudinal::FP2_F4;
    bipolarLongPairIDs["F4-C4" ] =  ElectrodePosition::BipolarLongitudinal::F4_C4;
    bipolarLongPairIDs["C4-P4" ] =  ElectrodePosition::BipolarLongitudinal::C4_P4;
    bipolarLongPairIDs["P4-O2" ] =  ElectrodePosition::BipolarLongitudinal::P4_O2;
    bipolarLongPairIDs["FZ-CZ" ] =  ElectrodePosition::BipolarLongitudinal::FZ_CZ;
    bipolarLongPairIDs["CZ-PZ" ] =  ElectrodePosition::BipolarLongitudinal::CZ_PZ;
}

void ElectrodePlacement::ElectrodePlacementsDict::buildTransBipolarDict()
{
    //bipolarTransPairNames.resize(ElectrodePosition::BipolarTransversal::supportedBipolarTransversalPairs);
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::F7_FP1 ] =  "F7-FP1";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::FP1_FP2] = "FP1-FP2";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::FP2_F8 ] =  "FP2-F8";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::F7_F3  ] =   "F7-F3";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::F3_FZ  ] =   "F3-FZ";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::FZ_F4  ] =   "FZ-F4";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::F4_F8  ] =   "F4-F8";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::T7_C3  ] =   "T7-C3";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::C3_CZ  ] =   "C3-CZ";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::CZ_C4  ] =   "CZ-C4";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::C4_T8  ] =   "C4-T8";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::P5_P3  ] =   "P5-P3";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::P3_PZ  ] =   "P3-PZ";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::PZ_P4  ] =   "PZ-P4";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::P4_P8  ] =   "P4-P8";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::P7_01  ] =   "P7-01";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::O1_O2  ] =   "O1-O2";
    bipolarTransPairNames[ElectrodePosition::BipolarTransversal::O2_P8  ] =   "O2-P8";

    bipolarTransPairIDs["F7-FP1"]  = ElectrodePosition::BipolarTransversal::F7_FP1 ;
    bipolarTransPairIDs["FP1-FP2"] = ElectrodePosition::BipolarTransversal::FP1_FP2;
    bipolarTransPairIDs["FP2-F8"]  = ElectrodePosition::BipolarTransversal::FP2_F8 ;
    bipolarTransPairIDs["F7-F3"]   = ElectrodePosition::BipolarTransversal::F7_F3  ;
    bipolarTransPairIDs["F3-FZ"]   = ElectrodePosition::BipolarTransversal::F3_FZ  ;
    bipolarTransPairIDs["FZ-F4"]   = ElectrodePosition::BipolarTransversal::FZ_F4  ;
    bipolarTransPairIDs["F4-F8"]   = ElectrodePosition::BipolarTransversal::F4_F8  ;
    bipolarTransPairIDs["T7-C3"]   = ElectrodePosition::BipolarTransversal::T7_C3  ;
    bipolarTransPairIDs["C3-CZ"]   = ElectrodePosition::BipolarTransversal::C3_CZ  ;
    bipolarTransPairIDs["CZ-C4"]   = ElectrodePosition::BipolarTransversal::CZ_C4  ;
    bipolarTransPairIDs["C4-T8"]   = ElectrodePosition::BipolarTransversal::C4_T8  ;
    bipolarTransPairIDs["P5-P3"]   = ElectrodePosition::BipolarTransversal::P5_P3  ;
    bipolarTransPairIDs["P3-PZ"]   = ElectrodePosition::BipolarTransversal::P3_PZ  ;
    bipolarTransPairIDs["PZ-P4"]   = ElectrodePosition::BipolarTransversal::PZ_P4  ;
    bipolarTransPairIDs["P4-P8"]   = ElectrodePosition::BipolarTransversal::P4_P8  ;
    bipolarTransPairIDs["P7-01"]   = ElectrodePosition::BipolarTransversal::P7_01  ;
    bipolarTransPairIDs["O1-O2"]   = ElectrodePosition::BipolarTransversal::O1_O2  ;
    bipolarTransPairIDs["O2-P8"]   = ElectrodePosition::BipolarTransversal::O2_P8  ;
}

void ElectrodePlacement::ElectrodePlacementsDict::buildDictionaries()
{
    buildChannelDict();
    buildLegacyPositionDict();
    buildPositionDict();
    buildPositionByScalp();
    buildPositionByName();
    buildBipolarLongitudinalDict();
    buildTransBipolarDict();
    buildDevicePositionDict();
}

}
