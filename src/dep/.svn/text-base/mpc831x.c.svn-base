/* src/mpc831x.c */
/* Miscellaneous functions to support HW clock control and timestamping
 * for the Freescale MPC831x family of communication processors
 */

/* This code was originally posted for the sourceforge.net
 * BSD licensed ptpd project by Anup Gangwar
 * of Freescale Semiconductor Inc.  At the time of posting, there 
 * was no copyright for this file.
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

/* AKB: NOTE: This code is currently in the main "src" directory.  At some
 * point in the future, this should be moved to the "dep" directories
 * as all of this code is system dependent.  For initial publication
 * as open source, leaving this "as is" for now.
 */


#include "ptpd.h"
#include "mpc831x.h"
#include <unistd.h>

// AKB: Updated 2007-09-28 to add set addend and set period
// AKB: Updated 2007-11-08 added multiport support (pass netpath as parameter for 
//                         calls for tx and rx timestamps) 

#define NSEC		((unsigned long long)1000000000)

#define	BOARD_TIME_TO_SEC(board_time)	\
	((uint32_t)(board_time/NSEC))
	
#define BOARD_TIME_TO_NSEC(board_time, board_time_sec)\
	((uint32_t)(board_time - (((unsigned long long)board_time_sec) * NSEC)))

void mpc831x_get_curr_time(
                           TimeInternal * time
                          )
{

//
// Note: Avoid putting in debug print statements in this routine
// unless you are doing very basic debugging.  This is because
// typically this function is used for calculating time and
// adding printing will skew those calculations as it takes
// time to do the print to console, even when buffered
//

	struct ifreq 		if_data;
	struct mpc831x_cnt	hw_cnt;
	unsigned long long	board_time = 0;

	if_data.ifr_data = (void *)(&hw_cnt);
	memcpy(if_data.ifr_name, 
               mpc831x_netPath->ifName, 
               IFACE_NAME_LENGTH
              );
	if(!ioctl(mpc831x_netPath->eventSock, PTP_GET_CNT, &if_data))
	{
		board_time = hw_cnt.high;
		board_time = (board_time << 32) | hw_cnt.low;
		//DBG( "mpc831x_get_curr_time board_time = %llu\n", board_time );

		time->seconds     = BOARD_TIME_TO_SEC( board_time);
		time->nanoseconds = BOARD_TIME_TO_NSEC(board_time, time->seconds);
		// DBG( "mpc831x_get_curr_time %9.9d.%9.9d\n",
		//	time->seconds, time->nanoseconds );
	} 
	else
	{
		perror( "Error in mpc831x_get_curr_time()" );
		time->seconds     = 0;
		time->nanoseconds = 0;
	}
}

void mpc831x_set_curr_time(TimeInternal *time)
{
	struct ifreq 		if_data;
	struct mpc831x_cnt	hw_cnt;
	unsigned long long	board_time;

	board_time = ((unsigned long long)time->seconds) 
                   * ((unsigned long long)1000000000)
		   + ((unsigned long long)time->nanoseconds);

	board_time = board_time/1000;
	board_time = board_time*1000;

	hw_cnt.high = (board_time >> 32) & 0xffffffff;
	hw_cnt.low  = (board_time)       & 0xffffffff;

	if_data.ifr_data = (void *)(&hw_cnt);
	memcpy(if_data.ifr_name, 
               mpc831x_netPath->ifName, 
               IFACE_NAME_LENGTH
              );
	if(!ioctl(mpc831x_netPath->eventSock, PTP_SET_CNT, &if_data))
	{
        DBG( "mpc831x_set_curr_time board_time = %llu Hex (0x%llx)\n",
             board_time, board_time );
		DBG( "mpc831x_set_curr_time %9.9d.%9.9d\n",
		 	time->seconds, time->nanoseconds );
	} 
	else
	{
		perror( "Error in mpc831x_set_curr_time()" );
	}
}

