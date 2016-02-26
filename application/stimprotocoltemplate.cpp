#include <QDomDocument>
#include <QHash>
#include <QStringList>
#include "stimprotocoltemplate.h"
#include "protocoltemplates.h"
//#include "electrodes.h"


namespace StimProtocolSessionConfig
{
    ElectrodeTypeAdv getAdvTypeFromBasic(ElectrodeTypeBasic t)
    {
        ElectrodeTypeAdv a;
        switch(t)
        {
        case NOT_USED_BASIC: a = NOT_USED_ADV; break;
        case EEG_RECORDING_BASIC: a = EEG_RECORDING_ADV; break;
        case STIM_CATHODAL_BASIC: case STIM_ANODAL_BASIC: a = RETURN_ADV; break;
        case RETURN_BASIC: a = STIMULATION_ADV; break;
        }
        return a;
    }
    ElectrodeTypeBasic getBasicTypeFromAdv(ElectrodeTypeAdv t)
    {
        ElectrodeTypeBasic a;
        switch(t)
        {
        case NOT_USED_ADV: a = NOT_USED_BASIC; break;
        case EEG_RECORDING_ADV: a = EEG_RECORDING_BASIC; break;
        case STIMULATION_ADV: a = RETURN_BASIC; break;
        case RETURN_ADV: a = NOT_USED_BASIC; break; //undefined, so fail
        }
        return a;
    }

    int getElectrodeCountBasic(const ProtocolSessionConfiguration &ps, ElectrodeTypeBasic etype)
    {
        int electrodes = 0;
        for(int i = 0; i < NUM_STIM_CHANNELS; i++ ){
            if(ps.electrode[i].electrodeTypeBasic == etype) electrodes++;
        }
        return electrodes;
    }

    int getElectrodeCountAdv(const ProtocolSessionConfiguration &ps, ElectrodeTypeAdv etype)
    {
        int electrodes = 0;
        for(int i = 0; i < NUM_STIM_CHANNELS; i++ ){
            if(ps.electrode[i].electrodeTypeAdv == etype) electrodes++;
        }
        return electrodes;
    }

    int getElectrodeCount(const ProtocolSessionConfiguration &ps, EType etype)
    {
        int count = 0;
        switch(ps.protocolType)
        {
        case ADVANCED:
            switch(etype)
            {
            case E_NOT_USED:
                count = getElectrodeCountAdv(ps, NOT_USED_ADV);
                break;
            case E_EEG_REC:
                count = getElectrodeCountAdv(ps, EEG_RECORDING_ADV);
                break;
            case E_RETURN:
                count = getElectrodeCountAdv(ps, RETURN_ADV);
                break;
            case E_STIM:
                count = getElectrodeCountAdv(ps, STIMULATION_ADV);
                break;
            default:
                break;
            }
            break;
        case BASIC:
            switch(etype)
            {
            case E_NOT_USED:
                count = getElectrodeCountBasic(ps, NOT_USED_BASIC);
                break;
            case E_EEG_REC:
                count = getElectrodeCountBasic(ps, EEG_RECORDING_BASIC);
                break;
            case E_RETURN:
                count = getElectrodeCountBasic(ps, RETURN_BASIC);
                break;
            case E_STIM_ANODAL:
                count = getElectrodeCountBasic(ps, STIM_ANODAL_BASIC);
                break;
            case E_STIM_CATHODAL:
                count = getElectrodeCountBasic(ps, STIM_CATHODAL_BASIC);
                break;
            case E_STIM:
                count = getElectrodeCountBasic(ps, STIM_CATHODAL_BASIC);
                count += getElectrodeCountBasic(ps, STIM_ANODAL_BASIC);
                break;
            default:
                break;
            }
            break;
        }
        return count;
    }

    QString findReturnElectrodePositionAdv(const ProtocolSessionConfiguration& ps)
    {
        for (int i=0;i<NUM_STIM_CHANNELS;i++){
            if(ps.electrode[i].electrodeTypeAdv == RETURN_ADV)
                return ps.electrode[i].position;
        }
        return "";
    }

    QString findStimElectrodePositionBasic(const ProtocolSessionConfiguration& ps)
    {
        for (int i=0;i<NUM_STIM_CHANNELS;i++){
            if(ps.electrode[i].electrodeTypeBasic == STIM_ANODAL_BASIC
                    || ps.electrode[i].electrodeTypeBasic == STIM_CATHODAL_BASIC)
                return ps.electrode[i].position;
        }
        return "";
    }

    int totalTRNSStimulation(const ProtocolSessionConfiguration& ps)
    {
        int sumTRNS=0;
        for (int i=0;i<NUM_STIM_CHANNELS;i++)
            sumTRNS += ps.electrode[i].Atrns;
        return sumTRNS;
    }

    int checkTDCSStimulation(const ProtocolSessionConfiguration& ps)
    {
        int dcValue = 0;
        for (int i = 0; i < NUM_STIM_CHANNELS;i++)
            dcValue += ps.electrode[i].Atdcs;
        return dcValue;
    }

    //Warn when exceeding 400 uA/cm2
    //Three types of electrodes: pi, 8 and 25 cm2
    int maximumCurrentDensityPerChannelAdvanced(const StimulationElectrodeConfig& electrode)
    {
        int DCValue  = electrode.Atdcs;
        int ACValue  = electrode.Atacs;
        int RNSValue = electrode.Atrns;
        return abs(DCValue) + ACValue + RNSValue*3;
    }

    //Error if value per channel is DC + AC + RNS * 3 > 2 mA
    bool checkMaximumCurrentPerChannelAdvanced(const StimulationElectrodeConfig &electrode)
    {
        if( maximumCurrentDensityPerChannelAdvanced(electrode) > 2000 )
            return false;
        else
            return true;
    }

    //We call this functon when AC is 0
    int computeReturnValue(const ProtocolSessionConfiguration &ps)
    {
    //    QString name;
        int DCValue,ACValue,RNSValue,returnValue;

        DCValue=ACValue=RNSValue=0;

        for (int i = 0;i < NUM_STIM_CHANNELS;i++)
        {
            const StimulationElectrodeConfig& electrode = ps.electrode[i];
            if( electrode.electrodeTypeAdv == STIMULATION_ADV ){
                DCValue  += electrode.Atdcs;
                ACValue  += electrode.Atacs;
                RNSValue += electrode.Atrns;
            }
        }

        returnValue = -1*(DCValue+ACValue+RNSValue*3);
        if (abs(DCValue+ACValue+RNSValue*3)<abs(DCValue+ACValue-RNSValue*3))
            returnValue=-1*(DCValue+ACValue-RNSValue*3);
        qDebug()<<"ProtocolManager::calculateReturnValue()"<<-1*abs(DCValue+ACValue+RNSValue*3)<<-1*abs(DCValue+ACValue-RNSValue*3);

        return returnValue;
    }

