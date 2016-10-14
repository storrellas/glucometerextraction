#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt Includes
#include <QMainWindow>
#include <QTcpSocket>

// Project includes
#include "commonparameters.h"
#include "devicemanager.h"
#include "devicestatus.h"
#include "filewriter.h"
#include "devicemanagertypes.h"
#include "deviceconfiguration.h"

#define APP_NAME "NICBenchmark2"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    // ATTRIBUTES
    // ----------------

    /*!
     * \brief nicMessageHandlerVisual Handlers used for logging
     * both handlers write a log file whereas only nicMessageHandlerVisual prints
     * out the log message to the GUI
     */
    QtMessageHandler nicMessageHandlerVisual;
    QtMessageHandler nicMessageHandler;


    // FUNCTIONS
    // ----------------

    /*!
     * \brief MainWindow Public constructor/Destructor
     * \param parent
     */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    /*!
     * \brief closeEvent detects the close event of the window
     * \param event
     */
    void closeEvent (QCloseEvent *event);

    /*!
     * Writes a line to the visual console
     */
    void consoleWrite(const QString &line);

    /*!
     * This function is called after the constructor so that the logs are displayed
     * \return the
     */
    void initialize();




private:
    // ATTRIBUTES
    // ----------------

    QMutex _mutex;

    /*!
     * \brief ui holds the graphical representation made by the UI designer
     */
    Ui::MainWindow *ui;

    /*!
     * \brief _monitorThread thread executing all the instruction for monitoring icognos
     */
    QThread* monitorThread;

    /*!
     * \brief statusThread thread executing all the reporting of the status of the device
     */
    QThread* statusThread;

    /*!
     * A reference to the Starstim API to connect to the device
     */
    DeviceManager* deviceManager;

    /*!
     * \brief _manager executes the function onDeviceStatus
     */
    DeviceStatus* deviceStatus;

    /*!
     * A referece to the stimulation protocol manager
     */
    ProtocolManager* protocolManager;

    /*!
     * File writer
     */
    FileWriter* fileWriter;

    /*!
     * Searches the device with an UDP protocol
     */
    DeviceConfiguration deviceConfiguration;

    // FUNCTIONS
    // ----------------

    /*!
     * Opens the device and sends start operation
     */
    void openDevice();

    /*!
     * Configures Stimulation registers in NECBox
     */
    void launchStimulation();

    /*!
     * Enables SDCard for recording
     */
    void enableSDCard();

    /*!
     * Sends the start streaming command to device
     */
    void startStreaming();

    /*!
     * Sends the start streaming command to device
     */
    void stopStreaming();

    /*!
     * Closes the device and sends stop operation
     */
    void closeDevice();



signals:
    /*!
     * \brief consoleWriteSignal Used for writing into the GUI console. There is a threading issue here
     * \param line
     */
    void consoleWriteSignal(QString line);

    /*!
     * \brief quit terminates the current application
     */
    void quit();


private slots:

    /*!
     * \brief consoleWriteSlot slot used to write into console
     * This enables to be in the thread where the GUI was created and launch QApplication::processEvents()
     * \param msg
     */
    void consoleWriteSlot(const QString& msg);

    // Slots for buttons

    /*!
     * \brief on_clearConsoleBtn_clicked clears the console
     */
    void on_clearConsoleBtn_clicked();

    /*!
     * \brief on_executeOperation_btn_clicked
     */
    void on_executeOperation_btn_clicked();

    // Slots for signals from Device Manager

    /*!
     * Signal that is emitted reporting the remaining percentage of the
     * battery, firmware version and t1, t2 (ms from the moment where device is ON)
     *
     * \param deviceType type of device connected
     * \param n_channel number of channels of the device
     * \param level Remaining battery in percentage from 0 to 100%
     * \param firmwareVersion firmware version of the device
     * \param t1
     * \param t2
     */
    void receivedProfile(DeviceManagerTypes::DeviceType deviceType, int n_channel,
                         int batteryLevel, int firmwareVersion, int t1, int t2);
    /*!
     * Slot that is raised whenever a new EEG data is received.
     *
     * \param data The new received sample data
     */
    void receivedEEGData(ChannelData data);

    /*!
     * Slot that is raised whenever a new ACCEL data is received.
     *
     * \param data The new received sample data
     */
    void receivedAccelData(ChannelData data);


    /*!
     * Signal that is emitted reporting the firmware version of the
     * device.
     *
     * \param version Firmware version
     */
    void receivedFirmwareVersion(int firmwareVersion);

    /*!
     * Signal that is emitted whenever a new stimulation data is received.
     *
     * \param data The new received sample data
     */
    void receivedStimulationData(ChannelData data);

    /*!
     * Signal that is emitted whenever a new Impedance data is received.
     *
     * \param data The new received sample data
     *
     * \param timeStamp The timeStamp of the data
     */
    void receivedImpedanceData(ChannelData data);

    // Slots for signals from protocol Manager

    /*!
     * Report signal to the UI indicating:
     * \param code indicator of the stimulation stage or error if occured
     * \param index progress in seconds
     * \param stimDuration total duration of the stimulation
     */
   void stimulationStatus(StimulationState state, int stimulationProgress, int stimulationDuration);

    // Slots for device configuration operations

    /*!
     * \brief on_searchStarstimBtn_clicked searches icognos by means of the UDP protocol
     */
    void on_searchStarstimBtn_clicked();

    /*!
     * \brief on_configureInfrasBtn_clicked configures the network infrastructure (SSID, PASSWORD) for the
     * WiFi instruments
     */
    void on_configureInfrasBtn_clicked();

    /*!
     * \brief on_staticBtn_clicked configures the parameters to get the radio working in static mode
     */
    void on_staticBtn_clicked();

    /*!
     * \brief on_setModeBtn_clicked sets the mode of operation of the radio according to
     * selected in modeComboBox
     */
    void on_setModeBtn_clicked();

    /*!
     * \brief on_getMACBtn_clicked recovers the Serial Number of the device (MAC Address)
     */
    void on_getMACBtn_clicked();

    /*!
     * \brief on_flashWF121Button_clicked uploads the fw to the WF121
     */
    void on_flashWF121Button_clicked();
    void on_setMACBtn_clicked();
};

#endif // MAINWINDOW_H