void mpc831x_get_tx_time(NetPath      *tx_netPath, 
                         TimeInternal *time,
                         uint32_t      sequence_number,
                         uint32_t      message_type
                        )
{
	struct ifreq 		if_data;
	struct mpc831x_cnt	hw_cnt;
	unsigned long long	board_time = 0;


	/* updated IOCTL requires least significant 8 bits of the sequence number 
     * and the message type are passed to it in the ifr_data
	 * structure, and it returns in that structure the 64 bit time
	 * from the chip.
	 */

        DBGV("mpc831x_get_tx_time: message: %u, sequence: %u\n",
              message_type,
              sequence_number
            );

	hw_cnt.high = (sequence_number & 0xFF) | (message_type << 8);
	hw_cnt.low  = hw_cnt.high;  // Just in case of Endianess issues.

	if_data.ifr_data = (void *)(&hw_cnt);
	memcpy(if_data.ifr_name, 
               tx_netPath->ifName, 
               IFACE_NAME_LENGTH
              );
        DBGV("mpc831x_get_tx_time: interface: %s, socket: %u\n",
              if_data.ifr_name,
              tx_netPath->eventSock
            );

        DBGV("mpc831x_get_tx_time: message & sequence data: 0x%8.8x\n",
              hw_cnt.high
            );

	if(!ioctl(tx_netPath->eventSock, PTP_GET_TX_TIMESTAMP, &if_data))
	{
		board_time = hw_cnt.high;
                board_time = (board_time << 32) | hw_cnt.low;
                //DBG( "mpc831x_get_tx_time board_time = %llu\n", board_time );
		
		time->seconds     = BOARD_TIME_TO_SEC(board_time);
		time->nanoseconds = BOARD_TIME_TO_NSEC(board_time, time->seconds);
		DBGV( "mpc831x_get_tx_time: %9.9d.%9.9d sequence: %d\n", \
			time->seconds, time->nanoseconds, sequence_number );
	} 
	else
	{
		perror( "Error in mpc831x_get_tx_time()" );
		time->seconds     = 0;
		time->nanoseconds = 0;
	}
}

void mpc831x_get_rx_time(NetPath *      rx_netPath, 
                         TimeInternal * time,
                         uint32_t       sequence_number,
                         uint32_t       message_type
                        )
{
	struct ifreq 		if_data;
	struct mpc831x_cnt	hw_cnt;
	unsigned long long	board_time;
	int                     ioctl_command;

	/* IOCTL requires sequence number to be passed to it in the ifr_data
	 * structure, and it returns in that structure the 64 bit time
	 * from the chip.
	 */
        DBGV("mpc831x_get_rx_time: message: %u, sequence: %u\n",
              message_type,
              sequence_number
            );

	hw_cnt.high = sequence_number;
	hw_cnt.low  = sequence_number;  // Just in case of Endianess issues.

	switch (message_type)
	{
		case PTP_SYNC_MESSAGE:
			ioctl_command = PTP_GET_RX_TIMESTAMP_SYNC;
			break;

		case PTP_DELAY_REQ_MESSAGE:
			ioctl_command = PTP_GET_RX_TIMESTAMP_DEL_REQ;
			break;

		case V2_PDELAY_REQ_MESSAGE:
			ioctl_command = PTP_GET_RX_TIMESTAMP_PDEL_REQ;
			break;

		case V2_PDELAY_RESP_MESSAGE:
			ioctl_command = PTP_GET_RX_TIMESTAMP_PDEL_RESP;
			break;

		default:
			DBG("mpc831x_get_rx_time: unsupported message %d\n",
			    message_type
			   );
			time->seconds     = 0;
			time->nanoseconds = 0;
			return;
	}

	if_data.ifr_data = (void *)(&hw_cnt);
	memcpy(if_data.ifr_name, 
               rx_netPath->ifName, 
               IFACE_NAME_LENGTH
              );

        DBGV("mpc831x_get_rx_time: interface: %s, socket: %u\n",
              if_data.ifr_name,
              rx_netPath->eventSock
            );
        DBGV("mpc831x_get_rx_time: ioctl command: %u, data: 0x%8.8x\n",
              ioctl_command,
              hw_cnt.high
            );

	if(!ioctl(rx_netPath->eventSock, ioctl_command, &if_data))
	{
		board_time = hw_cnt.high;
                board_time = (board_time << 32) | hw_cnt.low;
                DBGV( "mpc831x_get_rx_time: board_time = %llu\n", board_time );

		time->seconds     = BOARD_TIME_TO_SEC( board_time);
		time->nanoseconds = BOARD_TIME_TO_NSEC(board_time, time->seconds);
		DBGV( "mpc831x_get_rx_time: %9.9d.%9.9d\n", \
		       time->seconds, time->nanoseconds );
	} 
	else
	{
		PERROR( "Error in mpc831x_get_rx_time()" );
		time->seconds     = 0;
		time->nanoseconds = 0;
	}
}

