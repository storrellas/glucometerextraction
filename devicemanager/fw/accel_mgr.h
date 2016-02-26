/*===========================================================================*/
/* design unit  : StarStim:  ACCEL Manager                                   */
/*                                                                           */
/* file name    : accel_mgr.h                                                */
/*                                                                           */
/* purpose      : Constants for ACCEL Manager module                         */
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

#ifndef _ACCEL_MGR_H_ 
#define _ACCEL_MGR_H_


/* Starlab Libraries */
#include "periph_mgr.h"

#define ACCEL_SUCCESS         0
#define ACCEL_FAIL            1

#define ACCEL_COORDINATE_0    0 
#define ACCEL_COORDINATE_1    1
#define ACCEL_COORDINATE_2    2

/* ========================================================================== */
/* PROTOTYPES */ 
/* -------------------------------------------------------------------------- */
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN) && !defined(Q_OS_LINUX)
uint8_t accel_init( void );
uint8_t accel_is_enabled( void );
//void accel_write( void );
uint8_t accel_get_coordinate(uint8_t index, uint8_t* msb, uint8_t* lsb);
uint8_t accel_get_data(uint8_t* buffer, uint16_t *length);

uint8_t accel_read( void );
uint8_t static inline accel_start( void ){
  return periph_ACCEL_start();
}
uint8_t static inline accel_stop( void ){
  return periph_ACCEL_stop();
}
#endif
/* ========================================================================== */



/* Total number of configuration Registers */
#define ACCEL_NUM_REGS                   (512) //(64)


/* Number of Accel data words */
#define ACCEL_NUM_WORDS                    (3)

/* Bytes per word*/
#define ACCEL_BYTES_PER_WORD               (2)


/* ========================================================================== */
/* ACCEL INTERNAL REGISTERS                                                   */
/* -------------------------------------------------------------------------- */

// Offset for the addresses pointing to ACCEL REGS
#define ACCEL_REGS_OFFSET                (0x2000)

/** ACCEL_MODE_ADDR: Indicates the mode of operation of the ACCEL manager. 
 *                   '0' indicates that ACCEL is OFF
 *                   '1' indicates that ACCEL is being read and reported to the host */
#define ACCEL_MODE_ADDR               (0x00)


/** ACCEL_THR_TAP_ADDR */
#define ACCEL_THR_TAP_ADDR            (0x01)

/** OFSX */
#define ACCEL_OFSX_ADDR               (0x02)

/** OFSY */
#define ACCEL_OFSY_ADDR               (0x03)

/** OFSZ */
#define ACCEL_OFSZ_ADDR               (0x04)

/** DUR */
#define ACCEL_DUR_ADDR                (0x05)

/** LAT */
#define ACCEL_LAT_ADDR                (0x06)

/** WIN */
#define ACCEL_WIN_ADDR                (0x07)

/** THR_ACT */
#define ACCEL_THR_ACT_ADDR            (0x08)

/** THR_INACT */
#define ACCEL_THR_INACT_ADDR          (0x09)

/** TIME_INACT */
#define ACCEL_TIME_INACT_ADDR         (0x0A)

/** ACT_INACT_CTL */
#define ACCEL_ACT_INACT_CTL_ADDR      (0x0B)

/** THR_FF */
#define ACCEL_THR_FF_ADDR             (0x0C)

/** TIME_FF */
#define ACCEL_TIME_FF_ADDR            (0x0D)

/** TAP_AXES */
#define ACCEL_TAP_AXES_ADDR           (0x0F)

/** ACT_TAP_ST */
#define ACCEL_ACT_TAP_ST_ADDR         (0x10)

/** BW_RATE */
#define ACCEL_BW_RATE_ADDR            (0x11)

/** POWER_CTL */
#define ACCEL_POWER_CTL_ADDR          (0x12)

/** INT_ENABLE */
#define ACCEL_INT_ENABLE_ADDR         (0x13)

/** INT_MAP */
#define ACCEL_INT_MAP_ADDR            (0x14)

/** INT_SRC */
#define ACCEL_INT_SRC_ADDR            (0x15)

/** DATA_FORMAT */
#define ACCEL_DATA_FORMAT_ADDR        (0x16)

/** DATAX0 */
#define ACCEL_DATAX0_ADDR             (0x17)

/** DATAX1 */
#define ACCEL_DATAX1_ADDR             (0x18)

/** DATAY0 */
#define ACCEL_DATAY0_ADDR             (0x19)

/** DATAY1 */
#define ACCEL_DATAY1_ADDR             (0x20)

/** DATAZ0 */
#define ACCEL_DATAZ0_ADDR             (0x21)

/** DATAZ1 */
#define ACCEL_DATAZ1_ADDR             (0x22)

/** FIFO_CTL */
#define ACCEL_FIFO_CTL_ADDR           (0x23)

/** FIFO_ST */
#define ACCEL_FIFO_ST_ADDR            (0x24)



/* ========================================================================== */


/* ========================================================================== */
/* OPERATION REGISTERS                                                        */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */



/* ========================================================================== */
/* DEFAULT VALUES                                                             */
/* -------------------------------------------------------------------------- */

/** Default values for EEG Internal Registers */

#define ACCEL_THR_TAP_DEF             (0x00)
#define ACCEL_OFSX_DEF                (0x00)
#define ACCEL_OFSY_DEF                (0x00)
#define ACCEL_OFSZ_DEF                (0x00)
#define ACCEL_DUR_DEF                 (0x00)
#define ACCEL_LAT_DEF                 (0x00)
#define ACCEL_WIN_DEF                 (0x00)
#define ACCEL_THR_ACT_DEF             (0x00)
#define ACCEL_THR_INACT_DEF           (0x00)
#define ACCEL_TIME_INACT_DEF          (0x00)
#define ACCEL_ACT_INACT_CTL_DEF       (0x00)
#define ACCEL_THR_FF_DEF              (0x00)
#define ACCEL_TIME_FF_DEF             (0x00)
#define ACCEL_TAP_AXES_DEF            (0x00)
#define ACCEL_ACT_TAP_ST_DEF          (0x00)
#define ACCEL_BW_RATE_DEF             (0x08)
#define ACCEL_POWER_CTL_DEF           (0x00)
#define ACCEL_INT_ENABLE_DEF          (0x00)
#define ACCEL_INT_MAP_DEF             (0x00)
#define ACCEL_INT_SRC_DEF             (0x02)
#define ACCEL_DATA_FORMAT_DEF         (0x00)
#define ACCEL_DATAX0_DEF              (0x00)
#define ACCEL_DATAX1_DEF              (0x00)
#define ACCEL_DATAY0_DEF              (0x00)
#define ACCEL_DATAY1_DEF              (0x00)
#define ACCEL_DATAZ0_DEF              (0x00)
#define ACCEL_DATAZ1_DEF              (0x00)
#define ACCEL_FIFO_CTL_DEF            (0x00)
#define ACCEL_FIFO_ST_DEF             (0x00)




/* ========================================================================== */


#endif 

