#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

// Compiler includes
#include <math.h>

// Qt includes
#include <QObject>
#include <QElapsedTimer>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>

// Project includes
#include "devicemanager.h"
#include "commonparameters.h"
#include "stimprotocoltemplate.h"
#include "protocoltypes.h"
#include "devicemanagertypes.h"

#define MAX_IC_RANGE  (20000)

/*!
 * \enum StimulationState
 *
 * Description of the possible status of the file writer.
 */
typedef enum StimulationState
{
    STIM_STOPPED = 0,
    STIM_PREEEG = 1,
    STIM_STIMULATION = 2,
    STIM_POSTEEG = 3,
    STIM_WAITINGFORSECONDSHAM = 5,
    STIM_DOINGSECONDSHAM = 6,
    STIM_RAMPUP = 8,
    STIM_RAMPDOWN = 9,
    STIM_IMPEDANCECHECK = 10,
    STIM_FIRSTSHAM_RAMPUP = 11,
    STIM_FIRSTSHAM_STIMULATION = 12,
    STIM_FIRSTSHAM_RAMPDOWN = 13,
    STIM_SECONDSHAM_RAMPUP  = 14,
    STIM_SECONDSHAM_RAMPDOWN = 15

}StimulationState;

/*!
 * \brief The ProtocolManager class
 */
class ProtocolManager : public QObject
{
    Q_OBJECT
public:

    // ATTRIBUTES
    // ------------------------------

    /*!
     * \brief _currentState Holds the current state of the stimulation
     */
    StimulationState _currentState;

    // FUNCTIONS
    // ------------------------------

    /*!
     * \brief ProtocolManager Public constructor
     */
    ProtocolManager(DeviceManager* device);

    /*!
     * Default destructor.
     */
    ~ProtocolManager();

    /*!
     * \brief delay Delays the execution for nSecs
     * \param nSecs
     */
    void delay(int nSecs);

    /*!
     * \brief setFwVersion sets the current fwVersion
     * \param fwVersion
     */
    void firmwareVersion(int value);

    /*!
     * \brief getCurrentState returns the current state of the stimulation
     * \return
     */
    StimulationState getCurrentState();

    // Control stimulaion

    /*!
     * \brief fillTACSorTRNS fills whether current configuration is tACS or tRNS
     * \param currentConfiguration
     */
    void fillTACSorTRNS(ProtocolSessionConfiguration *currentConfiguration);

    /*!
     * This starts the stimulation
     */
    bool startStimulationProtocol(StimSession stimSession, ProtocolSessionConfiguration currentConfiguration);

    /*!
     * \brief updateAdvancedStimFromBasic fills the advanced stimulation table from basic
     */
    static void updateAdvancedStimFromBasic(ProtocolSessionConfiguration *currentConfiguration);

    /*!
     * This stops the stimulation
     */
    bool stopStimulationProtocol();

    /**
     * DEC2FRAC: Return the number X as the fraction of two numbers, NUM/DEN.
     *
     * \param X is the input number to be converter
     * \param NUM is the pointer to the numerator
     * \param DEN is the pointer to the denominator
     *
     * */
    enum VariableLength_t{ DEC2FRAC_8BIT, DEC2FRAC_7BIT};
    void dec2frac(double x, uint8_t *num, uint8_t *den, VariableLength_t variable_length);

    /*!
     * \brief isSessionFinished Indicates whether session was finished.
     * Controls whether current protocol is NOT impendance check and sham stimulation has
     * been completed (i.e. two pulses are completed)
     * \return
     */
    bool isSessionFinished();

    /*!
     * \brief setImpedanceCheck sets the value for _isCheckingImpedance
     * \param value
     */
    void setCheckingImpedance( bool value ){ _isCheckingImpedance = value; }

    /*!
     * \brief isCheckingImpedance returns whether it is checking the impedance and sets _isCheckingImpedance to false
     * \return
     */
    bool checkingImpedance(){ return _isCheckingImpedance; }

    // Information retrieval

    /*!
     * \brief getProtocolId returns the protocolId of the current session
     * \return
     */
    StimSession getCurrentSession(){ return _stimSession; }

    /*!
     * \brief getStimulationDurationElapsed returns the number of seconds elapsed
     * from stimulation start
     * \return
     */
    int getStimulationDurationElapsed(){ return _stimDurationEllapsed;}

    /*!
     * \brief getStimulationDurationElapsedFromResume returns the number of seconds elapsed
     * when resume was clicked
     * \return
     */
    int getStimulationDurationElapsedFromResume() { return _stimDurationElapsedFromResume; }

    // Impedance Check

    /*!
     * \brief manualImpedanceCheckProtocol impedance check for the indicated protocolSessionConfiguration
     */
    void manualImpedanceCheckProtocol(ProtocolSessionConfiguration sessionConfiguration);



    // Check correctness of the template

    /*!
     * \brief isProtocolSessionConfigurationCorrect Calls to isProtocolSessionConfigurationCorrect
     * \param currentConfiguration
     * \return
     */
    bool isProtocolSessionConfigurationCorrectWrapper(ProtocolSessionConfiguration &currentConfiguration, QString *reason);

private:
    // ATTRIBUTES
    // ------------------------------

    /*!
     * A reference to the icognos engine
     */
    DeviceManager* _device;
    
    /*!
     * \brief isTACSorTRNS indicates whether current stimulation is tACs or tRNs
     */
    bool isTACSorTRNS;

    // Stimulation Configuration

