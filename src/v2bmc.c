/* src/v2bmc.c */
/* Author: Alan K. Bartky */
/* PTP version 2 specific best master clock algorithms (used by IEEE 1588
 * version 2 and also IEEE 802.1AS (direct Ethernet encapsulation)
 */
/****************************************************************************/
/* Begin copyright and licensing information, do not remove                 */
/*                                                                          */
/* This file (v2bmc.c) contains original work by Alan K. Bartky             */
/* Copyright (c) 2007-2010 by Alan K. Bartky, all rights reserved           */
/*                                                                          */
/* This source code and its associated software algorithms are under        */
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
/* End Alan K. Bartky copyright notice: Do not remove                       */
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

#ifdef PTPD_DBG
/* AKB: Debug Function to dump data set info
 */

void debug_dump_data_set_info(PtpClock *ptpClock)
{

	DBGV("debug_dump_data_set_info, version 1 info:\n");

	/* Current data set */
	DBGV(" steps_removed........................ %u\n",
		ptpClock->steps_removed
		);

	/* Parent data set */
	DBGV(" parent_communication_technology...... %u\n",
		ptpClock->parent_communication_technology
		);
	DBGV(" parent_uuid.......................... %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
		ptpClock->parent_uuid[0],
		ptpClock->parent_uuid[1],
		ptpClock->parent_uuid[2],
		ptpClock->parent_uuid[3],
		ptpClock->parent_uuid[4],
		ptpClock->parent_uuid[5]
		);
	DBGV(" parent_port_id....................... %u\n",
		ptpClock->parent_port_id
		);
	DBGV(" parent_last_sync_sequence_number..... %u\n",
		ptpClock->parent_last_sync_sequence_number
		);
	DBGV(" parent_followup_capable.............. %s\n",
		ptpClock->parent_followup_capable?"TRUE":"FALSE"
		);
	DBGV(" parent_external_timing............... %s\n",
		ptpClock->parent_external_timing?"TRUE":"FALSE"
		);
	DBGV(" parent_v1_variance................... %d\n",
		ptpClock->parent_v1_variance
		);
	DBGV(" grandmaster_communication_technology. %u\n",
		ptpClock->grandmaster_communication_technology
		);
	DBGV(" grandmaster_uuid_field............... %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
		ptpClock->grandmaster_uuid_field[0],
		ptpClock->grandmaster_uuid_field[1],
		ptpClock->grandmaster_uuid_field[2],
		ptpClock->grandmaster_uuid_field[3],
		ptpClock->grandmaster_uuid_field[4],
		ptpClock->grandmaster_uuid_field[5]
		);
	DBGV(" grandmaster_port_id_field............ %u\n",
		ptpClock->grandmaster_port_id_field
		);
	DBGV(" grandmaster_stratum.................. %u\n",
		ptpClock->grandmaster_stratum
		);
	DBGV(" grandmaster_identifier............... %c%c%c%c\n",
		ptpClock->grandmaster_identifier[0],
		ptpClock->grandmaster_identifier[1],
		ptpClock->grandmaster_identifier[2],
		ptpClock->grandmaster_identifier[3]
		);
	DBGV(" grandmaster_v1_variance.............. %u\n",
		ptpClock->grandmaster_v1_variance
		);
	DBGV(" grandmaster_preffered................ %s\n",
		ptpClock->grandmaster_preferred?"TRUE":"FALSE"
		);
	DBGV(" grandmaster_is_boundary_clock........ %s\n",
		ptpClock->grandmaster_is_boundary_clock?"TRUE":"FALSE"
		);
	DBGV(" grandmaster_sequence_number.......... %u\n",
		ptpClock->grandmaster_sequence_number
		);

	/* Global time properties data set */

	DBGV(" current_utc_offset................... %d\n",
		ptpClock->current_utc_offset
		);
	DBGV(" leap_59.............................. %s\n",
		ptpClock->leap_59?"TRUE":"FALSE"
		);
	DBGV(" leap_61.............................. %s\n",
		ptpClock->leap_61?"TRUE":"FALSE"
		);
	DBGV(" epoch_number......................... %d\n",
		ptpClock->epoch_number  
		);



	DBGV("debug_dump_data_set_info, version 2 info:\n");


	DBGV(" parent_clock_identity................ %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
		ptpClock->parent_clock_identity[0],
		ptpClock->parent_clock_identity[1],
		ptpClock->parent_clock_identity[2],
		ptpClock->parent_clock_identity[3],
		ptpClock->parent_clock_identity[4],
		ptpClock->parent_clock_identity[5],
		ptpClock->parent_clock_identity[6],
		ptpClock->parent_clock_identity[7]
		);
	DBGV(" grandmaster_clock_identity........... %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
		ptpClock->grandmaster_clock_identity[0],
		ptpClock->grandmaster_clock_identity[1],
		ptpClock->grandmaster_clock_identity[2],
		ptpClock->grandmaster_clock_identity[3],
		ptpClock->grandmaster_clock_identity[4],
		ptpClock->grandmaster_clock_identity[5],
		ptpClock->grandmaster_clock_identity[6],
		ptpClock->grandmaster_clock_identity[7]
		);
	DBGV(" grandmaster_priority1................ %u\n",
		ptpClock->grandmaster_priority1
		);
	DBGV(" grandmaster_priority2................ %u\n",
		ptpClock->grandmaster_priority2
		);

	DBGV(" grandmaster clockClass............... %u\n",
		ptpClock->grandmaster_clock_quality.clockClass
		);
	DBGV(" grandmaster clockAccuracy............ %u\n",
		ptpClock->grandmaster_clock_quality.clockAccuracy
		);
	DBGV(" grandmaster offsetScaledLogVariance.. %d\n",
		ptpClock->grandmaster_clock_quality.offsetScaledLogVariance
		);
}