    //Dosage Adv:
    //sqrt(Atdcs^2+Atacs^2/8+Atrns^2)
    double computeDosageSqAdvanced(const ProtocolSessionConfiguration &t)
    {
        int DCValue = 0,ACValue = 0,RNSValue = 0;
        double dosage = 0.0;
        int DCValueCath = 0, RNSValueNegative = 0;

        //for (int i = 0; i < Electrodes::deviceStimulationChannels(); i++)
        for (int i = 0; i < NUM_STIM_CHANNELS; i++)
        {
            const StimulationElectrodeConfig & e = t.electrode[i];
            if(e.Atdcs > 0)
                DCValue += e.Atdcs;
            else
                DCValueCath += e.Atdcs;
            ACValue += e.Atacs;
            if(e.Atrns > 0)
                RNSValue += e.Atrns;
            else
                RNSValueNegative += e.Atrns;
        }

        if(RNSValue != 0 || RNSValueNegative != 0) //avoid printing this all the time
            qDebug()<<"RNSValue"<<RNSValue<<"RNSValueNegative"<<RNSValueNegative;

        //Find max DC value and add squared to total
        int curMax = qMax(DCValue, abs(DCValueCath));
        dosage = curMax * curMax;
        //Add AC
        dosage+=(ACValue*ACValue)/8.0;
        //Add RN
        curMax = qMax(RNSValue, abs(RNSValueNegative));
        dosage += curMax*curMax;

        return dosage;
    }

    //Dosage Basic:
    //tDCs: I (dc) * time
    //tACs: sqrt(offset^2+(Amplitude^2/8)) * time
    //tRNS: sqrt(offset^2+Amplitude^2) * time
    double computeDosageSqBasic(const ProtocolSessionConfiguration &t)
    {
        int amplitude = 0, offset = 0;
        double dosage = 0.0;

        //for (int i = 0; i < Electrodes::deviceStimulationChannels(); i++)
        for (int i = 0; i < NUM_STIM_CHANNELS; i++)
        {
            const StimulationElectrodeConfig & e = t.electrode[i];

            if (e.electrodeTypeBasic == STIM_ANODAL_BASIC
                    || e.electrodeTypeBasic == STIM_CATHODAL_BASIC)
            {
                amplitude = e.ampValue;
                offset = e.offsetValue;
            }
        }

        switch(t.protocolBasicType)
        {
        case BASIC_TYPE_TDCS:
            dosage = double(amplitude);
            break;
        case BASIC_TYPE_TACS:
            dosage = double(offset*offset + amplitude*amplitude)/8.0;
            break;
        case 2:
            dosage =  double(offset*offset + amplitude*amplitude);
            break;
        }

        return dosage;
    }

    bool equivalentBasicConf(const ProtocolSessionConfiguration &lhs, const ProtocolSessionConfiguration &rhs)
    {
        if(lhs.protocolType != BASIC || rhs.protocolType != BASIC)
            return false;
        if (lhs.protocolBasicType != rhs.protocolBasicType)
            return false;
        for(unsigned int i = 0; i < NUM_STIM_CHANNELS; i++)//find electrode[] size
        {
            const StimulationElectrodeConfig &le = lhs.electrode[i];
            const StimulationElectrodeConfig &re = rhs.electrode[i];

            if(le.electrodeTypeBasic != re.electrodeTypeBasic)
                return false;
            if(le.ampValue != re.ampValue)
                return false;
            if(le.offsetValue != re.offsetValue)
                return false;
            if(le.freqValue != re.freqValue)
                return false;
            if(le.retValue != re.retValue)
                return false;
        }
        return true;
    }

    bool equivalentAdvancedConf(const ProtocolSessionConfiguration &lhs, const ProtocolSessionConfiguration &rhs)
    {
        if(lhs.protocolType != ADVANCED || rhs.protocolType != ADVANCED)
            return false;
        for(unsigned int i = 0; i < NUM_STIM_CHANNELS; i++)//find electrode[] size
        {
            const StimulationElectrodeConfig &le = lhs.electrode[i];
            const StimulationElectrodeConfig &re = rhs.electrode[i];

            if(le.electrodeTypeAdv != re.electrodeTypeAdv)
                return false;
            if(le.electrodeTypeAdv == STIMULATION_ADV)
            {
                if(le.Atdcs != re.Atdcs)
                    return false;
                if(le.Atacs != re.Atacs)
                    return false;
                if(le.Ftacs != re.Ftacs)
                    return false;
                if(le.Ptacs != re.Ptacs)
                    return false;
                if(le.Atrns != re.Atrns)
                    return false;
            }
        }
        return true;
    }

    bool equivalentConf(const ProtocolSessionConfiguration &lhs, const ProtocolSessionConfiguration &rhs)
    {
        ProtocolSessionConfiguration p1 = lhs, p2 = rhs;
        updateAdvancedStimFromBasic(p1);
        updateAdvancedStimFromBasic(p2);
        if(!equivalentAdvancedConf(p1, p2))
            return false;
        /*
        if(lhs.protocolType == BASIC && !equivalentBasicConf(lhs, rhs))
            return false;
        */
        /*not used in NIC (currently)
        if(lhs.protocolId != rhs.protocolId)
            return false;
        if(lhs.errorBehavior != rhs.errorBehavior)
            return false;
        if(lhs.percentageSessionRestart != rhs.percentageSessionRestart)
            return false;
        if(lhs.percentageSessionSkip != rhs.percentageSessionSkip)
            return false;
        //*/
        if(lhs.isSham != rhs.isSham)
            return false;
        if(lhs.isSham)
        {
            if(lhs.rampSham != rhs.rampSham)
                return false;
        }
        if(lhs.stimulationDuration != rhs.stimulationDuration)
            return false;
        if(lhs.rampDownDuration != rhs.rampDownDuration)
            return false;
        if(lhs.rampUpDuration != rhs.rampUpDuration)
            return false;
        if (lhs.preEEG != rhs.preEEG)
            return false;
        if (lhs.postEEG != rhs.postEEG)
            return false;
        return true;
    }

