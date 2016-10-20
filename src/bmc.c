/* src/bmc.c */
/* Best Master clock algorithm rountines for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (bmc.c) contains Modifications (updates, corrections           */
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

void initData(RunTimeOpts * rtOpts,   // Pointer to Run Time Options
	PtpClock *    ptpClock  // Pointer to PTP main data
	)
{
	int return_value;

	DBG("initData\n");

	/* AKB: Test if options set for running 802.1AS (direct
	 * encapsulation), if so, set transport specific
	 * flag to 1
	 */
	ptpClock->tx_transport_specific = 0;


	if(rtOpts->slaveOnly)
		rtOpts->clockStratum = 255;

	/* Port configuration data set */
	ptpClock->last_sync_tx_sequence_number = 0;
	ptpClock->last_general_event_sequence_number = 0;
	/* AKB: moved port ID field setting to startup.c for multiple port support
	  ptpClock->port_id_field                      = 1;
	  */
	ptpClock->burst_enabled = BURST_ENABLED;

	/* Default data set */
	ptpClock->clock_communication_technology = ptpClock->port_communication_technology;
	memcpy(ptpClock->clock_uuid_field,
		ptpClock->port_uuid_field,
		PTP_UUID_LENGTH
		);
	ptpClock->clock_port_id_field = 0;
	ptpClock->clock_stratum = rtOpts->clockStratum;
	memcpy(ptpClock->clock_identifier,
		rtOpts->clockIdentifier,
		PTP_CODE_STRING_LENGTH
		);
	ptpClock->sync_interval = rtOpts->syncInterval;

	ptpClock->clock_v1_variance = rtOpts->clockVariance;  /* see 1588 v1 spec 7.7 */
	ptpClock->clock_followup_capable = CLOCK_FOLLOWUP;
	ptpClock->preferred = rtOpts->clockPreferred;
	ptpClock->initializable = INITIALIZABLE;
	ptpClock->external_timing = EXTERNAL_TIMING;
	ptpClock->is_boundary_clock = BOUNDARY_CLOCK;
	memcpy(ptpClock->subdomain_name,
		rtOpts->subdomainName,
		PTP_SUBDOMAIN_NAME_LENGTH
		);
	ptpClock->number_ports = NUMBER_PORTS;
	ptpClock->number_foreign_records = 0;
	ptpClock->max_foreign_records = rtOpts->max_foreign_records;

	/* Global time properties data set */
	ptpClock->current_utc_offset = rtOpts->currentUtcOffset;
	ptpClock->epoch_number = rtOpts->epochNumber;

	/* other stuff */
	ptpClock->random_seed = ptpClock->port_uuid_field[PTP_UUID_LENGTH - 1];

	/* AKB: V2 stuff */

	ptpClock->last_announce_tx_sequence_number = 0;
	ptpClock->last_delay_req_tx_sequence_number = 0;  // Used for both Delay and PDelay request
	ptpClock->last_pdelay_req_rx_sequence_number = 0;
	ptpClock->last_pdelay_resp_tx_sequence_number = 0;


	/* For now, most input is still in v1 parms, so convert those ones
	 * from v1 to v2
	 */

	/* Uses functions from v2utils.c */

	ptpClock->priority1 = v1_preferred_to_v2_priority1(ptpClock->preferred);
	ptpClock->priority2 = 128;  /* hard coded for now */

	ptpClock->announce_interval = rtOpts->announceInterval;
	ptpClock->delay_req_interval = PTP_DELAY_REQ_INTERVAL;
	ptpClock->pdelay_req_interval = PTP_DELAY_REQ_INTERVAL;

	return_value = v1_stratum_to_v2_clockClass(ptpClock->clock_stratum);
	if(return_value == -1)
	{
		DBG("initData: Invalid stratum: %d\n", ptpClock->clock_stratum);
		ptpClock->clock_stratum = 4;  /* Force stratum to 4 */
		ptpClock->clock_quality.clockClass = 251;
	}
	else
	{
		ptpClock->clock_quality.clockClass = return_value;
	}

	ptpClock->clock_quality.offsetScaledLogVariance = ptpClock->clock_v1_variance;

	return_value = v1_clock_identifier_to_v2(ptpClock->clock_identifier,
		&ptpClock->clock_quality.clockAccuracy,
		&ptpClock->time_source
		);

	if(return_value == -1)
	{
		DBG("initData: Unable to translate V1 Clock identifier %s\n",
			ptpClock->clock_identifier
			);
	}

	memcpy(ptpClock->v2_clock_identity,
		ptpClock->port_clock_identity,
		8
		);




#ifdef PTPD_DBG
	/* AKB: Debug Function to dump PTP V2 header and first timestamp from
	 * raw buffer
	 */
	/* debug_dump_data_set_info(ptpClock); */
#endif

}