#endif

void v2_s1(V2MsgHeader *header,   // Pointer to unpacked PTP Version 2 header
	MsgAnnounce *announce, // Pointer to unpacked PTP Version 2 announce message data
	PtpClock    *ptpClock  // Pointer to main PTP data structure
	)
{
	DBGV("v2_s1:\n");
	/* Current data set */
	ptpClock->steps_removed = announce->stepsRemoved + 1;

	/* Parent data set */
	memcpy(ptpClock->parent_clock_identity,
		header->sourcePortId.clockIdentity,
		8
		);
	ptpClock->parent_port_id = header->sourcePortId.portNumber;
	ptpClock->parent_last_announce_sequence_number = header->sequenceId;

	ptpClock->parent_v2_variance
		= announce->grandmasterClockQuality.offsetScaledLogVariance;

	ptpClock->grandmaster_priority1 = announce->grandmasterPriority1;
	ptpClock->grandmaster_priority2 = announce->grandmasterPriority2;

	memcpy(&ptpClock->grandmaster_clock_quality,
		&announce->grandmasterClockQuality,
		sizeof(announce->grandmasterClockQuality)
		);

	memcpy(ptpClock->grandmaster_clock_identity,
		announce->grandmasterIdentity,
		8
		);
	ptpClock->grandmaster_port_id_field = 1;
	ptpClock->grandmaster_stratum
		= v2_clockClass_to_v1_stratum(announce->grandmasterClockQuality.clockClass,
		announce->grandmasterPriority1
		);
	ptpClock->grandmaster_v1_variance = ptpClock->parent_v1_variance;
	ptpClock->grandmaster_preferred
		= v2_priority1_to_v1_preferred(announce->grandmasterPriority1);
	ptpClock->grandmaster_sequence_number = header->sequenceId;

	/* Global time properties data set */

	ptpClock->current_utc_offset = announce->currentUTCOffset;
	ptpClock->leap_59 = ((header->flags[1] & ANNOUNCE_LI_59) == ANNOUNCE_LI_59);
	ptpClock->leap_61 = ((header->flags[1] & ANNOUNCE_LI_61) == ANNOUNCE_LI_61);
	ptpClock->epoch_number = announce->originTimestamp.epoch_number;

#ifdef PTPD_DBG
	/* AKB: Debug Function to dump PTP V2 header and first timestamp from
	 * raw buffer
	 */
	/* debug_dump_data_set_info(ptpClock); */
#endif

}

