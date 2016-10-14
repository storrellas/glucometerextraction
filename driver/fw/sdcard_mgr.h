/*===========================================================================*/
/* design unit  : SDCard Manager                                             */
/*                                                                           */
/* file name    : sdcard_mgr.h                                               */
/*                                                                           */
/* purpose      : Manage the SDCard Operation and configuration              */
/*                                                                           */
/* errors:      : none known                                                 */
/*                                                                           */
/* library      : Starlab                                                    */
/*                                                                           */
/* dependencies : none                                                       */
/*                                                                           */
/* author       : Sergi Torrellas                                            */
/*                Starlab                                                    */
/*                C/ Teodor Roviralta, 45                                    */
/*                08022 Barcelona                                            */
/*                Spain                                                      */
/*                Phone +34 93 254 03 66 / dir.70                            */
/*                Fax   +34 93 212 64 45                                     */
/*                sergi.torrellas@icognos.com                         */
/*                                                                           */
/* --------------------------------------------------------------------------*/
/* revision list                                                             */
/* version  author   date (yy/mm/dd)   changes                               */
/*                                                                           */
/* 1.0      AM       2013/06/26        creation                              */
/*===========================================================================*/


#ifndef SDCARD_MGR_H_
#define SDCARD_MGR_H_

#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN) && !defined(Q_OS_LINUX)
  #include <asf.h>
#endif

/* ========================================================================== */
/* TYPE DEFINITION                                                            */
/* -------------------------------------------------------------------------- */

typedef enum {
  NOT_INITIALISED = 0, NOT_READY = 1, NOT_PRESENT = 2,     
  READY = 3, STOPPING = 4,RECORDING = 5}
sdcard_status_t;

/* ========================================================================== */
/* DEFINITIONS                                                                */
/* -------------------------------------------------------------------------- */

/* Total number of configuration Registers */
#define SDCARD_NUM_REGS                 (133) // (128 + 4 + 1) (0x85)

/* MAXIMUM LENGTH OF FILENAME */
#define SDCARD_MAX_FILENAME_LENGTH      SDCARD_EXPERIMENT_TIME

/* ========================================================================== */
/* SDCARD INTERNAL REGISTERS                                                  */
/* -------------------------------------------------------------------------- */

// Offset for the addresses pointing to ACCEL REGS
#define SDCARD_REGS_OFFSET                (0x3000)

/** SDCARD file_name (128 characters) - 128 byte */
#define SDCARD_FILE_NAME               (0x00)

/** SDCARD_EXPERIMENT_TIME [s] (4,294,967,296 sec - 1193046.47 hours - 49710.27 days) - 4 byte */
#define SDCARD_EXPERIMENT_TIME      (0x80)

/* MODE CONFIGURATION :                                                     */
/* bit 0 : Enable Recording -  1 Recording ON / 0 Recording OFF byte        */
/* bit 1 : Experiment time enabled - 1 ExpTime Enabled / 0 ExpTime disabled */
#define SDCARD_RECORDING_MODE        (0x84)

/* ========================================================================== */
/* PROTOTYPES */ 
/* -------------------------------------------------------------------------- */
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN) && !defined(Q_OS_LINUX)

uint8_t  sdcard_init( void );
uint8_t  sdcard_is_ready( void );
uint8_t  sdcard_mount_drive( void );


// File management
uint8_t  sdcard_init_file( void );
uint8_t  sdcard_create_file ( void );
uint8_t  sdcard_create_file_given ( const char* file_name );
uint8_t  sdcard_append_to_file ( char* file_name );
uint8_t  sdcard_write_data( uint8_t *buffer, uint64_t length );
uint8_t  sdcard_write_and_flush_data( uint8_t *buffer, uint64_t length );
uint8_t  sdcard_close_file( void );
uint8_t  sdcard_check_available_space( uint8_t n_channels );

// Real-time SDCard writing/reading
void     sdcard_enable_blocking_operation(bool value);
uint8_t  sdcard_is_hsmci_available( void );
uint8_t  sdcard_check_hsmci_available( void );
uint8_t  sdcard_set_hsmci_available( uint8_t value );

// Get module data
uint8_t  sdcard_is_file_open( void );
sdcard_status_t sdcard_get_status( void );
uint8_t  sdcard_is_recording_enable( void );
uint8_t  sdcard_set_recording_mode ( uint8_t value );
uint8_t  sdcard_is_experiment_time_enable( void );
uint64_t sdcard_get_experiment_time( void );
uint8_t  sdcard_get_date_file( uint32_t *ul_year, uint32_t *ul_month, uint32_t *ul_day);
uint8_t  sdcard_get_time_file( uint32_t *ul_hour,  uint32_t *ul_minute, uint32_t *ul_second );

static inline void sdcard_init_stack( void ){
  sd_mmc_init();
}
uint8_t sdcard_read_data(char const* file_name, uint8_t* writtenToFile);


#ifdef SDCARD_RECORDING
void    sdcard_open_stuff();
uint8_t sdcard_read_stuff(uint8_t* buffer);
void    sdcard_close_stuff();
uint8_t sdcard_read_eeg_data(uint8_t* buffer);

uint8_t sdcard_is_hsmci_available_and_txdone( void );

#endif

#endif
/* ========================================================================== */


/* ========================================================================== */
/* OPERATION REGISTERS                                                        */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */



/* ========================================================================== */
/* DEFAULT VALUES                                                             */
/* -------------------------------------------------------------------------- */

#define SDCARD_SUCCESS 0
#define SDCARD_FAIL    1

// Maximum number of iteration while checking SDCard not present
#define MAX_IT_NOT_PRESENT 50
// Maximum number of iteration while checking SDCard not ready
#define MAX_IT_NOT_READY   50

// MODE RECORDING MASKS
#define SDCARD_ENABLE_RECORDING_Msk       (0x01u << 0)
#define SDCARD_ENABLE_EXPERIMENT_TIME_Msk (0x01u << 1)
#define SDCARD_DISABLE_RECORDING          (0x00)


/* ========================================================================== */



#endif /* SDCARD_MGR_H_ */
