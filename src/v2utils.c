/* src/v2utils.c */
/* Author: Alan K. Bartky */
/* Utilities to convert between v1 and v2 fields/values/etc. */
/* Unless otherwise specified, functions return -1 on error, unknown,
 * undefined, etc.
 */

/****************************************************************************/
/* Begin copyright and licensing information, do not remove                 */
/*                                                                          */
/* This file (maapd.h) contains original work by Alan K. Bartky             */
/* Copyright (c) 2008-2010 by Alan K. Bartky, all rights reserved           */
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

/* V2 Clause 7.5.2.2.2 */
void convert_eui48_to_eui64(Octet * eui48,
	Octet * eui64
	)
{

	/* Copy MAC address UUID into the clock identity field
	 * and format into an EUI-64 address.  EUI-48 to EUI-64
	 * conversion consists of copying OUI to first 3 bytes,
	 * then 0xFF and 0xFE in next 2 bytes and then copying last 3
	 * bytes of MAC address to last 3 bytes of EUI-64 (for total of 8 bytes).
	 */

	memcpy(eui64,      // Copy OUI field
		eui48,
		3
		);

	*(eui64 + 3) = 0xFF;  // IEEE EUI-48 (MAC address) to EUI-64
	*(eui64 + 4) = 0xFE;


	memcpy(eui64 + 5,    // Copy remaining 3 bytes of MAC address
		eui48 + 3,
		3
		);
}


/* V2 Clause 18.3.1 */
void convert_v2_domain_to_v1_subdomain(UInteger8  domain,
	Octet     *subdomain
	)
{
	if(domain == 0)
	{
		memcpy(subdomain,
			DEFAULT_PTP_DOMAIN_NAME,
			PTP_SUBDOMAIN_NAME_LENGTH
			);
	}
	else
	{
		memcpy(subdomain,
			ALTERNATE_PTP_DOMAIN1_NAME,
			PTP_SUBDOMAIN_NAME_LENGTH
			);
		if(domain > 9)
		{
			domain = 9;
		}
		*(subdomain + 4) = domain + 0x30;
	}
}

/* V2 Clause 18.3.2, Table 96 */
int v1_stratum_to_v2_clockClass(UInteger8 stratum)
{

	switch(stratum)
	{
	case 0:
		return 6;

	case 1:
		return 9;

	case 2:
		return 10;

	case 3:
		return 248;

	case 4:
		return 251;
		break;

	case 255:
		return 255;
	}
	DBG("v1_stratum_to_v2_clockClass: unable to translate: %u\n", stratum);
	return -1;

}


/* V2 Clause 18.3.2, Table 97 and Clause 18.3.3 Table 99 */
UInteger8 v2_clockClass_to_v1_stratum(UInteger8 clock_class, UInteger8 priority1)
{
	if(priority1 < 127)
	{
		return 0;
	}
	if(priority1 > 128)
	{
		return 255;
	}
	/* V2 Priority1 is 127 or 128, return V1 stratum based on clock class */
	if((clock_class >= 13)
		&& (clock_class <= 248)
		)
	{
		return 3;
	}
	else
	{
		switch(clock_class)
		{
		case 6:
		case 7:
			return 0;

		case 9:
			return 1;

		case 10:
			return 2;
		}
	}
	return 4;
}

/* V2 Clause 18.3.3, Table 98 */
int v1_preferred_to_v2_priority1(Boolean preferred)
{

	if(preferred)
	{
		return 127;
	}
	return 128;
}

/* V2 Clause 18.3.3, Table 99 */
Boolean v2_priority1_to_v1_preferred(UInteger8 priority1)
{
	if(priority1 >= 128)
	{
		return 0;
	}
	return 1;
}

/* V2 Clause 18.3.4, Table 100 */
int v1_clock_identifier_to_v2(Octet     *identifier,
	UInteger8 *clockAccuracy,
	UInteger8 *timeSource
	)
{
	if(!memcmp(identifier, IDENTIFIER_ATOM, PTP_CODE_STRING_LENGTH))
	{
		*clockAccuracy = 0x22;
		*timeSource = TS_ATOMIC_CLOCK;
		return 0;
	}
	else if(!memcmp(identifier, IDENTIFIER_GPS, PTP_CODE_STRING_LENGTH))
	{
		*clockAccuracy = 0x22;
		*timeSource = TS_GPS;
		return 0;
	}
	else if(!memcmp(identifier, IDENTIFIER_NTP, PTP_CODE_STRING_LENGTH))
	{
		*clockAccuracy = 0x2F;
		*timeSource = TS_NTP;
		return 0;
	}
	else if(!memcmp(identifier, IDENTIFIER_HAND, PTP_CODE_STRING_LENGTH))
	{
		*clockAccuracy = 0x30;
		*timeSource = TS_HAND_SET;
		return 0;
	}
	else if(!memcmp(identifier, IDENTIFIER_INIT, PTP_CODE_STRING_LENGTH))
	{
		*clockAccuracy = 0xFD;
		*timeSource = TS_OTHER;
		return 0;
	}
	else if(!memcmp(identifier, IDENTIFIER_DFLT, PTP_CODE_STRING_LENGTH))
	{
		*clockAccuracy = 0xFE;
		*timeSource = TS_INTERNAL_OSCILLATOR;
		return 0;
	}

	return -1;  // if none of the above, return -1

}