void v2copyD0(V2MsgHeader *header,
	MsgAnnounce *announce,
	PtpClock    *ptpClock
	)
{
	DBGV("v2copyD0:\n");
	memcpy(announce->grandmasterIdentity,
		ptpClock->port_clock_identity,
		8
		);
	announce->grandmasterPriority1 = ptpClock->grandmaster_priority1;
	announce->grandmasterPriority2 = ptpClock->grandmaster_priority2;

	memcpy(&announce->grandmasterClockQuality,
		&ptpClock->grandmaster_clock_quality,
		sizeof(announce->grandmasterClockQuality)
		);

	announce->stepsRemoved = 0;

	memcpy(header->sourcePortId.clockIdentity,
		ptpClock->port_clock_identity,
		8
		);
	header->sourcePortId.portNumber = ptpClock->port_id_field;
	header->sequenceId = ptpClock->grandmaster_sequence_number;
}

/* return similar to memcmp()s
 * Returns positive if A is better than B, 0 if they are equal,
 * negative if B is better than A
 */

Integer8 v2bmcDataSetComparison(V2MsgHeader *headerA,
	MsgAnnounce *announceA,
	V2MsgHeader *headerB,
	MsgAnnounce *announceB,
	PtpClock    *ptpClockA,
	PtpClock    *ptpClockB   // For future full multi port support
	)
{
	int memcmp_result;

	DBGV("v2bmcDataSetComparison: start\n");
	memcmp_result = memcmp(announceB->grandmasterIdentity,
		announceA->grandmasterIdentity,
		8
		);
	/* reminder, memcmp returns less than 0 if arg1 < arg2, 0 if equal, greater
	 * than 0 if arg1 > arg2.
	 */
	if(memcmp_result != 0)
	{
		DBGV("v2bmcDataSetComparison: grandmaster IDs not equal\n");
		DBGV("v2bmcDataSetComparison: part 1\n");
		/* Grandmaster IDs not equal */
		/* test if priority1 less, greater or same */
		if(announceA->grandmasterPriority1 < announceB->grandmasterPriority1)
			return  1;
		else if(announceA->grandmasterPriority1 > announceB->grandmasterPriority1)
			return -1;

		/* Compare Clock Quality */

		if(announceA->grandmasterClockQuality.clockClass
			< announceB->grandmasterClockQuality.clockClass
			)
			return  2;
		else if(announceA->grandmasterClockQuality.clockClass
			> announceB->grandmasterClockQuality.clockClass
			)
			return -2;

		if(announceA->grandmasterClockQuality.clockAccuracy
			< announceB->grandmasterClockQuality.clockAccuracy
			)
			return  3;
		else if(announceA->grandmasterClockQuality.clockAccuracy
			> announceB->grandmasterClockQuality.clockClass
			)
			return -3;

		if(announceA->grandmasterClockQuality.offsetScaledLogVariance
			< announceB->grandmasterClockQuality.offsetScaledLogVariance
			)
			return  4;
		else if(announceA->grandmasterClockQuality.offsetScaledLogVariance
			< announceB->grandmasterClockQuality.offsetScaledLogVariance
			)
			return -4;

		/* Compare priority2 */
		if(announceA->grandmasterPriority2 < announceB->grandmasterPriority2)
			return  5;
		else if(announceA->grandmasterPriority2 > announceB->grandmasterPriority2)
			return -5;

		if(memcmp_result > 0)
			return  6;  /* A < B */
		else
			return -6;
	}


	DBGV("v2bmcDataSetComparison: part 2 (X)\n");

	/* Entry point of flowchart, part 2, label "X" */
	/* Grandmaster IDs are identical */
	/* Compare Steps removed of A and B */

	if(announceA->stepsRemoved > announceB->stepsRemoved + 1)
		return -7;  /* A > B+1, return B better than A */

	if(announceA->stepsRemoved + 1 < announceB->stepsRemoved)
		return -7;  /* A+1 < B, return A better than B */

	/* A within 1 of B */
	/* Compare steps Removed again (second decision point in 1588 v2 spec diagram) */


	if(announceA->stepsRemoved > announceB->stepsRemoved)
	{
		/* A steps removed > B */
		/* Compare identities of Receiver and Sender of A */
		memcmp_result = memcmp(ptpClockA->port_clock_identity,     /* Receiver */
			headerA->sourcePortId.clockIdentity,/* Sender   */
			8
			);
		if(memcmp_result < 0)
			return -8;   /* Receiver < Sender, Return B better than A */
		else if(memcmp_result > 0)
			return -9;   /* Receiver > Sender, Return B better by topology than A */
		else
		{
			/* Clock IDs same, check port numbers */
			if(ptpClockA->port_id_field             /* Receiver */
				< headerA->sourcePortId.portNumber /* Sender */
				)
				return -8;   /* Receiver < Sender, Return B better than A */
			else if(ptpClockA->port_id_field              /* Receiver */
				> headerA->sourcePortId.portNumber  /* Sender */
				)
				return -9;   /* Receiver > Sender, Return B better by topology than A */
			else
			{
				DBG("v2bmcDataSetComparison: error-1 !!!!!!!!!!!!!!!\n");
				return 0;
			}
		}
	}
	else if(announceA->stepsRemoved < announceB->stepsRemoved)
	{
		/* A steps removed < B */
		/* Compare identities of Receiver and Sender of B */
		memcmp_result = memcmp(ptpClockB->port_clock_identity,     /* Receiver */
			headerB->sourcePortId.clockIdentity,/* Sender   */
			8
			);
		if(memcmp_result < 0)
			return  8;   /* Receiver < Sender, Return A better than B */
		else if(memcmp_result > 0)
			return  9;   /* Receiver > Sender, Return A better by topology than B */
		else
		{
			/* Clock IDs same, check port numbers */
			if(ptpClockB->port_id_field             /* Receiver */
				< headerB->sourcePortId.portNumber /* Sender */
				)
				return  8;   /* Receiver < Sender, Return A better than B */
			else if(ptpClockB->port_id_field              /* Receiver */
				> headerB->sourcePortId.portNumber  /* Sender */
				)
				return  9;   /* Receiver > Sender, Return A better by topology than B */
			else
			{
				DBG("v2bmcDataSetComparison: error-1 !!!!!!!!!!!!!!!\n");
				return 0;
			}
		}
	}
	else
	{
		/* A steps removed == B */
		/* Compare identities of senders of A and B */
		memcmp_result = memcmp(headerA->sourcePortId.clockIdentity,/* Sender A */
			headerB->sourcePortId.clockIdentity,/* Sender B */
			8
			);
		if(memcmp_result < 0)
			return  9;   /* Sender A < Sender B, Return A better by topology than B */
		else if(memcmp_result > 0)
			return -9;   /* Sender A > Sender B, Return B better by topology than A */
		else
		{
			/* Clock IDs same, check port numbers */
			if(headerA->sourcePortId.portNumber /* Sender A */
				< headerB->sourcePortId.portNumber /* Sender B */
				)
				return  9;   /* Sender A < Sender B, Return A better by topology than B */

			else if(headerA->sourcePortId.portNumber  /* Receiver */
				> headerB->sourcePortId.portNumber  /* Sender */
				)
				return -9;   /* Sender A > Sender B, Return B better by topology than A */
			else
			{
				/* Sender A == Sender B */
				/* Compare port numbers of Receivers of A and B */
				if(ptpClockA->port_id_field < ptpClockB->port_id_field)
					return  9;  /* Receiver A port number < B, Return A better by topology than B */
				else if(ptpClockA->port_id_field < ptpClockB->port_id_field)
					return -9;  /* Receiver A port number > B, Return B better by topology than A */
				else
				{
					/* Receiver A port number == B, "error-2" */
					DBG("v2bmcDataSetComparison: error-2 !!!!!!!!!!!!!!!\n");
					return 0;
				}
			}
		}
	}
}

