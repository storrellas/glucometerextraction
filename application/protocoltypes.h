#ifndef _PROTOCOLTYPES_
#define _PROTOCOLTYPES_
#include <QVector>
#include <QHash>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QMutex>

#include <commonparameters.h>

typedef enum StimStatus{ STIM_STATUS_PENDING = 0, STIM_STATUS_FAILED = 1 , STIM_STATUS_OK = 2} StimStatus;

// Stores a give Session of stimulation
class StimSession{

public:
    StimSession() : sessionId(-1), percentageCompleted(0){}

    /*!
     * \brief sessionId id for the session
     */
    int sessionId;

    /*!
     * \brief protocolId Identifiaction of the protocol for this session
     */
    int protocolId;

    /*!
     * \brief datePrescribed Date of prescription
     */
    QDateTime datePrescribed;

    /*!
     * \brief dateExecution Date of execution
     */
    QDateTime dateExecution;

    /*!
     * \brief percentageCompleted  Percentage of completition
     */
    int percentageCompleted;

    /*!
     * \brief stimStatus status for this session
     */
    StimStatus stimStatus;

    /*!
     * \brief preQuestionnaireId id for the Pre Questionnaire
     */
    int preQuestionnaireId;

    /*!
     * \brief postQuestionnaireId id for the preQuestionnaire
     */
    int postQuestionnaireId;

    /*!
     * \brief answerPreQuestionnaire answers of prequestionnaire
     */
    QVector<QString> answerPreQuestionnaire;

    /*!
     * \brief answerPostQuestionnaire answer of postQuestionnaire
     */
    QVector<QString> answerPostQuestionnaire;



    /*!
     * \brief toString converts all the values to a string representation
     * \return
     */
    QString toString(){
        QString str = "sessionId:"      + QString::number(sessionId) + "\n" +
                      "protocolId:"     + QString::number(protocolId) + "\n" +
                      "datePrescribed:" + datePrescribed.toString("yyyy-MM-dd HH:mm") + "\n" +
                      "dateExecution:"  + dateExecution.toString("yyyy-MM-dd HH:mm") + "\n" +
                      "percentageCompleted:"     + QString::number(percentageCompleted) + "\n";

        switch(stimStatus){
            case STIM_STATUS_PENDING: str += "stimStatus: PENDING \n";
                break;
            case STIM_STATUS_FAILED:  str += "stimStatus: FAILED \n";
                break;
            case STIM_STATUS_OK:      str += "stimStatus: OK \n";
                break;
        }

        str += "preQuestionnaireId  :" + QString::number(preQuestionnaireId) + "\n"
               "postQuestionnaireId :" + QString::number(postQuestionnaireId) + "\n";

        str += "answerPreQuestionnaire:  ";
        for(int i = 0; i < answerPreQuestionnaire.size(); i++ ){
            str += answerPreQuestionnaire.at(i) + " ";
        }
        str += "\n";

        str += "answerPostQuestionnaire: ";
        for(int i = 0; i < answerPostQuestionnaire.size(); i++ ){
            str += answerPostQuestionnaire.at(i) + " ";
        }
        str += "\n";

        return str;
    }
};

typedef enum ElectrodeTypeAdv  {NOT_USED_ADV = 0,   EEG_RECORDING_ADV   = 1, STIMULATION_ADV = 2, RETURN_ADV = 3 } ElectrodeTypeAdv;
typedef enum ElectrodeTypeBasic{NOT_USED_BASIC = 0, EEG_RECORDING_BASIC = 1, STIM_ANODAL_BASIC = 2, STIM_CATHODAL_BASIC = 3, RETURN_BASIC = 4 } ElectrodeTypeBasic;
typedef enum ProtocolType{BASIC = 0, ADVANCED = 1 } ProtocolType;
typedef enum ErrorBehavior{ERROR_BHV_SKIP = 0, ERROR_BHV_REPEAT = 1, ERROR_BHV_CONTINUE = 2} ErrorBehaviour;
typedef enum ProtocolBasicType{BASIC_TYPE_TDCS = 0, BASIC_TYPE_TACS = 1, BASIC_TYPE_TRNS = 2 } ProtocolBasicType;

/*!
 * A struct storing the configuration parameters for each electrode
 */
class StimulationElectrodeConfig {

public:
    StimulationElectrodeConfig() : electrodeTypeBasic(NOT_USED_BASIC)
      , ampValue(0), offsetValue(0), freqValue(0), retValue(0)
      , electrodeTypeAdv(NOT_USED_ADV), Atdcs(0), Atacs(0), Ftacs(0), Ptacs(0), Atrns(0)
    {}

    StimulationElectrodeConfig(const StimulationElectrodeConfig &other) :
        electrodeTypeBasic(other.electrodeTypeBasic), ampValue(other.ampValue)
      , offsetValue(other.offsetValue), freqValue(other.freqValue), retValue(other.retValue)
      , electrodeTypeAdv(other.electrodeTypeAdv), Atdcs(other.Atdcs), Atacs(other.Atacs)
      , Ftacs(other.Ftacs), Ptacs(other.Ptacs), Atrns(other.Atrns), position(other.position)
    {
    }

    // Configuration of the electrodes - Basic mode
    ElectrodeTypeBasic electrodeTypeBasic;
    int    ampValue;
    int    offsetValue;
    double freqValue;
    int    retValue;

    // Configuration of the electrodes - Advanced mode
    ElectrodeTypeAdv    electrodeTypeAdv;
    int     Atdcs;
    int     Atacs;
    double  Ftacs;
    int     Ptacs;
    int     Atrns;
    QString position;

