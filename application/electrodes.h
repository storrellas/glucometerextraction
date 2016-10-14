#ifndef _ELECTRODES_
#define _ELECTRODES_

#include<QString>
#include<QHash>
#include<QVector>
#include<QMutex>

#include "commonparameters.h"

/* This is a bit of reinventing the wheel and could be done better,
 * but at the very least it centralizes electrode positioning info.
 * it could be reworked for the next major NIC/NE software version,
 * but until that's done, this namespace attempts to provide a thread-safe
 * operation of all electrode labeling/positioning configuration for NIC
 * or other softwares, which as of now was dependent on  NIC GUI's (oh, the horror)
*/

namespace Electrodes
{
const static int INVALID_ELECTRODE = -1;

namespace DeviceType{
    typedef enum{ UNKNOWN = 0
                  , STARSTIM//, STARSTIM20, STARSTIM32
                  , ENOBIO8, ENOBIO20, ENOBIO32
                }SupportedNICDevice;
}

namespace ElectrodeChannel{
    typedef enum{CH1=0,  CH2,  CH3,  CH4,  CH5,  CH6,  CH7,  CH8 // = 7
                 ,CH9 , CH10, CH11, CH12, CH13, CH14, CH15, CH16 // = 15
                 ,CH17, CH18, CH19, CH20, CH21, CH22, CH23, CH24 // = 23
                 ,CH25, CH26, CH27, CH28, CH29, CH30, CH31, CH32 // = 31
                } EChanEnum;
    const static unsigned int   supportedChannels = 32;
    const static int            invalidChannel = INVALID_ELECTRODE;
    const static QString        invalidStr;

    inline bool isChannel(unsigned int chan)
    {
        return int(chan) > invalidChannel && (chan < supportedChannels);
    }
}

/* These are the original orders found in older versions:
 *  oldElectrodePositionNames = {"Fpz", "Fp1", "Fp2", "AF7", "AF8"
 *                            , "F7", "F3", "Fz", "F4", "F8"
 *                            , "T7", "C3", "C1", "Cz", "C2"
 *                            , "C4", "T8", "P7", "P3", "Pz"
 *                            , "P4", "P8", "PO7", "PO8", "O1"
 *                            , "Oz", "O2", "EOG1", "EOG2"};
 *
 *   electrodePositionNames = {"Fp1", "Fpz", "Fp2"
 *                                             , "AF7", "AF3", "AF4", "AF8"
 *                                             , "F7", "F3", "Fz", "F4", "F8"
 *                                             , "FC5", "FC1", "FC2", "FC6"
 *                                             , "T7", "C3", "C1", "Cz", "C2", "C4"
 *                                             , "T8", "CP5", "CP1", "CP2", "CP6"
 *                                             ,"P7", "P3", "Pz", "P4", "P8"
 *                                             , "PO7", "PO3", "PO4", "PO8"
 *                                             , "O1", "Oz", "O2"};
 *
 * As of version NIC 1.3.14 (fw 1247), our cap model scalp positions observe a mixture of 10/20 (complete)
 * and 10/10 (partial) standard positions
 * Missing positions from 10/10 are from nasion (forehead) to inion (nape)
 *  and left side to right side :
 *
 * Nz
 *
 * AFz
 * F9, F5, F1, F2, F6, F10
 * FT9, FT7, FC3, FCz, FC4, FT8, FT10
 * A1, T9, C5, C6, T10, A2
 * TP9, TP7, CP3, CPz, CP4, TP8, TP10
 * P9, P5, P1, P2, P6, P10
 * P0z
 *
 * Iz
 */
/*
//REMOVE THIS FRAGMENT
//This old set of electrode positions used in older versions is defined for compatibility
//with older Protocol Templates
namespace OldElectrode{
    typedef enum{Fpz = 0, Fp1, Fp2, AF7, AF8, F7, F3, Fz, F4, F8// = 9
                 , T7, C3, C1, Cz, C2, C4, T8, P7, P3, Pz // = 19
                 , P4, P8, PO7, PO8, O1, Oz, O2, EOG1, EOG2, ECG // = 29
                 ,CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8 //Unknown setup of icognos 8
                } OldElectrodePosEnum;
}
#define OLD_ELECTRODE_TOTAL 38 //This must match OldElectrodePosEnum total
//END OF FRAGMENT
*/
//old electrode name arrays are (incorrectly) sorted by scalp position, from nasion (forehead)
//to inion (nape), and left side to right side
namespace OldElectrodePosition{
    typedef enum{        Fpz,Fp1, Fp2 //2
                 ,  AF7,               AF8 // 4
                 , F7,  F3,   Fz,   F4,   F8// = 9
                 ,T7,   C3,C1,Cz,C2,C4,    T8 // = 16
                 , P7,   P3,  Pz,   P4,   P8 // = 21
                 ,  PO7,               PO8 // = 23
                 ,        O1, Oz, O2 // = 26
                 , EOG1, EOG2 // = 28
                } OldScalpPosEnum;
    const static unsigned int supportedPositions = 29;
    const static unsigned int invalidOldElectrodePosition = INVALID_ELECTRODE;
    const static QString        invalidStr;