/* Function to test for Best Master between two announce messages */

UInteger8 v2bmcStateDecision(V2MsgHeader *header,   // PTP header info
	MsgAnnounce *announce, // Announce message data
	RunTimeOpts *rtOpts,   // Run time options
	PtpClock    *ptpClock  // Main PTP data structure
	)
{
	/* Test if run time option is set for Slave only,
	 * if so, run v2_s1 and return SLAVE state
	 */

	if(rtOpts->slaveOnly)
	{
		v2_s1(header, announce, ptpClock);
		return PTP_SLAVE;
	}

	v2copyD0(&ptpClock->v2MsgTmpHeader, &ptpClock->msgTmp.announce, ptpClock);

	if(ptpClock->msgTmp.announce.grandmasterClockQuality.clockClass < 128)
	{
		/* Grandmaster clock class < 128 */
		if(v2bmcDataSetComparison(&ptpClock->v2MsgTmpHeader,    // A: D0 RX PTP header
			&ptpClock->msgTmp.announce,   // A: D0 announce data
			header,                      // B: Best PTP header
			announce,                    // B: Best announce data
			ptpClock,
			ptpClock
			) > 0
			)
		{
			/* D0 better than current recommended best master clock */
			m1(ptpClock);
			return PTP_MASTER;
		}
		/* D0 not better than recommended best master clock */
		v2_s1(header, announce, ptpClock);
		return PTP_PASSIVE;
	}

	/* D0 is class 128 or higher */
	/* Compare D0 versus recommended best master clock */
	else if(v2bmcDataSetComparison(&ptpClock->v2MsgTmpHeader,     // A: D0 PTP header
		&ptpClock->msgTmp.announce,    // A: D0 announce data
		header,                        // B: Best PTP header
		announce,                      // B: Best PTP header
		ptpClock,
		ptpClock
		) > 0
		)
	{
		/* D0 is better then recommended best master clock
		 * return MASTER
		 */
		m1(ptpClock);
		return PTP_MASTER;
	}
	else
	{
		/* AKB: multi port not fully supported, so check for if best master clock on "port R"
		 * and E "best" better by topology than E "rbest" is not implemented at this time.
		 * Instead, we go straith to SLAVE state.
		 */
		v2_s1(header, announce, ptpClock);
		return PTP_SLAVE;
	}
}