    /*!
     * \brief _currentTemplate holds the current protocol template
     */
    ProtocolSessionConfiguration _currentConfiguration;

    /*!
     * \brief _sessionId Current session
     */
    StimSession _stimSession;


    // Stimulation Monitoring

    /*!
     * \brief _timerProtocol timer monitoring the execution of the stimulation
     */
    QTimer* _timerProtocol;

    /*!
     * \brief controlTimer user for controlling the execution of the stimulation
     */
    QElapsedTimer controlTimer;

    /*!
     * \brief _stimDurationEllapsed holds the current stimulation ellapsed
     */
    int _stimDurationEllapsed;

    /*!
     * \brief _stimDurationEllapsedFromResume number of ellapsed seconds when resume was pressed
     */
    int _stimDurationElapsedFromResume;


    /*!
     * \brief _impedanceTestValueBuffer holds the values of the impedance
     */
    QVector<int> _impedanceTestValueBuffer[NUM_STIM_CHANNELS];
    QVector<float> _isTestValueBuffer[NUM_STIM_CHANNELS];
    QVector<float> _vlTestValueBuffer[NUM_STIM_CHANNELS];

    /*!
     * \brief _isTestValueMovingAverage holds the value of the current averaged
     */
    float _isTestValueMovingAverage[NUM_STIM_CHANNELS];
    /*!
     * \brief _vlTestValueMovingAverage holds the value of the voltage averaged
     */
    float _vlTestValueMovingAverage[NUM_STIM_CHANNELS];
    /*!
     * \brief _impedanceTestValueMovingAverage holds the value of the impedance averaged
     */
    int _impedanceTestValueMovingAverage[NUM_STIM_CHANNELS];

    /*!
     * \brief _impedanceWrongValues number of wrong values for the impedance
     * When number of received wrong values is more than 2 for a given channel
     * we abort the stimulation
     */
    int _impedanceWrongValues[NUM_STIM_CHANNELS];

    /*!
     * \brief startStimSecondSham Holds whether the registers were loaded for second sham
     */
    bool loadStimSecondSham;

    /*!
     * \brief startStimSecondSham Holds the stimulation for second sham was started
     */
    bool startStimSecondSham;

    /*!
     * \brief isCheckingImpedance indicates whether we are checking impedance
     */
    bool _isCheckingImpedance;

    /*!
     * \brief _isStoppingStimulation indicates whether stimulation protocol was aborted
     */
    bool _isStoppingStimulation;

    // FUNCTIONS
    // ------------------------------

    /*!
     * This starts the stimulation
     */
    bool loadStimulationConfiguration(ProtocolSessionConfiguration currentConfiguration);

    /*!
     * \brief connectTimeOutSlot connection of the timeout slot
     */
    void connectTimeOutSlot(bool isSham);
    /*!
     * \brief disconnectTimeOutSlot disconnection of the timeout slot
     */
    void disconnectTimeOutSlot(bool isSham);

    /*!
     * \brief updateFakeImpedanceInfo updates the GUI with a fake impedance info
     */
    void updateFakeImpedanceInfo();

    /*!
     * \brief isStimulationReturnElectrode returns whether the current electrode is return or stimulation
     * \param electrode
     * \return
     */
    bool isStimulationReturnElectrode(StimulationElectrodeConfig electrode ) ;



    // Impedance Check



    /*!
     * \brief resetImpedance resets the impedance values
     */
    void resetImpedance();

signals:
    
    /*!
     * Report signal to the UI indicating:
     * \param code indicator of the stimulation stage or error if occured
     * \param index progress in seconds
     * \param stimDuration total duration of the stimulation
     */
   void stimulationStatus(StimulationState state, int stimulationProgress, int stimulationDuration);

   /*!
    * Reports for a Channel if it has passed its impedance test or not
    * \param index number of the channel [1-8]
    */
   void reportImpedanceTest(int index, int impedanceTestValue, float Is, float Vl, unsigned long long timeStamp);

   /*!
    * \brief updateSessionResult updates the protocolId with the number of seconds ellapsed
    * \param stimulationElapsedSeconds
    * \param stimulationElapsedSecondsFromResume number of elapsed seconds when resume was clicked
    * \return
    */
   void updateSessionResult(int protocolId, int stimulationElapsedSeconds, QString dateExecution);

   /*!
    * \brief impedanceCheckFailed notifies that the impedance check has failed during the last 2 seconds
    */
   void impedanceCheckFailed(int channel);

public slots:

   /*!
    * Signal that is emitted whenever a new Impedance data is received.
    *
    * \param data The new received sample data
    *
    * \param timeStamp The timeStamp of the data
    */
   void newImpedanceData(ChannelData data, unsigned long long timeStamp);


   /*!
    * \brief stopStimulationProtocolDelayed delay the stopStimulationProtocol
    */
   void stopStimulationProtocolDelayed();

    /*!
     * \brief stimulationStarted executed when stimulation has been started
     */
    void stimulationStarted();

    /*!
     * \brief stimulationFinished executed when stimulation is finished
     * \param abortedByNEC indicates whether execution was aborted by NEC
     */
    void stimulationFinished(bool abortedbyNEC);

private slots:

   /*!
    * \brief timerProtocolTimeOut slot to be executed when timer protocol has timed out
    * for the standard protocol
    */
   void timerProtocolTimeOutStandard();

    /*!
     * \brief timerProtocolTimeOut slot to be executed when timer protocol has timed out
     * for the sham protocol
     */
    void timerProtocolTimeOutSham();
    
};

#endif // PROTOCOLMANAGER_H
