/*===========================================================================*/
/* design unit  : StarStim: STM-REGS                                         */
/*                                                                           */
/* file name    : stm_regs.h                                                 */
/*                                                                           */
/* purpose      : Declare the list of Simulation Registers in StarStim       */
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
/* 1.0      AM       2011/09/19        creation                              */
/*===========================================================================*/

#ifndef _STM_REGS_H_
#define _STM_REGS_H_

/* ========================================================================== */
/* PROTOTYPES */
/* -------------------------------------------------------------------------- */


// stm_regs management
void stm_write_registers( uint8_t* buffer_regs, uint16_t start_addr, uint8_t num_regs );
void stm_read_registers ( uint8_t* buffer_regs, uint16_t start_addr, uint8_t num_regs );

// Get stimulation data
uint8_t stm_get_stm_data  ( uint8_t* buffer_regs );
uint8_t stm_get_imp_data  ( uint8_t* buffer_regs );

// Stimulation profiling
uint8_t stm_get_ch_info( uint8_t *array );
uint32_t stm_get_ch_info_32bit( void );
uint32_t stm_get_ch_free_32bit( void );
uint8_t stm_get_freq_dec( void );
uint8_t stm_get_report_data( void );
void stm_set_error( uint8_t error );
void stm_set_stop_flag(uint8_t value);
uint8_t stm_get_region();

// Stimulation functions
void GetNextSTMData(void); // __attribute__((optimize("-O3")));
void UpdateDAC( void );
void UpdRqstSTMConfig(uint8_t *src_vec, uint8_t src_nbtyes, uint8_t addr_start, uint8_t *stm_regs_lc);
void stim_init(void);
void stm_init_seed( void );
uint8_t ReadIMPData(uint8_t ch_ind);
void StimWaveInit( void );
void stim_phase_init( void );


void stim_seed_init( void );
void stm_configure_and_enable_spi ( void );
uint8_t stm_get_num_of_channels( void );

// Waveform generators
int32_t stm_dc_waveform_generate(uint8_t ch_ind);
int32_t stm_sin_waveform_generate(uint8_t ch_ind);
int32_t stm_gauss_waveform_generate(uint8_t ch_ind);
int32_t stm_gauss_waveform_filter(int32_t g_num);

// Online change update functions
void stm_update_ol_step_dc_num( void );
void stm_update_ol_step_sin_num( void );

/* ========================================================================== */

/* DAC SPI */
/* clock - 48MHz*/
#define DAC_SPI_CLOCK						(48000000U)
/* Configuration of SPI Master Delay BCT */
#define DAC_CONFIG_SPI_MASTER_DELAY_BCT		(3)

/* Total number of configuration Registers */
#define STM_NUM_REGS                   (2048)

/* Maximum Number of Stimulation channels */
//#define STM_NUM_CHANNEL                (8)
#define STM_NUM_CHANNEL                (32)

/* Value corresponding to ZERO */
#define STM_ZERO                       ( 32768)

/* Maximum Valute on Stimulation */
#define STM_MAX                        ( 32767)

/* Minimum Valute on Stimulation */
#define STM_MIN                        (-32767)

/* Fixed point 2*PI representation */
#define STM_PI2_FX   (65536)

/* Fixed point PI representation */
#define STM_PI_FX    (32768)

/* Fixed point PI/2 representation */
#define STM_PI_2_FX  (16384)

/* STM REGION Definitions */
#define STM_REGION_UP                  (0)
#define STM_REGION_WAVE                (1)
#define STM_REGION_DN                  (2)
#define STM_REGION_END                 (3)
#define STM_REGION_STOP                (4)

/** STM_MODE Definitions */
#define STM_INTERNAL                   (0)
#define STM_USER_DEFINED               (1)

/** STM_FIFO_LEN: Length of the FIFO for user-defined Stimulation */
#define STM_FIFO_LEN                   (45)


/** STM_FLT_LEN:  */
#define STM_FLT_LEN                    (50)

/** Stim data for channel is reported */
#define STM_CH_INFO_REPORTED            (1)

/** Stim data for channel is NOT reported */
#define STM_CH_INFO_NOT_REPORTED        (0x00)

/** Stim data for channel all channels reported */
#define STM_CH_INFO_ALL_REPORTED        (0xFF)


/* ========================================================================== */
/* STM Internal Registers                                                     */
/* -------------------------------------------------------------------------- */

// Offset for the addresses pointing to EEG REGS
#define STM_REGS_OFFSET                (0x1000)


/* DAC REGISTERS                                                              */
/* -------------------------------------------------------------------------- */

#define DAC_REGISTER_OFFSET            (0)

/**
 * DAC Input shoft register BYTE-0
 */
#define STM_REGS_DAC_B0_ADDR           (0 + DAC_REGISTER_OFFSET)

/**
 * DAC Input shoft register BYTE-1
 */
#define STM_REGS_DAC_B1_ADDR           (1 + DAC_REGISTER_OFFSET)

/**
 * DAC Input shoft register BYTE-2
 */
#define STM_REGS_DAC_B2_ADDR           (2 + DAC_REGISTER_OFFSET)

/**
 * DAC Input shoft register BYTE-3
 */
#define STM_REGS_DAC_B3_ADDR           (3 + DAC_REGISTER_OFFSET)
/* ========================================================================== */


/* OPERATION REGISTERS                                                        */
/* -------------------------------------------------------------------------- */

#define OPERATION_REGISTER_OFFSET      (4)

/**
 * STM_REGS_CH_INFO (4 bytes): Indicates the CHANNELS that must be reported
 *                             though the communication link.
 *
 *   Bit-0: '0' channel 0 is NOT reported. '1' Channel 0 is reported
 *   Bit-1: '0' channel 1 is NOT reported. '1' Channel 1 is reported
 *   Bit-2: '0' channel 2 is NOT reported. '1' Channel 2 is reported
 *   Bit-3: '0' channel 3 is NOT reported. '1' Channel 3 is reported
 *   Bit-4: '0' channel 4 is NOT reported. '1' Channel 4 is reported
 *   Bit-5: '0' channel 5 is NOT reported. '1' Channel 5 is reported
 *   Bit-6: '0' channel 6 is NOT reported. '1' Channel 6 is reported
 *   Bit-7: '0' channel 7 is NOT reported. '1' Channel 7 is reported
 * */
#define STM_REGS_CH_INFO_ADDR          (0 + OPERATION_REGISTER_OFFSET)


/** STM_REGS_MODE: Mode of Stimulation
 *  0: Stimulation of pre-defined waveforms
 *  1: User Defined Stimulation
 *
 * */

#define STM_REGS_MODE_ADDR             (4 + OPERATION_REGISTER_OFFSET)

/** STM_REGS_CFG_0: Tobe defined  */
#define STM_REGS_CFG_1_ADDR            (5 + OPERATION_REGISTER_OFFSET)