    inline bool isOldPosition(unsigned int pos)
    {
        return pos < supportedPositions;
    }
}

/*
//REMOVE THIS FRAGMENT
namespace Electrode{
    typedef enum{Fp1 = 0, Fpz, Fp2, AF7, AF3, AF4, AF8, F7, F3, Fz // = 9 (Scalp positions)
                 , F4,  F8, FC5, FC1, FC2, FC6,  T7, C3, C1, Cz // = 19 (Scalp positions)
                 , C2,  C4,  T8, CP5, CP1, CP2, CP6, P7, P3, Pz // = 29 (Scalp positions)
                 , P4,  P8, PO7, PO3, PO4, PO8,  O1, Oz, O2 // = 38  (Scalp positions)
                 , EOG1, EOG2, ECG, EXT // = 42 Special positions
                 , CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8 //Unknown setup of icognos 8
                } ElectrodePosEnum;
}
#define ELECTRODE_TOTAL 51 //This must match ElectrodePosEnum total
//END OF FRAGMENT
*/
namespace ElectrodePosition{
    //electrode name arrays are sorted by scalp position, from nasion (forehead)
    //to inion (nape), and left side to right side
    typedef enum{undefinedScalp=9999,
                          Fp1,Fpz,Fp2 // = 3
                 ,  AF7,AF3,        AF4,AF8 // = 7
                 , F7 , F3,    Fz,    F4,   F8 // = 12
                 ,  FC5,  FC1,      FC2,  FC6 // = 16
                 ,T7,   C3, C1,Cz,C2, C4,    T8 // = 23
                 ,  CP5,  CP1,      CP2,  CP6 // = 27
                 , P7,  P3,    Pz,    P4,   P8 // = 32
                 ,  PO7,PO3,        PO4,PO8 // = 36
                 ,         O1, Oz, O2 // = 39
                } ScalpPosEnum;

    const static unsigned int supportedScalpPositions = 39;

    typedef enum{undefinedSpecial =  undefinedScalp + 1000,
                 EOG1, EOG2, ECG, EXT
                }SpecialPosEnum;// Special positions

    const static unsigned int supportedSpecialPositions = 4;

