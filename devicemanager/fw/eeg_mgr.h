/*===========================================================================*/
/* design unit  : StarStim:  EEG Manager                                     */
/*                                                                           */
/* file name    : eeg_mgr.h                                                  */
/*                                                                           */
/* purpose      : Constants for EEG Manager module                           */
/*                                                                           */
/* errors:      : none known                                                 */
/*                                                                           */
/* library      : Starlab                                                    */
/*                                                                           */
/* dependencies : none                                                       */
/*                                                                           */
/* author       : Antonio Mollfulleda                                        */
/*                Starlab                                                    */
/*                C/ Teodor Roviralta, 45                                    */
/*                08022 Barcelona                                            */
/*                Spain                                                      */
/*                Phone +34 93 254 03 66 / dir.70                            */
/*                Fax   +34 93 212 64 45                                     */
/*                antonio.mollfulleda@starlab.es                             */
/*                                                                           */
/* --------------------------------------------------------------------------*/
/* revision list                                                             */
/* version  author   date (yy/mm/dd)   changes                               */
/*                                                                           */
/* 1.0      AM       2011/10/06        creation                              */
/*===========================================================================*/

#ifndef _EEG_MGR_H_ 

#define _EEG_MGR_H_


typedef enum {EEG_NO_COMPRESSION = 0, EEG_16BIT_COMPRESSION = 1, EEG_12BIT_COMPRESSION = 2} eeg_compression_t;
typedef enum {EEG_500SPS = 1, EEG_250SPS = 2, EEG_125SPS = 4, EEG_75SPS = 8} eeg_streaming_rate_t;


/* ========================================================================== */
/* PROTOTYPES */ 
/* -------------------------------------------------------------------------- */
#if !defined(Q_WS_MAC) && !defined(Q_WS_WIN) && !defined(Q_OS_LINUX)
// EEG Read functions
void      eeg_write_config( void );
void      eeg_read_config( void );
 
uint8_t   eeg_read_data( uint8_t* eeg_data_local );
void      eeg_send_start( void );
void      eeg_send_stop( void );
uint8_t   eeg_init( void );
void	    eeg_configure_and_enable_spi (void);

// EEG Profile functions
void eeg_set_num_of_channel( uint8_t value );
uint8_t eeg_get_num_of_channels( void );
void eeg_calculate_num_of_channel( void );
uint8_t eeg_get_instrument_channels( void );
uint8_t eeg_is_channel_present( uint8_t ch );
uint8_t eeg_is_multisample( void );
uint8_t eeg_samples_per_beacon( void );
uint8_t eeg_get_ch_info( uint8_t *array );
uint8_t eeg_construct_mask( int numberOfOnes );
eeg_streaming_rate_t eeg_get_streaming_rate( void );

// Compression Functions
void eeg_decompress_sample(uint8_t* eeg_reference, uint8_t* eeg_transferred, uint8_t* eeg_sample_reconstructed, uint8_t n_channel, eeg_compression_t type);
void eeg_compress_sample(uint8_t* eeg_reference, uint8_t* eeg_sample, uint8_t* eeg_transferred, uint8_t n_channel);
void eeg_decompress_channel(uint8_t* eeg_reference, uint8_t* eeg_compressed, uint8_t* eeg_channel);
void eeg_compress_channel(uint8_t* eeg_reference, uint8_t* eeg_channel, uint8_t* eeg_compressed);
void eeg_decombine_channel(uint32_t value, uint8_t* array);
uint32_t eeg_combine_channel(uint8_t* array);
eeg_compression_t eeg_compression_type( void );
uint8_t eeg_is_compression_enabled( void );




/* DEBUG EEG: */
void eeg_check_data(void);
/* DEBUG EEG: */

#endif
/* ========================================================================== */

/* EEG SPI */
/* clock - 2MHz*/
#define EEG_SPI_CLOCK						(15000000U)
/* Configuration of SPI Master Delay BCT */
#define EEG_CONFIG_SPI_MASTER_DELAY_BCT		(3)

