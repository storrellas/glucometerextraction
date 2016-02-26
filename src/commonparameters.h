#ifndef COMMONPARAMETERS_H
#define COMMONPARAMETERS_H

#include <QString>
#include <QVector>
#include <QHash>
#include <QDebug>

#ifdef Q_WS_MAC
#include <unistd.h>
#endif

// This is used for debug
#define loggerMacroDebug(line) qDebug() << __FILE__ << " | " << __LINE__ << " | " << __FUNCTION__ << " | " << line;

// NIC
#define N_MAX_CHANNELS              32    // Expected maximum number of channels for NIC

// FFT parameters
#define N_WINDOW_SHIFT              50     // Samples before computing FFT [FS/N_FFT_SEC]
#define FREQ_SAMP                       500.0     // [Hz] ENOBIO sampling frequency [Hz].
#define N_FFT_SEC                   10     // 10 FFT/sec - Bins per second in the spectrogram

// Spectrogram
#define SPECTROGRAM_FREQ_RANGE    50.0     // [Hz] Spectrogram goes from 0-50Hz
#define INIT_WINDOW_LENGTH           1     // [s] WindowLength to perform the initial resolution
#define INIT_DISPLAYED_TIME          5     // [s] Displayed time for the spectrogram

// DEFINE
#define DEVICE_TYPE_ENOBIO           0     // Device type TODO: Trasnform into enum
#define DEVICE_TYPE_STARSTIM         1

// EOG Correction
#define EOG_CORRECTION_TRAINING_TIME 20   // [s] Training time for EOG Correction

#define PI 3.141592653589793

#define NUM_STIM_CHANNELS (32)

#define FWVERSION_SDCARD                       1200
#define FWVERSION_SDCARD_STARSTIM              1232
#define FWVERSION_MULTISAMPLES                 1201
#define FWVERSION_INSTRUMENT_CHANNEL           1206
#define FWVERSION_FIR_TRNS                     1210
#define FWVERSION_COMPRESSION_REV              1213
#define FWVERSION_ONLINE_STIMULATION_REV       1218
#define FWVERSION_SYNCHRONIZATION              1242
#define FWVERSION_TRNS_PAIRS                   1244
#define FWVERSION_SYNCHRO_TIMESTAMP            1245

#define FWVERSION_ATSAM4S                      2000

#define LATEST_SUPPPORTED_FW_VERSION           1251

// MOVEMENT DETECTION
#define UP_THRES_DEFAULT_VALUE 400
#define LOW_THRES_DEFAULT_VALUE 400
#define MAXIMUM_THRES_VALUE 2500

//HIGH AMPLITUDE ARTIFACTS THRESHOLD
#define HIGH_AMP_ARTIFACTS_THRES 100

/*!
 * A struct storing the configuration parameters for each electrode
 */
struct stimulationConfig {

    int StimType;
    int Atdcs;
    int Atacs;
    double Ftacs;
    int Ptacs;
    int Atrns;
    int posIndex;
};

//these defines may perhaps be moved to nftypes.h
#define NF_STATUS_EOG_START             1
#define NF_STATUS_EOG_FINISH            2
#define NF_STATUS_BASELINE_START        3
#define NF_STATUS_BASELINE_FINISH       4
#define NF_STATUS_TRIAL_START           5
#define NF_STATUS_REST_START            6
#define NF_STATUS_PAUSE                 7
#define NF_STATUS_ABORT                 8


#define GAME_STATUS_NOT_LAUNCHED        0
#define GAME_STATUS_IDLE                1
#define GAME_STATUS_BASELINE            2
#define GAME_STATUS_EOG                 3
#define GAME_STATUS_NFPLAYING           4
#define GAME_STATUS_NFREST              5

#endif // COMMONPARAMETERS_H