    namespace BipolarLongitudinal
    {//NOTE: see how BipolarLongitudinal IDs have identical values to Channel IDs 1-18
        typedef enum{   FP1_F7 = undefinedScalp + 2000, F7_T7, T7_P7, P7_O1
                   ,FP2_F8, F8_T8, T8_P8, P8_O2
                   ,FP1_F3, F3_C3, C3_P3, P3_O1
                   ,FP2_F4, F4_C4, C4_P4, P4_O2
                   ,FZ_CZ , CZ_PZ//, CH19, CH20
                }BipolarLongitudinalPairEnum;// BipolarLongitudinal pairs
        const static unsigned int supportedBipolarLongitudinalPairs = 18;
        //inline bool isBipolarLongitudinal(unsigned int pos)
    }
    namespace BipolarTransversal
    {//NOTE: see how BipolarTransversal IDs have identical values to Channel IDs 1-18
        typedef enum{   F7_FP1 = undefinedScalp + 3000, FP1_FP2, FP2_F8
                  , F7_F3, F3_FZ, FZ_F4, F4_F8
                  , T7_C3, C3_CZ, CZ_C4, C4_T8
                  , P5_P3, P3_PZ, PZ_P4, P4_P8
                  , P7_01, O1_O2, O2_P8//, CH19, CH20
                }TransBipPairEnum;// Bipolar Transversal pairs
        const static unsigned int supportedBipolarTransversalPairs = 18;
        //inline bool isTransBip(unsigned int pos)
    }

    const static unsigned int supportedPositions = supportedScalpPositions
                                                    + supportedSpecialPositions;

    const static unsigned int invalidElectrodePosition = INVALID_ELECTRODE;
    const static QString      invalidStr;

    inline bool isScalpPosition(unsigned int pos)
    {
        return (pos > undefinedScalp) && (pos < undefinedScalp +1 + supportedScalpPositions);
    }

    inline bool isSpecialPosition(unsigned int pos)
    {
        return (pos > undefinedSpecial) && (pos < undefinedSpecial +1 + supportedSpecialPositions);
    }