    bool updateAdvancedStimFromBasic(ProtocolSessionConfiguration& ps)
    {
        if(ps.protocolType == ADVANCED)
            return true;

        //loggerMacroDebug("Protocol is BASIC. Filling Advanced features")

        int    Atdcs = 0;
        int    Atacs = 0;
        double Ftacs = 0;
        int    Ptacs = 0;
        int    Atrns = 0;

        bool   isAnodal; // Check whether stimulation is Anodal/Cathodal

       //get the stimulation parameters for the single stimulation electrode
       for(int i = 0; i < NUM_STIM_CHANNELS; i++)
       {
           StimulationElectrodeConfig& electrode = ps.electrode[i];

           // Stimulation Electrode can be anodal or cathodal
           if( electrode.electrodeTypeBasic == STIM_ANODAL_BASIC
                   || electrode.electrodeTypeBasic == STIM_CATHODAL_BASIC )
           {
               isAnodal = electrode.electrodeTypeBasic == STIM_ANODAL_BASIC;

               if( ps.protocolBasicType == BASIC_TYPE_TDCS)
               {
                   Atdcs = electrode.ampValue;
                   Atacs = 0;
                   Ftacs = 0;
                   Ptacs = 0;
                   Atrns = 0;
               }
               else if( ps.protocolBasicType == BASIC_TYPE_TACS)
               {
                   Atdcs = electrode.offsetValue;
                   Atacs = electrode.ampValue;
                   Ftacs = electrode.freqValue;
                   Ptacs = 0;
                   Atrns = 0;
               }
               else if( ps.protocolBasicType == BASIC_TYPE_TRNS)
               {
                   Atdcs = electrode.offsetValue;
                   Atacs = 0;
                   Ftacs = 0;
                   Ptacs = 0;
                   Atrns = electrode.ampValue;
               }
           }
       } // End for loop
/*
       qDebug()<<"Atdcs"<<Atdcs;
       qDebug()<<"Atacs"<<Atacs;
       qDebug()<<"Ftacs"<<Ftacs;
       qDebug()<<"Ptacs"<<Ptacs;
       qDebug()<<"Atrns"<<Atrns;
*/

       // Configure Advanced template
       for(int i = 0; i < NUM_STIM_CHANNELS;i++)
       {
           StimulationElectrodeConfig& electrode = ps.electrode[i];

           //Recording Electrode
           if (electrode.electrodeTypeBasic == EEG_RECORDING_BASIC)
           {
               electrode.electrodeTypeAdv = EEG_RECORDING_ADV;
               electrode.Atdcs = 0;
               electrode.Atacs = 0;
               electrode.Ftacs = 0;
               electrode.Ptacs = 0;
               electrode.Atrns = 0;
           }

           //Stimulation Anodal Electrode
           else if( electrode.electrodeTypeBasic == STIM_ANODAL_BASIC
                   || electrode.electrodeTypeBasic == STIM_CATHODAL_BASIC )
           {
               // Always invert the stimulation/return
               // electrodes in the advanced configuration
               electrode.electrodeTypeAdv = RETURN_ADV;
               electrode.Atdcs = 0;
               electrode.Atacs = 0;
               electrode.Ftacs = 0;
               electrode.Ptacs = 0;
               electrode.Atrns = 0;
           }
           //Return Electrode
           else if( electrode.electrodeTypeBasic == RETURN_BASIC)
           {
               // Always invert the stimulation/return
               // electrodes in the advanced configuration
               electrode.electrodeTypeAdv = STIMULATION_ADV;
               if (isAnodal)
               {
                   electrode.Atdcs = -1*Atdcs*electrode.retValue/100;
                   if( Atacs > 0 ){
                       electrode.Ptacs = (Ptacs+180)%360;
                   }
               }
               else //Is cathodal
               {
                   electrode.Atdcs = Atdcs*electrode.retValue/100;
                   electrode.Ptacs = Ptacs;
               }

               electrode.Atacs = Atacs * electrode.retValue/100;
               electrode.Ftacs = Ftacs;
               electrode.Atrns = Atrns * electrode.retValue/100;

           }
           //Disabled Electrode
           else //if (electrode.electrodeTypeBasic == NOT_USED_BASIC)
           {
               electrode.electrodeTypeAdv = NOT_USED_ADV;
               electrode.Atdcs = 0;
               electrode.Atacs = 0;
               electrode.Ftacs = 0;
               electrode.Ptacs = 0;
               electrode.Atrns = 0;
           }
       } // End for

       // Switch configuration to indicate that protocol was transformed
       ps.protocolType = ADVANCED;
       return true;
    }

    MessageMap::MessageMap_impl buildAdvMessageMap()
    {   //will use $number placeholders for additional info inserts, to avoid conflicts with %number arg() calls,
        //which will be used for the electrode positions
        MessageMap::MessageMap_impl m;

        QChar micro(0x00B5);
        QString cm2("cm%1");
        cm2 = cm2.arg(QChar(0x00B2));
        QString msg;
        //create adv. message map
        m[ERR_NO_ERROR] = "";
        m[ERR_TACS_FREQ_ZERO] = "The frequency of the TACS stimulation cannot be 0.";
        msg = "The sum of TDCS stimulation current (%1 ";
        msg.append(micro).append("A) must be 0.");
        m[ERR_TDCS_SUM_NZERO] = msg;
        msg = "The sum of TRNS stimulation current (%1 ";
        msg.append(micro).append("A) must be 0.");
        m[ERR_TRNS_SUM_NZERO] = msg;
        m[ERR_TRNS_NO_RET] = "The TRNS stimulation on electrode%2 %1 requires a return channel for firmware versions prior to $1.";
        msg = "The sum of TRNS stimulation current (%1 ";
        msg = msg.append(micro).append("A)").append(" cannot exceed $1 ").append(micro).append("A ");
        msg.append(" for firmware versions prior to $2.");
        m[ERR_TRNS_OVER_LIMIT] = msg;
        m[ERR_TRNS_NEG] = "Negative TRNS amplitudes in electrode%2 %1 not supported by firmware version $1.";
        //msg =
        m[ERR_CHAN_CURR_OVER_LIMIT] = "The maximum current per channel cannot exceed $1mA.\nPlease check electrode%2 %1.";
        m[ERR_RET_NO_STIM] = "In Advanced protocols, Return electrodes can not be used without using a Stimulation electrode";
        m[ERR_MULTIPLE_RET] = "Only one Return electrode can be used in Advanced protocols.\nPlease check electrode%2 %1.";
        m[ERR_PRE_EEG_TIME_NO_EEG] = "No electrode was set up as EEG Recording but a Pre EEG Recording of %1 sec. was set up.";
        m[ERR_POST_EEG_TIME_NO_EEG] = "No electrode was set up as EEG Recording but a Post EEG Recording of %1 sec. was set up.";
        m[ERR_SHAM_TIME] = "For sham, the stimulation duration (%1 sec.) must be greater than three times the sham ramp time ($1 sec.).";
        m[ERR_SIM_CHAN_CURR_OVER_LIMIT] = "The maximum current per channel will exceed the limit of $1mA after $2 seconds.\nPlease check electrode%2 %1.";
        m[ERR_SIM_RET_CURR_OVER_LIMIT] = "The maximum current through the Return channel in %1 will add up to $2 mA and exceed the limit of $1 mA after $3 seconds.";
        msg = "The sum of stimulation currents would not equal $1. Estimated sum at time $2 seconds is %1 ";
        msg.append(micro).append("A.");
        m[ERR_SIM_STIM_CURR_SUM_NZERO] = msg;
        m[ERR_SIM_INJ_CURR_OVER_LIMIT] = "The sum of injected currents cannot be greater than $1mA. . Estimated sum at time $2 is %1 mA.";
        msg = "Current intensity at electrode%2 %1 can make stimulation uncomfortable or even dangerous for electrode sizes $1 ";
        msg.append(cm2).append(" or smaller (refer to the chart in the NIC manual).");
        m[WRN_CURR_DANGER] = msg;
        m[WRN_SHORT_RAMP_UP] = "With ramp up (%1 seconds) shorter than 3 seconds stimulation could be uncomfortable or even dangerous.";//"Ramp Up (%1 seconds) time shorter than $1 seconds.";
        m[WRN_SHORT_RAMP_DOWN] = "With ramp down (%1 seconds) shorter than 3 seconds stimulation could be uncomfortable or even dangerous.";//"Ramp Down (%1 seconds) shorter than $1 seconds.";
        //m[] = "";
        return m;
    }