// Best Master Clock (bmc) function:

UInteger8 v2bmc(ForeignMasterRecord *foreign,
	RunTimeOpts         *rtOpts,
	PtpClock            *ptpClock
	)
{
	Integer16 i, best;

	/* Check if any foreign masters */

	if(!ptpClock->number_foreign_records)
	{
		DBGV("v2bmc: number_foreign_records is zero, state: %u\n",
			ptpClock->port_state
			);
		/* No foreign masters, run m1 if we are currently master */
		if(ptpClock->port_state == PTP_MASTER)
		{
			DBGV("v2bmc: calling m1\n");
			m1(ptpClock);
		}
		return ptpClock->port_state;  /* no change */
	}

	DBGV("v2bmc: number_foreign_records is non zero, state: %u\n",
		ptpClock->port_state
		);
	/* There is at least one foreign master.  Scan through foreign master database and compare
	 * to look for best master to use
	 */
	for(i = 1, best = 0; i < ptpClock->number_foreign_records; ++i)
	{
		/* Check current loop indx record versus current "best" record */
		if(v2bmcDataSetComparison(&foreign[i].v2_header,
			&foreign[i].announce,
			&foreign[best].v2_header,
			&foreign[best].announce,
			ptpClock,
			ptpClock
			) > 0
			)
		{
			/* Current loop index is better than previous "best", set new "best" */
			best = i;
		}
		DBGV("v2bmc: comparison loop i=%d, best=%d\n", i, best);
	}

	/* Best record found, store index to best foreign master */

	DBGV("v2bmc: best record %d\n", best);
	ptpClock->foreign_record_best = best;

	/* Now that best is found, determine recommended state */

	return v2bmcStateDecision(&foreign[best].v2_header,
		&foreign[best].announce,
		rtOpts,
		ptpClock
		);
}

// eof v2bmc.c
