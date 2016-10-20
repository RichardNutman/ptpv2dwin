/* src/constants.h */
/* General constant definitions for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (constants.h) contains Modifications (updates, corrections     */
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

/****************************************************************************/
/* Additional licensing info by Richard Nutman                              */
/*                                                                          */
/* Original code base by Alan K. Bartky as above.                           */
/* Modified for Windows by Richard Nutman.                                  */
/*                                                                          */
/* These modifications are made public due to previous GNU General public   */
/* License as published by the Free Software Foundation.                    */
/*                                                                          */
/* These modifications have no specific additional licensing.               */
/* Software is provided as-is without any express or implied warranty       */
/*                                                                          */
/* richard.nutman@gmail.com / https://github.com/RichardNutman/ptpv2dwin    */
/****************************************************************************/


#ifndef CONSTANTS_H
#define CONSTANTS_H

/* implementation specific constants */
/* Manufacturer ID, 48 bytes long */
/* used in spec but not named */
#define MANUFACTURER_ID_LENGTH              48
#define MANUFACTURER_ID \
  "ptpv2d;licensed_under_GNU_Public_license_ver2.0\0\0"
// 12345678901234567890123456789012345678901234567 8
//          1         2         3         4

#define DEFAULT_SYNC_INTERVAL        0
#define DEFAULT_ANNOUNCE_INTERVAL    2  // AKB: Added for PTP V2
#define DEFAULT_UTC_OFFSET           36 /* V2: TAI = UTC plus 33 seconds as of 1/1/2006 */
#define DEFAULT_V1_CLOCK_VARIANCE    (-1000)  /* AKB: renamed for V2, changed for HW clock */
#define DEFAULT_V2_CLOCK_VARIANCE    ( 1000)  /* AKB: added   for V2, changed for HW clock */
#define DEFAULT_CLOCK_STRATUM        4
#define DEFAULT_INBOUND_LATENCY      0       /* in nsec */
#define DEFAULT_OUTBOUND_LATENCY     0       /* in nsec */
#define DEFAULT_NO_RESET_CLOCK       FALSE
#define DEFAULT_AP                   200       /* Changed for HW clock */
#define DEFAULT_AI                   5000      /* Changed for HW clock */
#define DEFAULT_DELAY_S              6
#define DEFAULT_MAX_FOREIGN_RECORDS  5

/* features, only change to refelect changes in implementation */
#define CLOCK_FOLLOWUP    TRUE
#define INITIALIZABLE     TRUE
#define BURST_ENABLED     FALSE
#define EXTERNAL_TIMING   FALSE
#define BOUNDARY_CLOCK    FALSE
#define NUMBER_PORTS      1
#define VERSION_PTP       1  // AKB: Changed to default version, added V1 and V2 values
#define VERSION1_PTP      1
#define VERSION2_PTP      2
#define VERSION_NETWORK   1

/* spec defined constants  */
#define DEFAULT_PTP_DOMAIN_NAME      "_DFLT\0\0\0\0\0\0\0\0\0\0\0" // Hex: 5F-44-46-4C-54
#define ALTERNATE_PTP_DOMAIN1_NAME   "_ALT1\0\0\0\0\0\0\0\0\0\0\0" // Hex: 5F-41-4C-54-31
#define ALTERNATE_PTP_DOMAIN2_NAME   "_ALT2\0\0\0\0\0\0\0\0\0\0\0" // Hex: 5F-41-4C-54-32
#define ALTERNATE_PTP_DOMAIN3_NAME   "_ALT3\0\0\0\0\0\0\0\0\0\0\0" // Hex: 5F-41-4C-54-33

#define IDENTIFIER_ATOM   "ATOM"  // Hex: 41-54-4F-4D
#define IDENTIFIER_GPS    "GPS\0" // Hex: 47-50-53-00
#define IDENTIFIER_NTP    "NTP\0" // Hex: 4E-54-50-00
#define IDENTIFIER_HAND   "HAND"  // Hex: 48-41-4E-44
#define IDENTIFIER_INIT   "INIT"  // Hex: 49-4E-49-54
#define IDENTIFIER_DFLT   "DFLT"  // Hex: 44-46-4C-54

/* ptp constants */
#define PTP_UUID_LENGTH                     6
#define PTP_CODE_STRING_LENGTH              4
#define PTP_SUBDOMAIN_NAME_LENGTH           16
#define PTP_MAX_MANAGEMENT_PAYLOAD_SIZE     90

/* no support for intervals less than one */
#define PTP_SYNC_INTERVAL_TIMEOUT(x) (1<<((x)<0?0:(x))) // AKB: changed from ?1:(x) to ?0
#define PTP_SYNC_RECEIPT_TIMEOUT(x)  (10*(1<<((x)<0?0:(x))))