/**
 * STM_REGS_CH_FREE (4 byte): This byte indicate which channel is free to
 *                            Fix the current so that the summation of all
 *                            channel is equal to zero.
 *
 *   Only and only one bit of this byte can be different than '0'.
 *   Otherwhise the FREE channel is set to CH-0 by default
 *
 *   Bit-0: '1' CH-0 configured as free channel.
 *   Bit-1: '1' CH-1 configured as free channel.
 *   Bit-2: '1' CH-2 configured as free channel.
 *   Bit-3: '1' CH-3 configured as free channel.
 *   Bit-4: '1' CH-4 configured as free channel.
 *   Bit-5: '1' CH-5 configured as free channel.
 *   Bit-6: '1' CH-6 configured as free channel.
 *   Bit-7: '1' CH-7 configured as free channel.
 * */
#define STM_REGS_CH_FREE_ADDR          (6 + OPERATION_REGISTER_OFFSET)

/**
 * STM_REGS_RATE_DEC: Decimation factor applied to the sampling rate to
 *                    generate the stimation waveform.
 *
 *    By default the sampling rate is FRATE = 1000 SPS.
 *    In order to add flexibility to the waveform generation the sampling rate
 *    can be reduced. The resulting sampling rate is
 *
 *    1000 / STM_REGS_RATE_DEC SPS
 *
 *    RATE_DEC (bits 7 - 0): division factor applied to the sampling rate
 * */
#define STM_REGS_RATE_DEC_ADDR         (10 + OPERATION_REGISTER_OFFSET)


/**
 * STM_REGS_RAMP_UP (2 bytes): Duration in secs. of the ramp-up section
 *                             in the stimulation waveform
 *
 *    STM_REGS_RAMP_UP_0: Least significant byte
 *    STM_REGS_RAMP_UP_1: Most significant byte
 *
 * */
#define STM_REGS_RAMP_UP_0_ADDR        (11 + OPERATION_REGISTER_OFFSET)
#define STM_REGS_RAMP_UP_1_ADDR        (12 + OPERATION_REGISTER_OFFSET)

/**
 * STM_REGS_RAMP_DN: Duration in secs. of the ramp-down section
 *                   in the stimulation waveform
 *
 *    STM_REGS_RAMP_DN_0: Least significant byte
 *    STM_REGS_RAMP_DN_1: Most significant byte
 *
 * */
#define STM_REGS_RAMP_DN_0_ADDR        (13 + OPERATION_REGISTER_OFFSET)
#define STM_REGS_RAMP_DN_1_ADDR        (14 + OPERATION_REGISTER_OFFSET)

/**
 * STM_REGS_TIME: Duration in secs. of the stimulation waveform
 *                NOT including the ramp-up and ramp-down regions.
 *
 *    STM_REGS_TIME_0: Least significant byte
 *    STM_REGS_TIME_1: Most significant byte
 *
 * */
#define STM_REGS_TIME_0_ADDR           (15 + OPERATION_REGISTER_OFFSET)
#define STM_REGS_TIME_1_ADDR           (16 + OPERATION_REGISTER_OFFSET)
/* ========================================================================== */



/* GAUSSIAN WAVEFORM GENERATOR                                                */
/* -------------------------------------------------------------------------- */

#define GAUSSIAN_WAVEFORM_OFFSET       (50)


/**
 * STM_REGS_CHX_SEED: Define the initial seed of the random number generator
 *                    used to generate random waveform
 *
 *    STM_REGS_CHX_SEED_0: Least significant byte
 *    STM_REGS_CHX_SEED_1: Most significant byte
 */
#define STM_REGS_SEED_0_ADDR       (0 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_SEED_1_ADDR       (1 + GAUSSIAN_WAVEFORM_OFFSET)

/**
 * STM_REGS_CHX_SEED: Define the initial seed of the random number generator
 *                    used to generate random waveform in channel 0
 *
 *    STM_REGS_CHX_SEED_0: Least significant byte
 *    STM_REGS_CHX_SEED_1: Most significant byte
 */
#define STM_REGS_CH0_SEED_0_ADDR       (0 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH0_SEED_1_ADDR       (1 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH1_SEED_0_ADDR       (2 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH1_SEED_1_ADDR       (3 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH2_SEED_0_ADDR       (4 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH2_SEED_1_ADDR       (5 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH3_SEED_0_ADDR       (6 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH3_SEED_1_ADDR       (7 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH4_SEED_0_ADDR       (8 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH4_SEED_1_ADDR       (9 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH5_SEED_0_ADDR       (10 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH5_SEED_1_ADDR       (11 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH6_SEED_0_ADDR       (12 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH6_SEED_1_ADDR       (13 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH7_SEED_0_ADDR       (14 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH7_SEED_1_ADDR       (15 + GAUSSIAN_WAVEFORM_OFFSET)


/**
 * STM_REGS_CHX_GAUSS (16-bit): Configures the Gaussian waveofrm generator for channel 0
 *
 *   By default the output of the gaussian generator is mapped to the range from -2mA to 2mA.
 *   The standard deviation of the default waveform is STD = 640uA, which is 6.25 times the
 *   allowable dynamic range. The output waveform can be scaled to generate a random signal
 *   with lower STD by applying a gain factor of the form K = GAIN_N/GAIN_D where K <= 1.
 *
 *   GAIN_N (bits 7 - 0): Numerator of the gain factor K
 *   GAIN_D (bits 15- 8): Denominator of the gain factor K
 * */