/* see spec table 18 in 1588 v1 and table 13 in 1588 v2 */
/* AKB: Added V2 init of vars for m1 */
void m1(PtpClock *ptpClock)
{
	/* Default data set */
	ptpClock->steps_removed = 0;  /* v1 & v2 */
	ptpClock->offset_from_master.seconds = 0;  /* v1 & v2 */
	ptpClock->offset_from_master.nanoseconds = 0;  /* v1 & v2 */
	ptpClock->one_way_delay.seconds = 0;  /* v1 & v2 (meanPathDelay) */
	ptpClock->one_way_delay.nanoseconds = 0;

	/* Parent data set */
	ptpClock->parent_communication_technology = ptpClock->clock_communication_technology; /* v1 */

	memcpy(ptpClock->parent_uuid,            /* v1 */
		ptpClock->clock_uuid_field,
		PTP_UUID_LENGTH
		);
	memcpy(ptpClock->parent_clock_identity,  /* v2 */
		ptpClock->v2_clock_identity,
		8
		);
	ptpClock->parent_port_id = ptpClock->clock_port_id_field; /* v1 & v2 */
	ptpClock->parent_last_sync_sequence_number = 0;
	ptpClock->parent_followup_capable = ptpClock->clock_followup_capable;
	ptpClock->parent_external_timing = ptpClock->external_timing;
	ptpClock->parent_v1_variance = ptpClock->clock_v1_variance;

	ptpClock->grandmaster_communication_technology = ptpClock->clock_communication_technology; /*v1*/
	memcpy(ptpClock->grandmaster_uuid_field,      /* v1 */
		ptpClock->clock_uuid_field,
		PTP_UUID_LENGTH
		);
	memcpy(ptpClock->grandmaster_clock_identity,  /* v2 */
		ptpClock->v2_clock_identity,
		8
		);
	ptpClock->grandmaster_port_id_field = ptpClock->clock_port_id_field; /* v1 & v2 */
	ptpClock->grandmaster_stratum = ptpClock->clock_stratum;
	memcpy(ptpClock->grandmaster_identifier,
		ptpClock->clock_identifier,
		PTP_CODE_STRING_LENGTH
		);
	ptpClock->grandmaster_v1_variance = ptpClock->clock_v1_variance;
	ptpClock->grandmaster_preferred = ptpClock->preferred;
	ptpClock->grandmaster_is_boundary_clock = ptpClock->is_boundary_clock;
	ptpClock->grandmaster_sequence_number = ptpClock->last_sync_tx_sequence_number;

	/* AKB: More V2 stuff */

	ptpClock->grandmaster_priority1 = ptpClock->priority1;
	ptpClock->grandmaster_priority2 = ptpClock->priority2;
	memcpy(&ptpClock->grandmaster_clock_quality,
		&ptpClock->clock_quality,
		sizeof(ptpClock->clock_quality)
		);

#ifdef PTPD_DBG
	/* AKB: Debug Function to dump PTP V2 header and first timestamp from
	 * raw buffer
	 */
	/* debug_dump_data_set_info(ptpClock); */
#endif

}

/* see IEEE 1588 version 1: spec table 21 */

void s1(MsgHeader *header,   // Pointer to unpacked PTP version 1 header
	MsgSync   *sync,     // Pointer to unpacked PTP version 1 sync message data
	PtpClock  *ptpClock  // Pointer to main PTP data structure
	)
{
	DBGV("s1:\n");
	/* Current data set */
	ptpClock->steps_removed = sync->localStepsRemoved + 1;

	/* Parent data set */
	ptpClock->parent_communication_technology = header->sourceCommunicationTechnology;
	memcpy(ptpClock->parent_uuid,
		header->sourceUuid,
		PTP_UUID_LENGTH
		);
	ptpClock->parent_port_id = header->sourcePortId;
	ptpClock->parent_last_sync_sequence_number = header->sequenceId;
	ptpClock->parent_followup_capable = getFlag(header->flags, PTP_ASSIST);
	ptpClock->parent_external_timing = getFlag(header->flags, PTP_EXT_SYNC);
	ptpClock->parent_v1_variance = sync->localClockVariance;
	ptpClock->grandmaster_communication_technology = sync->grandmasterCommunicationTechnology;
	memcpy(ptpClock->grandmaster_uuid_field,
		sync->grandmasterClockUuid,
		PTP_UUID_LENGTH
		);
	ptpClock->grandmaster_port_id_field = sync->grandmasterPortId;
	ptpClock->grandmaster_stratum = sync->grandmasterClockStratum;
	memcpy(ptpClock->grandmaster_identifier,
		sync->grandmasterClockIdentifier,
		PTP_CODE_STRING_LENGTH
		);
	ptpClock->grandmaster_v1_variance = sync->grandmasterClockVariance;
	ptpClock->grandmaster_preferred = sync->grandmasterPreferred;
	ptpClock->grandmaster_is_boundary_clock = sync->grandmasterIsBoundaryClock;
	ptpClock->grandmaster_sequence_number = sync->grandmasterSequenceId;

	/* Global time properties data set */

	ptpClock->current_utc_offset = sync->currentUTCOffset;
	ptpClock->leap_59 = getFlag(header->flags, PTP_LI_59);
	ptpClock->leap_61 = getFlag(header->flags, PTP_LI_61);
	ptpClock->epoch_number = sync->epochNumber;

#ifdef PTPD_DBG
	/* AKB: Debug Function to dump PTP V2 header and first timestamp from
	 * raw buffer
	 */
	/* debug_dump_data_set_info(ptpClock); */
#endif

}