#define PTP_DELAY_REQ_INTERVAL              4  // AKB: Changed from 30 to 4 for MPC8313E testing
#define PTP_FOREIGN_MASTER_THRESHOLD        2
#define PTP_FOREIGN_MASTER_TIME_WINDOW(x)   (4*(1<<((x)<0?0:(x))))
#define PTP_RANDOMIZING_SLOTS               18
#define PTP_LOG_VARIANCE_THRESHOLD          256
#define PTP_LOG_VARIANCE_HYSTERESIS         128

/* ptp data enums */
enum {
  PTP_CLOSED=0,         // 0
  PTP_ETHER,            // 1
  PTP_FFBUS=4,          // 4
  PTP_PROFIBUS,         // 5
  PTP_LON,              // 6
  PTP_DNET,             // 7
  PTP_SDS,              // 8
  PTP_CONTROLNET,       // 9
  PTP_CANOPEN,          // 10
  PTP_IEEE1394=243,     // 243
  PTP_IEEE802_11A,      // 244
  PTP_IEEE_WIRELESS,    // 245
  PTP_INFINIBAND,       // 246
  PTP_BLUETOOTH,        // 247
  PTP_IEEE802_15_1,     // 248
  PTP_IEEE1451_2,       // 249
  PTP_IEEE1451_5,       // 250
  PTP_USB,              // 251
  PTP_ISA,              // 252
  PTP_PCI,              // 253
  PTP_VXI,              // 254
  PTP_DEFAULT           // 255
};

enum {
  PTP_INITIALIZING=0,   // 0 
  PTP_FAULTY,           // 1
  PTP_DISABLED,         // 2
  PTP_LISTENING,        // 3
  PTP_PRE_MASTER,       // 4
  PTP_MASTER,           // 5
  PTP_PASSIVE,          // 6
  PTP_UNCALIBRATED,     // 7
  PTP_SLAVE             // 8
};

enum {
  PTP_SYNC_MESSAGE=0,         // 0
  PTP_DELAY_REQ_MESSAGE,      // 1 
  PTP_FOLLOWUP_MESSAGE,       // 2
  PTP_DELAY_RESP_MESSAGE,     // 3
  PTP_MANAGEMENT_MESSAGE,     // 4
  PTP_SYNC_MESSAGE_BURST,     // 5
 PTP_DELAY_REQ_MESSAGE_BURST  // 6
};


enum {
  PTP_LI_61=0,          // 0: Add leap second indication
  PTP_LI_59,            // 1: Subtract leap second indication
  PTP_BOUNDARY_CLOCK,   // 2
  PTP_ASSIST,           // 3
  PTP_EXT_SYNC,         // 4
  PARENT_STATS,         // 5
  PTP_SYNC_BURST        // 6
};

/* PTP Managment Messages: */
enum {
  PTP_MM_NULL=0,                                // 0
  PTP_MM_OBTAIN_IDENTITY,                       // 1
  PTP_MM_CLOCK_IDENTITY,                        // 2
  PTP_MM_INITIALIZE_CLOCK,                      // 3
  PTP_MM_SET_SUBDOMAIN,                         // 4
  PTP_MM_CLEAR_DESIGNATED_PREFERRED_MASTER,     // 5
  PTP_MM_SET_DESIGNATED_PREFERRED_MASTER,       // 6
  PTP_MM_GET_DEFAULT_DATA_SET,                  // 7
  PTP_MM_DEFAULT_DATA_SET,                      // 8
  PTP_MM_UPDATE_DEFAULT_DATA_SET,               // 9
  PTP_MM_GET_CURRENT_DATA_SET,                  // 10
  PTP_MM_CURRENT_DATA_SET,                      // 11
  PTP_MM_GET_PARENT_DATA_SET,                   // 12
  PTP_MM_PARENT_DATA_SET,                       // 13
  PTP_MM_GET_PORT_DATA_SET,                     // 14
  PTP_MM_PORT_DATA_SET,                         // 15
  PTP_MM_GET_GLOBAL_TIME_DATA_SET,              // 16
  PTP_MM_GLOBAL_TIME_DATA_SET,                  // 17
  PTP_MM_UPDATE_GLOBAL_TIME_PROPERTIES,         // 18
  PTP_MM_GOTO_FAULTY_STATE,                     // 19
  PTP_MM_GET_FOREIGN_DATA_SET,                  // 20
  PTP_MM_FOREIGN_DATA_SET,                      // 21
  PTP_MM_SET_SYNC_INTERVAL,                     // 22
  PTP_MM_DISABLE_PORT,                          // 23
  PTP_MM_ENABLE_PORT,                           // 24
  PTP_MM_DISABLE_BURST,                         // 25
  PTP_MM_ENABLE_BURST,                          // 26
  PTP_MM_SET_TIME,                              // 27
  PTP_MM_GET_PENDING_UPDATES,                   // 28
  PTP_MM_PENDING_UPDATES                        // 29
// Reserved: 30-127
// Implementation specific: 127-255 
};

