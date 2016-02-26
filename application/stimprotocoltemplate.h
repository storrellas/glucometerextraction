#ifndef STIMPROTOCOLTEMPLATE_H
#define STIMPROTOCOLTEMPLATE_H
//#include <QHash>
//#include <QDomDocument>
#include <QPair>
#include <QVector>
#include <QStringList>
#include <QHash>
#include <math.h>
#include "electrodes.h"
#include "protocoltypes.h"

namespace StimProtocolSessionConfig
{
    typedef enum    {   E_NOT_USED,
                        E_EEG_REC,
                        E_STIM,
                        E_STIM_ANODAL,
                        E_STIM_CATHODAL,
                        E_RETURN
                    } EType;

    typedef enum    {   //ERROR CODES
                        ERR_NO_ERROR,
                        ERR_TACS_FREQ_ZERO,
                        ERR_TDCS_SUM_NZERO,
                        ERR_TRNS_SUM_NZERO,
                        ERR_TRNS_NO_RET,
                        ERR_TRNS_OVER_LIMIT,
                        ERR_TRNS_NEG,
                        ERR_CHAN_CURR_OVER_LIMIT,
                        ERR_RET_NO_STIM,
                        ERR_MULTIPLE_RET,
                        ERR_PRE_EEG_TIME_NO_EEG,
                        ERR_POST_EEG_TIME_NO_EEG,
                        ERR_SHAM_TIME,
                        ERR_SIM_CHAN_CURR_OVER_LIMIT,
                        ERR_SIM_RET_CURR_OVER_LIMIT,
                        ERR_SIM_STIM_CURR_SUM_NZERO,
                        ERR_SIM_INJ_CURR_OVER_LIMIT,
                        //WARNING CODES
                        WRN_CURR_DANGER,
                        WRN_SHORT_RAMP_UP,
                        WRN_SHORT_RAMP_DOWN
                    } StimConfigMessageID;

    typedef QMap<StimConfigMessageID, QStringList > ErrorMap;
    typedef QPair<QString, QStringList> ErrorInfo;
    typedef QHash<StimConfigMessageID, QVector<ErrorInfo> > ErrorInfoMap;

    typedef struct SPII
    {
        typedef enum {CONTAINS_ERRORS= 0x1,CONTAINS_WARNINGS= 0x2} IssueType;
        ErrorMap        _errors;
        ErrorInfoMap    _errorAdditionalInfo;
        unsigned int    _issues;
        inline bool containsErrors() const{ return _issues & CONTAINS_ERRORS;}
        inline bool containsWarnings() const{ return _issues & CONTAINS_WARNINGS;}

        SPII():_issues(0){}
    } StimProtocolIssueInfo;

    class MessageMap
    {
    public :
        /*
        static const MessageMap& getInstance()
        {
            static MessageMap _m;
            return _m;
        }
        */
        static bool getMessages(ProtocolType t, const StimProtocolIssueInfo &msgInfo, QString &msg);

        typedef QHash<StimConfigMessageID, QString> MessageMap_impl;
    private:
        static const MessageMap_impl _msgMapAdvc;
        static const MessageMap_impl _msgMapBasic;

        MessageMap()
        {}
    };


    ElectrodeTypeAdv getAdvTypeFromBasic(ElectrodeTypeBasic t);
    ElectrodeTypeBasic getBasicTypeFromAdv(ElectrodeTypeAdv t);

    int getElectrodeCountBasic(const ProtocolSessionConfiguration& ps, ElectrodeTypeBasic etype);
    int getElectrodeCountAdv(const ProtocolSessionConfiguration& ps, ElectrodeTypeAdv   etype);
    int getElectrodeCount(const ProtocolSessionConfiguration& ps, EType etype);

    QString findReturnElectrodePositionAdv(const ProtocolSessionConfiguration& ps);
    QString findStimElectrodePositionBasic(const ProtocolSessionConfiguration& ps);

    int checkTDCSStimulation(const ProtocolSessionConfiguration& ps);

    //Warn when exceeding 400 uA/cm2
    //Three types of electrodes: pi, 8 and 25 cm2
    int maximumCurrentDensityAdvanced(const StimulationElectrodeConfig &electrode);

    //Error if value per channel is DC + AC + RNS * 3 > 2 mA
    bool checkMaximumCurrentAdvanced(const StimulationElectrodeConfig &electrode);

    //We call this functon when AC is 0
    int computeReturnValue(const ProtocolSessionConfiguration &ps);