void copyD0(MsgHeader *header,
	MsgSync   *sync,
	PtpClock  *ptpClock
	)
{
	DBGV("copyD0:\n");
	sync->grandmasterCommunicationTechnology = ptpClock->clock_communication_technology;
	memcpy(sync->grandmasterClockUuid,
		ptpClock->port_uuid_field,
		PTP_UUID_LENGTH
		);
	sync->grandmasterPortId = ptpClock->port_id_field;
	sync->grandmasterClockStratum = ptpClock->clock_stratum;
	memcpy(sync->grandmasterClockIdentifier,
		ptpClock->clock_identifier,
		PTP_CODE_STRING_LENGTH
		);
	sync->grandmasterClockVariance = ptpClock->clock_v1_variance;
	sync->grandmasterIsBoundaryClock = ptpClock->is_boundary_clock;
	sync->grandmasterPreferred = ptpClock->preferred;
	sync->localStepsRemoved = ptpClock->steps_removed;
	header->sourceCommunicationTechnology = ptpClock->clock_communication_technology;
	memcpy(header->sourceUuid,
		ptpClock->port_uuid_field,
		PTP_UUID_LENGTH
		);
	header->sourcePortId = ptpClock->port_id_field;
	sync->grandmasterSequenceId = ptpClock->grandmaster_sequence_number;
	header->sequenceId = ptpClock->grandmaster_sequence_number;
}

int getIdentifierOrder(Octet identifier[PTP_CODE_STRING_LENGTH])
{
	if(!memcmp(identifier, IDENTIFIER_ATOM, PTP_CODE_STRING_LENGTH))
		return 1;
	else if(!memcmp(identifier, IDENTIFIER_GPS, PTP_CODE_STRING_LENGTH))
		return 1;
	else if(!memcmp(identifier, IDENTIFIER_NTP, PTP_CODE_STRING_LENGTH))
		return 2;
	else if(!memcmp(identifier, IDENTIFIER_HAND, PTP_CODE_STRING_LENGTH))
		return 3;
	else if(!memcmp(identifier, IDENTIFIER_INIT, PTP_CODE_STRING_LENGTH))
		return 4;
	else if(!memcmp(identifier, IDENTIFIER_DFLT, PTP_CODE_STRING_LENGTH))
		return 5;

	return 6;  // if none of the above, return 6
}

/* return similar to memcmp()s
   note: communicationTechnology can be ignored because
   if they differed they would not have made it here */