    MessageMap::MessageMap_impl buildBasicMessageMap(const MessageMap::MessageMap_impl& advanced)
    {
        MessageMap::MessageMap_impl m(advanced);
        //create basic message map, overwriting appropriate messages from advanced
//*
        //QChar micro(0x00B5);
        //QString cm2("cm%1");
        //cm2 = cm2.arg(QChar(0x00B2));
        QString msg;
        //create adv. message map
        //m[ERR_NO_ERROR] = "";
        //m[ERR_TACS_FREQ_ZERO] = "The frequency of the TACS stimulation cannot be 0.";
        msg = "Return percentages add up to %1%. They must add to $1% - $2%.";
        m[ERR_TDCS_SUM_NZERO] = msg;
        m[ERR_TRNS_SUM_NZERO] = msg;
        m[ERR_SIM_STIM_CURR_SUM_NZERO] = msg;
        //m[ERR_TRNS_NO_RET] = "The TRNS stimulation on electrode%2 %1 requires a return channel for firmware versions prior to $1.";
        //msg = "The sum of TRNS stimulation current (%1";
        //msg = msg.append(micro).append("A)").append(" cannot exceed $1").append(micro).append("A ");
        //msg.append(" for firmware versions prior to $2.");
        //m[ERR_TRNS_OVER_LIMIT] = msg;
        //m[ERR_TRNS_NEG] = "Negative TRNS amplitudes in electrode%2 %1 not supported by firmware version $1.";
        //msg =
        //m[ERR_CHAN_CURR_OVER_LIMIT] = "The maximum current per channel cannot exceed $1mA.\nPlease check electrode%2 %1.";
        m[ERR_RET_NO_STIM] = "In a Basic protocol, a Stimulation electrode can not be used without using at least one Return electrode.";
        m[ERR_MULTIPLE_RET] = "Only one Stimulation electrode can be used in a Basic protocol.\nPlease check electrode%2 %1.";
        //m[ERR_PRE_EEG_TIME_NO_EEG] = "No electrode was set up as EEG Recording but a Pre EEG Recording of %1 sec. was set up.";
        //m[ERR_POST_EEG_TIME_NO_EEG] = "No electrode was set up as EEG Recording but a Post EEG Recording of %1 sec. was set up.";
        //m[ERR_SHAM_TIME] = "For sham, the stimulation duration (%1 sec.) must be greater than three times the sham ramp time ($1 sec.).";
        //m[ERR_SIM_CHAN_CURR_OVER_LIMIT] = "The maximum current per channel will exceed the limit of $1mA after $2 seconds.\nPlease check electrode%2 %1.";
        m[ERR_SIM_RET_CURR_OVER_LIMIT] = "The maximum current through the Stimulation channel in %1 will add up to $2 and exceed the limit of $1mA after $3 seconds.";
        //msg = "The sum of stimulation currents would not equal $1. Estimated sum at time $2 seconds is %1";
        //msg.append(micro).append("A.");
        //m[ERR_SIM_STIM_CURR_SUM_NZERO] = msg;
        //m[ERR_SIM_INJ_CURR_OVER_LIMIT] = "The sum of injected currents cannot be greater than $1mA. . Estimated sum at time $2 is %1mA.";
        //msg = "Current intensity at electrode%2 %1 can make stimulation uncomfortable or even dangerous for electrode sizes $1";
        //msg.append(cm2).append(" or smaller (refer to the chart in the NIC manual).");
        //m[WRN_CURR_DANGER] = msg;
        //m[WRN_SHORT_RAMP_UP] = "Ramp Up (%1 seconds) time shorter than $1 seconds.";
        //m[WRN_SHORT_RAMP_DOWN] = "Ramp Down (%1 seconds) shorter than $1 seconds.";
        //m[] = "";
//*/
        return m;
    }

    const MessageMap::MessageMap_impl MessageMap::_msgMapAdvc = buildAdvMessageMap();
    const MessageMap::MessageMap_impl MessageMap::_msgMapBasic = buildBasicMessageMap(MessageMap::_msgMapAdvc);
    //const MessageMap& messages = MessageMap::getInstance();

    bool MessageMap::getMessages(ProtocolType t, const StimProtocolIssueInfo &msgInfo, QString &msg)
    {
        const MessageMap_impl& mmap = t == ADVANCED ? _msgMapAdvc : _msgMapBasic;

        for(ErrorMap::const_iterator i = msgInfo._errors.begin(); i != msgInfo._errors.end(); i++)
        {
            QString extra(i.value().size() > 1 ? "s" : "");
            msg += mmap[i.key()].arg(i.value().join(",")).arg(extra);

            ErrorInfoMap::const_iterator addinfo = msgInfo._errorAdditionalInfo.find(i.key());
            if(addinfo != msgInfo._errorAdditionalInfo.end())
            {
                const QVector<ErrorInfo> &errorInfos = addinfo.value();
                for(QVector<ErrorInfo>::const_iterator infoPair = errorInfos.begin();
                    infoPair != errorInfos.end(); infoPair++)
                {
                    msg = msg.replace(infoPair->first, infoPair->second.join(","));
                }
            }
            msg+="\n";
        }
        return !msg.isEmpty();
    }

    bool getIssueMessage(ProtocolType msgType, StimProtocolIssueInfo issueInfo, QString& message)
    {
        return MessageMap::getMessages(msgType, issueInfo, message);
    }

    bool protocolSessionConfigurationHasWarnings(const ProtocolSessionConfiguration &ps, StimProtocolIssueInfo& info)
    {
        int channels = NUM_STIM_CHANNELS;

        // Check Maximum Current Density per Electrode
        // -----------------------------------------
        int maximumCurrentDensity;
        float minsize = 25, size;
        QStringList errInfo;
        //QString cm2("[%2cm%1]");
        //cm2 = cm2.arg(QChar(0x00B2));
        //QStringList channelsToWarn;
        for (int i = 0; i < channels; i++)
        {
            const StimulationElectrodeConfig& e = ps.electrode[i];
            size = 0; // if shown for each electrode
            maximumCurrentDensity = 0;
            if( e.electrodeTypeAdv == STIMULATION_ADV )
            {
                maximumCurrentDensity = maximumCurrentDensityPerChannelAdvanced(e);
            }
            else if ( e.electrodeTypeAdv == RETURN_ADV )
            {
                maximumCurrentDensity = abs(computeReturnValue( ps ));
            }
            //depending on the size of the electrodes, there are three different thresholds
            //until we can determine which is used, we check all of them

            //qDebug()<<"GUI::checkMaximumCurrentDensityPerChannelAdvanced"<<i<<maximumCurrentDensity<<maximumCurrentDensity/PI<<maximumCurrentDensity/8<<maximumCurrentDensity/25;

            //pay attention to the order of the comparisons and that size is overwritten
            if (maximumCurrentDensity/PI>400)
            {
                size = PI;
                minsize = qMin(minsize, size);
                //channelsToWarn.append(electrode.position);
            }
            if (maximumCurrentDensity/8>400)
            {
                size = 8;
                minsize = qMin(minsize, size);
                //channelsToWarn.append(electrode.position);
            }
            if (maximumCurrentDensity/25>400)
            {
                size = 25;
                minsize = qMin(minsize, size);
                //channelsToWarn.append(electrode.position);
            }
            if(size!=0)
            {
                info._errors[WRN_CURR_DANGER].append(e.position);
                //errInfo.append(e.position + cm2.arg(size,0,'g',2));//if info for each electrode
            }
        }
        if(info._errors.contains(WRN_CURR_DANGER))
        {
            //if info about all together
            if(minsize > PI+1)
                errInfo.append(QString::number(minsize,'g',2));
            else
                errInfo.append("PI");
            info._errorAdditionalInfo[WRN_CURR_DANGER].append(ErrorInfo("$1", errInfo));
            info._issues |= StimProtocolIssueInfo::CONTAINS_WARNINGS;
        }
        // Check if ramps are shorter than 3 seconds
        // -----------------------------------------
        if (ps.rampUpDuration<3)
        {
            info._errors[WRN_SHORT_RAMP_UP].append(QString::number(ps.rampUpDuration));
            info._errorAdditionalInfo[WRN_SHORT_RAMP_UP].append(ErrorInfo(QString("$1"), QStringList(QString::number(3))));
            info._issues |= StimProtocolIssueInfo::CONTAINS_WARNINGS;
        }
        if (ps.rampDownDuration<3)
        {
            info._errors[WRN_SHORT_RAMP_DOWN].append(QString::number(ps.rampDownDuration));
            info._errorAdditionalInfo[WRN_SHORT_RAMP_DOWN].append(ErrorInfo(QString("$1"), QStringList(QString::number(3))));
            info._issues |= StimProtocolIssueInfo::CONTAINS_WARNINGS;
        }
        //return ERR_NO_ERROR;
        return info.containsWarnings();
    }