/* Total number of configuration Registers */
#define EEG_NUM_REGS                   (512)

/* Number of bytes per channel */
#define EEG_BYTES_PER_CHANNEL            (3)

/* Number of bytes of status */
#define EEG_STATUS_BYTES                 (3)

/* Maximum Number of Stimulation channels */
#ifdef EEG_32_MODE
  #warning "BEACON_MGR Compiled in EEG_32_MODE"
  #define EEG_NUM_CHANNEL                (32)

  // Maximum length of the SPI Message 
  #define EEG_SPI_BUFF_LEN               (154)

  // Number of bytes to read thorugh the SPI port 
  // EEG8 (24 data bytes + 3 status bytes)
  // (3 + 8*3) + (1/8) + (3 + 8*3) + (1/8) + (3 + 4*3) + (1/8) + (3 + 8*3) + (1/8) + (3 + 4*3) = 111.5 ===> ceil(111.5) = 112
  // NOTE: EEG8 chips are connected through daisy chain model.
  // Between every readings from chip to chip a single bit is introduced
  #define  EEG_SPI_NUM_READ              (112)

#endif

#ifdef EEG_20_MODE
  #warning "BEACON_MGR Compiled in EEG_20_MODE"
  #define EEG_NUM_CHANNEL                (20)

  // Maximum length of the SPI Message 
  #define EEG_SPI_BUFF_LEN               (96)

  // Number of bytes to read thorugh the SPI port 
  // (3 + 8*3 ) + (1/8) + (3 + 8*3) + (1/8) + (3 + 4*3) = 69.25 ===> ceil (69.25) = 70
  // NOTE: EEG8 chips are connected through daisy chain model. 
  // Between every readings from chip to chip a single bit is introduced
  #define  EEG_SPI_NUM_READ              (70)
  
#endif

#ifdef EEG_8_MODE
  #warning "BEACON_MGR Compiled in EEG_8_MODE"
  #define EEG_NUM_CHANNEL                (8)

  // Maximum length of the SPI Message 
  #define EEG_SPI_BUFF_LEN               (32)

  // Number of bytes to read thorugh the SPI port 
  #define  EEG_SPI_NUM_READ              (3*EEG_NUM_CHANNEL + 3)
#endif

#ifdef STM_MODE
  #warning "BEACON_MGR Compiled in STM_MODE"
  #define EEG_NUM_CHANNEL                (8)

  // Maximum length of the SPI Message 
  #define EEG_SPI_BUFF_LEN               (32)

  // Number of bytes to read thorugh the SPI port 
  #define  EEG_SPI_NUM_READ              (3*EEG_NUM_CHANNEL + 3)  
#endif



/* ========================================================================== */
/* EEG INTERNAL REGISTERS                                                     */
/* -------------------------------------------------------------------------- */

// Offset for the addresses pointing to EEG REGS
#define EEG_REGS_OFFSET                (0x0000)

/** EEG_REGS_ID: Indicate the device characteristics */
#define EEG_REGS_ID_ADDR               (0x00)

/** EEG_REGS_CONFIG_1: EEG Configuration Register 1 
 *                    Configures sampling rate and resolution mode
 *
 */
#define EEG_REGS_CONFIG_1_ADDR         (0x01)

/** EEG_REGS_CONFIG_2: Configures the rest signal generation
 *
 *    Bit [7:6]: Must be set to 00
 *    Bit     5: WCT Chooping Scheme
 *    Bit     4: Test Source
 *    Bit     3: Must allways set to '0'
 *    Bit     2: TEST_AMP: Test Signal amplitude
 *    Bit [1:0]: Signal frequency
 *
 * */
#define EEG_REGS_CONFIG_2_ADDR         (0x02)

/**
 * EEG_REGS_CONFIG_3: Configures multi-reference and RLD operation
 *
 * */
