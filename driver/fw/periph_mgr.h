/*===========================================================================*/
/* design unit  : StarStim:  Peripheral Manager                              */
/*                                                                           */
/* file name    : periph_mgr.h                                               */
/*                                                                           */
/* purpose      : Constants for Peripheral Manager                           */
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
/* 1.0      AM       2011/10/07        creation                              */
/*===========================================================================*/

#ifndef _PERIPH_MGR_H_ 
#define _PERIPH_MGR_H_


#define PERIPH_SUCCESS     0
#define PERIPH_FAIL        1

/* ========================================================================== */
/* IMPEDANCE CONSTANTS */
/* -------------------------------------------------------------------------- */
#define PERIPH_READ_VOLT   0
#define PERIPH_READ_CURR   1


/* ========================================================================== */
/* LED CONSTANTS */
/* -------------------------------------------------------------------------- */

/* Period for blinking LED when ready */
#define PERIPH_LED_BLINKING_PERIOD_READY       (250)

/* Period for blinking the LED */
#define PERIPH_LED_BLINKING_PERIOD_CONTINOUS   (500)

/* Period for blinking the LED in ERROR mode*/
#define PERIPH_LED_BLINKING_PERIOD_ERROR       (100)

/* Times to blink in ERROR mode */
#define PERIPH_LED_N_BLINK_ERROR               (16)

/* ========================================================================== */
/* RADIO CONSTANTS */
/* -------------------------------------------------------------------------- */
#define PERIPH_TX_MAX_LEN                (600)

//#define PERIPH_TX_BUFF_LEN             (16384)
//
//#define PERIPH_BUFF_NUM_PCK            (512)  /*CAUTION: if > 256 Review PERIPH_PCK_IND Variable */

/* ========================================================================== */


/* ========================================================================== */
/* TIMER CONSTANTS */
/* -------------------------------------------------------------------------- */

/** BEACON PERIOD: FcLK = 6 MHz (48MHz / 8): 6000 counts take 1ms  */
//#if (BOARD_MCK == 48000000)
//#define BEACON_PERIOD  (6000)
//#elif (BOARD_MCK == 59000000)
//#define BEACON_PERIOD  (7375)
//#elif (BOARD_MCK == 58720256)
//#define BEACON_PERIOD  (7168)
//#elif (BOARD_MCK == 57344000)
#define BEACON_PERIOD  (7168)
//#endif

/** EEG_CLK PERIOD */
#define EEG_CLK_PERIOD          (14)
#define EEG_CLK_HALF            (7)

/** BOOST_STEPS_12V */
//#define BOOST_STEPS_12V         (15)    /* 15V: 70   -   11.8V: 10    -   8.7V: 10(DOWN)    -     6.2V: 27(DOWN)   -    8.0V: 15(DOWN)*/

/* BOOST_STEPS CONSTANTS */
#define BOOST_STEPS_INIT        (15)    /* 8.0V: 15 DOWN from power on point */
#define BOOST_STEPS_15V         (45)    /* 15.0V:45 UP from 8.0V */
#define BOOST_STEPS_08V         (45)    /* 8.0V: 45 DOWN from 15.0V */

/** TC_BOOST_UP */
#define TC_BOOST_UP             ( 150 )

/** TC_BOOST_DN */
#define TC_BOOST_DN             ( 1500 )

/** Interrupt priority for DRDY */
#define DRDY_INTERRUPT_PRIORITY ( 0x00 )

/** Interrupt priority for TC0 */
#define TC0_INTERRUPT_PRIORITY  ( 0x01 )

/** Interrupt priority for TC1 */
#define TC1_INTERRUPT_PRIORITY  ( 0x02 )

/* ========================================================================== */



/* ========================================================================== */
/* USART CONSTANTS */
/* -------------------------------------------------------------------------- */

/** Baud Rate for UART link with radio module */
#define PERIPH_RADIO_CONF                     (0x01)
#define PERIPH_RADIO_CONF_RATE                (115200)
#define PERIPH_UART_BAUD                      (0x00)
#define PERIPH_UART_BAUD_RATE                 (921600)

#if !defined(Q_WS_MAC) && !defined(Q_WS_WIN) && !defined(Q_OS_LINUX)
  #define USART_SERIAL_CHAR_LENGTH				US_MR_CHRL_8_BIT
  #define USART_SERIAL_PARITY						US_MR_PAR_NO
  #define USART_SERIAL_STOP_BIT					US_MR_NBSTOP_1_BIT
  #define USART_SERIAL_MODE						US_MR_CHMODE_NORMAL
#endif
/** Interrupt priority for TC0 */
#define UART_INTERRUPT_PRIORITY  ( 0x01 )

/* ========================================================================== */


/* ========================================================================== */
/* FLASH ADDRESS                                                              */
/* -------------------------------------------------------------------------- */

#define PERIPH_FLASH_ADDR                     (0x43FF00)

#define PERIPH_FLASH_RADIO_LEN                (1)
#define PERIPH_FLASH_RADIO_OFF                (0)

#define PERIPH_FLASH_FWREV_LEN                (1)
#define PERIPH_FLASH_FWREV_OFF                (PERIPH_FLASH_RADIO_OFF + PERIPH_FLASH_RADIO_LEN)

#define PERIPH_FLASH_IOFF_LEN                 (8)
#define PERIPH_FLASH_IOFF_OFF                 (PERIPH_FLASH_FWREV_OFF + PERIPH_FLASH_FWREV_LEN)