    bool protocolSessionConfigurationIsCorrect(const ProtocolSessionConfiguration &psc, int fwVersion
                                               , bool checkWarnings, StimProtocolIssueInfo& info)
    {
        int channels = NUM_STIM_CHANNELS;
        bool noDeviceConnected = fwVersion == 0;
        QString fwVerStr = noDeviceConnected ? "(device not connected)" : QString::number(fwVersion);
        // Transform template from basic to advanced
        ProtocolSessionConfiguration ps = psc;
        if( psc.protocolType == BASIC)
        {
            StimProtocolSessionConfig::basicSessionConfigurationIsCorrect(psc, info);
            updateAdvancedStimFromBasic(ps);
        }
        // Check tACs has frequency and amplitude
        // ----------------------------------------------
        for (int i = 0; i < channels;i++)
        {
            const StimulationElectrodeConfig& e = ps.electrode[i];
            if( e.Atacs > 0 ){
                if(e.Ftacs == 0){
                    info._errors[ERR_TACS_FREQ_ZERO].append(e.position);
                    info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                }
            }
        }
        // Check return electrode present
        // ----------------------------------------------
        int returnElectrodes = getElectrodeCountAdv(ps, RETURN_ADV);
        int trnsTotal = totalTRNSStimulation(ps);

        if( returnElectrodes == 0 )
        {
            // Check that TDCS currents are equal to 0
            int dcValue = checkTDCSStimulation(ps);
            if ( dcValue !=0)
            {
                info._errors[ERR_TDCS_SUM_NZERO].append(QString::number(dcValue));
                info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
            }

            if(noDeviceConnected || fwVersion >= FWVERSION_TRNS_PAIRS)
            {
                if(trnsTotal != 0)
                {
                    info._errors[ERR_TRNS_SUM_NZERO].append(QString::number(trnsTotal));
                    info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                }
            }
            else
            {   // Check it is not tRNs
                for (int i = 0;i < channels; i++)
                {
                    const StimulationElectrodeConfig& e = ps.electrode[i];
                    if (e.Atrns>0)
                    {
                        info._errors[ERR_TRNS_NO_RET].append(e.position);
                        ErrorInfo fwVer(QString("$1"),QStringList(fwVerStr));
                        info._errorAdditionalInfo[ERR_TRNS_NO_RET].append(fwVer);
                        info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                    }
                }
            }
        }
        if(!noDeviceConnected && fwVersion < FWVERSION_TRNS_PAIRS)
        {
            // Check that sum of tRNs currents does not exceed 650
            // ----------------------------------------------
            if ( trnsTotal > 650 )
            {
                info._errors[ERR_TRNS_OVER_LIMIT].append(QString::number(trnsTotal));
                ErrorInfo trnsLimit(QString("$1"),QStringList(QString::number(650)));
                ErrorInfo fwVer(QString("$2"),QStringList(fwVerStr));
                info._errorAdditionalInfo[ERR_TRNS_OVER_LIMIT].append(trnsLimit);
                info._errorAdditionalInfo[ERR_TRNS_OVER_LIMIT].append(fwVer);
                info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
            }
            // Check no negative tRNs currents are set
            // ----------------------------------------------
            for (int i = 0; i < channels; i++)
            {
                const StimulationElectrodeConfig& e = ps.electrode[i];
                if(e.Atrns < 0)
                {
                    info._errors[ERR_TRNS_NEG].append(e.position);
                    ErrorInfo fwVer(QString("$1"),QStringList(fwVerStr));
                    info._errorAdditionalInfo[ERR_TRNS_NEG].append(fwVer);
                    info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                }
            }            
        }
        // Check that maximum current allowed cannot exceed 2mA
        // ----------------------------------------------
        int stimElectrodes = 0;
        for (int i = 0; i < channels; i++)
        {
            const StimulationElectrodeConfig& e = ps.electrode[i];
            if (e.electrodeTypeAdv == STIMULATION_ADV )
            {
                //changed because all num values of other checks are in this function too
                int maxCurrChan = maximumCurrentDensityPerChannelAdvanced(e);
                if(maxCurrChan > 2000)
                {
                    info._errors[ERR_CHAN_CURR_OVER_LIMIT].append(e.position);
                    info._errorAdditionalInfo[ERR_CHAN_CURR_OVER_LIMIT].append(ErrorInfo(QString("$1"),QStringList(QString::number(2000))));
                    //info._errorAdditionalInfo[ERR_CHAN_CURR_OVER_LIMIT].append(ErrorInfo(QString("$1"),QStringList(QString::number(maxCurrChan))));
                    info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                }
                stimElectrodes++;
            }
        }
        if(returnElectrodes != 0 && stimElectrodes == 0) //this may happen when converting from basic to adv
        {
            info._errors[ERR_RET_NO_STIM].append(QString::number(returnElectrodes));
            info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
        }

        // Check whether there is more than one return electrode
        // ----------------------------------------------
        if ( returnElectrodes > 1)
        {
            for (int i = 0; i < channels; i++)
            {
                const StimulationElectrodeConfig& e = ps.electrode[i];
                if(e.electrodeTypeAdv == RETURN_ADV)
                    info._errors[ERR_MULTIPLE_RET].append(e.position);
            }
            info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
        }

/*      In NIC this is already checked previously
        // Check if there is any repeated position
        // ----------------------------------------------
        if(!checkRepeatedElectrodePosition(ps))
        {
            reason = "Position for some electrodes is repeated";
            loggerMacroDebug( reason )
            return false;
        }
*/
        // NOTE: This does not apply to NICHome, and is not considered wrong in NIC
        /*
        if (getElectrodeCountAdv(ps, EEG_RECORDING_ADV)==0)
        {
            if(ps.preEEG > 0)
            {
                info._errors[ERR_PRE_EEG_TIME_NO_EEG].append(QString::number(ps.preEEG));
                info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
            }
            if(ps.postEEG > 0)
            {
                info._errors[ERR_POST_EEG_TIME_NO_EEG].append(QString::number(ps.postEEG));
                info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
            }
        }
        */
        // Check correctness of sham stimulation
        // ----------------------------------------------
        if( ps.isSham )
        {
            if( (ps.rampSham*2 + ps.rampDownDuration + ps.rampUpDuration) >= ps.stimulationDuration ){
                //reason = "For sham, the stimulation duration must be greater than three times the sham ramp time()." ;
                //loggerMacroDebug( reason )
                //return ERR_SHAM_TIME;
                info._errors[ERR_SHAM_TIME].append(QString::number(ps.stimulationDuration));
                info._errorAdditionalInfo[ERR_SHAM_TIME].append(ErrorInfo(QString("$1"),QStringList(QString::number(ps.rampSham))));
                info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
            }
        }
        if(!info.containsErrors())//if we already have errors, do not even simulate
        {
            // Simulate stimulation during 2 seconds to check impedances
            // -----------------------------------------
            double PIPtacs[channels];
            double PIFtacs[channels];

            for (int i = 0; i < channels; i++)
            {
                const StimulationElectrodeConfig& e = ps.electrode[i];
                PIPtacs[i] = PI*e.Ptacs/180;
                PIFtacs[i] = 2*PI*e.Ftacs;
            }


            float current=0;
            float currentPerSample=0;
            float positiveCurrentPerSample=0;
            double maxTACS=0.0;
            double tacsCurrent[channels];

            for (float t=0;t<2;t+=0.001)
            {
                //We initially get TDCS and TACS currents
                for (int i = 0; i < channels;i++)
                {
                    const StimulationElectrodeConfig& e = ps.electrode[i];

                    tacsCurrent[i] = e.Atacs*cos(t*PIFtacs[i] + PIPtacs[i]);

                    maxTACS=qMax(maxTACS,tacsCurrent[i]);

                    if (abs(e.Atdcs + tacsCurrent[i] + abs(e.Atrns)) > 2000.00)
                    {
                        info._errors[ERR_SIM_CHAN_CURR_OVER_LIMIT].append(e.position);
                        info._errorAdditionalInfo[ERR_SIM_CHAN_CURR_OVER_LIMIT].append(ErrorInfo(QString("$1"),QStringList(QString::number(2))));
                        info._errorAdditionalInfo[ERR_SIM_CHAN_CURR_OVER_LIMIT].append(ErrorInfo(QString("$2"),QStringList(QString::number(t,'g',2))));
                        info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                        return false;
                    }
                }

                // We must try with a positive and a negative TRNS
                for (int j=0;j<2;j++)
                {
                    currentPerSample = 0;
                    positiveCurrentPerSample = 0;
                    int sign = (j==0) ? 1 : -1; // (j==0) => Positive / (j==1) => Negative
                    for (int i = 0;i < channels ; i++)
                    {
                        const StimulationElectrodeConfig& e = ps.electrode[i];
                        current = e.Atdcs + tacsCurrent[i] + sign*e.Atrns;

                        if (current>0)
                            positiveCurrentPerSample+=current;

                        currentPerSample+=current;
                    }

                    if (returnElectrodes != 0)
                    {
                        if (fabs(currentPerSample)>2000.00)
                        {
                            info._errors[ERR_SIM_RET_CURR_OVER_LIMIT].append(findReturnElectrodePositionAdv(ps));
                            info._errorAdditionalInfo[ERR_SIM_RET_CURR_OVER_LIMIT].append(ErrorInfo(QString("$1"),QStringList(QString::number(2))));
                            info._errorAdditionalInfo[ERR_SIM_RET_CURR_OVER_LIMIT].append(ErrorInfo(QString("$2"),QStringList(QString::number(currentPerSample/1000.0))));
                            info._errorAdditionalInfo[ERR_SIM_RET_CURR_OVER_LIMIT].append(ErrorInfo(QString("$3"),QStringList(QString::number(t))));
                            info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                            return false;
                        }
                    }
                    else
                    {
                        if (fabs(currentPerSample)>0.01)
                        {
                            info._errors[ERR_SIM_STIM_CURR_SUM_NZERO].append(QString::number(currentPerSample,'g',2));
                            info._errorAdditionalInfo[ERR_SIM_STIM_CURR_SUM_NZERO].append(ErrorInfo(QString("$1"),QStringList(QString::number(0))));
                            info._errorAdditionalInfo[ERR_SIM_STIM_CURR_SUM_NZERO].append(ErrorInfo(QString("$2"),QStringList(QString::number(t))));
                            info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                            return false;
                        }
                    }
                    if (returnElectrodes != 0)
                    {
                        if (currentPerSample<0)
                            positiveCurrentPerSample+=abs(currentPerSample);
                    }

                    double absInjCurrent = fabs(positiveCurrentPerSample);
                    if (absInjCurrent > 4000.0)
                    {
                        info._errors[ERR_SIM_INJ_CURR_OVER_LIMIT].append(QString::number(absInjCurrent/1000.0,'g',2));
                        info._errorAdditionalInfo[ERR_SIM_INJ_CURR_OVER_LIMIT].append(ErrorInfo(QString("$1"),QStringList(QString::number(4))));
                        info._errorAdditionalInfo[ERR_SIM_INJ_CURR_OVER_LIMIT].append(ErrorInfo(QString("$2"),QStringList(QString::number(t))));
                        info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
                        return false;
                    }
                }
            } // END: for (float t=0;t<2;t+=0.001)
        }
        if(checkWarnings) //we ignore the return value of the warning check, but take note of the cause
        {
            protocolSessionConfigurationHasWarnings(ps, info);
        }
        return !info.containsErrors();
    }