    inline bool isPosition(unsigned int pos)
    {
        return isScalpPosition(pos) || isSpecialPosition(pos);
    }

}

typedef enum {SortedByScalp, SortedByName} ElectrodeSort;
typedef enum {icognosStd, E20BipolarLong, E20BipolarTrans} icognosSetup;

void            sortElectrodes(const ElectrodeSort sort);
ElectrodeSort   currentSort();
void            setDevice(DeviceType::SupportedNICDevice dev);
DeviceType::SupportedNICDevice deviceType();
void            seticognosSetup(icognosSetup s);
icognosSetup     currenticognosSetup();
//void  setupDevice(unsigned int eegChannels, unsigned int stimChannels = 0);
unsigned int    deviceStimulationChannels();
unsigned int    deviceEEGChannels();
unsigned int    deviceStandardChannels();
const QVector<unsigned int> supportedChannelCounts();
int             setElectrodePlacement(unsigned int channelID, unsigned int positionID);
int             setElectrodePlacement(unsigned int channelID, const QString& positionName);
void            resetElectrodePlacement();
void            resetElectrodePlacement(unsigned int channelID);
int             getPositionChannel(const QString& positionName);
int             getPositionChannel(unsigned int posID);
void            setEOGPlacement(bool enabled);
bool            isEOGEnabled();

const QString&  getChannelName(unsigned int channelID);
int             getChannelID(const QString &channelName);
const QString&  getOldElectrodeName(unsigned int oldElectrodePosition) ;
//static int    getOldElectrodeID(const QString& e) const;
const QString&  getPositionName(unsigned int positionID);
int             getPositionID(const QString &positionName);

QString         getPlacementName(unsigned int channelID);
int             getPlacementID(const QString &channelName);

const QVector<QString>&         getChannelNames();
const QVector<QString>          getChannelNames(unsigned int total);

const QVector<unsigned int>&    getChannelIDs();
const QVector<unsigned int>     getChannelIDs(unsigned int total);

const QVector<QString> &        getPositionNames();
const QVector<unsigned int>&    getPositionIDs();

const QVector<QString> &        getPositionNames(DeviceType::SupportedNICDevice dev,/* icognosSetup config = icognosStd,*/ ElectrodeSort s = SortedByName, bool withEOG = false);
const QVector<unsigned int>&    getPositionIDs(DeviceType::SupportedNICDevice dev,/* icognosSetup config = icognosStd,*/ ElectrodeSort s = SortedByName, bool withEOG = false);
/*
const QVector<QString> &        getBipolarLongitudinalNames();
const QVector<unsigned int>&    getBipolarLongitudinalIDs();

const QVector<QString> &        getBipolarTransversalNames();
const QVector<unsigned int>&    getBipolarTransversalIDs();
*/
const QVector<QString>          getPlacementNames();
const QVector<unsigned int>&    getPlacementIDs();


//Total number of EEG channels (icognos currently supports up to 32)
const unsigned int maxEEGChannels = ElectrodeChannel::supportedChannels;
//Total number of Stimulation channels (StarStim currently supports up to 8)
const unsigned int maxStimChannels = NUM_STIM_CHANNELS; //defined in commmonparameters, might define here?
//Total number of electrodes in old NIC versions (for compatibility with old templates)
const unsigned int OldElectrodeTotal = OldElectrodePosition::supportedPositions;
//Total number of electrode positions available (sum of Scalp + Special positions)
const unsigned int ElectrodePositionTotal = ElectrodePosition::supportedPositions;
//Total number of non-scalp electrode positions available (like EOG1, EOG2, ECG and EXT)
const unsigned int SpecialPositionTotal = ElectrodePosition::supportedSpecialPositions;
//Total number of scalp electrode positions available (currently a mix of 10/10 and 10/20)
const unsigned int ScalpElectrodeTotal = ElectrodePosition::supportedScalpPositions;

class ElectrodePlacement{

public:
    static ElectrodePlacement& getInstance()
    {
        static ElectrodePlacement _p;
        return _p;
    }
    /*
    //Total number of EEG channels (icognos currently supports up to 32)
    const static    unsigned int maxEEGChannels;
    //Total number of Stimulation channels (StarStim currently supports up to 8)
    const static    unsigned int maxStimChannels;
    //Total number of electrodes in old NIC versions (for compatibility with old templates)
    const static    unsigned int OldElectrodeTotal;
    //Total number of electrode positions available (sum of Scalp + Special positions)
    const static    unsigned int ElectrodePositionTotal;
    //Total number of non-scalp electrode positions available (like EOG1, EOG2, ECG and EXT)
    const static    unsigned int SpecialPositionTotal;
    //Total number of scalp electrode positions available (currently a mix of 10/10 and 10/20)
    const static    unsigned int ScalpElectrodeTotal;
*/
//typedef enum {SortedByScalp, SortedByName} ElectrodeSort;

friend void             sortElectrodes(const ElectrodeSort sort);
friend void             setDevice(DeviceType::SupportedNICDevice dev);
friend DeviceType::SupportedNICDevice deviceType();
friend void             seticognosSetup(icognosSetup s);
friend icognosSetup      currenticognosSetup();
//friend void     setupDevice(unsigned int eegChannels, unsigned int stimChannels);
friend unsigned int     deviceStimulationChannels();
friend unsigned int     deviceEEGChannels();
friend unsigned int     deviceStandardChannels();
friend int              setElectrodePlacement(unsigned int channelID, unsigned int positionID);
friend int              setElectrodePlacement(unsigned int channelID, const QString& positionName);
friend void             resetElectrodePlacement();
friend void             resetElectrodePlacement(unsigned int channelID);
friend int              getPositionChannel(const QString& positionName);
friend int              getPositionChannel(unsigned int posID);
friend void             setEOGPlacement(bool enabled);
friend bool             isEOGEnabled();

friend const QString&   getChannelName(unsigned int channelID);
friend int              getChannelID(const QString &channelName);
friend const QString&   getOldElectrodeName(unsigned int oldElectrodePosition) ;
//friend int      getOldElectrodeID(const QString& e) const;
friend const QString&   getPositionName(unsigned int positionID);
friend int              getPositionID(const QString &positionName);

//friend const QVector<QString> &        getBipolarLongitudinalNames();
//friend const QVector<unsigned int>&    getBipolarLongitudinalIDs();

//friend const QVector<QString> &        getBipolarTransversalNames();
//friend const QVector<unsigned int>&    getBipolarTransversalIDs();

friend QString          getPlacementName(unsigned int channel);
friend int              getPlacementID(const QString &channelName);

friend const QVector<QString>&      getChannelNames();
friend const QVector<QString>       getChannelNames(unsigned int total);
friend const QVector<unsigned int>& getChannelIDs();
friend const QVector<unsigned int>  getChannelIDs(unsigned int total);

friend const QVector<QString>&      getPositionNames();
friend const QVector<unsigned int>& getPositionIDs();

friend const QVector<QString>&      getPositionNames(DeviceType::SupportedNICDevice dev,/* icognosSetup config,*/ ElectrodeSort s, bool withEOG);
friend const QVector<unsigned int>& getPositionIDs(DeviceType::SupportedNICDevice dev,/* icognosSetup config,*/ ElectrodeSort s, bool withEOG);

friend const QVector<QString>       getPlacementNames();
friend const QVector<unsigned int>& getPlacementIDs();

inline bool deviceHasChannel(unsigned int chan)
{
    return ElectrodeChannel::isChannel(chan) && (chan < _deviceChannelTotal);
}

inline bool deviceHasEEGChannel(unsigned int chan)
{
    return ElectrodeChannel::isChannel(chan) && (chan < _deviceEEGChannelTotal);
}

inline bool deviceHasStimChannel(unsigned int chan)
{
    return ElectrodeChannel::isChannel(chan) && (chan < _deviceStimulationChannelTotal);
}

ElectrodeSort   currentSort();

private:


ElectrodePlacement()
{
    _dev = DeviceType::UNKNOWN;
    _icognos20Setup = icognosStd;
    _edb.sortByName(_dev/*, _icognos20Setup*/, false);
    /*
    _supportedEEGChannelCounts.append(8);//ENOBIO8
    _supportedEEGChannelCounts.append(18);//ENOBIO20 BIPOLAR
    _supportedEEGChannelCounts.append(20);//ENOBIO20 STD
    _supportedEEGChannelCounts.append(32);//ENOBIO32
    _supportedStimChannelCounts.append(8);//STARSTIM
    */
    //_supportedStimChannelCounts.append(20);
    //_supportedStimChannelCounts.append(32);
    //_edb.sortByScalp();
}

void    sortElectrodes(const ElectrodeSort sort);

void    setupDevice(unsigned int eegChannels, unsigned int stimChannels = 0);

void    setupicognos20Standard();
void    setupicognos20BipolarLongitudinal();
void    setupicognos20BipolarTransversal();
void    setupicognos32();

void    setBipolarLongitudinalPlacement(unsigned int channelID, ElectrodePosition::BipolarLongitudinal::BipolarLongitudinalPairEnum pairID);
void    setBipolarTransversalPlacement(unsigned int channelID, ElectrodePosition::BipolarTransversal::TransBipPairEnum pairID);
int     setElectrodePlacement(unsigned int channelID, unsigned int positionID);
void    resetElectrodePlacement();
void    resetElectrodePlacement(unsigned int channelID);

struct ElectrodePlacementsDict{
    ElectrodePlacementsDict();

