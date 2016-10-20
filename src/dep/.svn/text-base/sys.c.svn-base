/* src/dep/sys.c */
/* Misc. system dependent functions, mainly time related and also contains
 * function to display statistics in printable format or for output 
 * in .csv format for export to spreadsheet programs
 */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (sys.c) contains Modifications (updates, corrections           */
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

/*
 * @file sys.c
 *
 * Misc. system dependent functions, mainly time related and also contains
 * function to display statistics in printable format or for output 
 * in .csv format for export to spreadsheet programs
 *
 * @par Original Copyright
 * This file is a derivative work from sys.c
 * Copyright (c) 2005-2007 Kendall Correll 
 *
 * @par Modifications and enhancements Copyright
 * Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights
 * reserved
 *
 * @par
 * This file (sys.c) contains Modifications (updates, corrections      
 * comments and addition of initial support for IEEE 1588 version 1, IEEE 
 * version 2 and IEEE 802.1AS PTP) and other features by Alan K. Bartky.
 * 
 * @par License
 * These modifications and their associated software algorithms are under 
 * copyright and for this file are licensed under the terms of the GNU   
 * General Public License as published by the Free Software Foundation;   
 * either version 2 of the License, or (at your option) any later version.
 */
#include "../ptpd.h"
#ifdef CONFIG_MPC831X
#include "../mpc831x.h"
#endif

#if defined(__WINDOWS__)
/* AKB: Windows C compiler does not include getopt */
#include "getopt.h"
#endif

/** Function to display ptpv2d statistics */
void displayStats(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
  static int  start = 1;
  static char sbuf[SCREEN_BUFSZ]; /* AKB NOTE: make sure SCRREN_BUFSZ is larger than SCREEN_MAXSZ */
  char *      s;
  int         len = 0;
  
  if(start && rtOpts->csvStats)
  {
    start = 0;
#ifdef CONFIG_MPC831X
    printf("state, one way delay, offset from master, drift, timer add value, sync send time, sync receive time, master to slave delay\n");
#else
    printf("state, one way delay, offset from master, drift, sync send time, sync receive time, master to slave delay\n");
#endif
    fflush(stdout);
  }
  
  memset(sbuf, ' ', SCREEN_BUFSZ);
  
  switch(ptpClock->port_state)
  {
  case PTP_INITIALIZING:  s = "init";  break;
  case PTP_FAULTY:        s = "flt ";  break;
  case PTP_LISTENING:     s = "lstn";  break;
  case PTP_PASSIVE:       s = "pass";  break;
  case PTP_UNCALIBRATED:  s = "uncl";  break;
  case PTP_SLAVE:         s = "slv ";  break;
  case PTP_PRE_MASTER:    s = "pmst";  break;
  case PTP_MASTER:        s = "mst ";  break;
  case PTP_DISABLED:      s = "dsbl";  break;
  default:                s = "?   ";  break;
  }
  
  if (!rtOpts->csvStats)
  {
    len += sprintf(sbuf + len, "\rstate: ");
  }

  len += sprintf(sbuf + len, "%s", s);
  
  if(ptpClock->port_state == PTP_SLAVE)
  {

    len += sprintf(sbuf + len,
                   ", %s%s%d.%09d",
                   rtOpts->csvStats ? "" : "owd: ",
                   (ptpClock->one_way_delay.nanoseconds < 0) ? "-" : " ",
                   ptpClock->one_way_delay.seconds,
                   abs(ptpClock->one_way_delay.nanoseconds)
                  );

    len += sprintf(sbuf + len,
                   ", %s%s%d.%09d",
                   rtOpts->csvStats ? "" : "ofm: ",
                   (ptpClock->offset_from_master.nanoseconds < 0) ? "-" : " ",
                   ptpClock->offset_from_master.seconds,
                   abs(ptpClock->offset_from_master.nanoseconds)
                  );
    
    len += sprintf(sbuf + len, 
                   ", %s%d",
                   rtOpts->csvStats ? "" : "drift: ",
                   ptpClock->observed_drift
                  );

/* Variance not supported, comment statistic out
    len += sprintf(sbuf + len, 
                   ", %s%d",
                   rtOpts->csvStats ? "" : "var: ",
                   ptpClock->observed_v1_variance
                  );
*/


#ifdef CONFIG_MPC831X
    len += sprintf(sbuf + len,
                   ", %s%10.10u",
                   rtOpts->csvStats ? "" : "clock_add: ",
                   current_timer_add_value
                  );
#endif

    // AKB: Added additional stats for Sync Transmit and one way delay


    len += sprintf(sbuf + len,
                   ", %s%s%d.%09d",
                   rtOpts->csvStats ? "" : "sst: ",
                   (ptpClock->t1_sync_tx_time.nanoseconds < 0) ? "-" : " ",
                   ptpClock->t1_sync_tx_time.seconds,
                   abs(ptpClock->t1_sync_tx_time.nanoseconds)
                  );

    len += sprintf(sbuf + len,
                   ", %s%s%d.%09d",
                   rtOpts->csvStats ? "" : "srt: ",
                   (ptpClock->t2_sync_rx_time.nanoseconds < 0) ? "-" : " ",
                   ptpClock->t2_sync_rx_time.seconds,
                   abs(ptpClock->t2_sync_rx_time.nanoseconds)
                  );

    len += sprintf(sbuf + len,
                   ", %s%s%d.%09d",
                   rtOpts->csvStats ? "" : "msd: ",
                   (ptpClock->master_to_slave_delay.nanoseconds < 0) ? "-" : " ",
                   ptpClock->master_to_slave_delay.seconds,
                   abs(ptpClock->master_to_slave_delay.nanoseconds)
                  );


  }

  if (rtOpts->csvStats)
  {
    len += sprintf(sbuf + len, "\n");
  }

#ifdef __WINDOWS__
  /* Windows compiler doesn't have write function in stdio.h
   * Using printf instead.  Note statistics print will be as
   * long as the string created
   */
  if (!(rtOpts->csvStats))
  {
      sbuf[SCREEN_MAXSZ+1] = 0;  /* Put a NULL character to force max size */
  }
  printf(sbuf);

#else
  write(1, sbuf, rtOpts->csvStats ? len : SCREEN_MAXSZ + 1);
#endif
}

/** 
 * Function to sleep for a user specified nubmer of nanoseconds 
 * This function converts to local system time capabilities
 * as necessary (example, convert nanoseconds to microseconds,
 * milliseconds, etc. depending on system timer resolution
 */
Boolean nanoSleep(TimeInternal *t)
{
#ifdef __WINDOWS__
    /* Windows basic Sleep timer only supports milliseconds
     * Convert Seconds and nanoseconds to milliseconds
     * NOTE: Also this always returns TRUE
     */
    DWORD milliseconds;

    milliseconds = (t->seconds * 1000) + (t->nanoseconds / 1000000000);
    if (milliseconds == 0)
    {
        milliseconds = 1;
    }
    Sleep(milliseconds);
#else
  struct timespec ts, tr;
  
  ts.tv_sec  = t->seconds;
  ts.tv_nsec = t->nanoseconds;
  
  if(nanosleep(&ts, &tr) < 0)
  {
    t->seconds     = tr.tv_sec;
    t->nanoseconds = tr.tv_nsec;
    return FALSE;
  }
#endif  
  return TRUE;
}

/** 
 * Function to take the time from the system and use it to set the
 * PTP current time.
 *
 * @note This function is currently only used in systems with 
 * hardware timestamping and is used to set the hardware
 * timestamping clock based on the current system time (linux, windows, etc.)
 * as a base time to start with.
 *
 * @param[in]  utc_offset Integer16 value of number of UTC 
 *             leap seconds since January 1, 1970
 */
void setPtpTimeFromSystem(Integer16 utc_offset)
{

#ifdef CONFIG_MPC831X
  struct timeval tv;
  TimeInternal   time;

  gettimeofday(&tv, 0);

  time.seconds     =  tv.tv_sec;
  time.nanoseconds =  tv.tv_usec*1000;

  /* PTP uses TAI, gettime of day is UTC, so adjust */
  time.seconds     += utc_offset;

  mpc831x_set_curr_time(&time);

  NOTIFY("setPtpTimeFromSystem to TAI: %ds %dns\n",
         time.seconds,
         time.nanoseconds
        );
#endif
}

/** 
 * Function to take the time from PTP and use it to set the
 * system's time of day clock.
 *
 * @param[in]  utc_offset Integer16 value of number of UTC 
 *             leap seconds since January 1, 1970
 */
void setSystemTimeFromPtp(Integer16 utc_offset)
{
#ifdef CONFIG_MPC831X
  struct timeval tv;
  TimeInternal   time;

  mpc831x_get_curr_time(&time);
  
  tv.tv_sec  =  time.seconds;
  tv.tv_usec =  time.nanoseconds/1000;

  /* PTP uses TAI, gettime of day is UTC, so adjust */
  tv.tv_sec  -= utc_offset;

  settimeofday(&tv, 0);

  NOTIFY("setSystemTimeFromPtp to TAI: %ds %dns\n",
         time.seconds,
         time.nanoseconds
        );
#endif
}

/** 
 * Function to get time from the system or specific
 * hardware PTP timer (based on system capabilities
 * and architecture) and return it as ptpv2d system
 * internal time format
 *
 * @param[out] time       Pointer to TimeInternal structure to write time in seconds and nanoseconds TAI time
 * @param[in]  utc_offset Integer16 value of number of UTC leap seconds since January 1, 1970
 */
void getTime(TimeInternal *time, Integer16 utc_offset)
{
#ifdef CONFIG_MPC831X
  mpc831x_get_curr_time(time);
#elif defined(__WINDOWS__)
    unsigned long long U64Seconds;
    unsigned long      Nanoseconds;
    FILETIME           WindowsUTCTime;

    GetSystemTimeAsFileTime(&WindowsUTCTime);            // Gets the current system time

    // We now have the time based on 100 nanosecond increments since
    // January 1, 1601.
    // We need to change that to seconds and nanoseconds since
    // January 1, 1970 to get to Linux time format
    //

    // Get system time into a unsigned 64 bit time in 100 ns increments

    // Get Windows current high time
    U64Seconds =    (unsigned long long) WindowsUTCTime.dwHighDateTime;

    // Left shift into most signficant DWORD;
    U64Seconds <<=  32;

    // Get Windows current low time
    U64Seconds +=   (unsigned long long) WindowsUTCTime.dwLowDateTime;

    // Mod by 10,000 to get sub seconds (in 100 ns increments)
    Nanoseconds =   (unsigned long)(U64Seconds % 10000ULL);

    // Subtract out sub seconds
    U64Seconds -=   (unsigned long long) Nanoseconds;

    // Convert sub seconds in 100 ns increments to nanoseconds
    Nanoseconds *=  100U;

    // Convert 100 ns increments to seconds
    U64Seconds /=   10000ULL;    

    // Subtract absolute number of seconds between 
    // January 1, 1970 and January 1, 1601
    U64Seconds -=   1164447360ULL;  

    // Copy calculated seconds and nanoseconds to 
    // internal time structure (seconds and nanoseconds
    // since January 1, 1970
    time->seconds     = (unsigned long) U64Seconds;
    time->nanoseconds = Nanoseconds;
#else
  /* Not windows time, system time is in Linux format */
  struct timeval tv;
  
  gettimeofday(&tv, 0);

  time->seconds     =  tv.tv_sec;
  time->nanoseconds =  tv.tv_usec*1000;
#endif

  /* PTP uses TAI time (time without leap seconds
   * since January 1, 1970), gettime of day is UTC
   * (which includes leap seconds), so adjust for leap
   * seconds since January 1, 1970
   */
  time->seconds     += utc_offset;
}


/* AKB: 1/31/08, Changed set time to set both the MPC8313 HW clock and also the
 * Linux time of day clock 
 */
/** 
 * Function to set time to the system or specific
 * hardware PTP timer (based on system capabilities
 * and architecture) base on ptpv2d system
 * internal time format
 *
 * @param[in]  time        Pointer to TimeInternal structure with time in seconds and nanoseconds TAI time
 * @param[in]  utc_offset Integer16 value of number of UTC leap seconds since January 1, 1970
 */
void setTime(TimeInternal *time, Integer16 utc_offset)
{
#ifdef CONFIG_MPC831X
  mpc831x_set_curr_time(time);
#elif defined(__WINDOWS__)
    unsigned long long U64WindowsTime;
    FILETIME           WindowsUTCTime;
    SYSTEMTIME         WindowsSystemTime;


    U64WindowsTime = (unsigned long long) time->seconds;
    
    // Change epoch in seconds from January 1, 1970
    // to Windows January 1, 1901

    U64WindowsTime += 1164447360ULL;

    // Change seconds from January 1, 19701 UTC time to TAI time
    
    U64WindowsTime  -= (unsigned long long) utc_offset;

    // Convert seconds to 100 ns increments since January 1, 1601

    U64WindowsTime *= 10000ULL;

    // Add in nanoseconds converted to 100 ns increments

    U64WindowsTime += (unsigned long long)(time->nanoseconds / 100U);

    // Convert unsigned long long 64 bit variable to
    // File time structure time

    WindowsUTCTime.dwLowDateTime  = (unsigned long)(U64WindowsTime & (unsigned long long)0xFFFFFFFF);
    WindowsUTCTime.dwHighDateTime = (unsigned long)(U64WindowsTime >> 32);

    // Convert Windows UTC "file time" to Windows UTC "system time"

    FileTimeToSystemTime(&WindowsUTCTime, &WindowsSystemTime);

    // Now finally we can set the windows system time
    SetSystemTime(&WindowsSystemTime); // Sets the current system time

#else
  // Linux time type, simply need to convert nanoseconds
  // to microseconds, subtract the TAI time UTC offset
  // (leap seconds since January 1, 1970) and set
  // the time
  struct timeval tv;
  
  tv.tv_sec  = time->seconds;
  tv.tv_usec = time->nanoseconds/1000;

  /* PTP uses TAI, gettime of day is UTC, so adjust */
  tv.tv_sec  -= utc_offset;
  settimeofday(&tv, 0);
#endif  

  NOTIFY("setTime: resetting clock to TAI %ds %dns\n", time->seconds, time->nanoseconds);
}

/** 
 * Function to map generic need for a 16 bit unsigned random integer to
 * appropriate system random function/library
 */
UInteger16 getRand(UInteger32 *seed)
{
#ifdef __WINDOWS__
    srand(*seed);
    return((UInteger16)rand());
#else
  return rand_r((unsigned int*)seed);
#endif
}

short temp_debug_max_adjustments=0;

Boolean adjFreq(Integer32 adj)
{
#ifndef __WINDOWS__
#ifndef CONFIG_MPC831X
  struct timex t;
#endif
#endif
  
  if(adj > ADJ_FREQ_MAX)
    adj = ADJ_FREQ_MAX;
  else if(adj < -ADJ_FREQ_MAX)
    adj = -ADJ_FREQ_MAX;
  
#ifdef CONFIG_MPC831X
  if (++temp_debug_max_adjustments < 10000)
    mpc831x_adj_addend(adj);
  return (TRUE);
#elif defined(__WINDOWS__)
  return(!( SetSystemTimeAdjustment((adj/100),FALSE)));
#else
  t.modes = MOD_FREQUENCY;
  t.freq = adj*((1<<16)/1000);

  return !adjtimex(&t);
#endif
}

// eof sys.c

