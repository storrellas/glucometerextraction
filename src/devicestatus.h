#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

// Qt includes
#include <QObject>


// Project includes
#include "protocolmanager.h"
#include "devicemanager.h"
#include "devicemanagertypes.h"

class DeviceStatus : public QObject
{
    Q_OBJECT
public:

    // Attributtes

    /*!
     * \brief _protocolManager reference to protocol manager
     */
    ProtocolManager* _protocolManager;

    /*!
     * \brief _device reference of device manager
     */
    DeviceManager* _device;


    // Functions

    /*!
     * \brief DeviceStatus public constructor
     * \param device
     * \param protocolManager
     */
    DeviceStatus(DeviceManager* device, ProtocolManager* protocolManager);
    ~DeviceStatus();

private:

signals:
    /*!
     * \brief stimulationStarted indicates that stimulation has started
     */
    void stimulationStarted();

    /*!
     * \brief stimulationFinished indicates that stimulation has been finished
     * \param abortedByNEC whether stimulation was aborted by NEC
     */
    void stimulationFinished(bool abortedByNEC);

public slots:

    // Functions

    /*!
     * \brief onDeviceStatus this slot receives a new status from the device
     * \param status
     * \param macAddress
     */
    void onDeviceStatus (int status);

    void receivedDeviceStatus(DeviceManagerTypes::DeviceStatus status);
};

#endif // DEVICESTATUS_H