    /* These two sorting functions, actually create the electrode position
     * maps inserting elements in one order or another. The effect of this
     * is that when the list of electrodes is recovered from this class
     * it is taken from the list of keys in the maps, therefore providing the
     * set order
    */
    void sortByScalp(DeviceType::SupportedNICDevice dev,/* icognosSetup s,*/ bool eog);
    void sortByName(DeviceType::SupportedNICDevice dev,/* icognosSetup s,*/ bool eog);


    QVector<QString>                electrodeChannelNames;
    QVector<QString>                deviceElectrodeChannelNames;
    QHash<unsigned int, QString>    oldElectrodePositionNames;
    QHash<unsigned int, QString>    electrodePositionNames;

    //QVector<QString>                bipolarLongPairNames;
    //QVector<QString>                bipolarTransPairNames;
    QHash<unsigned int, QString>    bipolarLongPairNames;
    QHash<unsigned int, QString>    bipolarTransPairNames;

    QHash<QString, unsigned int>    electrodeChannelIDs;
    QHash<QString, unsigned int>    oldElectrodePositionIDs;
    QHash<QString, unsigned int>    electrodePositionIDs;
    QHash<QString, unsigned int>    bipolarLongPairIDs;
    QHash<QString, unsigned int>    bipolarTransPairIDs;

