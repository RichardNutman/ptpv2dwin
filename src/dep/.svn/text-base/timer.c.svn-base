/* sys/dep/timer.c */
/* System dependent Functions to handle PTP timers (such as protocol timers
 * to map them to system timer functions
 */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (timer.c) contains Modifications (updates, corrections         */
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
 * @file timer.c 
 *
 * System dependent Functions to handle PTP timers (such as protocol timers)
 * to map them to system timer functions
 *
 * @par Original Copyright
 * This file is a derivative work from timer.c
 * Copyright (c) 2005-2007 Kendall Correll 
 *
 * @par Modifications and enhancements Copyright
 * Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights
 * reserved
 *
 * @par
 * This file (timer.c) contains Modifications (updates, corrections      
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

/**
 * TIMER_INTERVAL is based on number of "TICKS" as set
 * by initTimer function where currently 1 Tick
 * maps to the seconds and microseconds value passed
 * when calling initTimer
 */
#define TIMER_INTERVAL 1

/* Timer global variables */

#ifdef __WINDOWS__
HANDLE TimerQueue;
#endif

/** Elaspsed time (allocated one integer per PTP port) */
int elapsed[MAX_PTP_PORTS];

#ifdef LIMIT_RUNTIME
/* Variables to allow for limited run time (i.e. terminate the
 * system after a certain amount of time since the daemon
 * was started
 */
int max_ticks=14400; /*14400 4 hours in seconds (60 *60 * 4) */ 
int license_tick_counter = 0;
#endif

/* catch_alarm:
 * Simple function to be called by the operating
 * system on a periodic basis, update the
 * private timer array table and return
 */
#ifdef __WINDOWS__
VOID CALLBACK catch_alarm(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
#else
void catch_alarm(int sig)
#endif
{
  int i;

  for (i=0; i<MAX_PTP_PORTS; i++)
  {
    elapsed[i] += TIMER_INTERVAL;
  }
  // NOTE: If you put in a debug print here, it will interleave with other messages
  // from the normal execution task.  So if you need to put one in, put it
  // in to verify it, but remove it once you verify timeout is occurring OK.
#ifdef TEMPORARY_DEBUG_TIMER_TICK
#ifdef PTPD_DBG
  if (debugLevel >= 1) 
  {
    DBGV("\n(***tick %u***)\n",elapsed[0]);
    fflush(NULL);
  }
#endif
#endif

  
  /*  fprintf(stderr, "tick: %d, %d\n",max_ticks,license_tick_counter); */

#ifdef LIMIT_RUNTIME
 /* Check Limit run timer */
 if (++license_tick_counter > max_ticks)
 {
    fprintf(stderr, "\nptpv2d: Evaluation license maximum run time reached, terminating!\n"); 
    raise(SIGTERM); // Time up, send a terminate signal to the ptpv2d task
 }
 /* End run time limit timer*/
#endif

}

/** Function to initialize the global timer for ptpv2d
 * timer functions
 */
void initTimer(Integer32  seconds,
               UInteger32 microseconds
              )
{
#ifdef __WINDOWS__
  BOOL success;
#else
  struct itimerval itimer;
  int i;  
#endif

  DBG("initTimer: %d seconds, %u microseconds\n",
      seconds,
      microseconds
     );
#ifdef __WINDOWS__
    success = CreateTimerQueueTimer
       (
        &TimerQueue,            // Handle for timer
        NULL,                   // NULL for default timer queue
        catch_alarm,            // Callback function (NOTE: must be fast)
        NULL,                   // Pointer to data for callback function
        0,                      // Time (ms) before timer signaled for the first time
        ( (seconds*1000)        // Interval time in milliseconds
         +(microseconds/1000)
        ),
        WT_EXECUTEINIOTHREAD
        //WT_EXECUTEINTIMERTHREAD // Run in timer thread (callback function must be quick
       );
    if (!success)
        DBG("Failed to created windows timer\n");
#else
  //
  // Not Windows code (e.g. Linux)
  //
  // Set Alarm clock signal to ignore
  signal(SIGALRM, SIG_IGN);

  //
  // Clear elapsed time for all ports
  //
  for (i=0; i<MAX_PTP_PORTS; i++)
  {
      elapsed[i] = 0;
  }

#ifdef LIMIT_RUNTIME
  // Adjust run time limit timer if timer less than one second per tick
  if (seconds == 0)
  {
    max_ticks *= (1000000/microseconds);
  }
#endif

  // Set interval timer values  
  itimer.it_value.tv_sec  = itimer.it_interval.tv_sec  = seconds;
  itimer.it_value.tv_usec = itimer.it_interval.tv_usec = microseconds;
  
  // Set Alarm clock signal to catch_alarm function above
  signal(SIGALRM, catch_alarm);

  // Set and start interval timer
  setitimer(ITIMER_REAL,  // Use real time clock (do not use time of day)
            &itimer,      // Pointer to new itimerval structure 
            0             // Pointer to old structure (none in this case)
           );  
#endif
}

void timerUpdate(IntervalTimer *itimer, int port_id)
{
  int i, delta;
  
  i = port_id -1;
  delta   = elapsed[i];
  elapsed[i] = 0;
  
  if(delta <= 0)
    return;
  
  for(i = 0; i < TIMER_ARRAY_SIZE; ++i)
  {
    if(itimer[i].interval > 0 && (itimer[i].left -= delta) <= 0)
    {
      itimer[i].left   = itimer[i].interval;
      itimer[i].expire = TRUE;
      DBGV("timerUpdate: timer index %u expired (interval=%d delta=%d)\n",
           i,
           itimer[i].interval,
           delta
          );
    }
  }
}

void timerStop(UInteger16 index, IntervalTimer *itimer)
{
  if(index >= TIMER_ARRAY_SIZE)
    return;
  
  itimer[index].interval = 0;
  DBGV("timerStop: timer index %u stopped\n", index);
}

void timerStart(UInteger16 index, UInteger16 interval, IntervalTimer *itimer)
{
  if(index >= TIMER_ARRAY_SIZE)
    return;
  
  itimer[index].expire   = FALSE;
  itimer[index].left     = interval;
  itimer[index].interval = itimer[index].left;
  
  DBGV("timerStart: set timer index %d to %d\n", index, interval);
}

Boolean timerExpired(UInteger16 index, IntervalTimer *itimer, int port_id)
{
  DBGV("timerExpired: Checking index %d\n", index);
  timerUpdate(itimer, port_id);
  
  if(index >= TIMER_ARRAY_SIZE)
    return FALSE;                // ERROR:index out of range, return false
  
  if(!itimer[index].expire)
    return FALSE;                // Timer not expired, return FALSE
  
  DBGV("timerExpired: timer index %d has expired\n", index);
  itimer[index].expire = FALSE;  // Clear expired flag
  return TRUE;                   // Return TRUE
}

// eof timer.c