#define STM_REGS_CH0_GAUSS_GAIN_N_ADDR (16 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH0_GAUSS_GAIN_D_ADDR (17 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH1_GAUSS_GAIN_N_ADDR (18 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH1_GAUSS_GAIN_D_ADDR (19 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH2_GAUSS_GAIN_N_ADDR (20 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH2_GAUSS_GAIN_D_ADDR (21 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH3_GAUSS_GAIN_N_ADDR (22 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH3_GAUSS_GAIN_D_ADDR (23 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH4_GAUSS_GAIN_N_ADDR (24 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH4_GAUSS_GAIN_D_ADDR (25 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH5_GAUSS_GAIN_N_ADDR (26 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH5_GAUSS_GAIN_D_ADDR (27 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH6_GAUSS_GAIN_N_ADDR (28 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH6_GAUSS_GAIN_D_ADDR (29 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH7_GAUSS_GAIN_N_ADDR (30 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH7_GAUSS_GAIN_D_ADDR (31 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH8_GAUSS_GAIN_N_ADDR (32 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH8_GAUSS_GAIN_D_ADDR (33 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH9_GAUSS_GAIN_N_ADDR (34 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH9_GAUSS_GAIN_D_ADDR (35 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH10_GAUSS_GAIN_N_ADDR (36 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH10_GAUSS_GAIN_D_ADDR (37 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH11_GAUSS_GAIN_N_ADDR (38 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH11_GAUSS_GAIN_D_ADDR (39 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH12_GAUSS_GAIN_N_ADDR (40 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH12_GAUSS_GAIN_D_ADDR (41 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH13_GAUSS_GAIN_N_ADDR (42 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH13_GAUSS_GAIN_D_ADDR (43 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH14_GAUSS_GAIN_N_ADDR (44 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH14_GAUSS_GAIN_D_ADDR (45 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH15_GAUSS_GAIN_N_ADDR (46 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH15_GAUSS_GAIN_D_ADDR (47 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH16_GAUSS_GAIN_N_ADDR (48 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH16_GAUSS_GAIN_D_ADDR (49 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH17_GAUSS_GAIN_N_ADDR (50 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH17_GAUSS_GAIN_D_ADDR (51 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH18_GAUSS_GAIN_N_ADDR (52 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH18_GAUSS_GAIN_D_ADDR (53 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH19_GAUSS_GAIN_N_ADDR (54 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH19_GAUSS_GAIN_D_ADDR (55 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH20_GAUSS_GAIN_N_ADDR (56 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH20_GAUSS_GAIN_D_ADDR (57 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH21_GAUSS_GAIN_N_ADDR (58 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH21_GAUSS_GAIN_D_ADDR (59 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH22_GAUSS_GAIN_N_ADDR (60 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH22_GAUSS_GAIN_D_ADDR (61 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH23_GAUSS_GAIN_N_ADDR (62 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH23_GAUSS_GAIN_D_ADDR (63 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH24_GAUSS_GAIN_N_ADDR (64 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH24_GAUSS_GAIN_D_ADDR (65 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH25_GAUSS_GAIN_N_ADDR (66 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH25_GAUSS_GAIN_D_ADDR (67 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH26_GAUSS_GAIN_N_ADDR (68 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH26_GAUSS_GAIN_D_ADDR (69 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH27_GAUSS_GAIN_N_ADDR (70 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH27_GAUSS_GAIN_D_ADDR (71 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH28_GAUSS_GAIN_N_ADDR (72 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH28_GAUSS_GAIN_D_ADDR (73 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH29_GAUSS_GAIN_N_ADDR (74 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH29_GAUSS_GAIN_D_ADDR (75 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH30_GAUSS_GAIN_N_ADDR (76 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH30_GAUSS_GAIN_D_ADDR (77 + GAUSSIAN_WAVEFORM_OFFSET)

#define STM_REGS_CH31_GAUSS_GAIN_N_ADDR (78 + GAUSSIAN_WAVEFORM_OFFSET)
#define STM_REGS_CH31_GAUSS_GAIN_D_ADDR (79 + GAUSSIAN_WAVEFORM_OFFSET)

/* ========================================================================== */



/* SINUSOIDAL WAVEFORM GENERATOR                                              */
/* -------------------------------------------------------------------------- */

#define SINUSOIDAL_WAVEFORM_OFFSET    (200)

/**
 * STM_REGS_CHX_SIN (32-bit): Configures the Sinusoidal waveform in channel 0
 *
 *   FREQ  (bits 15- 0): Frequency of the sinusoidal waveform (FREQ = FSIN(Hz)*65536/1000)
 *   GAIN_N(bits 23-16): Numerator of Gain factor applied to the sinusoidal waveform from 1 to 255
 *   GAIN_D(bits 31-24): Denominator of the Gain factor applied to the sinusoidal waveform from 1 to 255
 *
 *   BYTE-0: Bits ( 7- 0) of the FERQ field
 *   BYTE-1: Bits (15- 8) of the FERQ field
 *   BYTE-2: Bits ( 7- 0) of the GAIN_N field
 *   BYTE-3: Bits ( 7- 0) of the GAIN_D field
 * */