    bool basicSessionConfigurationIsCorrect(const ProtocolSessionConfiguration& psc, StimProtocolIssueInfo& info)
    {
        if(psc.protocolType != BASIC)
            return false;

        int channels = NUM_STIM_CHANNELS;
        int sumOfPercentages=0;
        bool returnElectrodes=false;

        //for(int i=0; i < Electrodes::deviceStimulationChannels(); i++)
        for(int i=0; i < channels; i++)
        {
            const StimulationElectrodeConfig & e = psc.electrode[i];
            if (e.electrodeTypeBasic == RETURN_BASIC)
            {
                returnElectrodes = true;
                sumOfPercentages += e.retValue;
            }
        }

        qDebug() <<"The sum is" << sumOfPercentages;

        if (returnElectrodes && (sumOfPercentages < 96 || sumOfPercentages > 100))
        {
            info._errors[ERR_SIM_STIM_CURR_SUM_NZERO].append(QString::number(sumOfPercentages));
            info._errorAdditionalInfo[ERR_SIM_STIM_CURR_SUM_NZERO].append(ErrorInfo(QString("$1"),QStringList(QString::number(96))));
            info._errorAdditionalInfo[ERR_SIM_STIM_CURR_SUM_NZERO].append(ErrorInfo(QString("$2"),QStringList(QString::number(100))));
            info._issues |= StimProtocolIssueInfo::CONTAINS_ERRORS;
        }
        return !info.containsErrors();
    }
}

StimProtocolTemplate::StimProtocolTemplate(const StimProtocolTemplate &spt)
{    //alas mistery tales about copy constructors abound in these lands
    _data = spt._data;
    _eLayout = spt._eLayout;
}

const QString& StimProtocolTemplate::getName() const
{
    return _data.name;
}

void StimProtocolTemplate::setName(const QString& name)
{
    _data.name = name;
}

int StimProtocolTemplate::getChannelCount() const
{
    return _eLayout.size();
}

QVector<unsigned int> StimProtocolTemplate::getElectrodeLayout() const
{
    return _eLayout;
}

bool StimProtocolTemplate::setElectrodeLayout(const QVector<unsigned int> &e)
{
    //int size = (NUM_STIM_CHANNELS);
    //int size = _data.electrode.size();//electrode is a static C array for now
    switch(e.size())
    {
    case 8:
    //case 20:
    //case 32:
        _eLayout = e;
        for(int i = 0; i < e.size(); i++)
        {
            if(Electrodes::ElectrodePosition::isPosition(_eLayout[i]))
            {
                _data.electrode[i].position = Electrodes::getPositionName(_eLayout[i]);
            }
            else
            {
                 unsigned int id = Electrodes::getChannelIDs(e.size())[i];//replace with
                 //unsigned int id = Electrodes::getChannelIDs()[i];
                _eLayout[i] = id;
                _data.electrode[i].position = Electrodes::getChannelName(id);
            }
        }
        return true;
    default:
        return false;
    }
}

void StimProtocolTemplate::setElectrodeConfig(const StimulationElectrodeConfig &econf, unsigned int e)
{
    _data.electrode[e] = econf;
}

const StimulationElectrodeConfig& StimProtocolTemplate::getElectrodeConfig(unsigned int e) const
{
    return _data.electrode[e];
}

QString StimProtocolTemplate::toString() const
{
    return _data.toString();
}