#define EEG_REGS_CONFIG_3_ADDR         (0x03)

/**
 * EEG_REGS_LOFF: Configures the Lead-Off detection operation
 * */
#define EEG_REGS_LOFF_ADDR             (0x04)

/**
 * EEG_REGS_CH_1_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_1_SET_ADDR         (0x05)

/**
 * EEG_REGS_CH_2_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_2_SET_ADDR         (0x06)

/**
 * EEG_REGS_CH_3_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_3_SET_ADDR         (0x07)

/**
 * EEG_REGS_CH_4_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_4_SET_ADDR         (0x08)

/**
 * EEG_REGS_CH_5_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_5_SET_ADDR         (0x09)

/**
 * EEG_REGS_CH_6_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_6_SET_ADDR         (0x0A)

/**
 * EEG_REGS_CH_7_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_7_SET_ADDR         (0x0B)

/**
 * EEG_REGS_CH_8_SET: Configure the power mode, PGA gain and multiplexer settings channels. 
 * */
#define EEG_REGS_CH_8_SET_ADDR         (0x0C)

/**
 * EEG_REGS_RLD_SENS_P: Controls the selection of the positive signals from each channel
 *                      for right leg drive derivation.
 *
 * */
#define EEG_REGS_RLD_SENS_P_ADDR       (0x0D)

/**
 * EEG_REGS_RLD_SENS_N: Controls the selection of the necagiteve signals from each channel
 *                      for right leg drive derivation
 * */
#define EEG_REGS_RLD_SENS_N_ADDR       (0x0E)

/** EEG_REGS_LOFF_SENS_P: Selects the positive side from each channel for lead-off detection.
 *
 * */
#define EEG_REGS_LOFF_SENS_P_ADDR      (0x0F)

/** EEG_REGS_LOFF_SENS_N: Selects the negativeside from each channel for lead-off detection.
 *
 * */
#define EEG_REGS_LOFF_SENS_N_ADDR      (0x10)

/** 
 * EEG_REGS_LOFF_FLIP: Controls the direction of the current used for lead-off
 * */
#define EEG_REGS_LOFF_FLIP_ADDR        (0x11)

/**
 * EEG_REGS_LOFF_STAT_P: Stores the statis whether the positive electrode on each
 *                       channel is ON or OFF.
 *
 * */
#define EEG_REGS_LOFF_STAT_P_ADDR      (0x12)

/**
 * EEG_REGS_LOFF_STAT_N: Stores the statis whether the negative electrode on each
 *                       channel is ON or OFF.
 *
 * */
#define EEG_REGS_LOFF_STAT_N_ADDR      (0x13)

/**
 * EEG_REGS_GPIO: General purpose I/O Register that controls the action of the
 *                GPIO pins. 
 *
 * */
#define EEG_REGS_GPIO_ADDR             (0x14)

/**
 * EEG_REGS_PACE: provides the PACE controls that configure the channel signal
 *                used to feed the external PACE detect circuitry.
 * */
#define EEG_REGS_PACE_ADDR             (0x15)

/**
 * EEG_REGS_RESP: provides the controls for the respiration circuitry.
 *
 * */
#define EEG_REGS_RESP_ADDR             (0x16)

/**
 * EEG_REGS_CONFIG_4: Configures respiration parameters such as modulation
 *                    frequency
 * */
#define EEG_REGS_CONFIG_4_ADDR         (0x17)

/** 
 * EEG_REGS_WCT_1: Configures the device WCT circuit channel selection and
 *                 the augmented mode
 * */
#define EEG_REGS_WCT_1_ADDR            (0x18)

/** 
 * EEG_REGS_WCT_2: Configures the device WCT circuit channel selection.
 * */
#define EEG_REGS_WCT_2_ADDR            (0x19)