#define STM_REGS_CH0_SIN_FREQ_0_ADDR   (0 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH0_SIN_FREQ_1_ADDR   (1 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH0_SIN_PHASE_0_ADDR  (2 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH0_SIN_PHASE_1_ADDR  (3 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH0_SIN_GAIN_N_ADDR   (4 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH0_SIN_GAIN_D_ADDR   (5 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH1_SIN_FREQ_0_ADDR   (6 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH1_SIN_FREQ_1_ADDR   (7 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH1_SIN_PHASE_0_ADDR  (8 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH1_SIN_PHASE_1_ADDR  (9 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH1_SIN_GAIN_N_ADDR   (10 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH1_SIN_GAIN_D_ADDR   (11 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH2_SIN_FREQ_0_ADDR   (12 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH2_SIN_FREQ_1_ADDR   (13 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH2_SIN_PHASE_0_ADDR  (14 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH2_SIN_PHASE_1_ADDR  (15 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH2_SIN_GAIN_N_ADDR   (16 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH2_SIN_GAIN_D_ADDR   (17 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH3_SIN_FREQ_0_ADDR   (18 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH3_SIN_FREQ_1_ADDR   (19 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH3_SIN_PHASE_0_ADDR  (20 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH3_SIN_PHASE_1_ADDR  (21 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH3_SIN_GAIN_N_ADDR   (22 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH3_SIN_GAIN_D_ADDR   (23 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH4_SIN_FREQ_0_ADDR   (24 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH4_SIN_FREQ_1_ADDR   (25 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH4_SIN_PHASE_0_ADDR  (26 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH4_SIN_PHASE_1_ADDR  (27 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH4_SIN_GAIN_N_ADDR   (28 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH4_SIN_GAIN_D_ADDR   (29 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH5_SIN_FREQ_0_ADDR   (30 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH5_SIN_FREQ_1_ADDR   (31 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH5_SIN_PHASE_0_ADDR  (32 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH5_SIN_PHASE_1_ADDR  (33 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH5_SIN_GAIN_N_ADDR   (34 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH5_SIN_GAIN_D_ADDR   (35 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH6_SIN_FREQ_0_ADDR   (36 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH6_SIN_FREQ_1_ADDR   (37 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH6_SIN_PHASE_0_ADDR  (38 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH6_SIN_PHASE_1_ADDR  (39 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH6_SIN_GAIN_N_ADDR   (40 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH6_SIN_GAIN_D_ADDR   (41 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH7_SIN_FREQ_0_ADDR   (42 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH7_SIN_FREQ_1_ADDR   (43 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH7_SIN_PHASE_0_ADDR  (44 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH7_SIN_PHASE_1_ADDR  (45 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH7_SIN_GAIN_N_ADDR   (46 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH7_SIN_GAIN_D_ADDR   (47 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH8_SIN_FREQ_0_ADDR   (48 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH8_SIN_FREQ_1_ADDR   (49 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH8_SIN_PHASE_0_ADDR  (50 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH8_SIN_PHASE_1_ADDR  (51 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH8_SIN_GAIN_N_ADDR   (52 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH8_SIN_GAIN_D_ADDR   (53 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH9_SIN_FREQ_0_ADDR   (54 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH9_SIN_FREQ_1_ADDR   (55 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH9_SIN_PHASE_0_ADDR  (56 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH9_SIN_PHASE_1_ADDR  (57 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH9_SIN_GAIN_N_ADDR   (58 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH9_SIN_GAIN_D_ADDR   (59 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH10_SIN_FREQ_0_ADDR   (60 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH10_SIN_FREQ_1_ADDR   (61 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH10_SIN_PHASE_0_ADDR  (62 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH10_SIN_PHASE_1_ADDR  (63 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH10_SIN_GAIN_N_ADDR   (64 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH10_SIN_GAIN_D_ADDR   (65 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH11_SIN_FREQ_0_ADDR   (66 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH11_SIN_FREQ_1_ADDR   (67 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH11_SIN_PHASE_0_ADDR  (68 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH11_SIN_PHASE_1_ADDR  (69 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH11_SIN_GAIN_N_ADDR   (70 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH11_SIN_GAIN_D_ADDR   (71 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH12_SIN_FREQ_0_ADDR   (72 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH12_SIN_FREQ_1_ADDR   (73 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH12_SIN_PHASE_0_ADDR  (74 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH12_SIN_PHASE_1_ADDR  (75 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH12_SIN_GAIN_N_ADDR   (76 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH12_SIN_GAIN_D_ADDR   (77 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH13_SIN_FREQ_0_ADDR   (78 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH13_SIN_FREQ_1_ADDR   (79 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH13_SIN_PHASE_0_ADDR  (80 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH13_SIN_PHASE_1_ADDR  (81 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH13_SIN_GAIN_N_ADDR   (82 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH13_SIN_GAIN_D_ADDR   (83 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH14_SIN_FREQ_0_ADDR   (84 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH14_SIN_FREQ_1_ADDR   (85 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH14_SIN_PHASE_0_ADDR  (86 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH14_SIN_PHASE_1_ADDR  (87 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH14_SIN_GAIN_N_ADDR   (88 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH14_SIN_GAIN_D_ADDR   (89 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH15_SIN_FREQ_0_ADDR   (90 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH15_SIN_FREQ_1_ADDR   (91 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH15_SIN_PHASE_0_ADDR  (92 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH15_SIN_PHASE_1_ADDR  (93 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH15_SIN_GAIN_N_ADDR   (94 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH15_SIN_GAIN_D_ADDR   (95 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH16_SIN_FREQ_0_ADDR   (96 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH16_SIN_FREQ_1_ADDR   (97 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH16_SIN_PHASE_0_ADDR  (98 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH16_SIN_PHASE_1_ADDR  (99 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH16_SIN_GAIN_N_ADDR   (100 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH16_SIN_GAIN_D_ADDR   (101 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH17_SIN_FREQ_0_ADDR   (102 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH17_SIN_FREQ_1_ADDR   (103 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH17_SIN_PHASE_0_ADDR  (104 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH17_SIN_PHASE_1_ADDR  (105 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH17_SIN_GAIN_N_ADDR   (106 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH17_SIN_GAIN_D_ADDR   (107 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH18_SIN_FREQ_0_ADDR   (108 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH18_SIN_FREQ_1_ADDR   (109 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH18_SIN_PHASE_0_ADDR  (110 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH18_SIN_PHASE_1_ADDR  (111 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH18_SIN_GAIN_N_ADDR   (112 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH18_SIN_GAIN_D_ADDR   (113 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH19_SIN_FREQ_0_ADDR   (114 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH19_SIN_FREQ_1_ADDR   (115 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH19_SIN_PHASE_0_ADDR  (116 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH19_SIN_PHASE_1_ADDR  (117 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH19_SIN_GAIN_N_ADDR   (118 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH19_SIN_GAIN_D_ADDR   (119 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH20_SIN_FREQ_0_ADDR   (120 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH20_SIN_FREQ_1_ADDR   (121 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH20_SIN_PHASE_0_ADDR  (122 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH20_SIN_PHASE_1_ADDR  (123 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH20_SIN_GAIN_N_ADDR   (124 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH20_SIN_GAIN_D_ADDR   (125 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH21_SIN_FREQ_0_ADDR   (126 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH21_SIN_FREQ_1_ADDR   (127 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH21_SIN_PHASE_0_ADDR  (128 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH21_SIN_PHASE_1_ADDR  (129 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH21_SIN_GAIN_N_ADDR   (130 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH21_SIN_GAIN_D_ADDR   (131 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH22_SIN_FREQ_0_ADDR   (132 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH22_SIN_FREQ_1_ADDR   (133 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH22_SIN_PHASE_0_ADDR  (134 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH22_SIN_PHASE_1_ADDR  (135 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH22_SIN_GAIN_N_ADDR   (136 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH22_SIN_GAIN_D_ADDR   (137 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH23_SIN_FREQ_0_ADDR   (138 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH23_SIN_FREQ_1_ADDR   (139 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH23_SIN_PHASE_0_ADDR  (140 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH23_SIN_PHASE_1_ADDR  (141 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH23_SIN_GAIN_N_ADDR   (142 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH23_SIN_GAIN_D_ADDR   (143 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH24_SIN_FREQ_0_ADDR   (144 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH24_SIN_FREQ_1_ADDR   (145 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH24_SIN_PHASE_0_ADDR  (146 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH24_SIN_PHASE_1_ADDR  (147 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH24_SIN_GAIN_N_ADDR   (148 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH24_SIN_GAIN_D_ADDR   (149 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH25_SIN_FREQ_0_ADDR   (150 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH25_SIN_FREQ_1_ADDR   (151 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH25_SIN_PHASE_0_ADDR  (152 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH25_SIN_PHASE_1_ADDR  (153 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH25_SIN_GAIN_N_ADDR   (154 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH25_SIN_GAIN_D_ADDR   (155 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH26_SIN_FREQ_0_ADDR   (156 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH26_SIN_FREQ_1_ADDR   (157 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH26_SIN_PHASE_0_ADDR  (158 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH26_SIN_PHASE_1_ADDR  (159 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH26_SIN_GAIN_N_ADDR   (160 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH26_SIN_GAIN_D_ADDR   (161 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH27_SIN_FREQ_0_ADDR   (162 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH27_SIN_FREQ_1_ADDR   (163 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH27_SIN_PHASE_0_ADDR  (164 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH27_SIN_PHASE_1_ADDR  (165 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH27_SIN_GAIN_N_ADDR   (166 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH27_SIN_GAIN_D_ADDR   (167 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH28_SIN_FREQ_0_ADDR   (168 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH28_SIN_FREQ_1_ADDR   (169 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH28_SIN_PHASE_0_ADDR  (170 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH28_SIN_PHASE_1_ADDR  (171 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH28_SIN_GAIN_N_ADDR   (172 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH28_SIN_GAIN_D_ADDR   (173 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH29_SIN_FREQ_0_ADDR   (173 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH29_SIN_FREQ_1_ADDR   (174 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH29_SIN_PHASE_0_ADDR  (175 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH29_SIN_PHASE_1_ADDR  (176 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH29_SIN_GAIN_N_ADDR   (177 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH29_SIN_GAIN_D_ADDR   (178 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH30_SIN_FREQ_0_ADDR   (179 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH30_SIN_FREQ_1_ADDR   (180 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH30_SIN_PHASE_0_ADDR  (181 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH30_SIN_PHASE_1_ADDR  (182 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH30_SIN_GAIN_N_ADDR   (183 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH30_SIN_GAIN_D_ADDR   (184 + SINUSOIDAL_WAVEFORM_OFFSET)