    QVector<unsigned int>           electrodeChannelIDsSorted;
    QVector<unsigned int>           deviceElectrodeChannelIDs;

    typedef enum{ ConfigFree
                  , ConfigE20Std, ConfigE20StdEOG//, ConfigE20BipLong, ConfigE20BipTrans
                  , ConfigE32, ConfigE32EOG}     ElectrodeConfiguration;

    typedef QHash<ElectrodeConfiguration, QVector<unsigned int> >   PositionIDMap;
    typedef QHash<ElectrodeConfiguration, QVector<QString> >   PositionNameMap;
    //QVector<QString>                electrodePositionNamesByName;
    //QVector<QString>                electrodePositionNamesByScalp;
    QVector<QString>               electrodePositionNamesSorted;

    //QVector<unsigned int>           electrodePositionIDsByName;
    //PositionIDMap                   electrodePositionIDsByName;
    //QVector<unsigned int>           electrodePositionIDsByScalp;
    //PositionIDMap                   electrodePositionIDsByScalp;
    QVector<unsigned int>          electrodePositionIDsSorted;
    //Fixed configurations
    //PositionIDMap                   fixedConfigPositionIDs;
    //PositionNameMap                 fixedConfigPositionNames;
    PositionIDMap                   devicePositionIDsByName;
    PositionNameMap                 devicePositionNamesByName;
    PositionIDMap                   devicePositionIDsByScalp;
    PositionNameMap                 devicePositionNamesByScalp;
    //QHash<CurrentPosition, Channel>, default values will be: QHash<Channel, Channel>
    //static QHash<unsigned int, unsigned int>    currentElectrodePlacement;
    QVector<unsigned int>           currentElectrodePlacement;

    inline int setElectrodePlacement(unsigned int c, unsigned int p);
    void setupicognos20Standard();
    void setupicognos20BipolarLongitudinal();
    void setupicognos20BipolarTransversal();
    void setupicognos32();
    void setEOGPlacement(bool enabled);

private:
    void buildChannelDict();
    void buildLegacyPositionDict();
    void buildPositionDict();
    void buildPositionByScalp();
    void buildPositionByName();
    void buildFixedPositionMap();
    void buildBipolarLongitudinalDict();
    void buildTransBipolarDict();
    void buildDevicePositionDict(ElectrodeConfiguration econf);
    void buildDevicePositionDict();
    void buildDictionaries();
};

QMutex _electrodeSetupMutex;

ElectrodePlacementsDict _edb;

DeviceType::SupportedNICDevice  _dev;
icognosSetup                   _icognos20Setup;
ElectrodeSort                 _currentSort;
unsigned int _deviceEEGChannelTotal;
unsigned int _deviceStimulationChannelTotal;
unsigned int _deviceChannelTotal;
unsigned int _deviceScalpTotal;
unsigned int _deviceSpecialTotal;
bool         _eogEnabled;
};

//typedef QVector<int> OldElectrodeVector; //not used
typedef QVector<int> ElectrodeVector;
//ElectrodePlacement Setup;// = ElectrodePlacement();
}
#endif //_ELECTRODES_
