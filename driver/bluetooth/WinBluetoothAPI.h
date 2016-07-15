/*!
 * \file WinBluetoothAPI.h
 *
 * \author Javier Acedo
 * \date 23/01/2012
 * \version 1.0
 *
 * \brief API that allows accesing the Bluetooth stack in windows.
 *
 * Only Windows API is supported. Widcomm Bluetooth stack is not supported
 * yet.
 *
 */
#ifndef __WINBLUETOOTHAPI__
#define __WINBLUETOOTHAPI__
#ifdef WINBLUETOOTHAPI_EXPORTS
#define WINBLUETOOTH_API __declspec(dllexport)
#else
#define WINBLUETOOTH_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLUETOOTH_MAX_NAME_SIZE
#define BLUETOOTH_MAX_NAME_SIZE             (248)
#endif

/*!
 * The application calls this function for discovering new devices. The
 * information is retrieved by calling getNumNotPairedDevices and 
 * getInfoNotPairedDevice.
 *
 * \return A positive value is returned when the scans successfully finishes.
 * It returns 0 if there where some error during the discovery procedure.
 */
WINBLUETOOTH_API int scanNeighborhood ();

/*!
 * It gets the already paired devices in the system.
 *
 * \return Number of already paired devices in the system.
 */
WINBLUETOOTH_API int getNumPairedDevices ();

/*!
 * It gets the devices that are not paired in the system yet. The information
 * is updated by calling scanNeighborhood.
 *
 * \return Number of devices in the neigborhood but not paired yet.
 */
WINBLUETOOTH_API int getNumNotPairedDevices ();

/*!
 * It gets all the devices that are in the neigborhood. The information
 * is updated by calling scanNeighborhood.
 *
 * \return Number of devices in the neigborhood.
 */
WINBLUETOOTH_API int getNumAllDevices (int authenticated, int remembered, int unknown, int connected);

/*!
 * This function provides the name and the mac address of a Bluetooth device
 * already paired in the system.
 *
 * \param index 0-based index of the device which is desired to retrieve the
 * information
 *
 * \param deviceName output parameter where the device name is copied.
 *
 * \param macAddress output parameter where the MAC address is copied.
 *
 * \return It return a positive value if the information has been copied to
 * the output parameters. A zero value is returned if the provided index does
 * not correspond to a valid paired Bluetooth device. A negative value is
 * returned if there is some error while retrieving the information.
 */
WINBLUETOOTH_API int getInfoPairedDevice (unsigned int index,
                                                 wchar_t * deviceName,
                                                 unsigned char * macAddress);

/*!
 * This function provides the name and the mac address of a Bluetooth device.
 *
 * \param index 0-based index of the device which is desired to retrieve the
 * information
 *
 * \param deviceName output parameter where the device name is copied.
 *
 * \param macAddress output parameter where the MAC address is copied.
 *
 * \param authenticated input parameter indicating whether to look for authenticated devices.
 *
 * \param remembered input parameter indicating whether to look for remembered devices.
 *
 * \param unknown input parameter indicating whether to look for unknown devices.
 *
 * \param connected input parameter indicating whether to look for connected devices.
 *
 * \return It return a positive value if the information has been copied to
 * the output parameters. A zero value is returned if the provided index does
 * not correspond to a valid paired Bluetooth device. A negative value is
 * returned if there is some error while retrieving the information.
 */

WINBLUETOOTH_API int getInfoAllDevices (unsigned int index, wchar_t * deviceName, unsigned char * macAddress,
										int authenticated, int remembered, int unknown, int connected);

/*!
 * This function provides the name and the mac address of a Bluetooth device
 * that is persent in the eighborhood but is not paired yet. The information
 * is updated by calling scanNeighborhood.
 *
 * \param index 0-based index of the device which is desired to retrieve the
 * information
 *
 * \param deviceName output parameter where the device name is copied.
 *
 * \param macAddress output parameter where the MAC address is copied.
 *
 * \return It return a positive value if the information has been copied to
 * the output parameters. A zero value is returned if the provided index does
 * not correspond to a valid paired Bluetooth device. A negative value is
 * returned if there is some error while retrieving the information.
 */
WINBLUETOOTH_API int getInfoNotPairedDevice (unsigned int index,
                                                    wchar_t * deviceName,
                                                    unsigned char * macAddress);

/*!
 * It performs the authentication procuder for pairing the system with the
 * device whose mac address is provided.
 *
 * \param macAddress MAC address of the remote device.
 *
 * \return It returns a positive value if the pairing is successfully done.
 * It returns zero when the provided mac does not match with any of the
 * devices detected after calling the scanNeighborhood function. A negative
 * value is returned if there were any problem during the authentication
 * procedure.
 */
WINBLUETOOTH_API int pairDevice(const unsigned char * macAddress, const char * pin);

/*!
 * It remove the authentication link between the computer an the device whose
 * mac address is provided.
 *
 * \param macAddress MAC address of the remote device to be unpaired.
 *
 * \return It return a positive value if the unpairing is successful. It
 * returns a negative value when the device could not be removed from the
 * system.
 */
WINBLUETOOTH_API int removeDevice (const unsigned char * macAddress);

/*!
 * It gets the serial port number for an already paired device.
 *
 * \param macAddress MAC address of the remote device.
 *
 * \return number of the serial port associated to the device whose MAC
 * address is provided.
 */
WINBLUETOOTH_API unsigned char getSerialPort(const unsigned char * macAddress);


WINBLUETOOTH_API int iniBTSockets ();

WINBLUETOOTH_API void closeBTSockets ();

WINBLUETOOTH_API int openRFCOMM (const unsigned char * macAddress, unsigned int * handle);

WINBLUETOOTH_API int closeRFCOMM (unsigned int handle);

WINBLUETOOTH_API int readRFCOMM (unsigned int handle, char * buffer, unsigned long numberBytes, unsigned int timeout);

WINBLUETOOTH_API int writeRFCOMM (unsigned int handle, char * buffer, unsigned long numBytes);

WINBLUETOOTH_API void writeLogDebugBluetooth (const char * string);

#ifdef __cplusplus
}
#endif
#endif // __WINBLUETOOTHAPI__