/* V2 Clause 18.3.4, Table 101 */
int v2_clockAccuracy_to_v1_clock_identifier(UInteger8  accuracy,
	Octet     *identifier
	)
{

	if((accuracy >= 0x20) && (accuracy <= 0x22))
	{
		memcpy(identifier, IDENTIFIER_ATOM, PTP_CODE_STRING_LENGTH);
		return 0;
	}
	else if((accuracy >= 0x23) && (accuracy <= 0x2F))
	{
		memcpy(identifier, IDENTIFIER_NTP, PTP_CODE_STRING_LENGTH);
		return 0;
	}
	else if(accuracy == 0x30)
	{
		memcpy(identifier, IDENTIFIER_HAND, PTP_CODE_STRING_LENGTH);
		return 0;
	}
	else if((accuracy >= 0x31) && (accuracy <= 0xFD))
	{
		memcpy(identifier, IDENTIFIER_INIT, PTP_CODE_STRING_LENGTH);
		return 0;
	}

	memcpy(identifier, IDENTIFIER_INIT, PTP_CODE_STRING_LENGTH);
	return -1; // if none of the above, return -1

}


/* V2 Clause 18.3.5, Table 102 */
Boolean convert_priority2_to_v1_boundaryClock(UInteger8 priority2)
{
	if(priority2 < 128)
	{
		return TRUE;
	}
	return FALSE;
}

/* V2 Clause 18.3.5, Table 103 */
Boolean convert_v1_boundaryClock_to_priority2(Boolean boundary_clock)
{
	if(boundary_clock)
	{
		return 127;
	}
	return 128;
}

/* V2 Clause 18.3.6, Table 104 */
void convert_v2_message_type_to_v1(UInteger8  v2_msg_type,
	UInteger8 *v1_msg_type,
	UInteger8 *v1_control
	)
{
	if(v2_msg_type < 8)
	{
		*v1_msg_type = 1;
	}
	else
	{
		*v1_msg_type = 2;
	}

	switch(v2_msg_type)
	{
	case V2_SYNC_MESSAGE:                            /* 0x0  */
		*v1_control = PTP_SYNC_MESSAGE;               /* 0x00 */
		break;

	case V2_DELAY_REQ_MESSAGE:                       /* 0x1  */
		*v1_control = PTP_DELAY_REQ_MESSAGE;          /* 0x01 */
		break;

	case V2_FOLLOWUP_MESSAGE:                        /* 0x8  */
		*v1_control = PTP_FOLLOWUP_MESSAGE;           /* 0x02 */
		break;

	case V2_DELAY_RESP_MESSAGE:                      /* 0x9   */
		*v1_control = PTP_DELAY_RESP_MESSAGE;         /* 0x03  */
		break;

	case V2_MANAGEMENT_MESSAGE:                      /* 0xD  */
		*v1_control = PTP_MANAGEMENT_MESSAGE;         /* 0x04  */
		break;

	default:
		*v1_control = 5;  /* Does not translate, set to invalid v1 value */
	}
}

/* V2 Clause 18.3.6, Table 104 */
UInteger8 convert_v1_control_to_v2_message_type(UInteger8  v1_control)
{

	switch(v1_control)
	{
	case PTP_SYNC_MESSAGE:               /* 0x00 */
		return V2_SYNC_MESSAGE;           /* 0x0  */
		break;

	case PTP_DELAY_REQ_MESSAGE:          /* 0x01 */
		return V2_DELAY_REQ_MESSAGE;      /* 0x1  */
		break;

	case PTP_FOLLOWUP_MESSAGE:           /* 0x02 */
		return V2_FOLLOWUP_MESSAGE;       /* 0x8  */
		break;

	case PTP_DELAY_RESP_MESSAGE:         /* 0x03  */
		return V2_DELAY_RESP_MESSAGE;     /* 0x9   */
		break;

	case PTP_MANAGEMENT_MESSAGE:         /* 0x04  */
		return V2_MANAGEMENT_MESSAGE;     /* 0xD  */
		break;

	default:
		return 7;  /* Does not translate, set to invalid v2 value */
	}
}