void mpc831x_adj_freq(Integer32 freq)
{
 	struct ifreq    if_data;

	if_data.ifr_data = (void *)(&freq);
	memcpy(if_data.ifr_name, 
               mpc831x_netPath->ifName, 
               IFACE_NAME_LENGTH
              );
	if( !ioctl(mpc831x_netPath->eventSock, PTP_ADJ_FREQ, &if_data) )
	{
		DBGV( "mpc831x_adj_freq: 0x%8.8x (%d)\n", freq, freq );
	}
	else
	{
		perror( "Error in mpc831x_adj_freq" );
	}
}

void mpc831x_set_addend(UInteger32 addend)
{
 	struct ifreq    if_data;

	if (addend == current_timer_add_value)
	{
		DBGV("mpc831x_set_addend: same addend 0x%8.8x (%u), returning\n",
		     addend,
		     addend
		    );
		return;
	}

	if_data.ifr_data = (void *)(&addend);
	strncpy(if_data.ifr_name, 
                mpc831x_netPath->ifName, 
                IFACE_NAME_LENGTH
              );
	if( !ioctl(mpc831x_netPath->eventSock, PTP_SET_ADDEND, &if_data) )
	{
		DBGV( "mpc831x_set_addend: 0x%8.8x (%u)\n",
		     addend,
		     addend
		    );
		current_timer_add_value = addend;
	}
	else
	{
		perror( "Error in mpc831x_set_addend" );
	}
}

void mpc831x_adj_addend(Integer32 adj)
{
	/* Original code from Freescale
 	struct ifreq    if_data;

	if_data.ifr_data = (void *)(&adj);
	memcpy(if_data.ifr_name, mpc831x_if_name, IFACE_NAME_LENGTH);
	if( !ioctl(mpc831x_netPath->eventSock, PTP_ADJ_ADDEND, &if_data) )
	{
		DBG( "mpc831x_adj_addend: 0x%8.8x (%d)\n", adj, adj );
	}
	else
	{
		perror( "Error in mpc831x_adj_addend" );
	}

	*/

	// AKB: New code to do ajustments here to be able to change algorithms
	// without recompiling the kernel and due the control from here:
	// For initial testing keep it simple (will determine later after testing
	// if other adjustment are appropriate)

	UInteger32      timer_add_value;
	UInteger32      new_adjustment_value=0;
	Boolean         add_operation;

	DBGV("mpc831x_adj_addend: parameter passed  0x%8.8x (%d)\n", adj, adj);

	timer_add_value = base_timer_add_value;

	if (adj < 0)
	{
		adj = -(adj);
		add_operation = FALSE;
	}
	else
	{
		add_operation = TRUE;
	}
 
	if (adj != 0)
	{
		new_adjustment_value = 
	                      (adj * base_timer_adj_value) // Multiply by base adjustment value
	                      >> 10;                       // then divide by 1024
	                                                   // to get parts per billion adjustment
		if (add_operation == TRUE)
		{
			timer_add_value += new_adjustment_value;
		}
		else
		{
			timer_add_value -= new_adjustment_value;
		}
	}

	DBGV("mpc831x_adj_addend: base add value    0x%8.8x (%u)\n",
	     base_timer_add_value,
	     base_timer_add_value
	    );
	DBGV("mpc831x_adj_addend: base adjust value 0x%8.8x (%u)\n",
	     base_timer_adj_value,
	     base_timer_adj_value
	    );
	DBGV("mpc831x_adj_addend: %s       0x%8.8x (%u)\n",
	     (add_operation == TRUE) ? "add by     " : "subtract by",
	     new_adjustment_value,
	     new_adjustment_value
	    );
	DBGV("mpc831x_adj_addend: new add value     0x%8.8x (%u)\n",
	     timer_add_value,
	     timer_add_value
	    );

	mpc831x_set_addend( 
                           timer_add_value
                          );  // Use set addend function to do the adjustment
}

