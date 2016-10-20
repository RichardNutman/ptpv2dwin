/* src/mpc831x.h */
/* Miscellaneous defines to support HW clock control and timestamping
 * for the Freescale MPC831x family of communication processors
 */

/* This code was originally posted for the sourceforge.net
 * BSD licensed ptpd project by Anup Gangwar
 * of Freescale Semiconductor Inc.  At the time of the original
 * posting, there was no copyright for this file.
 */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (mpc831x.c) contains Modifications (updates, corrections       */
/* comments and addition of initial support for IEEE 1588 version 1, IEEE   */
/* version 2 and IEEE 802.1AS PTP) and other features by Alan K. Bartky     */
/*                                                                          */
/* Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights      */
/* reserved.                                                                */
/*                                                                          */
/* These modifications and their associated software algorithms are under   */
/* copyright and for this file are licensed under the terms of the GNU      */
/* General Public License as published by the Free Software Foundation;     */
/* either version 2 of the License, or (at your option) any later version.  */
/*                                                                          */
/*     /\        This file and/or data from this file is copyrighted and    */
/*    /| \       is provided under a software license.                      */
/*   / | /\                                                                 */
/*  /__|/  \     This notice is to be included in all derivative works      */
/*  \  /\  /\                                                               */
/*   \/  \/  \   For copyright and alternate licensing information contact: */
/*    \  /\  /     Alan K. Bartky                                           */
/*     \/  \/      email: alan@bartky.net                                   */
/*      \  /       Web: http://www.bartky.net                               */
/*       \/                                                                 */
/*                                                                          */
/* End Alan K. Bartky additional copyright notice: Do not remove            */
/****************************************************************************/
#ifndef MPC831X_H
#define MPC831X_H


/* Constants */
#ifdef CONFIG_MPC8315E_RDB
/* Data for MPC8315E-RDB board: */
#define SOURCE1_FREQUENCY  133333333UL  // Period: 7.5 nanoseconds
#define SOURCE2_FREQUENCY   50000000UL  // Period: 25  nanoseconds

/* As Period for MPC8313X board is 7.5 for system clock, and the reference
 * frequency is not a true integer 166.6666... MHz to get more accurate
 * calculations, constants are provided for a fraction
 */

#define SOURCE1_NUMERATOR     400000000UL   // 133.333... MHz * 3
#define SOURCE1_DENOMINATOR           3UL
#define SOURCE1_MIN_PERIOD            8UL   // Miminum integer nanosecond period still adjustable

#define SOURCE2_NUMERATOR      50000000UL   // Straight 50.000... MHz
#define SOURCE2_DENOMINATOR           1UL
#define SOURCE2_MIN_PERIOD           21UL   // Miminum integer nanosecond period still adjustable

#else
/* Data for MPC8313E-RDB board: */
#define SOURCE1_FREQUENCY  166666666UL  // Period: 6 nanoseconds
#define SOURCE2_FREQUENCY   50000000UL  // Period: 25 nanoseconds

/* As Period for MPC8313X board is 7.5 for system clock, and the reference
 * frequency is not a true integer 166.6666... MHz to get more accurate
 * calculations, constants are provided for a fraction
 */

#define SOURCE1_NUMERATOR     500000000UL   // 166.666... MHz * 3
#define SOURCE1_DENOMINATOR           3UL
#define SOURCE1_MIN_PERIOD            7UL   // Miminum integer nanosecond period still adjustable

#define SOURCE2_NUMERATOR      50000000UL   // Straight 50.000... MHz
#define SOURCE2_DENOMINATOR           1UL
#define SOURCE2_MIN_PERIOD           21UL   // Miminum integer nanosecond period still adjustable
#endif


#include <linux/types.h>

/* Various IOCTL's supported by our driver */

/* Original set of IOCTLS prior to 2007-08-31:

 define PTP_GET_RX_TIMESTAMP     SIOCDEVPRIVATE
 define PTP_GET_TX_TIMESTAMP    (SIOCDEVPRIVATE + 1)
 define PTP_SET_CNT             (SIOCDEVPRIVATE + 2)
 define PTP_GET_CNT             (SIOCDEVPRIVATE + 3)
 define PTP_ADJ_FREQ            (SIOCDEVPRIVATE + 4)
 define PTP_ADJ_ADDEND          (SIOCDEVPRIVATE + 5)
 define PTP_SET_ADDEND          (SIOCDEVPRIVATE + 6) / / AKB: added for direct control from app
 define PTP_SET_PERIOD          (SIOCDEVPRIVATE + 7) / / AKB: added for direct control from app
*/

/* New IOCTLS from MPC8315E gianfar.h 

   IMPORTANT NOTE: the IOCTLS defined by the FREESCALE driver
   are not backwards nor forwards compatable as they were
   renumbered.  Make sure if you use this version of the PTP
   code, you use the corresponding kernel driver version
   from Freescale.  Otherwise who knows what damage you
   could do.

   Also the new Get timestamp IOCTLs all require that you pass
   the sequence number to verify you are getting the correct
   timestamp.  If not found, these IOCLTs return a time
   value of 0 seconds, 0 nanoseconds.
*/

#define PTP_GET_RX_TIMESTAMP_SYNC        SIOCDEVPRIVATE
#define PTP_GET_RX_TIMESTAMP_DEL_REQ    (SIOCDEVPRIVATE + 1)
#define PTP_GET_RX_TIMESTAMP_PDEL_REQ   (SIOCDEVPRIVATE + 2) // AKB: Changed for version 2
#define PTP_GET_RX_TIMESTAMP_PDEL_RESP  (SIOCDEVPRIVATE + 3) // AKB: Changed for version 2
#define PTP_GET_TX_TIMESTAMP            (SIOCDEVPRIVATE + 4)
#define PTP_SET_CNT                     (SIOCDEVPRIVATE + 5)
#define PTP_GET_CNT                     (SIOCDEVPRIVATE + 6)
#define PTP_ADJ_FREQ                    (SIOCDEVPRIVATE + 7)
#define PTP_ADJ_ADDEND                  (SIOCDEVPRIVATE + 8)
#define PTP_ADJ_ADDEND_IXXAT            (SIOCDEVPRIVATE + 9)
#define PTP_GET_ADDEND                  (SIOCDEVPRIVATE + 10)
#define PTP_SET_ALARM1                  (SIOCDEVPRIVATE + 11)
#define PTP_SET_FIPER1                  (SIOCDEVPRIVATE + 12)
#define PTP_SET_PERIOD                  (SIOCDEVPRIVATE + 13) // AKB: Added to change clock period

#define PTP_SET_ADDEND  PTP_ADJ_ADDEND_IXXAT    // AKB: Use new IOCTL for similar one I defined


struct mpc831x_cnt
{
	uint32_t	high;
	uint32_t	low;
};

/* Global variables containing interface and time add register information */

NetPath *  mpc831x_netPath;
char       mpc831x_if_name[IFACE_NAME_LENGTH];

UInteger32 base_timer_add_value;    // AKB: Base add value 
UInteger32 base_timer_adj_value;    // AKB: Base adjustment value
UInteger32 current_timer_add_value; // AKB: Current (last one sent to chip) add value

/* Exported functions */
void mpc831x_get_curr_time(TimeInternal *time);
void mpc831x_set_curr_time(TimeInternal *time);
void mpc831x_get_tx_time(NetPath *      tx_netPath,      // AKB: add netPath (for multi port)
                         TimeInternal * time,
                         uint32_t       sequence_number, // AKB: added sequence number check
                         uint32_t       message_type     // AKB: added message type
                        );
void mpc831x_get_rx_time(NetPath *      rx_netPath,      // AKB: add netPath (for multi port)
                         TimeInternal * time,
                         uint32_t       sequence_number, // AKB: added sequence number check
                         uint32_t       message_type     // AKB: added message type
                        );
void mpc831x_adj_freq(  Integer32  freq);
void mpc831x_adj_addend(Integer32  adj);
void mpc831x_set_addend(UInteger32 addend);
void mpc831x_set_period(UInteger32 period);
#endif //MPC831X_H
/* eof mpc831x.h */