/** 
 * EEG_CH_INFO: Configures the EEG_CH_INFO that is required for 20-channel EEG.
 *              For compatibility the format is the same the CH_INFO managed on
 *              stim_mgr, that is, '0' means EEG active channel and '1' means
 *              that EEG channel is off. The LSB is ch_0 while the bit 19 is
 *              the channel 19.
 * */
#define EEG_CH_INFO_ADDR               (0x40)

/** 
 * EEG_N_INSTRUMENT_CHANNEL: EEG channels in the instrument
 */
#define EEG_N_INSTRUMENT_CHANNEL       (0x44)

#define EEG_DATA_FAIL_ADDR             (0x48)


/* Enables the multisample mode 
 *  '1' : multisample enabled
 *  '0' : multisample disabled
 */
#define EEG_MULTISAMPLE_MODE           (0x50)


/* Configures number of samples per beacon */ 
#define EEG_REG_SAMPLES_PER_BEACON     (0x51)

/* Configures whether the system compresses EEG samples 
 *  '2' : compression enabled @ 12 bit/EEGSample
 *  '1' : compression enabled @ 16 bit/EEGSample
 *  '0' : compression disabled
 */
#define EEG_COMPRESSION_TYPE        (0x52)


/* Configures the rate for the EEG Streaming
 *  '8' : EEG rate 75SPS
 *  '4' : EEG rate 125SPS
 *  '2' : EEG rate 250SPS
 *  '1' : EEG rate 500SPS
 */
#define EEG_STREAMING_RATE          (0x53)





/* ========================================================================== */


/* ========================================================================== */
/* OPERATION REGISTERS                                                        */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */



/* ========================================================================== */
/* DEFAULT VALUES                                                             */
/* -------------------------------------------------------------------------- */

/** Default values for EEG Internal Registers */
#define EEG_REGS_CONFIG_1_DEF          (0x86)
#define EEG_REGS_CONFIG_2_DEF          (0x40)
#define EEG_REGS_CONFIG_3_DEF          (0x40)
#define EEG_REGS_LOFF_DEF              (0x00)
#define EEG_REGS_CH_1_SET_DEF          (0x00)
#define EEG_REGS_CH_2_SET_DEF          (0x00)
#define EEG_REGS_CH_3_SET_DEF          (0x00)
#define EEG_REGS_CH_4_SET_DEF          (0x00)
#define EEG_REGS_CH_5_SET_DEF          (0x00)
#define EEG_REGS_CH_6_SET_DEF          (0x00)
#define EEG_REGS_CH_7_SET_DEF          (0x00)
#define EEG_REGS_CH_8_SET_DEF          (0x00)
#define EEG_REGS_RLD_SENS_P_DEF        (0x00)
#define EEG_REGS_RLD_SENS_N_DEF        (0x00)
#define EEG_REGS_LOFF_SENS_P_DEF       (0x00)
#define EEG_REGS_LOFF_SENS_N_DEF       (0x00)
#define EEG_REGS_LOFF_FLIP_DEF         (0x00)
#define EEG_REGS_LOFF_STAT_P_DEF       (0x00)
#define EEG_REGS_LOFF_STAT_N_DEF       (0x00)
#define EEG_REGS_GPIO_DEF              (0x0F)
#define EEG_REGS_PACE_DEF              (0x00)
#define EEG_REGS_RESP_DEF              (0x00)
#define EEG_REGS_CONFIG_4_DEF          (0x00)
#define EEG_REGS_WCT_1_DEF             (0x00)
#define EEG_REGS_WCT_2_DEF             (0x00)
#define EEG_CH_INFO_DEF                (0x00)
#define EEG_N_INSTRUMENT_CHANNEL_DEF   (0x00)
#define EEG_MULTISAMPLE_MODE_DEF       (0x00)
#define EEG_REG_SAMPLES_PER_BEACON_DEF (0x01)
#define EEG_COMPRESSION_TYPE_DEF       (0x00)
#define EEG_STREAMING_RATE_DEF         (0x01)




/* ========================================================================== */


#endif 

