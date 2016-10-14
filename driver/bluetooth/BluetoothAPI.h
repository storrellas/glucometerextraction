#ifndef BLUETOOTHAPI_H
#define BLUETOOTHAPI_H

#ifdef Q_OS_WIN
 #include "bluetooth/WinBluetoothAPI.h"
 #else
 #ifdef Q_OS_MACX
  #include "bluetooth/icognosOsxBT_Prefix.pch"
 #else
  #include "bluetooth/BluezBluetoothAPI.h"
 #endif
#endif

#endif // BLUETOOTHAPI_H