    QString toString() const{
        QString str = "electrodeTypeBasic: " + QString::number(electrodeTypeBasic) + "\n" +
                      "ampValue: "     + QString::number(ampValue) + "\n" +
                      "offsetValue: "  + QString::number(offsetValue) + "\n" +
                      "freqValue: "    + QString::number(freqValue) + "\n"
                      "retValue: "     + QString::number(retValue) + "\n";

        str += "electrodeTypeAdv: " + QString::number(electrodeTypeAdv) + "\n" +
               "Atdcs: " + QString::number(Atdcs) + "\n" +
               "Atacs: " + QString::number(Atacs) + "\n" +
               "Ftacs: " + QString::number(Ftacs) + "\n" +
               "Ptacs: " + QString::number(Ptacs) + "\n" +
               "Atrns: " + QString::number(Atrns) + "\n" +
               "position: " + position;

        return str;
    }
};

/*!
 * A struct storing the template of the stimulation
 */
class ProtocolSessionConfiguration
{

public:
    // Public constructor
    ProtocolSessionConfiguration() :
        protocolId(-1), protocolType(ADVANCED)
      , errorBehavior(ERROR_BHV_SKIP), protocolBasicType(BASIC_TYPE_TDCS)
      , stimulationDuration(0), rampUpDuration(0), rampDownDuration(0)
      , preEEG(0), postEEG(0), isSham(false), rampSham(0), isTACSorTRNS(false)
    {
    }

    ProtocolSessionConfiguration(const ProtocolSessionConfiguration& other) :
        protocolId(other.protocolId), protocolType(other.protocolType)
      , errorBehavior(other.errorBehavior), protocolBasicType(other.protocolBasicType)
      , stimulationDuration(qMax(0, other.stimulationDuration))
      , rampUpDuration(qMax(0, other.rampUpDuration))
      , rampDownDuration(qMax(0, other.rampDownDuration))
      , preEEG(qMax(0, other.preEEG))
      , postEEG(qMax(0, other.postEEG))
      , isSham(other.isSham)
      , rampSham(qMax(0, other.rampSham)), isTACSorTRNS(other.isTACSorTRNS)
    {   //it is noted that isTACSorTRNS is used internally
        name = other.name;
        description = other.description;
        percentageSessionRestart = other.percentageSessionRestart;
        percentageSessionSkip = other.percentageSessionSkip;
        //int nchannels = other.getChannelCount();
        int nchannels = NUM_STIM_CHANNELS;
        //electrode.resize(nchannels);
        for(int i = 0; i < nchannels; i++)
        {
            electrode[i] = other.electrode[i];
        }
        displayText = other.displayText;
        displayReference = other.displayReference;
        displayLink = other.displayLink;
    }
    // Protocol profile
    int protocolId;
    QString name;
    QString description;


    ProtocolType      protocolType; // BASIC/ADVANCED

    // Behavior in abort
    ErrorBehavior     errorBehavior;
    int               percentageSessionRestart;
    int               percentageSessionSkip;

    // Type of protocol in basic type
    ProtocolBasicType protocolBasicType; // TDCS,TACS,TRNS




    // Configuration time
    int      stimulationDuration;          // Stimulation time
    int      rampUpDuration;    // rampUp duration
    int      rampDownDuration;  // rampDown duration
    int      preEEG,postEEG;    // preEEG, postEEG duration

    bool     isSham;
    int      rampSham;          // Sham ramp


    // Electrode configuration
    StimulationElectrodeConfig electrode[NUM_STIM_CHANNELS];

    // Used internally
    bool isTACSorTRNS;

    QString  displayText;
    QString  displayReference;
    QString  displayLink;

    QString toString() const{
        QString str = "protocolId: "     + QString::number(protocolId) + " \n" +
                      "name: "           + name + "\n" +
                      "description: "    + description + "\n" +
                      "protocolType: "   + QString::number(protocolType) + "\n";
        switch(errorBehavior){
        case ERROR_BHV_SKIP :     str += "errorBehavior: SKIP\n";
            break;
            case ERROR_BHV_REPEAT :   str += "errorBehavior: REPEAT\n";
            break;
            case ERROR_BHV_CONTINUE : str += "errorBehavior: CONTINUE\n";
            break;
        }

        str += "percentageSessionRestart: " + QString::number(percentageSessionRestart)  + "\n"
               "percentageSessionSkip: "    + QString::number(percentageSessionSkip) + "\n";

        str += "protocolBasicType: "        + QString::number(protocolBasicType) + "\n";

        str += "stimulationDuration: " + QString::number(stimulationDuration) + "\n" +
               "rampUpDuration: " + QString::number(rampUpDuration) + "\n" +
               "rampDownDuration: " + QString::number(rampDownDuration) + "\n" +
               "preEEG: " + QString::number(preEEG) + "\n" +
               "postEEG: " + QString::number(postEEG) + "\n" +
               "isSham: "         + QString::number(isSham) + "\n" +
               "rampSham: " + QString::number(rampSham) + "\n";

        // to String of each electrode
        for(int i = 0; i < NUM_STIM_CHANNELS; i++){
            str += "--> Electrode" + QString::number(i) + "\n";
            str += electrode[i].toString() + "\n";
        }

        str += "isTACSorTRNS: " + QString::number(isTACSorTRNS);

        return str;
    }

};

#endif //_PROTOCOLTYPES_