Integer8 bmcDataSetComparison(MsgHeader *headerA,
	MsgSync   *syncA,
	MsgHeader *headerB,
	MsgSync   *syncB,
	PtpClock  *ptpClock
	)
{
	DBGV("bmcDataSetComparison: start\n");
	if(!(syncA->grandmasterPortId == syncB->grandmasterPortId
		&& !memcmp(syncA->grandmasterClockUuid,
		syncB->grandmasterClockUuid,
		PTP_UUID_LENGTH
		)
		)
		)
	{
		/* test if grandmasterClockStratums less, greater or same */
		if(syncA->grandmasterClockStratum < syncB->grandmasterClockStratum)
			goto A;
		else if(syncA->grandmasterClockStratum > syncB->grandmasterClockStratum)
			goto B;

		/* grandmasterClockStratums same */
		/* test if grandmasterClockIdentifiers less, greater or same */
		if(getIdentifierOrder(syncA->grandmasterClockIdentifier)
			< getIdentifierOrder(syncB->grandmasterClockIdentifier)
			)
			goto A;
		if(getIdentifierOrder(syncA->grandmasterClockIdentifier)
			> getIdentifierOrder(syncB->grandmasterClockIdentifier)
			)
			goto B;

		/* grandmasterClockIdentifiers same */
		/* test if grandmasterClockIdentifiers less, greater or same
		 * plus or minus the PTP_LOG_VARIANCE_THRESHOLD (i.e. "similar")
		 */
		if(syncA->grandmasterClockStratum > 2)
		{
			if(syncA->grandmasterClockVariance
		 > syncB->grandmasterClockVariance + PTP_LOG_VARIANCE_THRESHOLD
		 || syncA->grandmasterClockVariance
		 < syncB->grandmasterClockVariance - PTP_LOG_VARIANCE_THRESHOLD
		 )
			{
				/* grandmasterClockVariances are not similar */
				if(syncA->grandmasterClockVariance
					< syncB->grandmasterClockVariance
					)
					goto A;
				else
					goto B;
			}
			/* grandmasterClockVariances are similar */
			if(!syncA->grandmasterIsBoundaryClock != !syncB->grandmasterIsBoundaryClock) /* XOR */
			{
				if(syncA->grandmasterIsBoundaryClock)
					goto A;
				else
					goto B;
			}
			/* neither is grandmasterIsBoundaryClock */
			if(memcmp(syncA->grandmasterClockUuid,
				syncB->grandmasterClockUuid,
				PTP_UUID_LENGTH
				) < 0
				)
				goto A;
			else
				goto B;
		}

		/* syncA->grandmasterClockStratum <= 2 */
		if(!syncA->grandmasterPreferred != !syncB->grandmasterPreferred) /* XOR */
		{
			if(syncA->grandmasterPreferred)
				return 1;   /* A1 */
			else
				return -1;  /* B1 */
		}
		/* neither or both grandmasterPreferred */
	}

	DBGV("bmcDataSetComparison: X\n");
	if(syncA->localStepsRemoved > syncB->localStepsRemoved + 1
		|| syncA->localStepsRemoved < syncB->localStepsRemoved - 1)
	{
		/* localStepsRemoved not within 1 */
		if(syncA->localStepsRemoved < syncB->localStepsRemoved)
			return 1;   /* A1 */
		else
			return -1;  /* B1 */
	}

	/* localStepsRemoved within 1 */
	if(syncA->localStepsRemoved < syncB->localStepsRemoved)
	{
		DBGV("bmcDataSetComparison: A3\n");
		if(memcmp(ptpClock->port_uuid_field,
			headerB->sourceUuid,
			PTP_UUID_LENGTH
			) < 0
			)
			return 1;  /* A1 */
		else if(memcmp(ptpClock->port_uuid_field,
			headerB->sourceUuid,
			PTP_UUID_LENGTH
			) > 0
			)
			return 2;  /* A2 */

		/* this port_uuid_field same as headerB->sourceUuid */
		if(ptpClock->port_id_field < headerB->sourcePortId)
			return 1;  /* A1 */
		else if(ptpClock->port_id_field > headerB->sourcePortId)
			return 2;  /* A2 */

		/* this port_id_field same as headerB->sourcePortId */
		return 0;    /* same */
	}

	if(syncA->localStepsRemoved > syncB->localStepsRemoved)
	{
		DBGV("bmcDataSetComparison: B3\n");
		if(memcmp(ptpClock->port_uuid_field,
			headerA->sourceUuid,
			PTP_UUID_LENGTH
			) < 0
			)
			return -1; /* B1 */
		else if(memcmp(ptpClock->port_uuid_field,
			headerB->sourceUuid,
			PTP_UUID_LENGTH
			) > 0
			)
			return -2; /* B2 */

		/* this port_uuid_field same as headerA->sourceUuid */
		if(ptpClock->port_id_field < headerA->sourcePortId)
			return -1; /* B1 */
		else if(ptpClock->port_id_field > headerA->sourcePortId)
			return -2; /* B2 */

		/* this port_id_field same as headerA->sourcePortId */
		return 0;    /* same */
	}

	/* localStepsRemoved same */
	if(memcmp(headerA->sourceUuid,
		headerB->sourceUuid,
		PTP_UUID_LENGTH
		) < 0
		)
		return 2;   /* A2 */
	else if(memcmp(headerA->sourceUuid,
		headerB->sourceUuid,
		PTP_UUID_LENGTH
		) > 0
		)
		return -2;  /* B2 */

	/* sourceUuid same */
	DBGV("bmcDataSetComparison: Z\n");
	if(syncA->grandmasterSequenceId > syncB->grandmasterSequenceId)
		return 3;
	else if(syncA->grandmasterSequenceId < syncB->grandmasterSequenceId)
		return -3;

	/* grandmasterSequenceId same */
	if(headerA->sequenceId > headerB->sequenceId)
		return 3;
	else if(headerA->sequenceId < headerB->sequenceId)
		return -3;

	/* sequenceId same */
	return 0;  /* same */

	/* goto labels (come from above in this function */
A:
	if(!syncA->grandmasterPreferred && syncB->grandmasterPreferred)
		return -1;  /* B1 */
	else
		return 1;   /* A1 */
B:
	if(syncA->grandmasterPreferred && !syncB->grandmasterPreferred)
		return 1;   /* A1 */
	else
		return -1;  /* B1 */
}