    double computeDosageSqAdvanced(const ProtocolSessionConfiguration &t);

    double computeDosageSqBasic(const ProtocolSessionConfiguration &t);

    bool equivalentBasicConf(const ProtocolSessionConfiguration& lhs, const ProtocolSessionConfiguration& rhs);

    bool equivalentAdvancedConf(const ProtocolSessionConfiguration& lhs, const ProtocolSessionConfiguration& rhs);

    bool equivalentConf(const ProtocolSessionConfiguration& lhs, const ProtocolSessionConfiguration& rhs);

    bool updateAdvancedStimFromBasic(ProtocolSessionConfiguration& ps);

    inline bool isError(StimConfigMessageID id){ return id > ERR_NO_ERROR && id < WRN_CURR_DANGER; }
    inline bool isWarning(StimConfigMessageID id){ return id >= WRN_CURR_DANGER; }

    bool getIssueMessage(ProtocolType msgType, StimProtocolIssueInfo issueInfo, QString& message);

    //StimConfigMessageID protocolSessionConfigurationHasWarnings(const ProtocolSessionConfiguration &ps, QString &reason);
    bool protocolSessionConfigurationHasWarnings(const ProtocolSessionConfiguration &ps, StimProtocolIssueInfo& info);

    //Check if a configuration is correct, converting it to advanced if necessary
    bool protocolSessionConfigurationIsCorrect(const ProtocolSessionConfiguration& psc
                                                              , int fwVersion, bool checkWarnings, StimProtocolIssueInfo& info);
    bool basicSessionConfigurationIsCorrect(const ProtocolSessionConfiguration& psc, StimProtocolIssueInfo& info);
}

class StimProtocolTemplate//:ProtocolTemplate
{
public:
    StimProtocolTemplate()
    {
        setElectrodeLayout(Electrodes::getChannelIDs());
    }

    StimProtocolTemplate(const StimProtocolTemplate& spt);

    /*!
      * Return a string containing protocol template identifier
      */
    const QString &getName() const;
    /*!
      * Set protocol template identifier
      */
    void    setName(const QString &name);
    /*!
      * Return protocol template information as a string
      */
    QString toString() const;
    /*!
      * Return protocol template description for NIC's GUI
      */
    QString guiText() const;

    /*!
      * Return number of channels this template is prepared for
      */
    int getChannelCount() const;

    /*!
      * Return electrode setup for this template
      */
    QVector<unsigned int> getElectrodeLayout() const;

    /*!
      * Setup electrode placement for this template
      */
    bool setElectrodeLayout(const QVector<unsigned int>& e);

    void setElectrodeConfig(const StimulationElectrodeConfig &econf, unsigned int e);
    const StimulationElectrodeConfig & getElectrodeConfig(unsigned int e) const;

    /*!
      * Basic comparison operators
      */
    bool isEquivalent(const StimProtocolTemplate& rhs) const
    {
        if(_eLayout != rhs._eLayout)
            return false;
        return StimProtocolSessionConfig::equivalentConf(_data, rhs._data);
    }

    bool isEqual(const StimProtocolTemplate& rhs) const
    {
        if(!isEquivalent(rhs))
            return false;
        if (_data.name != rhs._data.name)//likely the most costly comparison
            return false;
        return true;
    }

    friend bool operator==(const StimProtocolTemplate& lhs, const StimProtocolTemplate& rhs);
    friend bool operator!=(const StimProtocolTemplate& lhs, const StimProtocolTemplate& rhs);

public:

    /*!
      * Data structure holding actual protocol template information, common to other NE software
      */
    ProtocolSessionConfiguration _data;
protected:
    QVector<unsigned int> _eLayout;
};

inline bool areEquivalent(const StimProtocolTemplate& lhs, const StimProtocolTemplate& rhs)
{
    return lhs.isEquivalent(rhs);
}

inline bool areEqual(const StimProtocolTemplate& lhs, const StimProtocolTemplate& rhs)
{
    return lhs.isEqual(rhs);
}

inline bool operator==(const StimProtocolTemplate& lhs, const StimProtocolTemplate& rhs)
{
    return lhs.isEqual(rhs);
}

inline bool operator!=(const StimProtocolTemplate& lhs, const StimProtocolTemplate& rhs)
{
    return !lhs.isEqual(rhs);
}


#endif// STIMPROTOCOLTEMPLATE_H
