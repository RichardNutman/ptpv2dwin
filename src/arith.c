/* src/arith.c */
/* General arithmetic routines for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (arith.c) contains Modifications (updates, corrections         */
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


#include "ptpd.h"

#define NSEC 1000000000

#define	NSEC_TIME_TO_SECS(nsec_time)	\
	( (Integer32) (nsec_time/NSEC) )

#define REMAINDER_NSECS(nsec_time, sec_time)\
	( (Integer32) (nsec_time - (((Integer64)sec_time) * NSEC)) )

/* from annex C of the 1588 version 1 spec */
UInteger32 crc_algorithm(Octet *buf, Integer16 length)
{
	Integer16 i;
	UInteger8 data;
	UInteger32 polynomial = 0xedb88320, crc = 0xffffffff;

	while(length-- > 0)
	{
		data = *(UInteger8 *)(buf++);

		for(i = 0; i < 8; i++)
		{
			if((crc^data) & 1)
			{
				crc = (crc >> 1);
				crc ^= polynomial;
			}
			else
			{
				crc = (crc >> 1);
			}
			data >>= 1;
		}
	}

	return crc ^ 0xffffffff;
}

UInteger32 sum(Octet *buf, Integer16 length)
{
	UInteger32 sum = 0;

	while(length-- > 0)
		sum += *(UInteger8 *)(buf++);

	return sum;
}

void v2FromInternalTime(TimeInternal         *internal, // signed secs,   signed nanoseconds
	V2TimeRepresentation *external, // unsigned secs, signed nanoseconds
	Boolean               halfEpoch,// current date past year 2038
	UInteger16            epoch     // Epoch number (even further in future)
	)
{
	external->epoch_number = epoch;

	external->seconds = labs(internal->seconds) + halfEpoch * INT_MAX;

	if(internal->seconds < 0 || internal->nanoseconds < 0)
	{
		external->nanoseconds = labs(internal->nanoseconds) | ~INT_MAX;
	}
	else
	{
		external->nanoseconds = labs(internal->nanoseconds);
	}


	DBGV("v2FromInternalTime: internal:%10ds %11dns\n",
		internal->seconds, internal->nanoseconds
		);
	DBGV("                 to external:%10ds %11dns\n",
		external->seconds, external->nanoseconds
		);
}

void fromInternalTime(TimeInternal       *internal, // signed secs,   signed nanoseconds
	TimeRepresentation *external, // unsigned secs, signed nanoseconds
	Boolean halfEpoch             // Half Epoch flag (for time past year 2038)
	)
{
	external->seconds = labs(internal->seconds) + halfEpoch * INT_MAX;

	if(internal->seconds < 0 || internal->nanoseconds < 0)
	{
		external->nanoseconds = labs(internal->nanoseconds) | ~INT_MAX;
	}
	else
	{
		external->nanoseconds = labs(internal->nanoseconds);
	}


	DBGV("fromInternalTime: internal:%10ds %11dns\n",
		internal->seconds, internal->nanoseconds
		);
	DBGV("               to external:%10ds %11dns\n",
		external->seconds, external->nanoseconds
		);
}

void toInternalTime(TimeInternal *internal, TimeRepresentation *external, Boolean *halfEpoch)
{
	*halfEpoch = external->seconds / INT_MAX;

	if(external->nanoseconds & ~INT_MAX)			// If negative.
	{
		internal->seconds = -(external->seconds     % INT_MAX);
		internal->nanoseconds = -(external->nanoseconds & INT_MAX);
	}
	else
	{
		internal->seconds = external->seconds % INT_MAX;
		internal->nanoseconds = external->nanoseconds;
	}

	DBGV("toInternalTime:   external:%10ds %11dns\n",
		external->seconds, external->nanoseconds
		);
	DBGV("               to internal:%10ds %11dns\n",
		internal->seconds, internal->nanoseconds
		);
}