#define STM_REGS_CH31_SIN_FREQ_0_ADDR   (185 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH31_SIN_FREQ_1_ADDR   (186 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH31_SIN_PHASE_0_ADDR  (187 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH31_SIN_PHASE_1_ADDR  (188 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH31_SIN_GAIN_N_ADDR   (189 + SINUSOIDAL_WAVEFORM_OFFSET)
#define STM_REGS_CH31_SIN_GAIN_D_ADDR   (190 + SINUSOIDAL_WAVEFORM_OFFSET)

/* ========================================================================== */



/* DC WAVEFORM GENERATOR                                                      */
/* -------------------------------------------------------------------------- */

#define DC_WAVEFORM_OFFSET             (400)

/**
 * STM_REGS_CHX_DC (16-bit): Configure the offset current in DC mode
 *
 *   The generated DC offset is OFFSET*4/65536 (mA)
 *
 *   OFFSET_0 (bits 7 - 0): The Least significant byte of the  offset
 *   OFFSET_1 (bits 15- 8): The Most significant byte of the  offset
 *
 * */
#define STM_REGS_CH0_DC_OFF_0_ADDR     (0 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH0_DC_OFF_1_ADDR     (1 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH1_DC_OFF_0_ADDR     (2 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH1_DC_OFF_1_ADDR     (3 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH2_DC_OFF_0_ADDR     (4 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH2_DC_OFF_1_ADDR     (5 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH3_DC_OFF_0_ADDR     (6 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH3_DC_OFF_1_ADDR     (7 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH4_DC_OFF_0_ADDR     (8 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH4_DC_OFF_1_ADDR     (9 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH5_DC_OFF_0_ADDR     (10 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH5_DC_OFF_1_ADDR     (11 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH6_DC_OFF_0_ADDR     (12 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH6_DC_OFF_1_ADDR     (13 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH7_DC_OFF_0_ADDR     (14 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH7_DC_OFF_1_ADDR     (15 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH8_DC_OFF_0_ADDR     (16 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH8_DC_OFF_1_ADDR     (17 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH9_DC_OFF_0_ADDR     (18 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH9_DC_OFF_1_ADDR     (19 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH10_DC_OFF_0_ADDR     (20 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH10_DC_OFF_1_ADDR     (21 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH11_DC_OFF_0_ADDR     (22 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH11_DC_OFF_1_ADDR     (23 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH12_DC_OFF_0_ADDR     (24 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH12_DC_OFF_1_ADDR     (25 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH13_DC_OFF_0_ADDR     (26 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH13_DC_OFF_1_ADDR     (27 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH14_DC_OFF_0_ADDR     (28 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH14_DC_OFF_1_ADDR     (29 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH15_DC_OFF_0_ADDR     (30 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH15_DC_OFF_1_ADDR     (31 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH16_DC_OFF_0_ADDR     (32 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH16_DC_OFF_1_ADDR     (33 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH17_DC_OFF_0_ADDR     (34 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH17_DC_OFF_1_ADDR     (35 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH18_DC_OFF_0_ADDR     (36 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH18_DC_OFF_1_ADDR     (37 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH19_DC_OFF_0_ADDR     (38 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH19_DC_OFF_1_ADDR     (39 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH20_DC_OFF_0_ADDR     (40 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH20_DC_OFF_1_ADDR     (41 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH21_DC_OFF_0_ADDR     (42 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH21_DC_OFF_1_ADDR     (43 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH22_DC_OFF_0_ADDR     (44 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH22_DC_OFF_1_ADDR     (45 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH23_DC_OFF_0_ADDR     (46 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH23_DC_OFF_1_ADDR     (47 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH24_DC_OFF_0_ADDR     (49 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH24_DC_OFF_1_ADDR     (50 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH25_DC_OFF_0_ADDR     (51 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH25_DC_OFF_1_ADDR     (52 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH26_DC_OFF_0_ADDR     (53 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH26_DC_OFF_1_ADDR     (54 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH27_DC_OFF_0_ADDR     (55 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH27_DC_OFF_1_ADDR     (56 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH28_DC_OFF_0_ADDR     (57 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH28_DC_OFF_1_ADDR     (58 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH29_DC_OFF_0_ADDR     (59 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH29_DC_OFF_1_ADDR     (60 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH30_DC_OFF_0_ADDR     (61 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH30_DC_OFF_1_ADDR     (62 + DC_WAVEFORM_OFFSET)

#define STM_REGS_CH31_DC_OFF_0_ADDR     (63 + DC_WAVEFORM_OFFSET)
#define STM_REGS_CH31_DC_OFF_1_ADDR     (64 + DC_WAVEFORM_OFFSET)


/* ========================================================================== */



/* IMPEDANCE MEASUREMENT                                                      */
/* -------------------------------------------------------------------------- */

#define IMP_OFFSET                     (480)

/** STM_IMP_FREQ_DEC: Indicates the frequency decimation of the Impedance
 *                    measurement with respect the Stimulation Sampling rate.
 *                    In 1 out of STM_IMP_FERQ_DEC the impedance will be reported.
 *
 */
#define STM_IMP_FREQ_DEC_ADDR          (0 + IMP_OFFSET)

/** STM_IMP_THR: Configurable threshold to allow automatically stops stimulation
 *               in case that measured impedance is higher than this threshold
 *
 *               IMP_THR_0: represents the LSB of a 16-bit number
 *               IMP_THR_1: represents the LSB of a 16-bit number
 * */
#define STM_IMP_THR_0_ADDR             (1 + IMP_OFFSET)
#define STM_IMP_THR_1_ADDR             (2 + IMP_OFFSET)


/** STM_ERROR: Indicates the reason why the stimulation is stopped
 *
 *    0: End of programmed waveform is reached
 *    1: Impedance check failed
 *    2: Communication problems
 *    3: Stop Requested by NIC
 *
 * */
#define STM_ERROR_ADDR                 (3 + IMP_OFFSET)


/** STM_REPORT_DATA: Used to configure whether the STM data must be reported
 *                   to the host or not.
 *
 *     0: STM waveform samples will NOT sent to the host
 *     1: STM waveform samples will be sent to the host
 *
 *  Default: 0
 * */
#define STM_REPORT_DATA_ADDR           (4 + IMP_OFFSET)


/* ========================================================================== */
/* ONLINE STIMULATION CHANGE - DC Waveform generator                          */
/* -------------------------------------------------------------------------- */

#define STM_OL_DC_OFFSET              (500)