/* Function to test for Best Master between two sync messages */

UInteger8 bmcStateDecision(MsgHeader   *header,  // PTP header info
	MsgSync     *sync,    // Sync message data
	RunTimeOpts *rtOpts,  // Run time options
	PtpClock    *ptpClock // Main PTP data structure
	)
{
	/* Test if run time option is set for Slave only,
	 * if so, run s1 and return SLAVE state
	 */

	if(rtOpts->slaveOnly)
	{
		s1(header, sync, ptpClock);
		return PTP_SLAVE;
	}

	copyD0(&ptpClock->msgTmpHeader, &ptpClock->msgTmp.sync, ptpClock);

	if(ptpClock->msgTmp.sync.grandmasterClockStratum < 3)
	{
		/* Grandmaster Statum is less than 3 (accurate clock) */
		if(bmcDataSetComparison(&ptpClock->msgTmpHeader,  // Current RX PTP header
			&ptpClock->msgTmp.sync,   // Current RX SYNC data
			header,
			sync,
			ptpClock
			) > 0
			)
		{
			/* Stratum is less 3 and we are a better clock, run m1 and return MASTER state */
			m1(ptpClock);
			return PTP_MASTER;
		}
		s1(header, sync, ptpClock);
		return PTP_PASSIVE;
	}
	else if(bmcDataSetComparison(&ptpClock->msgTmpHeader,
		&ptpClock->msgTmp.sync,
		header,
		sync,
		ptpClock
		) > 0
		&& ptpClock->msgTmp.sync.grandmasterClockStratum != 255
		)
	{
		/* Grandmaster stratum is 3 or more, but not equal to 255 and we are a better clock,
		 * return MASTER */
		m1(ptpClock);
		return PTP_MASTER;
	}
	else
	{
		/* Grandmaster stratum is 255 or we are not a better clock, return SLAVE state */
		s1(header, sync, ptpClock);
		return PTP_SLAVE;
	}
}

// Best Master Clock (bmc) function:

UInteger8 bmc(ForeignMasterRecord *foreign,
	RunTimeOpts         *rtOpts,
	PtpClock            *ptpClock
	)
{
	Integer16 i, best;

	/* Check if any foreign masters */

	if(!ptpClock->number_foreign_records)
	{
		DBGV("bmc: number_foreign_records is zero, state: %u\n",
			ptpClock->port_state
			);
		/* No foreign masters, run m1 if we are currently master */
		if(ptpClock->port_state == PTP_MASTER)
		{
			DBGV("bmc: calling m1\n");
			m1(ptpClock);
		}
		return ptpClock->port_state;  /* no change */
	}

	DBGV("bmc: number_foreign_records is non zero, state: %u\n",
		ptpClock->port_state
		);
	/* There is at least one foreign master.  Scan through foreign master database and compare
	 * to look for best master to use
	 */
	for(i = 1, best = 0; i < ptpClock->number_foreign_records; ++i)
	{
		/* Check current loop indx record versus current "best" record */
		if(bmcDataSetComparison(&foreign[i].header,
			&foreign[i].sync,
			&foreign[best].header,
			&foreign[best].sync,
			ptpClock
			) > 0)
		{
			/* Current loop index is better than previous "best", set new "best" */
			best = i;
		}
		DBGV("bmc: comparison loop i=%d, best=%d\n", i, best);
	}

	/* Best record found, store index to best foreign master */

	DBGV("bmc: best record %d\n", best);
	ptpClock->foreign_record_best = best;

	/* Now that best is found, determine recommended state */

	return bmcStateDecision(&foreign[best].header,
		&foreign[best].sync,
		rtOpts,
		ptpClock
		);
}

// eof bmc.c