void mpc831x_set_period( 
                                 UInteger32 period
                       )
{
#define TWO_32ND_TIMES_BILLION   4294967296000000000ULL
 	struct ifreq       if_data;
	UInteger32         a, b, min_period, timer_add_value=0;
        unsigned long long x, y;

#ifdef CONFIG_GFAR_PTP_VCO
	a = SOURCE2_NUMERATOR;    // VCO clock
        b = SOURCE2_DENOMINATOR;
        min_period = SOURCE2_MIN_PERIOD;
	DBGV("mpc831x_set_period: SOURCE2, requested period %uns\n",period);
#else 
	a = SOURCE1_NUMERATOR;    // System clock
        b = SOURCE1_DENOMINATOR;
        min_period = SOURCE1_MIN_PERIOD;
	DBGV("mpc831x_set_period: SOURCE1, requested period %uns\n",period);
#endif

        if (period < min_period)
	{
		DBGV("mpc831x_set_period: requested period less than minimum %uns\n",
		     min_period
		    );
		period = min_period;
	}

	if (b==1)
	{
		x = TWO_32ND_TIMES_BILLION;
	}
	else
	{
		x = TWO_32ND_TIMES_BILLION * b;
	}

	y = (unsigned long long)a * period;

	timer_add_value = x/y;

	DBGV( "mpc831x_set_period: period %u nanoseconds\n", period);
	DBGV( "mpc831x_set_period: a         = 0x%8.8X (%u)\n", a, a);
	DBGV( "mpc831x_set_period: b         = 0x%8.8X (%u)\n", b, b);
	DBGV( "mpc831x_set_period: x = 0x%16.16llX (%llu)\n", x, x);
	DBGV( "mpc831x_set_period: y = 0x%16.16llX (%llu)\n", y, y);
	DBGV( "mpc831x_set_period: add value = 0x%8.8X (%u)\n",
               timer_add_value,
               timer_add_value
            );

	if_data.ifr_data = (void *)(&period);
	memcpy(if_data.ifr_name, 
               mpc831x_netPath->ifName, 
               IFACE_NAME_LENGTH
              );
	if( !ioctl(mpc831x_netPath->eventSock, PTP_SET_PERIOD, &if_data) )
	{
		DBGV( "mpc831x_set_period: period set as 0x%8.8X (%u)\n", period, period);
	}
	else
	{
		perror( "Error in PTP_SET_PERIOD ioctl" );
		return;
	}

        mpc831x_set_addend(timer_add_value);

	// Set global variables for later use.

	base_timer_add_value = timer_add_value; // Base timer add register for HW timer

	// For base adjustment register, the target we are trying to 
	// achieve is one part per billion adjustment for call to 
	// mpc831x_adj_timer (this is what the ptpv2d daemon uses as
	// an adjustment resolution which is then mapped to the timer
	// capabilities (SW or HW)).
	//
	// Here we take advantage of the fine grain changes we can
	// do in the MPC831X chip.
	//
	// To get the adjustment base value we divide now by 
	// one billion divided by 1024, so later we can just do
	// a right shift by 10 operation to get out Parts per
	// billion calculation there (so we don't need to
	// have 64 bit divides being done all the time).

	base_timer_adj_value = timer_add_value / (1000000000UL/1024UL);

	DBGV( "mpc831x_set_period: base timer add value = 0x%8.8X (%u)\n",
               base_timer_add_value,
               base_timer_add_value
            );
	DBGV( "mpc831x_set_period: base timer adj value = 0x%8.8X (%u)\n",
               base_timer_adj_value,
               base_timer_adj_value
            );

}

/* eof mpc831x.c */