QString StimProtocolTemplate::guiText() const
{
    QString str = _data.name + "\t", electrodeText;
    switch(_data.protocolType)
    {
    case BASIC:
        str+= "Stimulation ";
        for(int i = 0; i < NUM_STIM_CHANNELS ; ++i)
        {
            switch(_data.electrode[i].electrodeTypeBasic)
            {
            case STIM_ANODAL_BASIC:
                electrodeText.prepend("Anodal "
                                      + QString::number(_data.electrode[i].ampValue)
                                      + "uA "
                                      + _data.electrode[i].position);
                break;
            case STIM_CATHODAL_BASIC:
                electrodeText.prepend("Cathodal "
                                      + QString::number(_data.electrode[i].ampValue)
                                      + "uA "
                                      + _data.electrode[i].position);
                break;
            case RETURN_BASIC:
                electrodeText += "/"+ _data.electrode[i].position;
                break;
            default://case EEG_RECORDING_BASIC:
                break;
            }
        }
        switch(_data.protocolBasicType)
        {
        case BASIC_TYPE_TDCS:
            str+="tDCS ";
            break;
        case BASIC_TYPE_TACS:
            str+="tACS ";
            break;
        case BASIC_TYPE_TRNS:
            str+="tRNS ";
            break;
        }
        str += electrodeText;
        break;
    case ADVANCED:
        str+= "Advanced configuration";
        break;
    }
    return str;
}

typedef QHash<QString, StimProtocolTemplate> StimTemplateMap;