void v2ToInternalTime(TimeInternal *internal, V2TimeRepresentation *external)
{

	if(external->nanoseconds & ~INT_MAX)
	{
		internal->seconds = -(external->seconds     % INT_MAX);
		internal->nanoseconds = -(external->nanoseconds & INT_MAX);
	}
	else
	{
		internal->seconds = external->seconds % INT_MAX;
		internal->nanoseconds = external->nanoseconds;
	}

	DBGV("V2ToInternalTime: external:%10ds %11dns\n",
		external->seconds, external->nanoseconds
		);
	DBGV("               to internal:%10ds %11dns\n",
		internal->seconds, internal->nanoseconds
		);
}

void v2CorrectionToInternalTime(TimeInternal *internal, Integer64 external)
{
	/* This code is optimized for speed (avoid a divide if at all possible) */
	if(abs(external) == 0)
	{
		internal->seconds = 0;
		internal->nanoseconds = 0;
		return;
	}
	if(external > 0ULL)
	{
		external >>= 16;
	}
	else
	{
		external /= 65536;
	}
	if(abs(external) < 1000000000ULL)
	{
		internal->seconds = 0;
		internal->nanoseconds = (Integer32)external;
	}
	else
	{
		internal->seconds = NSEC_TIME_TO_SECS(external);
		internal->nanoseconds = REMAINDER_NSECS(external, internal->seconds);
	}

	DBGV("V2CorrectionToInternalTime: external: %16.16llx\n",
		external
		);
	DBGV("                         to internal: %10.10ds.%9.9dns\n",
		internal->seconds, internal->nanoseconds
		);
}

static void normalizeTime(TimeInternal *result)
{
	result->seconds += result->nanoseconds / 1000000000;
	result->nanoseconds -= result->nanoseconds / 1000000000 * 1000000000;

	if(result->seconds > 0 && result->nanoseconds < 0)
	{
		result->seconds -= 1;
		result->nanoseconds += 1000000000;
	}
	else if(result->seconds < 0 && result->nanoseconds > 0)
	{
		result->seconds += 1;
		result->nanoseconds -= 1000000000;
	}
}

void addTime(TimeInternal *result, TimeInternal *x, TimeInternal *y)
{
	result->seconds = x->seconds + y->seconds;
	result->nanoseconds = x->nanoseconds + y->nanoseconds;

	normalizeTime(result);
}

void subTime(TimeInternal *result, TimeInternal *x, TimeInternal *y)
{
	result->seconds = x->seconds - y->seconds;
	result->nanoseconds = x->nanoseconds - y->nanoseconds;

	normalizeTime(result);
}

void clearTime(TimeInternal *time)
{
	time->seconds = 0;
	time->nanoseconds = 0;
}

void halveTime(TimeInternal *time)
{
	register Boolean seconds_positive;
	register Boolean seconds_odd;

	if(time->nanoseconds >= 0)
	{
		time->nanoseconds >>= 1;
	}
	else
	{
		time->nanoseconds /= 2;
	}
	if(time->seconds == 0)
	{
		return;
	}

	seconds_positive = (time->seconds > 0);
	seconds_odd = ((((UInteger32)time->seconds) & 1) == 1);

	if(seconds_positive)
	{
		time->seconds >>= 1;
		if(seconds_odd)
		{
			time->nanoseconds += 500000000;
		}
		return;
	}
	else
	{
		time->seconds /= 2;
		if(seconds_odd)
		{
			time->nanoseconds -= 500000000;
		}
		return;
	}
}

void copyTime(TimeInternal *destination, TimeInternal *source)
{
	destination->seconds = source->seconds;
	destination->nanoseconds = source->nanoseconds;
}

Integer32 getSeconds(TimeInternal *time)
{
	return (time->seconds);
}

Integer64 getNanoseconds(TimeInternal *time)
{
	return (
		((Integer64)time->seconds * 1000000000)
		+ time->nanoseconds
		);
}

Boolean isNonZeroTime(TimeInternal *time)
{
	return (time->seconds != 0
		|| time->nanoseconds != 0
		);
}

// eof src/arith.c