/**
 * STM_OL_CH0_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH0_DC_OFF_0_ADDR       (0 + STM_OL_DC_OFFSET)
#define STM_OL_CH0_DC_OFF_1_ADDR       (1 + STM_OL_DC_OFFSET)
#define STM_OL_CH0_DC_OFF_TRANS0_ADDR  (2 + STM_OL_DC_OFFSET)
#define STM_OL_CH0_DC_OFF_TRANS1_ADDR  (3 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH1_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH1_DC_OFF_0_ADDR       (4 + STM_OL_DC_OFFSET)
#define STM_OL_CH1_DC_OFF_1_ADDR       (5 + STM_OL_DC_OFFSET)
#define STM_OL_CH1_DC_OFF_TRANS0_ADDR  (6 + STM_OL_DC_OFFSET)
#define STM_OL_CH1_DC_OFF_TRANS1_ADDR  (7 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH2_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH2_DC_OFF_0_ADDR       (8 + STM_OL_DC_OFFSET)
#define STM_OL_CH2_DC_OFF_1_ADDR       (9 + STM_OL_DC_OFFSET)
#define STM_OL_CH2_DC_OFF_TRANS0_ADDR  (10 + STM_OL_DC_OFFSET)
#define STM_OL_CH2_DC_OFF_TRANS1_ADDR  (11 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH3_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH3_DC_OFF_0_ADDR       (12 + STM_OL_DC_OFFSET)
#define STM_OL_CH3_DC_OFF_1_ADDR       (13 + STM_OL_DC_OFFSET)
#define STM_OL_CH3_DC_OFF_TRANS0_ADDR  (14 + STM_OL_DC_OFFSET)
#define STM_OL_CH3_DC_OFF_TRANS1_ADDR  (15 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH4_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH4_DC_OFF_0_ADDR       (16 + STM_OL_DC_OFFSET)
#define STM_OL_CH4_DC_OFF_1_ADDR       (17 + STM_OL_DC_OFFSET)
#define STM_OL_CH4_DC_OFF_TRANS0_ADDR  (18 + STM_OL_DC_OFFSET)
#define STM_OL_CH4_DC_OFF_TRANS1_ADDR  (19 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH5_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH5_DC_OFF_0_ADDR       (20 + STM_OL_DC_OFFSET)
#define STM_OL_CH5_DC_OFF_1_ADDR       (21 + STM_OL_DC_OFFSET)
#define STM_OL_CH5_DC_OFF_TRANS0_ADDR  (22 + STM_OL_DC_OFFSET)
#define STM_OL_CH5_DC_OFF_TRANS1_ADDR  (23 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH6_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH6_DC_OFF_0_ADDR       (24 + STM_OL_DC_OFFSET)
#define STM_OL_CH6_DC_OFF_1_ADDR       (25 + STM_OL_DC_OFFSET)
#define STM_OL_CH6_DC_OFF_TRANS0_ADDR  (26 + STM_OL_DC_OFFSET)
#define STM_OL_CH6_DC_OFF_TRANS1_ADDR  (27 + STM_OL_DC_OFFSET)

/**
 * STM_OL_CH7_DC_OFF (24-bit): Configures the online stimulation change in DC Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH7_DC_OFF_0_ADDR       (28 + STM_OL_DC_OFFSET)
#define STM_OL_CH7_DC_OFF_1_ADDR       (29 + STM_OL_DC_OFFSET)
#define STM_OL_CH7_DC_OFF_TRANS0_ADDR  (30 + STM_OL_DC_OFFSET)
#define STM_OL_CH7_DC_OFF_TRANS1_ADDR  (31 + STM_OL_DC_OFFSET)


/* ONLINE STIMULATION CHANGE - Sinusoidal Waveform Generator                  */
/* -------------------------------------------------------------------------- */

#define STM_OL_SIN_OFFSET              (650)

/**
 * STM_OL_CH0_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH0_SIN_GAIN_N_ADDR     (0 + STM_OL_SIN_OFFSET)
#define STM_OL_CH0_SIN_GAIN_D_ADDR     (1 + STM_OL_SIN_OFFSET)
#define STM_OL_CH0_SIN_TRANS0_ADDR     (2 + STM_OL_SIN_OFFSET)
#define STM_OL_CH0_SIN_TRANS1_ADDR     (3 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH1_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH1_SIN_GAIN_N_ADDR     (4 + STM_OL_SIN_OFFSET)
#define STM_OL_CH1_SIN_GAIN_D_ADDR     (5 + STM_OL_SIN_OFFSET)
#define STM_OL_CH1_SIN_TRANS0_ADDR     (6 + STM_OL_SIN_OFFSET)
#define STM_OL_CH1_SIN_TRANS1_ADDR     (7 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH2_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH2_SIN_GAIN_N_ADDR     (8 + STM_OL_SIN_OFFSET)
#define STM_OL_CH2_SIN_GAIN_D_ADDR     (9 + STM_OL_SIN_OFFSET)
#define STM_OL_CH2_SIN_TRANS0_ADDR     (10 + STM_OL_SIN_OFFSET)
#define STM_OL_CH2_SIN_TRANS1_ADDR     (11 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH3_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH3_SIN_GAIN_N_ADDR     (12 + STM_OL_SIN_OFFSET)
#define STM_OL_CH3_SIN_GAIN_D_ADDR     (13 + STM_OL_SIN_OFFSET)
#define STM_OL_CH3_SIN_TRANS0_ADDR     (14 + STM_OL_SIN_OFFSET)
#define STM_OL_CH3_SIN_TRANS1_ADDR     (15 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH4_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH4_SIN_GAIN_N_ADDR     (16 + STM_OL_SIN_OFFSET)
#define STM_OL_CH4_SIN_GAIN_D_ADDR     (17 + STM_OL_SIN_OFFSET)
#define STM_OL_CH4_SIN_TRANS0_ADDR     (18 + STM_OL_SIN_OFFSET)
#define STM_OL_CH4_SIN_TRANS1_ADDR     (19 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH5_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 * */
#define STM_OL_CH5_SIN_GAIN_N_ADDR     (20 + STM_OL_SIN_OFFSET)
#define STM_OL_CH5_SIN_GAIN_D_ADDR     (21 + STM_OL_SIN_OFFSET)
#define STM_OL_CH5_SIN_TRANS0_ADDR     (22 + STM_OL_SIN_OFFSET)
#define STM_OL_CH5_SIN_TRANS1_ADDR     (23 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH5_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 */
#define STM_OL_CH6_SIN_GAIN_N_ADDR     (24 + STM_OL_SIN_OFFSET)
#define STM_OL_CH6_SIN_GAIN_D_ADDR     (25 + STM_OL_SIN_OFFSET)
#define STM_OL_CH6_SIN_TRANS0_ADDR     (26 + STM_OL_SIN_OFFSET)
#define STM_OL_CH6_SIN_TRANS1_ADDR     (27 + STM_OL_SIN_OFFSET)