template<>
QString serializeProtocolTemplateMap<StimProtocolTemplate>(const StimTemplateMap &tm)
{
    QDomDocument    doc;
    QDomElement root = doc.createElement("ProtocolTemplates");
    doc.appendChild(root);
    for(StimTemplateMap::const_iterator it = tm.constBegin(); it != tm.constEnd(); ++it)
    {
        StimProtocolTemplate st = it.value();

        QDomElement tag = doc.createElement("Template");
        root.appendChild(tag);

        QDomElement tagName = doc.createElement("name");
        tag.appendChild(tagName);

        QDomText t = doc.createTextNode(st.getName());
        tagName.appendChild(t);

        tagName = doc.createElement("isAdvancedConfiguration");
        tag.appendChild(tagName);

        t = doc.createTextNode(st._data.protocolType == ADVANCED ? "1" : "0");
        tagName.appendChild(t);

        tagName = doc.createElement("stimulationTime");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.stimulationDuration));
        tagName.appendChild(t);

        tagName = doc.createElement("rampUp");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.rampUpDuration));
        tagName.appendChild(t);

        tagName = doc.createElement("rampDown");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.rampDownDuration));
        tagName.appendChild(t);

        tagName = doc.createElement("preEEG");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.preEEG));
        tagName.appendChild(t);

        tagName = doc.createElement("postEEG");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.postEEG));
        tagName.appendChild(t);

        tagName = doc.createElement("isSham");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.isSham));
        tagName.appendChild(t);

        tagName = doc.createElement("rampSham");
        tag.appendChild(tagName);

        t = doc.createTextNode(QString::number(st._data.rampSham));
        tagName.appendChild(t);

        tagName = doc.createElement("type");
        tag.appendChild(tagName);

        ProtocolBasicType type = st._data.protocolBasicType;
        QString typeStr;
        switch( type )
        {
        case BASIC_TYPE_TDCS : typeStr = "0";
            break;
        case BASIC_TYPE_TACS : typeStr = "1";
            break;
        case BASIC_TYPE_TRNS : typeStr = "2";
            break;
        }
        t = doc.createTextNode(typeStr);
        tagName.appendChild(t);

        for(int i=0;i< NUM_STIM_CHANNELS;i++)
        {
            QDomElement chanTag = doc.createElement("Channel");
            tag.appendChild(chanTag);

            tagName = doc.createElement("index");
            chanTag.appendChild(tagName);

            t = doc.createTextNode(QString::number(i+1));
            tagName.appendChild(t);

            tagName = doc.createElement("electrodeType");
            chanTag.appendChild(tagName);

            switch(st._data.protocolType)
            {
            case BASIC:
                switch(st._data.electrode[i].electrodeTypeBasic)
                {
                case NOT_USED_BASIC:
                    typeStr = "0";
                    break;
                case EEG_RECORDING_BASIC:
                    typeStr = "1";
                    break;
                case STIM_ANODAL_BASIC:
                    typeStr = "2";
                    break;
                case STIM_CATHODAL_BASIC:
                    typeStr = "3";
                    break;
                case RETURN_BASIC:
                    typeStr = "4";
                    break;
                }
                t = doc.createTextNode(typeStr);
                tagName.appendChild(t);

                tagName = doc.createElement("ampValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].ampValue));
                tagName.appendChild(t);

                tagName = doc.createElement("offsetValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].offsetValue));
                tagName.appendChild(t);

                tagName = doc.createElement("freqValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].freqValue,'f',2));
                tagName.appendChild(t);

                tagName = doc.createElement("retValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].retValue));
                tagName.appendChild(t);
                break;
            case ADVANCED:
                switch(st._data.electrode[i].electrodeTypeAdv)
                {
                case NOT_USED_ADV:
                    typeStr = "0";
                    break;
                case EEG_RECORDING_ADV:
                    typeStr = "1";
                    break;
                case STIMULATION_ADV:
                    typeStr = "2";
                    break;
                case RETURN_ADV:
                    typeStr = "3";
                    break;
                }
                t = doc.createTextNode(typeStr);
                tagName.appendChild(t);

                tagName = doc.createElement("atdcsValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].Atdcs));
                tagName.appendChild(t);

                tagName = doc.createElement("atacsValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].Atacs));
                tagName.appendChild(t);

                tagName = doc.createElement("atrnsValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].Atrns));
                tagName.appendChild(t);

                tagName = doc.createElement("ftacsValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].Ftacs,'f',2));
                tagName.appendChild(t);

                tagName = doc.createElement("ptacsValue");
                chanTag.appendChild(tagName);

                t = doc.createTextNode(QString::number(st._data.electrode[i].Ptacs));
                tagName.appendChild(t);
                break;
            }
            tagName = doc.createElement("channelName");
            chanTag.appendChild(tagName);

            t = doc.createTextNode(st._data.electrode[i].position);
            tagName.appendChild(t);
        }
    }
    doc.appendChild(root);
    return doc.toString();
}

template<>
bool parseProtocolTemplate<StimProtocolTemplate>(QDomNode& n, StimProtocolTemplate& spt)
{
    QDomNode node = n.firstChild();
    bool ok = false;
    spt._data.protocolId = 0;
    ok = (node.isElement())&&(node.toElement().tagName()=="name");
    if(ok)
    {
        spt.setName(node.toElement().text());
        qDebug() << "Parsing template " << spt.getName();
    }
    else
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <name>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="isAdvancedConfiguration");
    if(ok)
    {
        int adv = node.toElement().text().toInt(&ok);
        if(adv == 0)
        {
            spt._data.protocolType = BASIC;
        }
        else
        {
            spt._data.protocolType = ADVANCED;
        }
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <isAdvancedConfiguration>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="stimulationTime");
    if(ok)
    {
        spt._data.stimulationDuration = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <stimulationTime>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="rampUp");
    if(ok)
    {
        spt._data.rampUpDuration = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <rampUp>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="rampDown");
    if(ok)
    {
        spt._data.rampDownDuration = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <rampDown>";
        return false;
    }
    node = node.nextSibling();

    ok = (node.isElement())&&(node.toElement().tagName()=="preEEG");
    if(ok)
    {
        spt._data.preEEG = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <preEEG>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="postEEG");
    if(ok)
    {
        spt._data.postEEG = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <postEEG>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="isSham");
    if(ok)
    {
        spt._data.isSham = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <isSham>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="rampSham");
    if(ok)
    {
        spt._data.rampSham = node.toElement().text().toInt(&ok);
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <rampSham>";
        return false;
    }
    node = node.nextSibling();
    ok = (node.isElement())&&(node.toElement().tagName()=="type");
    if(ok)
    {
        int type = node.toElement().text().toInt(&ok);
        switch( type )
        {
        case 0: spt._data.protocolBasicType = BASIC_TYPE_TDCS;
            break;
        case 1: spt._data.protocolBasicType = BASIC_TYPE_TACS;
            break;
        case 2: spt._data.protocolBasicType = BASIC_TYPE_TRNS;
            break;
        }
    }
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing <type>";
        return false;
    }
    node = node.nextSibling();

    QDomNode channelNode;
    int index;
    int channels = NUM_STIM_CHANNELS;
    QVector<unsigned int> elayout;
    QVector<unsigned int> chanIDs = Electrodes::getChannelIDs(channels);
    for (int i=0;i< channels;i++)
    {
        ok = (node.isElement())&&(node.toElement().tagName()=="Channel");
        if(ok)
        {
            channelNode = node.firstChild();
            ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="index");
            if (ok)
            {
                index = channelNode.toElement().text().toInt(&ok);
            }
            if(!ok)
            {
                qDebug()<<"parseStimProtocolTemplate ERROR parsing <index>";
                return false;
            }
            channelNode = channelNode.nextSibling();
            //qDebug() << "parseStimProtocolTemplate parsing Channel " << index;
            ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="electrodeType");
            if(ok)
            {
                int etype = channelNode.toElement().text().toInt(&ok);
                switch( etype )
                {
                case 0:
                    spt._data.electrode[index-1].electrodeTypeBasic = NOT_USED_BASIC;
                    spt._data.electrode[index-1].electrodeTypeAdv = NOT_USED_ADV;
                    break;
                case 1:
                    spt._data.electrode[index-1].electrodeTypeBasic = EEG_RECORDING_BASIC;
                    spt._data.electrode[index-1].electrodeTypeAdv = EEG_RECORDING_ADV;
                    break;
                case 2:
                    spt._data.electrode[index-1].electrodeTypeBasic = STIM_ANODAL_BASIC;
                    spt._data.electrode[index-1].electrodeTypeAdv = STIMULATION_ADV;
                    break;
                case 3:
                    spt._data.electrode[index-1].electrodeTypeBasic = STIM_CATHODAL_BASIC;
                    spt._data.electrode[index-1].electrodeTypeAdv = RETURN_ADV;
                    break;
                case 4:
                    spt._data.electrode[index-1].electrodeTypeBasic = RETURN_BASIC;
                    break;
                }
            }
            if(!ok)
            {
                qDebug()<<"parseStimProtocolTemplate ERROR parsing <electrodeType>";
                return false;
            }
            channelNode = channelNode.nextSibling();

            if (spt._data.protocolType == ADVANCED)
            {
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="atdcsValue");
                if(ok)
                {
                    spt._data.electrode[index-1].Atdcs = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <atdcsValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="atacsValue");
                if(ok)
                {
                    spt._data.electrode[index-1].Atacs = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <atacsValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="atrnsValue");
                if(ok)
                {
                    spt._data.electrode[index-1].Atrns = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <atrnsValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="ftacsValue");
                if(ok)
                {
                    spt._data.electrode[index-1].Ftacs = channelNode.toElement().text().toDouble(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <ftacsValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="ptacsValue");
                if(ok)
                {
                    spt._data.electrode[index-1].Ptacs = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <ptacsValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
            }
            else if(spt._data.protocolType == BASIC)
            {
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="ampValue");
                if(ok)
                {
                    spt._data.electrode[index-1].ampValue = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <ampValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="offsetValue");
                if(ok)
                {
                    spt._data.electrode[index-1].offsetValue = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <offsetValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="freqValue");
                if(ok)
                {
                    spt._data.electrode[index-1].freqValue = channelNode.toElement().text().toDouble(&ok);                    
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <freqValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="retValue");
                if(ok)
                {
                    spt._data.electrode[index-1].retValue = channelNode.toElement().text().toInt(&ok);
                }
                if(!ok)
                {
                    qDebug()<<"parseStimProtocolTemplate ERROR parsing <retValue>";
                    return false;
                }
                channelNode = channelNode.nextSibling();
            }
            //If XML is in old format, <channelpos> should be the next field
            ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="channelpos");
            if(ok)
            {
                int chpos = channelNode.toElement().text().toInt(&ok);
                if(ok)
                {
                    QString oldEname = Electrodes::getOldElectrodeName(chpos-1);
                     elayout.append(Electrodes::getPositionID(oldEname));
                }
            }
            else //otherwise, the next field should be <channelName>
            {
                ok = (channelNode.isElement())&&(channelNode.toElement().tagName()=="channelName");
                if(ok)
                {
                    elayout.append(Electrodes::getPositionID(channelNode.toElement().text()));
                }
            }
            if(!ok)
            {
                qDebug()<<"parseStimProtocolTemplate ERROR parsing <channelpos> or <channelName>";
                return false;
            }
            channelNode = channelNode.nextSibling();
        }
        if(!ok)
        {
            qDebug()<<"parseStimProtocolTemplate ERROR parsing <Channel>";
            return false;
        }
        node = node.nextSibling();
    }
    ok = spt.setElectrodeLayout(elayout);
    if(!ok)
    {
        qDebug()<<"parseStimProtocolTemplate ERROR parsing channel configuration";
        return false;
    }
    ok = (node.isElement())&&(node.toElement().tagName()=="Info");
    if(ok)
    {
        QDomNode infoNode;

        infoNode = node.firstChild();
        ok = (infoNode.isElement())&&(infoNode.toElement().tagName()=="displayText");
        if(ok)
        {
            spt._data.displayText = infoNode.toElement().text();
            infoNode = infoNode.nextSibling();
        }
        if(!ok)
        {
            qDebug()<<"parseStimProtocolTemplate ERROR parsing <displayText>";
            return false;
        }
        ok = (infoNode.isElement())&&(infoNode.toElement().tagName()=="displayReference");
        if(ok)
        {

            spt._data.displayReference=infoNode.toElement().text();
            infoNode = infoNode.nextSibling();
        }
        if(!ok)
        {
            qDebug()<<"parseStimProtocolTemplate ERROR parsing <displayReference>";
            return false;
        }
        ok = (infoNode.isElement())&&(infoNode.toElement().tagName()=="displayLink");
        if(ok)
        {
            spt._data.displayLink=infoNode.toElement().text();
            infoNode = infoNode.nextSibling();
        }
        if(!ok)
        {
            qDebug()<<"parseStimProtocolTemplate ERROR parsing <displayLink>";
            return false;
        }
    }
    else
    {
        spt._data.displayText="";
        spt._data.displayReference="";
        spt._data.displayLink="";
    }
    return true;
}

template<>
unsigned int getProtocolTemplateChannels<StimProtocolTemplate>(const StimProtocolTemplate &ptemplate)
{
    return ptemplate.getChannelCount();
}

template<>
const QString& getProtocolTemplateName<StimProtocolTemplate>(const StimProtocolTemplate& pt)
{
    return pt.getName();
}

template<>
void setProtocolTemplateName<StimProtocolTemplate>(StimProtocolTemplate& pt, const QString& name)
{
    pt.setName(name);
}
