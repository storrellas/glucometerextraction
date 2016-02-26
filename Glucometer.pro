#-------------------------------------------------
#
# Project created by QtCreator 2014-07-02T16:15:41
#
#-------------------------------------------------

QT       += core gui
QT       += network xml script

CONFIG += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NICBenchmark2
TEMPLATE = app


INCLUDEPATH += ./src
INCLUDEPATH += ./devicemanager/
INCLUDEPATH += ./application/

DEFINES     += STARSTIM_SIM  # Indicates Communicate DeviceManager to STARSTIM_SIM
DEFINES     += NICBENCHMARK  # This is used for the NICBenchmark when using FileWriter

# Output directories
OBJECTS_DIR    = obj
UI_HEADERS_DIR = obj
UI_SOURCES_DIR = obj
DESTDIR        = $${_PRO_FILE_PWD_}/output

# HEADERS
HEADERS  += src/mainwindow.h \
            src/commonparameters.h \
            src/devicestatus.h \
    devicemanager/sleeper.h


HEADERS += devicemanager/starstimprotocol.h \
           devicemanager/starstimdata.h \
           devicemanager/devicemanager.h \
           devicemanager/starstimcom.h \
           devicemanager/devicemanagertypes.h \
           devicemanager/fw/accel_mgr.h \
           devicemanager/fw/sdcard_mgr.h \
           devicemanager/fw/eeg_mgr.h \
           devicemanager/fw/stim_mgr.h \
           devicemanager/deviceconfiguration.h \
           devicemanager/wifidevice.h


HEADERS += application/protocoltemplates.h \
           application/stimprotocoltemplate.h \
           application/electrodes.h \
           application/protocoltypes.h \
           application/filewriter.h \
           application/trigger.h \
           application/protocolmanager.h \


# SOURCES
SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/devicestatus.cpp \


SOURCES += devicemanager/starstimprotocol.cpp \
           devicemanager/starstimdata.cpp \
           devicemanager/devicemanager.cpp \
           devicemanager/starstimcom.cpp \
           devicemanager/deviceconfiguration.cpp \
           devicemanager/wifidevice.cpp


SOURCES += application/stimprotocoltemplate.cpp  \
           application/electrodes.cpp \
           application/filewriter.cpp \
           application/trigger.cpp \
           application/protocolmanager.cpp \

# FORMS
FORMS    += src/mainwindow.ui
