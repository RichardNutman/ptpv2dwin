/* src/dep/servo.c */
/* Clock servo (clock adjustment and tracking) algorithm rountines for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (servo.c) contains Modifications (updates, corrections         */
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
 * @file servo.c
 *
 * Clock servo (clock adjustment and tracking) algorithm rountines for PTP
 *
 * @par Original Copyright
 * This file is a derivative work from servo.c
 * Copyright (c) 2005-2007 Kendall Correll 
 *
 * @par Modifications and enhancements Copyright
 * Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights
 * reserved
 *
 * @par
 * This file (servo.c) contains Modifications (updates, corrections      
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

void initClockVars(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
  DBG("initClockVars:\n");
  
  /* clear vars */
  clearTime(&ptpClock->master_to_slave_delay);
  clearTime(&ptpClock->slave_to_master_delay);
  clearTime(&ptpClock->offset_from_master);   /* AKB: 9/18/2007 Clear offset from master */
  ptpClock->ofm_filt.y                        = 0;
  ptpClock->ofm_filt.nsec_prev                = -1; /* AKB: -1 used for non-valid nsec time */

  ptpClock->observed_v1_variance = 0;
  ptpClock->observed_drift       = 0;  /* clears clock servo accumulator (the I term) */
  ptpClock->owd_filt.s_exp       = 0;  /* clears one-way delay filter */
  ptpClock->halfEpoch            = ptpClock->halfEpoch || rtOpts->halfEpoch;
  rtOpts->halfEpoch              = 0;
}

void initClock(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
  DBG("initClock:\n");
  
  /* clear vars */

  initClockVars(rtOpts,ptpClock);
  
  /* level clock */

  if(!rtOpts->noAdjust)
  {
    ptpClock->baseAdjustValue = rtOpts->baseAdjustValue;
    if (rtOpts->rememberAdjustValue == TRUE)
    {
       ptpClock->baseAdjustValue += ptpClock->lastAdjustValue;
    }
    adjFreq(ptpClock->baseAdjustValue);
  }
}

void updatePathDelay(one_way_delay_filter *owd_filt,  /**< one way delay filter */
                     RunTimeOpts          *rtOpts,    /**< run time options */
                     PtpClock             *ptpClock   /**< PTP main data structure */
                    )
{
  Integer16 s;
  TimeInternal remote_time;
  
  
  DBGV("updatePathDelay:\n");

  DBGV(" t1 PDelay Req  Tx time %10.10ds.%9.9dns\n",
       ptpClock->t1_pdelay_req_tx_time.seconds,
       ptpClock->t1_pdelay_req_tx_time.nanoseconds
      );
  
  DBGV(" t2 PDelay Req  Rx time %10.10ds.%9.9dns\n",
       ptpClock->t2_pdelay_req_rx_time.seconds,
       ptpClock->t2_pdelay_req_rx_time.nanoseconds
      );

  DBGV(" t3 PDelay Resp Tx time %10.10ds.%9.9dns\n",
       ptpClock->t3_pdelay_resp_tx_time.seconds,
       ptpClock->t3_pdelay_resp_tx_time.nanoseconds
      );

  DBGV(" t4 PDelay Resp Rx time %10.10ds.%9.9dns\n",
       ptpClock->t4_pdelay_resp_rx_time.seconds,
       ptpClock->t4_pdelay_resp_rx_time.nanoseconds
      );

  DBGV(" PDelay Resp correction %10.10ds.%9.9dns\n",
       ptpClock->pdelay_resp_correction.seconds,
       ptpClock->pdelay_resp_correction.nanoseconds
      );

  DBGV(" PDelay Resp follow up  %10.10ds.%9.9dns\n",
       ptpClock->pdelay_followup_correction.seconds,
       ptpClock->pdelay_followup_correction.nanoseconds
      );

  /* calc 'slave_to_master_delay' */
  subTime(&ptpClock->one_way_delay,          // Result
          &ptpClock->t4_pdelay_resp_rx_time, // PDelay Response Receive time
          &ptpClock->t1_pdelay_req_tx_time   // minus PDelay Request Transmit time
         );

  DBGV(" (t4-t1)                %10.10ds.%9.9dns\n",
       ptpClock->one_way_delay.seconds,
       ptpClock->one_way_delay.nanoseconds
      );

  subTime(&remote_time,                      // Result
          &ptpClock->t3_pdelay_resp_tx_time, // PDelay Resp Transmit time (from responder)
          &ptpClock->t2_pdelay_req_rx_time   // minus PDelay Request Receive time (from responder)
         );

  DBGV(" (t3-t2)                %10.10ds.%9.9dns\n",
       remote_time.seconds,
       remote_time.nanoseconds
      );


  subTime(&ptpClock->one_way_delay,          // Result
          &ptpClock->one_way_delay,          // (T4-T1)
          &remote_time                       // minus (T3-T2)
         );

  DBGV(" (t4-t1)-(t3-t2)        %10.10ds.%9.9dns\n",
       ptpClock->one_way_delay.seconds,
       ptpClock->one_way_delay.nanoseconds
      );

  subTime(&ptpClock->one_way_delay,          // Result
          &ptpClock->one_way_delay,          // Current Calculation
          &ptpClock->pdelay_resp_correction // minus PDelay Resp Correction
         );

  DBGV(" minus 1st correction   %10.10ds.%9.9dns\n",
       ptpClock->one_way_delay.seconds,
       ptpClock->one_way_delay.nanoseconds
      );

  subTime(&ptpClock->one_way_delay,             // Result
          &ptpClock->one_way_delay,             // Current Calculation
          &ptpClock->pdelay_followup_correction // minus PDelay Resp Correction
         );

  DBGV(" minus 2nd correction   %10.10ds.%9.9dns\n",
       ptpClock->one_way_delay.seconds,
       ptpClock->one_way_delay.nanoseconds
      );

  halveTime(&ptpClock->one_way_delay);

  DBGV(" divided by 2           %10.10ds.%9.9dns\n",
       ptpClock->one_way_delay.seconds,
       ptpClock->one_way_delay.nanoseconds
      );


  copyTime( &ptpClock->slave_to_master_delay, // Destination
            &ptpClock->one_way_delay          // Source
          );

  clearTime(&ptpClock->t1_pdelay_req_tx_time);
  clearTime(&ptpClock->t2_pdelay_req_rx_time);
  clearTime(&ptpClock->t3_pdelay_resp_tx_time);
  clearTime(&ptpClock->t4_pdelay_resp_rx_time);
  clearTime(&ptpClock->pdelay_resp_correction);
  clearTime(&ptpClock->pdelay_followup_correction);
  clearTime(&ptpClock->t1_sync_delta_time);
  clearTime(&ptpClock->t2_sync_delta_time);  

  if(ptpClock->one_way_delay.seconds)       // Check if delay is larger than one second
  {

    /* Delay is larger than one second, clear s_exp and timestamp
     * of previously received sent time of Sync message (usually from 
     * preciseOriginTimestamp of follow up message) and return
     */
    DBG("updatePathDelay: One way delay seconds != 0\n");
    DBG("updatePathDelay: Clearing one way delay filter s_exp, nsec_prev\n");
    owd_filt->s_exp     = 0;
    owd_filt->nsec_prev = 0;
    return;
  }
  
  /* avoid overflowing filter */
  s =  rtOpts->s;
  while(abs(owd_filt->y)>>(31-s))
    --s;

  DBGV("updatePathDelay: rtOpts->s: %d, s:%d\n", 
       rtOpts->s,
       s
      );
  DBGV("updatePathDelay: current owd_filt->y: %d, s_exp: %d\n", 
       owd_filt->y,
       owd_filt->s_exp
      );  

  /* crank down filter cutoff by increasing 's_exp' */
  if(owd_filt->s_exp < 1)
    owd_filt->s_exp = 1;
  else if(owd_filt->s_exp < 1<<s)
    ++owd_filt->s_exp;
  else if(owd_filt->s_exp > 1<<s)
    owd_filt->s_exp = 1<<s;
  
  /* filter 'one_way_delay' */
  owd_filt->y = (owd_filt->s_exp-1)
                *owd_filt->y/owd_filt->s_exp 
              + (ptpClock->one_way_delay.nanoseconds/2 
                 + owd_filt->nsec_prev/2
                ) 
                /owd_filt->s_exp;

  /* Record previous one way delay nanosecond value
   * and update it with value calculated above
   */  
  owd_filt->nsec_prev = ptpClock->one_way_delay.nanoseconds;
  ptpClock->one_way_delay.nanoseconds = owd_filt->y;
  
  DBGV("updatePathDelay: delay filter y:%d, s_exp:%d\n",
       owd_filt->y,
       owd_filt->s_exp
      );
}

void updateDelay(TimeInternal *         send_time, /**< Delay Req. sent by slave time */
                 TimeInternal *         recv_time, /**< Delay Req. received by master time */
                 one_way_delay_filter * owd_filt,  /**< one way delay filter */
                 RunTimeOpts *          rtOpts,    /**< run time options */
                 PtpClock *             ptpClock   /**< PTP main data structure */
                )
{
  Integer16 s;
  
  DBGV("updateDelay:\n");
  
  /* calc 'slave_to_master_delay' */
  subTime(&ptpClock->slave_to_master_delay, // Result
          recv_time,                        // Send time
          send_time                         // minus Receive time
         );

  /* Correction for V2 Delay Resp (variable is zero if V1) */

  subTime(&ptpClock->slave_to_master_delay,
          &ptpClock->slave_to_master_delay,
          &ptpClock->delay_resp_correction
         );
  
  /* update 'one_way_delay' */
  addTime(&ptpClock->one_way_delay,         // Result (divided by 2 later)
          &ptpClock->master_to_slave_delay, // Master to slave delay (from sync/follow-up)
          &ptpClock->slave_to_master_delay  // Slave to master delay (from delay request/response)
         );

  halveTime(&ptpClock->one_way_delay);      // Divide by 2 to get one way delay
                                            // Assumes delay is symetrical
  
  if(ptpClock->one_way_delay.seconds)       // Check if delay is larger than one second
  {

    /* Delay is larger than one second, clear s_exp and timestamp
     * of previously received sent time of Sync message (usually from 
     * preciseOriginTimestamp of follow up message) and return
     */
    DBG("updateDelay: One way delay seconds != 0\n");
    DBG("updateDelay: Clearing one way delay filter s_exp, nsec_prev\n");
    owd_filt->s_exp = owd_filt->nsec_prev = 0;
    return;
  }
  
  /* avoid overflowing filter */
  s =  rtOpts->s;
  while(abs(owd_filt->y)>>(31-s))
    --s;

  DBGV("updateDelay: rtOpts->s: %d, s:%d\n", 
       rtOpts->s,
       s
      );
  DBGV("updateDelay: current owd_filt->y: %d, s_exp: %d\n", 
       owd_filt->y,
       owd_filt->s_exp
      );  

  /* crank down filter cutoff by increasing 's_exp' */
  if(owd_filt->s_exp < 1)
    owd_filt->s_exp = 1;
  else if(owd_filt->s_exp < 1<<s)
    ++owd_filt->s_exp;
  else if(owd_filt->s_exp > 1<<s)
    owd_filt->s_exp = 1<<s;
  
  /* filter 'one_way_delay' */
  owd_filt->y = (owd_filt->s_exp-1)
                *owd_filt->y/owd_filt->s_exp 
              + (ptpClock->one_way_delay.nanoseconds/2 
                 + owd_filt->nsec_prev/2
                ) 
                /owd_filt->s_exp;

  /* Record previous one way delay nanosecond value
   * and update it with value calculated above
   */  
  owd_filt->nsec_prev = ptpClock->one_way_delay.nanoseconds;
  ptpClock->one_way_delay.nanoseconds = owd_filt->y;
  
  DBGV("updateDelay: delay filter y:%d, s_exp:%d\n",
       owd_filt->y,
       owd_filt->s_exp
      );
}


/**
 * @fn updateOffset
 * Filter function for calculating Offset from Master
 * when running in PTP_SLAVE mode.
 */
void updateOffset(TimeInternal *              send_time,  /**< Sync message reported Transmit time */
                  TimeInternal *              recv_time,  /**< Sync message local    Receive  time */
                  offset_from_master_filter * ofm_filt,   /**< Offset from Master filter */
                  RunTimeOpts *               rtOpts,     /**< Run Time Options */
                  PtpClock *                  ptpClock    /**< PTP main data structure */
                 )
{
  DBGV("updateOffset:\n");
  
  /* calc 'master_to_slave_delay' */
  subTime(&ptpClock->master_to_slave_delay, // Result: Master to slave delay
          recv_time,                        // Recorded time of Sync message
          send_time                         // minus Send time of Sync message (from follow-up)
         );
  
  /* Update for V2 corrections (set to zero if received Sync/Follow-up is from V1 MASTER) */

  subTime(&ptpClock->master_to_slave_delay,
          &ptpClock->master_to_slave_delay,
          &ptpClock->sync_correction
         );

  subTime(&ptpClock->master_to_slave_delay,
          &ptpClock->master_to_slave_delay,
          &ptpClock->followup_correction
         );

  /* update 'offset_from_master' */
  subTime(&ptpClock->offset_from_master,    // Result: Offset from master
          &ptpClock->master_to_slave_delay, // From above calculation
          &ptpClock->one_way_delay          // minus one way delay calc from Delay Request/response
         );

  if(ptpClock->offset_from_master.seconds)
  {
    /* cannot filter with secs, clear filter */
    ofm_filt->nsec_prev = -1;  /* AKB: Use invalid number to make sure next calc correct */

#ifdef CONFIG_MPC831X
    /* set meter to max */
    led_meter(255);
#endif

    return;
  }
  
  /* filter 'offset_from_master' */
  /* 
   * Offset from Master Filtering 
   * this uses a simple two sample average:
   *
   * y[x] = x[n]/2 + x[n-1]/2
   *
   */

  if (ofm_filt->nsec_prev != -1)  /* AKB: Make sure previous timestamp is valid */
  {
     // Previous timestamp is valid, calculate new offset from master 
     // based on previous and current timestamps

     ofm_filt->y =   // Offset from master filter Y 
                     ptpClock->offset_from_master.nanoseconds/2  // current  / 2
                   + ofm_filt->nsec_prev/2;                      // previous / 2

     ofm_filt->nsec_prev = ptpClock->offset_from_master.nanoseconds;// Store current for next time

     ptpClock->offset_from_master.nanoseconds = ofm_filt->y;  // Set offset to current Y value

#ifdef CONFIG_MPC831X
     if (abs(ptpClock->offset_from_master.nanoseconds) > 255)
     {
       /* set meter to max */
       led_meter(255);
     }
     else
     {
       /* set meter based on nanoseconds between 0 and 255 absolute */
       led_meter(abs(ptpClock->offset_from_master.nanoseconds));
     }
#endif

  }
  else
  {

     // AKB: Previous timestamp is not valid, set filter Y to current value
     ofm_filt->y         = ptpClock->offset_from_master.nanoseconds; 
     ofm_filt->nsec_prev = ptpClock->offset_from_master.nanoseconds;
  }
  
  DBGV("updateOffset: offset filter y:%d\n", ofm_filt->y);
}

void updateClock(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
  Integer32    adj=0;
  TimeInternal timeTmpA;  // AKB: Added values for adjusting calc based on time to get time
  TimeInternal timeTmpB;
  TimeInternal timeTmpC;
  TimeInternal timeTmpD;
  TimeInternal timeTmpE;
  TimeInternal timeTmpF;
  Integer64    delta_time_calc;
  
  DBGV("updateClock:\n");
  
  if(ptpClock->offset_from_master.seconds)
  {
    /* if offset from master seconds is non-zero, then this is a "big jump:
     * in time.  Check Run Time options to see if we will reset the clock or 
     * set frequency adjustment to max to adjust the time
     */
    if(!rtOpts->noAdjust)
    {
      if(!rtOpts->noResetClock)
      {

        if (!isNonZeroTime(&ptpClock->t1_sync_delta_time))
        {
           // Delta time is zero, so this is the first sync to capture and we'll do the major
           // adjustment on the next sync instead of this one
           //
           // Store t1 and t2 times as current delta, next time we'll subtract
           //
           copyTime(&ptpClock->t1_sync_delta_time,
                    &ptpClock->t1_sync_tx_time
                   );
           copyTime(&ptpClock->t2_sync_delta_time,
                    &ptpClock->t2_sync_rx_time
                   );
           NOTIFY("updateClock: Storing current T1 and T2 values for later calc\n");
           DBG("updateClock: Storing T1: %10ds %11dns\n",
               ptpClock->t1_sync_delta_time.seconds,
               ptpClock->t1_sync_delta_time.nanoseconds
              );
           DBG("updateClock: Storing T2: %10ds %11dns\n",
               ptpClock->t2_sync_delta_time.seconds,
               ptpClock->t2_sync_delta_time.nanoseconds
              );
           return;
        }

        // If we are here then t1 and t2 sync delta were set to previous t1 and t2
        // values.  Now we calculate the deltas

           DBG("updateClock: Current T1: %10ds %11dns\n",
               ptpClock->t1_sync_tx_time.seconds,
               ptpClock->t1_sync_tx_time.nanoseconds
              );
           DBG("updateClock: Current T2: %10ds %11dns\n",
               ptpClock->t2_sync_rx_time.seconds,
               ptpClock->t2_sync_rx_time.nanoseconds
              );

        subTime(&ptpClock->t1_sync_delta_time,
                &ptpClock->t1_sync_tx_time,
                &ptpClock->t1_sync_delta_time
               ); 
        subTime(&ptpClock->t2_sync_delta_time,
                &ptpClock->t2_sync_rx_time,
                &ptpClock->t2_sync_delta_time
               );

           DBG("updateClock: Delta   T1: %10ds %11dns\n",
               ptpClock->t1_sync_delta_time.seconds,
               ptpClock->t1_sync_delta_time.nanoseconds
              );
           DBG("updateClock: Delta   T2: %10ds %11dns\n",
               ptpClock->t2_sync_delta_time.seconds,
               ptpClock->t2_sync_delta_time.nanoseconds
              ); 
       
        // Now we get the difference between the two time bases and store in the T2 time delta
        // as we will use the T1 time as the divisor (so master clock drives the time)

        subTime(&ptpClock->t2_sync_delta_time,
                &ptpClock->t2_sync_delta_time,
                &ptpClock->t1_sync_delta_time
               );

           DBG("updateClock: Delta T2 - Delta T1: %10ds %11dns\n",
               ptpClock->t2_sync_delta_time.seconds,
               ptpClock->t2_sync_delta_time.nanoseconds
              );  

        delta_time_calc =  getNanoseconds(&ptpClock->t2_sync_delta_time)
                           * 1000000000;
        delta_time_calc /= getNanoseconds(&ptpClock->t1_sync_delta_time);

           DBG("updateClock: Calculated Parts/billion: %d\n",
               (int)delta_time_calc
              );  


        /* clamp the accumulator to ADJ_FREQ_MAX for sanity */
        if(     delta_time_calc > ADJ_FREQ_MAX)
          adj =  ADJ_FREQ_MAX;
        else if(delta_time_calc < -ADJ_FREQ_MAX)
          adj = -ADJ_FREQ_MAX;
        else
          adj = (UInteger32)delta_time_calc;

        NOTIFY("updateClock: Initial clock adjust: %d, base: %d\n",
                adj, 
                ptpClock->baseAdjustValue
              );

        NOTIFY("updateClock: Offset from Master %ds.%9.9d seconds\n", 
                ptpClock->offset_from_master.seconds,
                ptpClock->offset_from_master.nanoseconds
              );
        DBG( "updateClock: offset_from_master seconds != 0\n");
        DBGV("  master-to-slave delay:   %10ds %11dns\n",
             ptpClock->master_to_slave_delay.seconds,
             ptpClock->master_to_slave_delay.nanoseconds
            );
        DBGV("  slave-to-master delay:   %10ds %11dns\n",
             ptpClock->slave_to_master_delay.seconds,
             ptpClock->slave_to_master_delay.nanoseconds
            );
        DBGV("  one-way delay:           %10ds %11dns\n",
             ptpClock->one_way_delay.seconds,
             ptpClock->one_way_delay.nanoseconds
            );
        DBG( "  offset from master:      %10ds %11dns\n",
             ptpClock->offset_from_master.seconds, 
             ptpClock->offset_from_master.nanoseconds
           );
        DBG( "  observed drift:          %10d\n", 
            ptpClock->observed_drift
           );

        getTime(&timeTmpA, ptpClock->current_utc_offset);   // Get current time #1

        getTime(&timeTmpB, ptpClock->current_utc_offset);   // Get current time #2

        subTime(&timeTmpC,    // Calculate time   #3, time elapsed between calls
                &timeTmpB,
                &timeTmpA
               );

        getTime(&timeTmpD, ptpClock->current_utc_offset);   // Get current time #4

        subTime(&timeTmpE,    // Subtract calculated offset from master
                &timeTmpD,
                &ptpClock->offset_from_master
               );

        addTime(&timeTmpF,    // Add calculated time to get timer value
                &timeTmpE,
                &timeTmpC
               );

        setTime(&timeTmpF, ptpClock->current_utc_offset);   // Set new PTP time

        DBGV(" get  Time A           :   %10ds %11dns\n",
             timeTmpA.seconds,
             timeTmpA.nanoseconds
            );
        DBGV(" get  Time B           :   %10ds %11dns\n",
             timeTmpB.seconds,
             timeTmpB.nanoseconds
            );
        DBGV(" calc Time C (B-A)     :   %10ds %11dns\n",
             timeTmpC.seconds,
             timeTmpC.nanoseconds
            );
        DBGV(" get  Time D           :   %10ds %11dns\n",
             timeTmpD.seconds,
             timeTmpD.nanoseconds
            );
        DBGV(" offset from master    :   %10ds %11dns\n",
             ptpClock->offset_from_master.seconds,
             ptpClock->offset_from_master.nanoseconds
            );
        DBGV(" calc Time E (D+offset):   %10ds %11dns\n",
             timeTmpE.seconds,
             timeTmpE.nanoseconds
            );
        DBGV(" calc Time F (E+C)     :   %10ds %11dns\n",
             timeTmpF.seconds,
             timeTmpF.nanoseconds
            );
        DBGV("updateClock: set time to Time F\n");

        // Initialize clock variables based on run time options (rtOpts)

        initClockVars(rtOpts, ptpClock);

        // Adjust clock based on calculation from Delta T1, T2 times

        adjFreq(ptpClock->baseAdjustValue - adj);

        // Set initial observed drift to this calculated value

        ptpClock->observed_drift = adj;

        DBG( "updateClock: after initClock:\n");
        DBGV("  master-to-slave delay:   %10ds %11dns\n",
             ptpClock->master_to_slave_delay.seconds,
             ptpClock->master_to_slave_delay.nanoseconds
            );
        DBGV("  slave-to-master delay:   %10ds %11dns\n",
             ptpClock->slave_to_master_delay.seconds,
             ptpClock->slave_to_master_delay.nanoseconds
            );
        DBG( "  one-way delay:           %10ds %11dns\n",
             ptpClock->one_way_delay.seconds,
             ptpClock->one_way_delay.nanoseconds
           );
        DBG( "  offset from master:      %10ds %11dns\n",
             ptpClock->offset_from_master.seconds,
             ptpClock->offset_from_master.nanoseconds
           );
        DBG( "  observed drift:          %10d\n",
             ptpClock->observed_drift
           );

      }
      else
      {
        /* Run time options indicate we can't reset the clock, so we slow
         * it down or speed it up based on ADJ_FREQ_MAX adjustment rather
         * than actually setting the time.
         */
        adj = ptpClock->offset_from_master.nanoseconds > 0 ? ADJ_FREQ_MAX : -ADJ_FREQ_MAX;
        adjFreq(ptpClock->baseAdjustValue - adj);
      }
    }
  }
  else
  {
    /* Offset from master is less than one second.  Use the the PI controller
     * to adjust the time 
     */

    DBGV("updateClock: using PI controller to update clock\n");

    /* no negative or zero attenuation */
    if(rtOpts->ap < 1)
     rtOpts->ap = 1;
    if(rtOpts->ai < 1)
      rtOpts->ai = 1;

    DBGV("  previous observed drift: %10d\n",
         ptpClock->observed_drift
        );
    DBGV("  run time opts P:         %10d\n",
         rtOpts->ap
        );
    DBGV("  run time opts I:         %10d\n",
         rtOpts->ai
        );
    
    DBGV("  current observed drift:  %d\n",
         ptpClock->observed_drift
        );


    DBGV("  current offset           %dns\n",
         rtOpts->ai
        );

    /* the accumulator for the I component */
    ptpClock->observed_drift += ptpClock->offset_from_master.nanoseconds/rtOpts->ai;
    
    DBGV("  new observed drift (I):  %d\n",
         ptpClock->observed_drift
        );

    /* clamp the accumulator to ADJ_FREQ_MAX for sanity */
    if(     ptpClock->observed_drift > ADJ_FREQ_MAX)
      ptpClock->observed_drift =  ADJ_FREQ_MAX;
    else if(ptpClock->observed_drift < -ADJ_FREQ_MAX)
      ptpClock->observed_drift = -ADJ_FREQ_MAX;

    DBGV("  clamped drift:           %d\n",
         ptpClock->observed_drift
        );
    
    adj = ptpClock->offset_from_master.nanoseconds/rtOpts->ap + ptpClock->observed_drift;

    DBGV("  calculated adjust:       %d\n",
         adj
        );
    
    DBGV("  base adjust:             %d\n",
         ptpClock->baseAdjustValue
        );

    /* apply controller output as a clock tick rate adjustment */
    if(!rtOpts->noAdjust)
    {
      DBGV("  calling adjFreq with:    %d\n",
           (ptpClock->baseAdjustValue-adj)
          );

      adjFreq(ptpClock->baseAdjustValue - adj);
      if (rtOpts->rememberAdjustValue == TRUE)
      {
         if (   ptpClock->offset_from_master.nanoseconds <= 100
             && ptpClock->offset_from_master.nanoseconds >= -100
            )
         {
           ptpClock->lastAdjustValue = -adj;  // Store value if it gave a good clock
                                              // result.
         }
      }
    }
  }
  
  /* Display statistics (save to a file if -f specified) if run time option enabled */
  if(rtOpts->displayStats)
    displayStats(rtOpts, ptpClock);
  
  DBGV("  offset from master:      %10ds %11dns\n",
       ptpClock->offset_from_master.seconds,
       ptpClock->offset_from_master.nanoseconds
      );
  DBGV("  master-to-slave delay:   %10ds %11dns\n",
       ptpClock->master_to_slave_delay.seconds,
       ptpClock->master_to_slave_delay.nanoseconds
      );
  DBGV("  slave-to-master delay:   %10ds %11dns\n",
       ptpClock->slave_to_master_delay.seconds,
       ptpClock->slave_to_master_delay.nanoseconds
      );
  DBGV("  one-way delay:           %10ds %11dns\n",
       ptpClock->one_way_delay.seconds,
       ptpClock->one_way_delay.nanoseconds
      );
  DBGV( "  current observed drift:  %10d\n",
       ptpClock->observed_drift
      );
  DBGV("  clock adjust value:      %10d\n",
       (ptpClock->baseAdjustValue - adj)
      );
}

// eof servo.c