/* enum used by this implementation */
enum {
  SYNC_RECEIPT_TIMER=0, 
  SYNC_INTERVAL_TIMER,
  ANNOUNCE_RECEIPT_TIMER,       // AKB: Added for V2
  ANNOUNCE_INTERVAL_TIMER,      // AKB: Added for V2
  PDELAY_INTERVAL_TIMER,        // AKB: Added for V2
  QUALIFICATION_TIMER,
  TIMER_ARRAY_SIZE               /* these two are non-spec */
};


/* AKB: V2 constants */
/* Event messages */
#define V2_SYNC_MESSAGE                 0x0
#define V2_DELAY_REQ_MESSAGE            0x1
#define V2_PDELAY_REQ_MESSAGE           0x2
#define V2_PDELAY_RESP_MESSAGE          0x3

/* Reserved 0x4-0x7 */

/* General messages */
#define V2_FOLLOWUP_MESSAGE             0x8
#define V2_DELAY_RESP_MESSAGE           0x9
#define V2_PDELAY_RESP_FOLLOWUP_MESSAGE 0xA
#define V2_ANNOUNCE_MESSAGE             0xB
#define V2_SIGNALING_MESSAGE            0xC
#define V2_MANAGEMENT_MESSAGE           0xD

/* Reserved 0x0E-0x0F */


/* Event messages */
#define V2_SYNC_LENGTH                 44
#define V2_DELAY_REQ_LENGTH            44
#define V2_PDELAY_REQ_LENGTH           54
#define V2_PDELAY_RESP_LENGTH          54


/* General messages */
#define V2_FOLLOWUP_LENGTH             44
#define V2_DELAY_RESP_LENGTH           54
#define V2_PDELAY_RESP_FOLLOWUP_LENGTH 54
#define V2_ANNOUNCE_LENGTH             64
#define V2_SIGNALING_LENGTH            44  /* Base length not including any TLVs */
#define V2_MANAGEMENT_LENGTH           48  /* Base length not including any TLVs */


/* Version 2 control field values */

#define V2_SYNC_CONTROL                 0x00
#define V2_DELAY_REQ_CONTROL            0x01
#define V2_FOLLOWUP_CONTROL             0x02
#define V2_DELAY_RESP_CONTROL           0x03
#define V2_MANAGEMENT_CONTROL           0x04
#define V2_ALL_OTHERS_CONTROL           0x05

/* Reserved 0x06 - 0xFF */

/* V2 PTP Header flags, 1st byte */

#define V2_ALTERNATE_MASTER_FLAG        0x01  // Bit 0
#define V2_TWO_STEP_FLAG                0x02  // Bit 1
#define V2_UNICAST_FLAG                 0x04  // Bit 2
/* bits 3 and 4 not defined */
#define V2_PTP_PROFILE_SPECIFIC_1_FLAG  0x20  // Bit 5
#define V2_PTP_PROFILE_SPECIFIC_2_FLAG  0x40  // Bit 6
#define V2_RESERVED_FLAG                0x80  // Bit 7


/* Announce message Flags (2nd byte) */

#define ANNOUNCE_LI_61                    0x01
#define ANNOUNCE_LI_59                    0x02
#define ANNOUNCE_CURRENT_UTC_OFFSET_VALID 0x04
#define ANNOUNCE_PTP_TIMESCALE            0x08
#define ANNOUNCE_TIME_TRACEABLE           0x10
#define ANNOUNCE_FREQUENCY_TRACEABLE      0x20

/* 13.3.2.11 logMessageInterval (Integer8)
   Table 24: Values of logMessageInterval field

   (defines for Fixed values of the logMeanMessageInterval field)
*/

#define LOGMEAN_UNICAST               0x7f  // For unicast SYNC, FOLLOWUP and DELAY_RESP
#define LOGMEAN_DELAY_REQ             0x7f
#define LOGMEAN_SIGNALING             0x7f
#define LOGMEAN_MANAGEMENT            0x7f
#define LOGMEAN_PDELAY_REQ            0x7f
#define LOGMEAN_PDELAY_RESP           0x7f
#define LOGMEAN_PDELAY_RESP_FOLLOWUP  0x7f


/* 7.6.2.6 timeSource Values */

#define TS_ATOMIC_CLOCK         0x10
#define TS_GPS                  0x20
#define TS_TERRESTRIAL_RADIO    0x30
#define TS_PTP                  0x40
#define TS_NTP                  0x50
#define TS_HAND_SET             0x60
#define TS_OTHER                0x90
#define TS_INTERNAL_OSCILLATOR  0xA0



#endif

// eof src/constants.h