/* V2 Clause 18.3.7 and Clause 7.5.2.2.3, table 4 */
void convert_v1_uuid_to_v2_clockIdentity(Octet * uuid,
	Octet * clock_identity
	)
{

	*(clock_identity) = 0xFF;  // Communication protocol = Version 1 devices 
	*(clock_identity + 1) = 0xFE;


	memcpy(clock_identity + 2,    // Copy uuid octets into V2 octets 2-7
		uuid,
		6
		);
}

/* V2 Clause 18.3.7 and Clause 7.5.2.2.3, table 4 */
void convert_v2_clockIdentity_to_v1_uuid(Octet * clock_identity,
	Octet * uuid
	)
{

	memcpy(uuid,             // Copy uuid octets from V2 octets 2-7
		clock_identity + 2,
		6
		);
}

void convert_v2_header_to_v1(V2MsgHeader *v2_header,
	MsgHeader   *v1_header
	)
{

	v1_header->versionPTP = 1;
	v1_header->versionNetwork = 1;
	convert_v2_domain_to_v1_subdomain(v2_header->domainNumber,
		v1_header->subdomain
		);
	convert_v2_message_type_to_v1((v2_header->transportSpecificAndMessageType & 0xF),
		&v1_header->messageType,
		&v1_header->control
		);
	v1_header->sourceCommunicationTechnology = PTP_ETHER;  /* Fixed for this implementation */
	memcpy(v1_header->sourceUuid,
		v2_header->sourcePortId.clockIdentity,
		3
		);
	memcpy(v1_header->sourceUuid + 3,
		v2_header->sourcePortId.clockIdentity + 5,
		3
		);
	v1_header->sourcePortId = v2_header->sourcePortId.portNumber;
	v1_header->sequenceId = v2_header->sequenceId;



}


void convert_v2_announce_to_v1_sync(V2MsgHeader *v2_header,
	MsgAnnounce *announce,
	MsgSync     *sync
	)
{

	sync->originTimestamp.seconds = announce->originTimestamp.seconds;
	sync->originTimestamp.nanoseconds = announce->originTimestamp.seconds;
	sync->epochNumber = announce->originTimestamp.epoch_number;
	sync->currentUTCOffset = announce->currentUTCOffset;
	sync->grandmasterCommunicationTechnology = PTP_ETHER;

	convert_v2_clockIdentity_to_v1_uuid(announce->grandmasterIdentity,
		sync->grandmasterClockUuid
		);
	sync->grandmasterPortId = 1;
	sync->grandmasterSequenceId = v2_header->sequenceId;

	sync->grandmasterClockStratum
		= v2_clockClass_to_v1_stratum(announce->grandmasterClockQuality.clockClass,
		announce->grandmasterPriority1
		);

	v2_clockAccuracy_to_v1_clock_identifier(announce->grandmasterClockQuality.clockAccuracy,
		sync->grandmasterClockIdentifier
		);


	sync->grandmasterClockVariance
		= (Integer16)
		(announce->grandmasterClockQuality.offsetScaledLogVariance + 0x8000);

	sync->grandmasterPreferred
		= v2_priority1_to_v1_preferred(announce->grandmasterPriority1);

	sync->grandmasterIsBoundaryClock
		= convert_priority2_to_v1_boundaryClock(announce->grandmasterPriority2);

	sync->syncInterval = v2_header->logMeanMessageInterval;
	sync->localClockVariance = sync->grandmasterClockVariance;
	sync->localStepsRemoved = announce->stepsRemoved;
	sync->localClockStratum = sync->grandmasterClockStratum;

	memcpy(sync->localClockIdentifer,
		sync->grandmasterClockIdentifier,
		PTP_CODE_STRING_LENGTH
		);

	sync->parentCommunicationTechnology = sync->grandmasterCommunicationTechnology;

	convert_v2_clockIdentity_to_v1_uuid(v2_header->sourcePortId.clockIdentity,
		sync->parentUuid
		);
	sync->parentPortField = v2_header->sourcePortId.portNumber;
	sync->estimatedMasterVariance = sync->grandmasterClockVariance;
	sync->estimatedMasterDrift = 0;
	sync->utcReasonable = FALSE;

}

// eof v2utils.c