#define PERIPH_FLASH_IGN_LEN                  (8)
#define PERIPH_FLASH_IGN_OFF                  (PERIPH_FLASH_IOFF_OFF + PERIPH_FLASH_IOFF_LEN)

#define PERIPH_FLASH_IGD_LEN                  (8)
#define PERIPH_FLASH_IGD_OFF                  (PERIPH_FLASH_IGN_OFF + PERIPH_FLASH_IGN_LEN)

#define PERIPH_FLASH_LEN                    (PERIPH_FLASH_RADIO_LEN + PERIPH_FLASH_FWREV_LEN + PERIPH_FLASH_IOFF_LEN + PERIPH_FLASH_IGN_LEN + PERIPH_FLASH_IGD_LEN)

/* ========================================================================== */



/* ========================================================================== */
/* DEFAULT VALUES                                                             */
/* -------------------------------------------------------------------------- */

#define PERIPH_FLASH_IOFF_0_DEF               (0)
#define PERIPH_FLASH_IOFF_1_DEF               (0)
#define PERIPH_FLASH_IOFF_2_DEF               (0)
#define PERIPH_FLASH_IOFF_3_DEF               (0)
#define PERIPH_FLASH_IOFF_4_DEF               (0)
#define PERIPH_FLASH_IOFF_5_DEF               (0)
#define PERIPH_FLASH_IOFF_6_DEF               (0)
#define PERIPH_FLASH_IOFF_7_DEF               (0)

#define PERIPH_FLASH_IGN_0_DEF                (1)
#define PERIPH_FLASH_IGN_1_DEF                (1)
#define PERIPH_FLASH_IGN_2_DEF                (1)
#define PERIPH_FLASH_IGN_3_DEF                (1)
#define PERIPH_FLASH_IGN_4_DEF                (1)
#define PERIPH_FLASH_IGN_5_DEF                (1)
#define PERIPH_FLASH_IGN_6_DEF                (1)
#define PERIPH_FLASH_IGN_7_DEF                (1)

#define PERIPH_FLASH_IGD_0_DEF                (1)
#define PERIPH_FLASH_IGD_1_DEF                (1)
#define PERIPH_FLASH_IGD_2_DEF                (1)
#define PERIPH_FLASH_IGD_3_DEF                (1)
#define PERIPH_FLASH_IGD_4_DEF                (1)
#define PERIPH_FLASH_IGD_5_DEF                (1)
#define PERIPH_FLASH_IGD_6_DEF                (1)
#define PERIPH_FLASH_IGD_7_DEF                (1)

/* ========================================================================== */

/* ========================================================================== */
/* PROTOTYPES */ 
/* -------------------------------------------------------------------------- */
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN) && !defined(Q_OS_LINUX)

void periph_PIO_Init( void );
void periph_PCK0_init(void);

// TIMER Functions
void periph_TIMER_Init ( void );
void periph_set_ms_ellapsed( uint64_t value );

// BATTERY / ACCELEROMETER FUNCTIONS
void periph_TWI_init( void );
void periph_TWI_test_IOExpander( void );
uint8_t periph_IMP_read (uint8_t ch_ind, uint8_t *volt_read, uint8_t *curr_read, uint8_t type);
uint8_t periph_ACCEL_start ( void );
uint8_t periph_ACCEL_stop ( void );
uint8_t periph_ACCEL_status ( void );
uint32_t periph_ACCEL_read ( uint8_t *data_vec );
void periph_BATT_read ( uint8_t *twi_data);
void periph_IOEXP_set_output(uint8_t ch_value);

// EEG-SPI Functions
void SpiMasterTransfer( void * pBuf, uint16_t size, void * pNextBuf, uint16_t nextSize );
void periph_eeg_drdy_handler( uint32_t id, uint32_t mask );

// RADIO Functions
void periph_USART_init( void );
uint8_t periph_USART_ReadBuffer(void *buffer, uint32_t size);
uint8_t periph_USART_WriteBuffer(void *buffer, uint32_t size);
uint16_t periph_usart_cpy ( uint8_t full_copy );
void periph_usart_send ( void ); // __attribute__((optimize("-O3")));
void periph_get_radio_state ( void );
void periph_config_radio ( void );
void periph_name_radio ( uint8_t *nbuff, uint8_t find, uint8_t lind);

// Flash Functions
void periph_load_stm_cal ( void );
void periph_update_flash( uint8_t* radio_address );
void periph_get_fwrev ( void );
void periph_update_cal ( uint8_t *nbuff, uint8_t find, uint8_t lind);
void periph_write_flash( uint32_t nPage, uint8_t* data_to_write );

// Stimulation Functions
void periph_BOOST_init(uint8_t boost_switch );

// LED Functions
void periph_LED_init ( void );
static inline void periph_LED_On( void ){
  gpio_set_pin_high(LED_PIN);
}
static inline void periph_LED_Off( void ){
  gpio_set_pin_low(LED_PIN);
}
static inline void periph_LED_Toggle( void ){
  gpio_toggle_pin(LED_PIN);
}
void periph_LED_enable_blinking( uint8_t value, uint64_t period );
void periph_LED_error_blink( uint8_t n_blink );


static inline void periph_wait_to_beacon_period_percentage_ellapsed( float percentage ){
  
  uint16_t tc0_counter = BEACON_PERIOD* ((float )percentage/100);
  while(tc_read_cv(TC0, 0) < tc0_counter );
}

static inline uint32_t periph_get_beacon_period_percentage_ellapsed( void ){
  
  return tc_read_cv(TC0, 0)/BEACON_PERIOD*100;
}

#endif
/* ========================================================================== */

#endif 

