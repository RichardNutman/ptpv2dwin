/* src/dep/sys.c */
/**
 * @file sys.c
 * @brief Misc. system dependent functions, mainly time related and also contains
 * function to display statistics in printable format or for output
 * in .csv format for export to spreadsheet programs
 *
 * @author Kendall Correll
 * @author Alan K. Bartky
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


/*
 * @file sys.c
 *
 * @brief Misc. system dependent functions, mainly time related and also contains
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

#define _CRT_SECURE_NO_WARNINGS

#include "../ptpd.h"
#include "getopt.h"

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
		printf("state, one way delay, offset from master, drift, sync send time, sync receive time, master to slave delay\n");
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

	if(!rtOpts->csvStats)
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

	if(rtOpts->csvStats)
	{
		len += sprintf(sbuf + len, "\n");
	}

	/* Windows compiler doesn't have write function in stdio.h
	 * Using printf instead.  Note statistics print will be as
	 * long as the string created
	 */
	if(!(rtOpts->csvStats))
	{
		sbuf[SCREEN_MAXSZ + 1] = 0;  /* Put a NULL character to force max size */
	}
	printf(sbuf);

}


/**
 * @brief Function to get time from the system or specific
 * hardware PTP timer (based on system capabilities
 * and architecture) and return it as ptpv2d system
 * internal time format
 *
 * @param[out] time       Pointer to TimeInternal structure to write time in seconds and nanoseconds TAI time
 * @param[in]  utc_offset Integer16 value of number of UTC leap seconds since January 1, 1970
 */
void getTime(TimeInternal *time, Integer16 utc_offset)
{
	unsigned long long U64Seconds;
	unsigned long      Nanoseconds;
	FILETIME           WindowsUTCTime;

	// Windows 8 / Server 2012 function.
	GetSystemTimePreciseAsFileTime(&WindowsUTCTime);

	// Use this function for Windows 7.
	//GetSystemTimeAsFileTime(&WindowsUTCTime);

	// We now have the time based on 100 nanosecond increments since
	// January 1, 1601.
	// We need to change that to seconds and nanoseconds since
	// January 1, 1970 to get to Linux time format

	// Get system time into a unsigned 64 bit time in 100 ns increments

	// Get Windows current high time
	U64Seconds = (unsigned long long) WindowsUTCTime.dwHighDateTime;

	// Left shift into most signficant DWORD;
	U64Seconds <<= 32;

	// Get Windows current low time
	U64Seconds += (unsigned long long) WindowsUTCTime.dwLowDateTime;

	// Mod by 10,000,000 to get sub seconds (in 100 ns increments)
	Nanoseconds = (unsigned long)(U64Seconds % 10000000ULL);

	// Subtract out sub seconds to get seconds 
	// since January 1, 1601 (in 100 ns increments)
	U64Seconds -= (unsigned long long) Nanoseconds;

	// Convert sub seconds in 100 ns increments to nanoseconds
	Nanoseconds *= 100U;

	// Convert Seconds since 1601-01-01 in 100 ns increments to seconds
	U64Seconds /= 10000000ULL;

	// Subtract absolute number of seconds between 
	// January 1, 1970 and January 1, 1601
	U64Seconds -= 11644473600ULL;

	// Copy calculated seconds and nanoseconds to 
	// internal time structure (seconds and nanoseconds
	// since January 1, 1970
	time->seconds = (unsigned long)U64Seconds;
	time->nanoseconds = Nanoseconds;

	/* PTP uses TAI time (time without leap seconds
	 * since January 1, 1970), gettime of day is UTC
	 * (which includes leap seconds), so adjust for leap
	 * seconds since January 1, 1970 by addint number
	 * of seconds offset from UTC
	 */

	time->seconds += utc_offset;
}


/* AKB: 1/31/08, Changed set time to set both the MPC8313 HW clock and also the
 * Linux time of day clock
 */
/**
 * @brief  Function to set time to the system or specific
 * hardware PTP timer (based on system capabilities
 * and architecture) base on ptpv2d system
 * internal time format
 *
 * @param[in]  time        Pointer to TimeInternal structure with time in seconds and nanoseconds TAI time
 * @param[in]  utc_offset Integer16 value of number of UTC leap seconds since January 1, 1970
 */
void setTime(TimeInternal *time, Integer16 utc_offset)
{
	unsigned long long U64WindowsTime;
	FILETIME           WindowsUTCTime;
	SYSTEMTIME         WindowsSystemTime;


	U64WindowsTime = (unsigned long long) time->seconds;

	// Change epoch in seconds from January 1, 1970
	// to Windows January 1, 1601

	U64WindowsTime += 11644473600ULL;

	// Change seconds from January 1, 1970 TAI time to UTC time

	U64WindowsTime -= (unsigned long long) utc_offset;

	// Convert seconds to 100 ns increments since January 1, 1601

	U64WindowsTime *= 10000000ULL;

	// Add in nanoseconds converted to 100 ns increments

	U64WindowsTime += (unsigned long long)(time->nanoseconds / 100U);

	// Convert unsigned long long 64 bit variable to
	// File time structure time

	WindowsUTCTime.dwLowDateTime = (unsigned long)(U64WindowsTime & (unsigned long long)0xFFFFFFFF);
	WindowsUTCTime.dwHighDateTime = (unsigned long)(U64WindowsTime >> 32);

	// Convert Windows UTC "file time" to Windows UTC "system time"

	if(FileTimeToSystemTime(&WindowsUTCTime, &WindowsSystemTime) == TRUE)
	{
		// Now finally we can set the windows system time
		if(SetSystemTime(&WindowsSystemTime) == 0)		// Sets the current system time
		{
			printf("Set System Time failed!\n");
		}
	}


	NOTIFY("setTime: resetting clock to UTC %ds %dns\n", time->seconds, time->nanoseconds);
}

/**
 * @brief Function to map generic need for a 16 bit unsigned random integer to
 * appropriate system random function/library
 * @param[in]  seed  32 bit integer seed for random number generator
 * @returns Unsigned 32 bit integer pseudo-random number
 */
UInteger16 getRand(UInteger32 *seed)
{
	srand(*seed);
	return((UInteger16)rand());
}


Boolean adjFreq(Integer32 adj)
{
	/*
	if(adj > ADJ_FREQ_MAX)
	adj = ADJ_FREQ_MAX;
	else if(adj < -ADJ_FREQ_MAX)
	adj = -ADJ_FREQ_MAX;*/
	
	if(SetSystemTimeAdjustment((adj), FALSE) == 0)
	{
		PERROR("adjFreq: SetSystemTimeAdjustment failed with %d\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

// eof sys.c

