/* ptpd_dep.h */
/* Exported functions from all PTP modules that are target specific 
 * system specific includes, and some misc system dependent macros
 */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (ptpd_dep.h) contains Modifications (updates, corrections      */
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

/** 
 * @file ptpd_dep.h 
 *
 * Exported functions from all PTP modules that are target specific 
 * system specific includes, and some misc system dependent macros
 *
 * @par Original Copyright
 * This file is a derivative work from ptpd_dep.h
 * Copyright (c) 2005-2007 Kendall Correll 
 *
 * @par Modifications and enhancements Copyright
 * Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights
 * reserved
 *
 * @par
 * This file (ptpd_dep.c) contains Modifications (updates, corrections      
 * comments and addition of initial support for IEEE 1588 version 1, IEEE 
 * version 2 and IEEE 802.1AS PTP) and other features by Alan K. Bartky.
 * 
 * @par License
 * These modifications and their associated software algorithms are under 
 * copyright and for this file are licensed under the terms of the GNU   
 * General Public License as published by the Free Software Foundation;   
 * either version 2 of the License, or (at your option) any later version.
 */
#ifndef PTPD_DEP_H
#define PTPD_DEP_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#ifndef __WINDOWS__
#include<unistd.h>
#endif

#include<errno.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>

#ifndef __WINDOWS__
#include<sys/time.h>
#include<sys/timex.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#endif

/* System configuration info */

#define MAX_PTP_PORTS 1

/* system messages */

// AKB 2010-09-11: Removed ERROR as it is used as a standard lib
// call for some C compilers.  Globally replaced "ERROR" with
// PERROR in the code
//#define ERROR(x, ...)  fprintf(stderr, "\n(ptp error)  " x, ##__VA_ARGS__)

#define PERROR(x, ...) fprintf(stderr, "\n(ptp error)  " x ": %m\n", ##__VA_ARGS__)
#define NOTIFY(x, ...) fprintf(stderr, "\n(ptp notice) " x, ##__VA_ARGS__)

/* debug messages */
/* DBGV: Debug Verbose high level of debug
 * DBGM: Debug Messages
 * DBG:  All other debug messages
 */
#ifdef PTPD_DBGV
#define PTPD_DBG

/** Macro to print out a "Verbose" debug messasge (i.e. high level detail) */
#define DBGV(x, ...) if ((debugLevel & 2) == 2 ) fprintf(stderr, "(ptp debugV) " x, ##__VA_ARGS__)

/** Macro to print out a "Message" debug message (i.e. relating to PTP data messages) */
#define DBGM(x, ...) if ((debugLevel & 4) == 4 ) fprintf(stderr, "(ptp debugM) " x, ##__VA_ARGS__)

#define DBGM_ENABLED
#define DBGV_ENABLED
#else
#define DBGV(x, ...)
#define DBGM(x, ...)
#endif

#ifdef PTPD_DBG
/** Macro to print out a "Normal" debug messasge (i.e. normal and unusual cases) */
#define DBG(x, ...)  if ((debugLevel & 1)==1)  fprintf(stderr, "(ptp debug)  " x, ##__VA_ARGS__)
#define DBG_ENABLED
extern int debugLevel;
#else
#define DBG(x, ...)
#endif

/* endian corrections */
#if defined(PTPD_MSBF)
#define shift8(x,y)   ( (x) << ((3-y)<<3) )
#define shift16(x,y)  ( (x) << ((1-y)<<4) )
#elif defined(PTPD_LSBF)
#define shift8(x,y)   ( (x) << ((y)<<3) )
#define shift16(x,y)  ( (x) << ((y)<<4) )
#endif

#define flip16(x) htons(x) /**< Macro for locally defined htons like function */
#define flip32(x) htonl(x) /**< Macro for locally defined htonl like function */

/* i don't know any target platforms that do not have htons and htonl,
   but here are generic funtions just in case */
/*
#if defined(PTPD_MSBF)
#define flip16(x) (x)
#define flip32(x) (x)
#elif defined(PTPD_LSBF)
static inline Integer16 flip16(Integer16 x)
{
   return (((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00);
}

static inline Integer32 flip32(x)
{
  return (((x) >> 24) & 0x000000ff) | (((x) >> 8 ) & 0x0000ff00) |
         (((x) << 8 ) & 0x00ff0000) | (((x) << 24) & 0xff000000);
}
#endif
*/

/* bit array manipulation */
#define getFlag(x,y)  !!( *(UInteger8*)((x)+((y)<8?1:0)) &   (1<<((y)<8?(y):(y)-8)) )
#define setFlag(x,y)    ( *(UInteger8*)((x)+((y)<8?1:0)) |=   1<<((y)<8?(y):(y)-8)  )
#define clearFlag(x,y)  ( *(UInteger8*)((x)+((y)<8?1:0)) &= ~(1<<((y)<8?(y):(y)-8)) )


/* msg.c */
Boolean    msgPeek                   (void*,ssize_t);
void       msgUnpackHeader           (void*,MsgHeader*);
void       msgUnpackSync             (void*,MsgSync*);
void       msgUnpackDelayReq         (void*,MsgDelayReq*);
void       msgUnpackFollowUp         (void*,MsgFollowUp*);
void       msgUnpackDelayResp        (void*,MsgDelayResp*);
void       msgUnpackManagement       (void*,MsgManagement*);
UInteger8  msgUnloadManagement       (void*,MsgManagement*,PtpClock*,RunTimeOpts*);
void       msgUnpackManagementPayload(void *buf, MsgManagement *manage);
void       msgPackHeader             (void*,PtpClock*);
void       msgPackSync               (void*,Boolean,TimeRepresentation*,PtpClock*);
void       msgPackDelayReq           (void*,Boolean,TimeRepresentation*,PtpClock*);
void       msgPackFollowUp           (void*,UInteger16,TimeRepresentation*,PtpClock*);
void       msgPackDelayResp          (void*,MsgHeader*,TimeRepresentation*,PtpClock*);
UInteger16 msgPackManagement         (void*,MsgManagement*,PtpClock*);
UInteger16 msgPackManagementResponse (void*,MsgHeader*,MsgManagement*,PtpClock*);

/* AKB added V2 functions in msg.c */
UInteger8 msgGetPtpVersion             (void *buf);
void      msgUnpackV2Header            (void *buf, V2MsgHeader             *header);
void      msgUnpackV2Sync              (void *buf, V2MsgSync               *sync);
void      msgUnpackAnnounce            (void *buf, MsgAnnounce             *announce);
void      msgUnpackV2FollowUp          (void *buf, V2MsgFollowUp           *follow);
void      msgUnpackV2DelayResp         (void *buf, V2MsgDelayResp          *resp);
void      msgUnpackV2PDelayResp        (void *buf, V2MsgPDelayResp         *resp);
void      msgUnpackV2PDelayRespFollowUp(void *buf, V2MsgPDelayRespFollowUp *resp);

void msgPackV2Header(void                     *buf, 
                     PtpClock                 *ptpClock
                    );
void msgPackAnnounce(void                     *buf, 
                     Boolean                   unicast,
                     V2TimeRepresentation     *originTimestamp,
                     PtpClock                 *ptpClock
                    );
void msgPackV2Sync(void                       *buf, 
                   Boolean                     unicast,
                   V2TimeRepresentation       *originTimestamp,
                   PtpClock                   *ptpClock
                  );
void msgPackV2DelayReq(void                   *buf, 
                       Boolean                 unicast,
                       V2TimeRepresentation   *originTimestamp,
                       PtpClock               *ptpClock
                      );
void msgPackV2PDelayReq(void                   *buf, 
                        Boolean                 unicast,
                        V2TimeRepresentation   *originTimestamp,
                        PtpClock               *ptpClock
                       );
void msgPackV2FollowUp(void                   *buf, 
                       Boolean                 unicast,
                       UInteger16              associatedSequenceId,
                       V2TimeRepresentation   *preciseOriginTimestamp,
                       PtpClock               *ptpClock
                      );

void msgPackV2DelayResp(void                  *buf, 
                        Boolean                unicast,
                        V2MsgHeader           *header,
                        V2TimeRepresentation  *delayReceiptTimestamp,
                        PtpClock              *ptpClock
                       );
void msgPackV2PDelayResp(void                 *buf, 
                         Boolean               unicast,
                         V2MsgHeader          *header,
                         V2TimeRepresentation *requestReceiptTimestamp,
                         PtpClock             *ptpClock
                        );

void msgPackV2PDelayRespFollowUp(void                 *buf, 
                                 Boolean               unicast,
                                 V2MsgHeader          *header,
                                 V2TimeRepresentation *responseOriginTimestamp,
                                 PtpClock             *ptpClock
                                );


/* net.c */
Boolean netInit         (NetPath*,RunTimeOpts*,PtpClock*);
Boolean netShutdown     (NetPath*);
int     netSelect       (TimeInternal*,NetPath*);
int     netSelectAll    (TimeInternal*, PtpClock*);           /* Added for multiple port support */
ssize_t netRecvEvent    (Octet*,TimeInternal*,NetPath*);
ssize_t netRecvGeneral  (Octet*,NetPath*);
ssize_t netRecvRaw      (Octet*,NetPath*);                    /* Added for 802.1AS support */
ssize_t netSendEvent    (Octet*,UInteger16,NetPath*,Boolean); /* Added Pdelay flag */
ssize_t netSendGeneral  (Octet*,UInteger16,NetPath*,Boolean); /* Added Pdelay flag */
ssize_t netSendRaw      (Octet*,UInteger16,NetPath*,Boolean); /* Added for 802.1AS and 1588 Annex F support */

/* servo.c */
void initClock(RunTimeOpts*,PtpClock*);

void updatePathDelay(one_way_delay_filter *owd_filt,  // one way delay filter
                     RunTimeOpts          *rtOpts,    // run time options
                     PtpClock             *ptpClock   // PTP main data structure
                    );

void updateDelay(TimeInternal *         send_time, // Delay Req. sent by slave time
                 TimeInternal *         recv_time, // Delay Req. received by master time
                 one_way_delay_filter * owd_filt,  // one way delay filter
                 RunTimeOpts *          rtOpts,    // run time options
                 PtpClock *             ptpClock   // PTP main data structure
                );

void updateOffset(TimeInternal *              send_time,  // Sync message reported Transmit time
                  TimeInternal *              recv_time,  // Sync message local    Receive  time
                  offset_from_master_filter * ofm_filt,   // Offset from Master filter
                  RunTimeOpts *               rtOpts,     // Run Time Options
                  PtpClock *                  ptpClock    // PTP main data structure
                 );

void updateClock(RunTimeOpts*,PtpClock*);

/* startup.c */
PtpClock * ptpdStartup(int,char**,Integer16*,RunTimeOpts*);
void ptpdShutdown(void);

/* sys.c */
void       displayStats(RunTimeOpts*,PtpClock*);
Boolean    nanoSleep(TimeInternal*);
void       getTime(TimeInternal*, Integer16); // AKB: added UTC offset
void       setTime(TimeInternal*, Integer16); // AKB: added UTC offset
UInteger16 getRand(UInteger32*);
Boolean    adjFreq(Integer32);
void       setPtpTimeFromSystem(Integer16);
void       setSystemTimeFromPtp(Integer16);

/* timer.c */
void       initTimer   (Integer32,UInteger32);  // AKB: Changed from (void) to add secs and usecs
void       timerUpdate (IntervalTimer*,int);    // AKB: Added port ID for multiple port support
void       timerStop   (UInteger16,IntervalTimer*);
void       timerStart  (UInteger16,UInteger16,IntervalTimer*);
Boolean    timerExpired(UInteger16,IntervalTimer*,int);// AKB: add port ID for multi port support

/* ledlib.c */
/* Function to manipulate LEDs on MPC8313ERDB board, could
 * be ported to other boards to indicate PTP status via LEDs
 */
void init_leds(void);

void all_leds     (unsigned char on_or_off);
void red_alarm    (unsigned char on_or_off);
void yellow_alarm (unsigned char on_or_off);
void green_alarm  (unsigned char on_or_off);

void led_meter(unsigned char value);


#endif

// eof ptpd_dep.h