/**
 * STM_OL_CH5_SIN (24-bit): Configures the online stimulation change in Sinusoidal Waverform generator
 *
 *   BYTE-0: Bits ( 7- 0) of the targeted value change
 *   BYTE-1: Bits (15- 8) of the targeted value change
 *   BYTE-2: Bits ( 7- 0) milliseconds of the transition
 *   BYTE-3: Bits (15- 8) milliseconds of the transition
 */
#define STM_OL_CH7_SIN_GAIN_N_ADDR  (28 + STM_OL_SIN_OFFSET)
#define STM_OL_CH7_SIN_GAIN_D_ADDR  (29 + STM_OL_SIN_OFFSET)
#define STM_OL_CH7_SIN_TRANS0_ADDR  (30 + STM_OL_SIN_OFFSET)
#define STM_OL_CH7_SIN_TRANS1_ADDR  (31 + STM_OL_SIN_OFFSET)



/* ========================================================================== */

#define STM_FLT_OFFSET                 (700)

/** STM_FLT_COEF_0_ADDR: Address of the first coefficient of the FIR filter
 *                       applied to the output signal.
 *                       The filter is composed of 50 Coefficients which are lineal phase
 */
#define STM_FLT_COEF_0_ADDR            (0 + STM_FLT_OFFSET)
#define STM_FLT_COEF_1_ADDR            (1 + STM_FLT_OFFSET)
#define STM_FLT_COEF_2_ADDR            (2 + STM_FLT_OFFSET)
#define STM_FLT_COEF_3_ADDR            (3 + STM_FLT_OFFSET)
#define STM_FLT_COEF_4_ADDR            (4 + STM_FLT_OFFSET)
#define STM_FLT_COEF_5_ADDR            (5 + STM_FLT_OFFSET)
#define STM_FLT_COEF_6_ADDR            (6 + STM_FLT_OFFSET)
#define STM_FLT_COEF_7_ADDR            (7 + STM_FLT_OFFSET)
#define STM_FLT_COEF_8_ADDR            (8 + STM_FLT_OFFSET)
#define STM_FLT_COEF_9_ADDR            (9 + STM_FLT_OFFSET)
#define STM_FLT_COEF_10_ADDR           (10 + STM_FLT_OFFSET)
#define STM_FLT_COEF_11_ADDR           (11 + STM_FLT_OFFSET)
#define STM_FLT_COEF_12_ADDR           (12 + STM_FLT_OFFSET)
#define STM_FLT_COEF_13_ADDR           (13 + STM_FLT_OFFSET)
#define STM_FLT_COEF_14_ADDR           (14 + STM_FLT_OFFSET)
#define STM_FLT_COEF_15_ADDR           (15 + STM_FLT_OFFSET)
#define STM_FLT_COEF_16_ADDR           (16 + STM_FLT_OFFSET)
#define STM_FLT_COEF_17_ADDR           (17 + STM_FLT_OFFSET)
#define STM_FLT_COEF_18_ADDR           (18 + STM_FLT_OFFSET)
#define STM_FLT_COEF_19_ADDR           (19 + STM_FLT_OFFSET)
#define STM_FLT_COEF_20_ADDR           (20 + STM_FLT_OFFSET)
#define STM_FLT_COEF_21_ADDR           (21 + STM_FLT_OFFSET)
#define STM_FLT_COEF_22_ADDR           (22 + STM_FLT_OFFSET)
#define STM_FLT_COEF_23_ADDR           (23 + STM_FLT_OFFSET)
#define STM_FLT_COEF_24_ADDR           (24 + STM_FLT_OFFSET)
#define STM_FLT_COEF_25_ADDR           (25 + STM_FLT_OFFSET)
#define STM_FLT_COEF_26_ADDR           (26 + STM_FLT_OFFSET)
#define STM_FLT_COEF_27_ADDR           (27 + STM_FLT_OFFSET)
#define STM_FLT_COEF_28_ADDR           (28 + STM_FLT_OFFSET)
#define STM_FLT_COEF_29_ADDR           (29 + STM_FLT_OFFSET)
#define STM_FLT_COEF_30_ADDR           (30 + STM_FLT_OFFSET)
#define STM_FLT_COEF_31_ADDR           (31 + STM_FLT_OFFSET)
#define STM_FLT_COEF_32_ADDR           (32 + STM_FLT_OFFSET)
#define STM_FLT_COEF_33_ADDR           (33 + STM_FLT_OFFSET)
#define STM_FLT_COEF_34_ADDR           (34 + STM_FLT_OFFSET)
#define STM_FLT_COEF_35_ADDR           (35 + STM_FLT_OFFSET)
#define STM_FLT_COEF_36_ADDR           (36 + STM_FLT_OFFSET)
#define STM_FLT_COEF_37_ADDR           (37 + STM_FLT_OFFSET)
#define STM_FLT_COEF_38_ADDR           (38 + STM_FLT_OFFSET)
#define STM_FLT_COEF_39_ADDR           (39 + STM_FLT_OFFSET)
#define STM_FLT_COEF_40_ADDR           (40 + STM_FLT_OFFSET)
#define STM_FLT_COEF_41_ADDR           (41 + STM_FLT_OFFSET)
#define STM_FLT_COEF_42_ADDR           (42 + STM_FLT_OFFSET)
#define STM_FLT_COEF_43_ADDR           (43 + STM_FLT_OFFSET)
#define STM_FLT_COEF_44_ADDR           (44 + STM_FLT_OFFSET)
#define STM_FLT_COEF_45_ADDR           (45 + STM_FLT_OFFSET)
#define STM_FLT_COEF_46_ADDR           (46 + STM_FLT_OFFSET)
#define STM_FLT_COEF_47_ADDR           (47 + STM_FLT_OFFSET)
#define STM_FLT_COEF_48_ADDR           (48 + STM_FLT_OFFSET)
#define STM_FLT_COEF_49_ADDR           (49 + STM_FLT_OFFSET)
#define STM_FLT_COEF_50_ADDR           (50 + STM_FLT_OFFSET)


/**
 *  STM_FLT_CH_INFO_ADDR: FLT_CH_INFO Select the channel in which the Filter
 *                        will be applied
 */
#define STM_FLT_CH_INFO_ADDR           (STM_FLT_COEF_0_ADDR+STM_FLT_LEN+1)

/**
 * STM_FLT_DIV_ADDR: Division factor to apply to the output of the FIR filter
 */
#define STM_FLT_DIV_0_ADDR             (STM_FLT_CH_INFO_ADDR+2)
#define STM_FLT_DIV_1_ADDR             (STM_FLT_CH_INFO_ADDR+3)

#if (STM_FLT_COEF_0_ADDR+STM_FLT_LEN + 2)>255
    #warning "CAUTION: STM FILTER IS TOO LONG"
#endif


/* ========================================================================== */



/* ========================================================================== */
/* DEFAULT VALUES                                                             */
/* -------------------------------------------------------------------------- */

