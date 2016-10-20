/* src/ptpv2d.c */
/* Main entry point for ptpv2d PTP daeamon */
/* This file is a derivative work from ptpd.c
 * Copyright (c) 2005-2007 Kendall Correll 
 */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (ptpv2d.c) contains Modifications (updates, corrections        */
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
#include "ptpd.h"

RunTimeOpts rtOpts;  /* statically allocated run-time configuration data */
#ifdef PTPD_DBG
int debugLevel; /* Global variable for enabling various debug printf statments */
#endif







int main(int argc, char **argv)
{
  PtpClock *ptpClock;
  Integer16 ret;
  
  /* initialize run-time options to reasonable values */ 
  memset(&rtOpts, 0, sizeof(rtOpts));

  rtOpts.syncInterval                = DEFAULT_SYNC_INTERVAL;
  rtOpts.announceInterval            = DEFAULT_ANNOUNCE_INTERVAL;  // AKB: Added for V2
  memcpy(rtOpts.subdomainName,
         DEFAULT_PTP_DOMAIN_NAME,
         PTP_SUBDOMAIN_NAME_LENGTH
        );
  memcpy(rtOpts.clockIdentifier,
         IDENTIFIER_DFLT,
         PTP_CODE_STRING_LENGTH
        );
  rtOpts.clockVariance               = DEFAULT_V1_CLOCK_VARIANCE;
  rtOpts.clockStratum                = DEFAULT_CLOCK_STRATUM;
  rtOpts.unicastAddress[0]           = 0;
  rtOpts.inboundLatency.nanoseconds  = DEFAULT_INBOUND_LATENCY;
  rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
  rtOpts.noResetClock                = DEFAULT_NO_RESET_CLOCK;
  rtOpts.s                           = DEFAULT_DELAY_S;
  rtOpts.ap                          = DEFAULT_AP;
  rtOpts.ai                          = DEFAULT_AI;
  rtOpts.max_foreign_records         = DEFAULT_MAX_FOREIGN_RECORDS;
  rtOpts.currentUtcOffset            = DEFAULT_UTC_OFFSET;
  rtOpts.ptp8021AS                   = FALSE;  // AKB: Added for 802.1AS (PTP over Ethernet)

#ifdef PTPD_DBG
  debugLevel = 0; /* Set all debug printing off unless requested by user */
#endif

#ifdef CONFIG_MPC831X
  //
  // For running on MPC831X (Freescale system with hardware timestamping):
  // Multiport protocol not fully suported yet, so force default to eth1 interface 
  // which is the best port to use on the MPC8313E-RDB board
  // NOTE: this interface can be overriden by the user at startup
  // time using the -b option when starting ptpv2d
  //
  memset( rtOpts.ifaceName, 0,      IFACE_NAME_LENGTH);
  strncpy((char * )rtOpts.ifaceName, "eth1", IFACE_NAME_LENGTH);
#endif

  if( !(ptpClock = ptpdStartup(argc, argv, &ret, &rtOpts)) )
  {
    // ptpdStartup did not return a pointer to the ptpClock structure,
    // something went wrong
    // Return code set by ptpdStartup function, pass
    // back to operating system.
    return ret;
  }

  // Check if Probe option selected (send one management message and then exit)
  // if not, then go ahead and start the PTP main code (protocol)
  
  if(rtOpts.probe)
  {
    probe(&rtOpts, ptpClock);
    ptpdShutdown();
    return 1;
  }
  else
  {
    if (rtOpts.ifaceName[0] == '\0')
    {
       /* do the multiple port protocol engine, if not bound to a single port */
       multiPortProtocol(&rtOpts, ptpClock);
       NOTIFY("main: self shutdown, probably due to an error\n");
    }
    else
    {
       /* do the single port protocol engine */
       protocol(&rtOpts, ptpClock);
       NOTIFY("main: self shutdown, probably due to an error\n");
    }
  }
  
  ptpdShutdown();
  
  return 1;
}

// eof ptpv2d.c