/* DAC REGISTERS                                                              */
/* -------------------------------------------------------------------------- */
#define STM_REGS_DAC_B0_DEF            (0)
#define STM_REGS_DAC_B1_DEF            (0)
#define STM_REGS_DAC_B2_DEF            (0)
#define STM_REGS_DAC_B3_DEF            (0)


/* OPERATION REGISTERS:                                                       */
/* -------------------------------------------------------------------------- */

/* None channel is reported */
#define STM_REGS_CH_INFO_DEF           (0x00)

/* To be defined */
#define STM_REGS_MODE_DEF              (STM_INTERNAL)

/* To be defined */
#define STM_REGS_CFG_1_DEF             (0x0)

/* Free channel is CH7 */
#define STM_REGS_CH_FREE_DEF           (0x80)

/* Sampling rate of 1000 SPS */
#define STM_REGS_RATE_DEC_DEF          (1)

/* Ramp-up duration of 120 secs. */
#define STM_REGS_RAMP_UP_0_DEF         (0x0A)
#define STM_REGS_RAMP_UP_1_DEF         (0x00)

/* Ramp-down duration of 120 secs. */
#define STM_REGS_RAMP_DN_0_DEF         (0x0A)
#define STM_REGS_RAMP_DN_1_DEF         (0x00)

/* Stimulation tine of 30 min., that is 1800 secs */
#define STM_REGS_TIME_0_DEF            (0x08)
#define STM_REGS_TIME_1_DEF            (0x07)



/* GAUSSIAN WAVEFORM GENERATOR                                                */
/* -------------------------------------------------------------------------- */
#define STM_REGS_SEED_0_DEF        (0xA5)
#define STM_REGS_SEED_1_DEF        (0xB7)

/* STD is divided by SQRT(7) to allow 1 out 8 free channels */
#define STM_REGS_GAUSS_GAIN_N_DEF  (0x40)
#define STM_REGS_GAUSS_GAIN_D_DEF  (0xA9)


// SINUSOIDAL WAVEFORM GENERATOR
// --------------------------------------------------------------------------

// Def. FSIN = 31.5Hz =>  FREQ = 31.25*65536/1000 = 20480 = 0x5000
#define STM_REGS_SIN_FREQ_0_DEF    (0x00)
#define STM_REGS_SIN_FREQ_1_DEF    (0x08)

// Def. Phase = 0
#define STM_REGS_SIN_PHASE_0_DEF   (0x00)
#define STM_REGS_SIN_PHASE_1_DEF   (0x00)

// Def. 2mApp: N/D = 0.5 => N = 1 and D = 2
#define STM_REGS_SIN_GAIN_N_DEF    (0x00)
#define STM_REGS_SIN_GAIN_D_DEF    (0x02)

// ==========================================================================


// DC WAVEFORM GENERATOR
// --------------------------------------------------------------------------

/* DC Offset of 500uA */
#define STM_REGS_DC_OFF_0_DEF      (0x00)
#define STM_REGS_DC_OFF_1_DEF      (0x00)

/* IMPEDANCE MEASUREMENT                                                      */
/* -------------------------------------------------------------------------- */
/* Frequency Decmation for impedance measurements */
#define STM_IMP_FREQ_DEC_DEF           (0xFA)

/* Default impedance threshold is 20K-Ohm*/
#define STM_IMP_THR_0_DEF              (0x20)
#define STM_IMP_THR_1_DEF              (0x4E)

/* Default error Code */
#define STM_ERROR_DEF                  (0x00)
#define STM_ERROR_IMP                  (0x01)
#define STM_ERROR_COMM                 (0x02)
#define STM_ERROR_NIC                  (0x03)



/* REPORT DATA */
/* -------------------------------------------------------------------------- */
#define STM_REPORT_DATA_DEF            (0)


/* STIM FILTER */
/* -------------------------------------------------------------------------- */
#define STM_FLT_CH_INFO_DEF            (0)
#define STM_FLT_DIV_0_DEF              (154)
#define STM_FLT_DIV_1_DEF              (0)

#define STM_FLT_COEF_0_DEF             (0)
#define STM_FLT_COEF_1_DEF             (0)
#define STM_FLT_COEF_2_DEF             (0)
#define STM_FLT_COEF_3_DEF             (0)
#define STM_FLT_COEF_4_DEF             (0)
#define STM_FLT_COEF_5_DEF             (0)
#define STM_FLT_COEF_6_DEF             (0)
#define STM_FLT_COEF_7_DEF             (0)
#define STM_FLT_COEF_8_DEF             (0)
#define STM_FLT_COEF_9_DEF             (0)
#define STM_FLT_COEF_10_DEF            (0)
#define STM_FLT_COEF_11_DEF            (0)
#define STM_FLT_COEF_12_DEF            (0)
#define STM_FLT_COEF_13_DEF            (0)
#define STM_FLT_COEF_14_DEF            (1)
#define STM_FLT_COEF_15_DEF            (1)
#define STM_FLT_COEF_16_DEF            (1)
#define STM_FLT_COEF_17_DEF            (1)
#define STM_FLT_COEF_18_DEF            (1)
#define STM_FLT_COEF_19_DEF            (1)
#define STM_FLT_COEF_20_DEF            (1)
#define STM_FLT_COEF_21_DEF            (1)
#define STM_FLT_COEF_22_DEF            (2)
#define STM_FLT_COEF_23_DEF            (2)
#define STM_FLT_COEF_24_DEF            (2)
#define STM_FLT_COEF_25_DEF            (2)
#define STM_FLT_COEF_26_DEF            (2)
#define STM_FLT_COEF_27_DEF            (2)
#define STM_FLT_COEF_28_DEF            (3)
#define STM_FLT_COEF_29_DEF            (3)
#define STM_FLT_COEF_30_DEF            (3)
#define STM_FLT_COEF_31_DEF            (3)
#define STM_FLT_COEF_32_DEF            (4)
#define STM_FLT_COEF_33_DEF            (4)
#define STM_FLT_COEF_34_DEF            (4)
#define STM_FLT_COEF_35_DEF            (5)
#define STM_FLT_COEF_36_DEF            (5)
#define STM_FLT_COEF_37_DEF            (6)
#define STM_FLT_COEF_38_DEF            (6)
#define STM_FLT_COEF_39_DEF            (7)
#define STM_FLT_COEF_40_DEF            (7)
#define STM_FLT_COEF_41_DEF            (8)
#define STM_FLT_COEF_42_DEF            (9)
#define STM_FLT_COEF_43_DEF            (10)
#define STM_FLT_COEF_44_DEF            (11)
#define STM_FLT_COEF_45_DEF            (13)
#define STM_FLT_COEF_46_DEF            (15)
#define STM_FLT_COEF_47_DEF            (19)
#define STM_FLT_COEF_48_DEF            (24)
#define STM_FLT_COEF_49_DEF            (42)
#define STM_FLT_COEF_50_DEF            (127)




/* ========================================================================== */


#endif

