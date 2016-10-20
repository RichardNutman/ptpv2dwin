/* src/dep/msg.c */
/* PTP message handling for packing, unpacking, parsing and debug printing */
/* Copyright (c) 2005-2007 Kendall Correll */

/* For IEEE 1588 version 1, see annex d */
/* For IEEE 1588 version 2, see clause (chapter) 13 */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (msg.c) contains Modifications (updates, corrections           */
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
 * @file msg.c
 * PTP message handling for packing, unpacking, parsing and debug printing
 *
 * @par Original Copyright
 * This file is a derivative work from msg.c
 * Copyright (c) 2005-2007 Kendall Correll 
 *
 * @par Modifications and enhancements Copyright
 * Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights
 * reserved
 *
 * @par
 * This file (msg.c) contains Modifications (updates, corrections      
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

#ifdef PTPD_DBG
/* AKB: Debug Function to dump PTP V2 header and first timestamp from
 * raw buffer
 */

/* AKB 2010-09-06: In porting to Microsoft C, it complained about
 * math on a void pointer which is a valid point.
 * Changed all instances of buf + X, to ((Uinteger8*)buf) + X
 */

void debug_dump_ptp_v2_header (void * buf) 
{
  DBGM(" (00) Transport specific & type %02hhx\n", *(UInteger8*) (((UInteger8*)buf) + 0));
  DBGM(" (01) Version.................. %02hhx\n", *(UInteger8*) (((UInteger8*)buf) + 1));
  DBGM(" (02) Message length........... %02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 02),
       *(UInteger8*) (((UInteger8*)buf) + 03)
      );
  DBGM(" (04) Domain number............ %d\n",     *(UInteger8*) (((UInteger8*)buf) + 4));
  DBGM(" (05) reserved................. %02hhx\n", *(UInteger8*) (((UInteger8*)buf) + 5));
  DBGM(" (06) Flags.................... %02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 6 ),
       *(UInteger8*) (((UInteger8*)buf) + 7 )
      );
  DBGM(" (08) CorrectionField.......... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 8 ),
       *(UInteger8*) (((UInteger8*)buf) + 9 ),
       *(UInteger8*) (((UInteger8*)buf) + 10),
       *(UInteger8*) (((UInteger8*)buf) + 11),
       *(UInteger8*) (((UInteger8*)buf) + 12),
       *(UInteger8*) (((UInteger8*)buf) + 13),
       *(UInteger8*) (((UInteger8*)buf) + 14),
       *(UInteger8*) (((UInteger8*)buf) + 15)
      );
  DBGM(" (16) Reserved................. %02hhx:%02hhx:%02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 16),
       *(UInteger8*) (((UInteger8*)buf) + 17),
       *(UInteger8*) (((UInteger8*)buf) + 18),
       *(UInteger8*) (((UInteger8*)buf) + 19)
      );

  DBGM(" (20) Clock identity........... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 20),
       *(UInteger8*) (((UInteger8*)buf) + 21),
       *(UInteger8*) (((UInteger8*)buf) + 22),
       *(UInteger8*) (((UInteger8*)buf) + 23),
       *(UInteger8*) (((UInteger8*)buf) + 24),
       *(UInteger8*) (((UInteger8*)buf) + 25),
       *(UInteger8*) (((UInteger8*)buf) + 26),
       *(UInteger8*) (((UInteger8*)buf) + 27)
      );
  DBGM(" (28) Port identity............ %02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 28),
       *(UInteger8*) (((UInteger8*)buf) + 29)
      );
  DBGM(" (30) Sequence ID.............. %02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 30),
       *(UInteger8*) (((UInteger8*)buf) + 31)
      );
  DBGM(" (31) Control Field............ %d\n", *(UInteger8*) (((UInteger8*)buf) + 32));
  DBGM(" (32) Log Mean Message Interval %d\n", *(UInteger8*) (((UInteger8*)buf) + 33));
  DBGM(" (34) Time: epoch.............. %02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 34),
       *(UInteger8*) (((UInteger8*)buf) + 35)
      );
  DBGM(" (36) Time: seconds............ %02hhx:%02hhx:%02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 36),
       *(UInteger8*) (((UInteger8*)buf) + 37),
       *(UInteger8*) (((UInteger8*)buf) + 38),
       *(UInteger8*) (((UInteger8*)buf) + 39)
      );
  DBGM(" (40) Time: nanoseconds........ %02hhx:%02hhx:%02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 40),
       *(UInteger8*) (((UInteger8*)buf) + 41),
       *(UInteger8*) (((UInteger8*)buf) + 42),
       *(UInteger8*) (((UInteger8*)buf) + 43)
      );
}

void debug_dump_ptp_v2_port_identity_field (void * buf)
{

  DBGM(" (44) Clock Identity            %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 44),
       *(UInteger8*) (((UInteger8*)buf) + 45),
       *(UInteger8*) (((UInteger8*)buf) + 46),
       *(UInteger8*) (((UInteger8*)buf) + 47),
       *(UInteger8*) (((UInteger8*)buf) + 48),
       *(UInteger8*) (((UInteger8*)buf) + 49),
       *(UInteger8*) (((UInteger8*)buf) + 50),
       *(UInteger8*) (((UInteger8*)buf) + 51)
      );
  DBGM(" (52) Port Identity             %02hhx:%02hhx\n",
       *(UInteger8*) (((UInteger8*)buf) + 52),
       *(UInteger8*) (((UInteger8*)buf) + 53)
      );
}


#endif

Boolean msgPeek(void *buf, ssize_t length)
{
  /* not implemented, just return TRUE */
  return TRUE;
}

UInteger8 msgGetPtpVersion (void * buf)
{
  return ( *(((UInteger8*)buf)+1) & 0x0F);
}

void msgUnpackHeader(void *buf, MsgHeader *header)
{
  DBGM("msgUnpackHeader:\n");

  header->versionPTP                     = flip16(*(UInteger16*)(((UInteger8*)buf) + 0));
  header->versionNetwork                 = flip16(*(UInteger16*)(((UInteger8*)buf) + 2));
  memcpy(header->subdomain,                                     (((UInteger8*)buf) + 4), 16);
  header->messageType                    =        *(UInteger8*) (((UInteger8*)buf) + 20);
  header->sourceCommunicationTechnology  =        *(UInteger8*) (((UInteger8*)buf) + 21);
  memcpy(header->sourceUuid,                                    (((UInteger8*)buf) + 22), 6);
  header->sourcePortId                   = flip16(*(UInteger16*)(((UInteger8*)buf) + 28));
  header->sequenceId                     = flip16(*(UInteger16*)(((UInteger8*)buf) + 30));
  header->control                        =        *(UInteger8*) (((UInteger8*)buf) + 32);
  memcpy(header->flags,                                         (((UInteger8*)buf) + 34), 2);

  DBGM(" versionPTP.................... %u\n", header->versionPTP);
  DBGM(" versionNetwork................ %u\n", header->versionNetwork);
  DBGM(" subdomain..................... %s\n", header->subdomain);
  DBGM(" messageType................... %u\n", header->messageType);
  DBGM(" sourceCommTechnology.......... %u\n", header->sourceCommunicationTechnology);
  DBGM(" sourceUuid.................... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       header->sourceUuid[0], header->sourceUuid[1], header->sourceUuid[2],
       header->sourceUuid[3], header->sourceUuid[4], header->sourceUuid[5]
      );
  DBGM(" sourcePortId.................. %d\n", header->sourcePortId);
  DBGM(" sequenceId.................... %d\n", header->sequenceId);
  DBGM(" control....................... %d\n", header->control);
  DBGM(" flags......................... %02hhx %02hhx\n",
       header->flags[0],
       header->flags[1]
      );
}

void msgUnpackV2Header(void *buf, V2MsgHeader *header)
{
UInteger64 temp64BitInteger;

  DBGM("msgUnpackV2Header:\n");
#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
#endif

  header->transportSpecificAndMessageType = *(UInteger8*)        (((UInteger8*)buf) + 0);
  header->reserved1AndVersionPTP          = *(UInteger8*)        (((UInteger8*)buf) + 1);
  header->messageLength                   = flip16(*(UInteger16*)(((UInteger8*)buf) + 2));
  header->domainNumber                    = *(UInteger8*)        (((UInteger8*)buf) + 4);
  header->reserved2                       = *(UInteger8*)        (((UInteger8*)buf) + 5);
  memcpy(header->flags,                                          (((UInteger8*)buf) + 6), 2);
  temp64BitInteger                        = flip32(*(UInteger32*)(((UInteger8*)buf) + 8));
  temp64BitInteger = temp64BitInteger << 32 ;
  header->correctionField                 = flip32(*(UInteger32*)(((UInteger8*)buf) + 12))
                                          | temp64BitInteger;
  header->reserved3                       = flip32(*(UInteger32*)(((UInteger8*)buf) + 16));

  memcpy(header->sourcePortId.clockIdentity,                     (((UInteger8*)buf) + 20), 8);
  header->sourcePortId.portNumber         = flip16(*(UInteger16*)(((UInteger8*)buf) + 28));
  header->sequenceId                      = flip16(*(UInteger16*)(((UInteger8*)buf) + 30));
  header->control                         = *(UInteger8*)        (((UInteger8*)buf) + 32);
  header->logMeanMessageInterval          = *(UInteger8*)        (((UInteger8*)buf) + 33);

  DBGM(" Transport Specific and Type... %02hhx\n",
       header->transportSpecificAndMessageType
      );
  DBGM(" versionPTP.................... %02hhx\n",
       header->reserved1AndVersionPTP
      );
  DBGM(" messageLength................. %u\n",
       header->messageLength
      );

  DBGM(" domainNumber.................. %u\n",
       header->domainNumber
      );
  DBGM(" flags......................... %02hhx:%02hhx\n",
       header->flags[0],
       header->flags[1]
      );
  DBGM(" correctionField............... %16.16llx\n", header->correctionField);
  DBGM(" clockIdentity................. %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       header->sourcePortId.clockIdentity[0],
       header->sourcePortId.clockIdentity[1],
       header->sourcePortId.clockIdentity[2],
       header->sourcePortId.clockIdentity[3],
       header->sourcePortId.clockIdentity[4],
       header->sourcePortId.clockIdentity[5],
       header->sourcePortId.clockIdentity[6],
       header->sourcePortId.clockIdentity[7]
      ); 
  DBGM(" sourcePortId.................. %u\n",    header->sourcePortId.portNumber);
  DBGM(" sequenceId.................... %4.4x\n", header->sequenceId);
  DBGM(" control....................... %d\n",    header->control);
  DBGM(" logMeanMessageInterval........ %d\n",    header->logMeanMessageInterval);  
}

void msgUnpackV2Sync(void *buf, V2MsgSync *sync)
{
  DBGM("msgUnpackV2Sync:\n");

  sync->originTimestamp.epoch_number = flip16(*(UInteger16*)(((UInteger8*)buf) + 34));
  sync->originTimestamp.seconds      = flip32(*(UInteger32*)(((UInteger8*)buf) + 36));
  sync->originTimestamp.nanoseconds  = flip32(*(Integer32*) (((UInteger8*)buf) + 40));

  DBGM(" originTimestamp.epoch_number.. %u\n", sync->originTimestamp.epoch_number);
  DBGM(" originTimestamp.seconds....... %u\n", sync->originTimestamp.seconds);
  DBGM(" originTimestamp.nanoseconds... %d\n", sync->originTimestamp.nanoseconds);
}

void msgUnpackAnnounce(void *buf, MsgAnnounce *announce)
{
  DBGM("msgUnpackAnnounce:\n");

  /* Note for code below: stepsRemoved field is a 16 bit field that is not 16
   * bit aligned which is why the code gets one byte at a time to avoid
   * a possible alignment violation in some CPUs
   */

  announce->originTimestamp.epoch_number   = flip16(*(UInteger16*)(((UInteger8*)buf) + 34));
  announce->originTimestamp.seconds        = flip32(*(UInteger32*)(((UInteger8*)buf) + 36));
  announce->originTimestamp.nanoseconds    = flip32(*(Integer32*) (((UInteger8*)buf) + 40));
  announce->currentUTCOffset               = flip16(*(Integer16*) (((UInteger8*)buf) + 44));
  announce->reserved                       =        *(UInteger8*) (((UInteger8*)buf) + 46);
  announce->grandmasterPriority1           =        *(UInteger8*) (((UInteger8*)buf) + 47);
  announce->grandmasterClockQuality.clockClass    = *(UInteger8*) (((UInteger8*)buf) + 48);
  announce->grandmasterClockQuality.clockAccuracy = *(UInteger8*) (((UInteger8*)buf) + 49); 
  announce->grandmasterClockQuality.offsetScaledLogVariance 
                                           = flip16(*(UInteger16*)(((UInteger8*)buf) + 50));
  announce->grandmasterPriority2           =        *(UInteger8*) (((UInteger8*)buf) + 52);
  memcpy(announce->grandmasterIdentity,                           (((UInteger8*)buf) + 53), 8);
  
  /* NOTE: stepRemoved is not 16 bit aligned in the message. 
   * We therfore get it as two single byte reads instead
   * for CPUs that need alignment to work properly
   */
  announce->stepsRemoved         =  ( (UInteger16) (*(UInteger8*) (((UInteger8*)buf) + 61) ) << 8)
                                 |  ( (UInteger16) (*(UInteger8*) (((UInteger8*)buf) + 62) ) );
  announce->timeSource                     =        *(UInteger8*) (((UInteger8*)buf) + 63);


  DBGM(" originTimestamp.epoch_number.. %u\n", announce->originTimestamp.epoch_number);
  DBGM(" originTimestamp.seconds....... %u\n", announce->originTimestamp.seconds);
  DBGM(" originTimestamp.nanoseconds... %d\n", announce->originTimestamp.nanoseconds);
  DBGM(" currentUTCOffset.............. %d\n", announce->currentUTCOffset);
  DBGM(" grandmasterPriority1.......... %u\n", announce->grandmasterPriority1);
  DBGM(" clockClass.................... %u\n", announce->grandmasterClockQuality.clockClass);
  DBGM(" clockAccuracy................. %u\n", announce->grandmasterClockQuality.clockAccuracy);
  DBGM(" offsetScaledLogVariance....... %u\n", 
       announce->grandmasterClockQuality.offsetScaledLogVariance
      );
  DBGM(" grandmasterPriority2.......... %u\n", announce->grandmasterPriority2);
  DBGM(" grandmasterIdentity........... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       announce->grandmasterIdentity[0],
       announce->grandmasterIdentity[1],
       announce->grandmasterIdentity[2],
       announce->grandmasterIdentity[3],
       announce->grandmasterIdentity[4],
       announce->grandmasterIdentity[5],
       announce->grandmasterIdentity[6],
       announce->grandmasterIdentity[7]
      ); 
  DBGM(" stepsRemoved.................. %u\n", announce->stepsRemoved);
  DBGM(" timeSource.................... %u\n", announce->timeSource);
}


void msgUnpackSync(void *buf, MsgSync *sync)
{
  DBGM("msgUnpackSync:\n");
  sync->originTimestamp.seconds            = flip32(*(UInteger32*)(((UInteger8*)buf) + 40));
  sync->originTimestamp.nanoseconds        = flip32(*(Integer32*) (((UInteger8*)buf) + 44));
  sync->epochNumber                        = flip16(*(UInteger16*)(((UInteger8*)buf) + 48));
  sync->currentUTCOffset                   = flip16(*(Integer16*) (((UInteger8*)buf) + 50));
  sync->grandmasterCommunicationTechnology =        *(UInteger8*) (((UInteger8*)buf) + 53);
  memcpy(sync->grandmasterClockUuid,                              (((UInteger8*)buf) + 54), 6);
  sync->grandmasterPortId                  = flip16(*(UInteger16*)(((UInteger8*)buf) + 60));
  sync->grandmasterSequenceId              = flip16(*(UInteger16*)(((UInteger8*)buf) + 62));
  sync->grandmasterClockStratum            =        *(UInteger8*) (((UInteger8*)buf) + 67);
  memcpy(sync->grandmasterClockIdentifier,                        (((UInteger8*)buf) + 68), 4);
  sync->grandmasterClockVariance           = flip16(*(Integer16*) (((UInteger8*)buf) + 74));
  sync->grandmasterPreferred               =        *(UInteger8*) (((UInteger8*)buf) + 77);
  sync->grandmasterIsBoundaryClock         =        *(UInteger8*) (((UInteger8*)buf) + 79);
  sync->syncInterval                       =        *(Integer8*)  (((UInteger8*)buf) + 83);
  sync->localClockVariance                 = flip16(*(Integer16*) (((UInteger8*)buf) + 86));
  sync->localStepsRemoved                  = flip16(*(UInteger16*)(((UInteger8*)buf) + 90));
  sync->localClockStratum                  =        *(UInteger8*) (((UInteger8*)buf) + 95);
  memcpy(sync->localClockIdentifer,                               (((UInteger8*)buf) + 96),
         PTP_CODE_STRING_LENGTH
        );
  sync->parentCommunicationTechnology      =        *(UInteger8*) (((UInteger8*)buf) + 101);
  memcpy(sync->parentUuid,                                        (((UInteger8*)buf) + 102),
         PTP_UUID_LENGTH
        );
  sync->parentPortField                    = flip16(*(UInteger16*)(((UInteger8*)buf) + 110));
  sync->estimatedMasterVariance            = flip16(*(Integer16*) (((UInteger8*)buf) + 114));
  sync->estimatedMasterDrift               = flip32(*(Integer32*) (((UInteger8*)buf) + 116));
  sync->utcReasonable                      =        *(UInteger8*) (((UInteger8*)buf) + 123);


  DBGM(" originTimestamp.seconds....... %u\n",sync->originTimestamp.seconds);
  DBGM(" originTimestamp.nanoseconds... %d\n", sync->originTimestamp.nanoseconds);
  DBGM(" epochNumber................... %d\n", sync->epochNumber);
  DBGM(" currentUTCOffset.............. %d\n", sync->currentUTCOffset);
  DBGM(" grandmasterCommTechnology..... %d\n", sync->grandmasterCommunicationTechnology);
  DBGM(" grandmasterClockUuid.......... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       sync->grandmasterClockUuid[0], sync->grandmasterClockUuid[1], 
       sync->grandmasterClockUuid[2], sync->grandmasterClockUuid[3],
       sync->grandmasterClockUuid[4], sync->grandmasterClockUuid[5]
      );
  DBGM(" grandmasterPortId............. %d\n", sync->grandmasterPortId);
  DBGM(" grandmasterSequenceId......... %d\n", sync->grandmasterSequenceId);
  DBGM(" grandmasterClockStratum....... %d\n", sync->grandmasterClockStratum);
  DBGM(" grandmasterClockIdentifier.... %c%c%c%c\n",
       sync->grandmasterClockIdentifier[0],
       sync->grandmasterClockIdentifier[1],
       sync->grandmasterClockIdentifier[2],
       sync->grandmasterClockIdentifier[3]
      );
  DBGM(" grandmasterClockVariance...... %d\n", sync->grandmasterClockVariance);
  DBGM(" grandmasterPreferred.......... %d\n", sync->grandmasterPreferred);
  DBGM(" grandmasterIsBoundaryClock.... %d\n", sync->grandmasterIsBoundaryClock);
  DBGM(" syncInterval.................. %d\n", sync->syncInterval);
  DBGM(" localClockVariance............ %d\n", sync->localClockVariance);
  DBGM(" localStepsRemoved............. %d\n", sync->localStepsRemoved);
  DBGM(" localClockStratum............. %d\n", sync->localClockStratum);
  DBGM(" localClockIdentifer........... %c%c%c%c\n", 
       sync->localClockIdentifer[0],
       sync->localClockIdentifer[1],
       sync->localClockIdentifer[2],
       sync->localClockIdentifer[3]
      );
  DBGM(" parentCommunicationTechnology. %d\n", sync->parentCommunicationTechnology);
  DBGM(" parentUuid.................... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       sync->parentUuid[0], sync->parentUuid[1], sync->parentUuid[2],
       sync->parentUuid[3], sync->parentUuid[4], sync->parentUuid[5]
      );
  DBGM(" parentPortField............... %d\n", sync->parentPortField);
  DBGM(" estimatedMasterVariance....... %d\n", sync->estimatedMasterVariance);
  DBGM(" estimatedMasterDrift.......... %d\n", sync->estimatedMasterDrift);
  DBGM(" utcReasonable................. %d\n", sync->utcReasonable);
}

/* Unpacking function for Delay Request is not needed as it is exactly
 * the same format as a Sync message, so msgUnpackSync is used instead
 */
/*
 * void msgUnpackDelayReq(void *buf, MsgDelayReq *req) {}
 */

void msgUnpackV2FollowUp(void *buf, V2MsgFollowUp *follow)
{
  DBGM("msgUnpackV2FollowUp:\n");

  follow->preciseOriginTimestamp.epoch_number = flip16(*(UInteger16*)(((UInteger8*)buf) + 34));
  follow->preciseOriginTimestamp.seconds      = flip32(*(UInteger32*)(((UInteger8*)buf) + 36));
  follow->preciseOriginTimestamp.nanoseconds  = flip32(*(Integer32*) (((UInteger8*)buf) + 40));

  DBGM(" preciseOriginTimestamp.epoch.. %u\n", follow->preciseOriginTimestamp.epoch_number);
  DBGM(" preciseOriginTimestamp.secs... %u\n", follow->preciseOriginTimestamp.seconds);
  DBGM(" preciseOriginTimestamp.nsecs.. %d\n", follow->preciseOriginTimestamp.nanoseconds);
}


void msgUnpackFollowUp(void *buf, MsgFollowUp *follow)
{
  DBGM("msgUnpackFollowUp:\n");

  follow->associatedSequenceId               = flip16(*(UInteger16*)(((UInteger8*)buf) + 42));
  follow->preciseOriginTimestamp.seconds     = flip32(*(UInteger32*)(((UInteger8*)buf) + 44));
  follow->preciseOriginTimestamp.nanoseconds = flip32(*(Integer32*) (((UInteger8*)buf) + 48));

  DBGM(" associatedSequenceId.......... %u\n", follow->associatedSequenceId);
  DBGM(" preciseOriginTimestamp.secs... %u\n", follow->preciseOriginTimestamp.seconds);
  DBGM(" preciseOriginTimestamp.nsecs.. %d\n", follow->preciseOriginTimestamp.nanoseconds);
}

void msgUnpackV2DelayResp(void *buf, V2MsgDelayResp *resp)
{
  DBGM("msgUnpackV2DelayResp:\n");

  resp->receiveTimestamp.epoch_number = flip16(*(UInteger16*) (((UInteger8*)buf) + 34));
  resp->receiveTimestamp.seconds      = flip32(*(UInteger32*) (((UInteger8*)buf) + 36));
  resp->receiveTimestamp.nanoseconds  = flip32(*(Integer32*)  (((UInteger8*)buf) + 40));
  memcpy(resp->requestingPortId.clockIdentity,                (((UInteger8*)buf) + 44), 8);
  resp->requestingPortId.portNumber    = flip16(*(UInteger16*)(((UInteger8*)buf) + 52));

  DBGM(" receiveTimestamp.epoch_number. %u\n", resp->receiveTimestamp.epoch_number);
  DBGM(" receiveTimestamp.seconds...... %u\n", resp->receiveTimestamp.seconds);
  DBGM(" receiveTimestamp.nanoseconds.. %d\n", resp->receiveTimestamp.nanoseconds);
  DBGM(" Requesting port clockIdentity. %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       resp->requestingPortId.clockIdentity[0],
       resp->requestingPortId.clockIdentity[1],
       resp->requestingPortId.clockIdentity[2],
       resp->requestingPortId.clockIdentity[3],
       resp->requestingPortId.clockIdentity[4],
       resp->requestingPortId.clockIdentity[5],
       resp->requestingPortId.clockIdentity[6],
       resp->requestingPortId.clockIdentity[7]
      ); 
  DBGM(" Requesting port number........ %d\n", resp->requestingPortId.portNumber);
}

void msgUnpackV2PDelayResp(void *buf, V2MsgPDelayResp *resp)
{
  DBGM("msgUnpackV2DelayResp:\n");

  resp->requestReceiptTimestamp.epoch_number = flip16(*(UInteger16*)(((UInteger8*)buf) + 34));
  resp->requestReceiptTimestamp.seconds      = flip32(*(UInteger32*)(((UInteger8*)buf) + 36));
  resp->requestReceiptTimestamp.nanoseconds  = flip32(*(Integer32*) (((UInteger8*)buf) + 40));
  memcpy(resp->requestingPortId.clockIdentity,                      (((UInteger8*)buf) + 44), 8);
  resp->requestingPortId.portNumber          = flip16(*(UInteger16*)(((UInteger8*)buf) + 52));

  DBGM(" requestReceiptTimestamp.epoch. %u\n", resp->requestReceiptTimestamp.epoch_number);
  DBGM(" requestReceiptTimestamp.secs.. %u\n", resp->requestReceiptTimestamp.seconds);
  DBGM(" requestReceiptTimestamp.nsecs. %d\n", resp->requestReceiptTimestamp.nanoseconds);
  DBGM(" Requesting port clockIdentity. %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       resp->requestingPortId.clockIdentity[0],
       resp->requestingPortId.clockIdentity[1],
       resp->requestingPortId.clockIdentity[2],
       resp->requestingPortId.clockIdentity[3],
       resp->requestingPortId.clockIdentity[4],
       resp->requestingPortId.clockIdentity[5],
       resp->requestingPortId.clockIdentity[6],
       resp->requestingPortId.clockIdentity[7]
      ); 
  DBGM(" Requesting port number........ %d\n", resp->requestingPortId.portNumber);
}

void msgUnpackV2PDelayRespFollowUp(void *buf, V2MsgPDelayRespFollowUp *resp)
{
  DBGM("msgUnpackV2DelayRespFollowUp:\n");

  resp->responseOriginTimestamp.epoch_number = flip16(*(UInteger16*)(((UInteger8*)buf) + 34));
  resp->responseOriginTimestamp.seconds      = flip32(*(UInteger32*)(((UInteger8*)buf) + 36));
  resp->responseOriginTimestamp.nanoseconds  = flip32(*(Integer32*) (((UInteger8*)buf) + 40));
  memcpy(resp->requestingPortId.clockIdentity,                      (((UInteger8*)buf) + 44), 8);
  resp->requestingPortId.portNumber          = flip16(*(UInteger16*)(((UInteger8*)buf) + 52));

  DBGM(" responseOriginTimestamp.epoch. %u\n", resp->responseOriginTimestamp.epoch_number);
  DBGM(" responseOriginTimestamp.secs.. %u\n", resp->responseOriginTimestamp.seconds);
  DBGM(" responseOriginTimestamp.nsecs. %d\n", resp->responseOriginTimestamp.nanoseconds);
  DBGM(" Requesting port clockIdentity. %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       resp->requestingPortId.clockIdentity[0],
       resp->requestingPortId.clockIdentity[1],
       resp->requestingPortId.clockIdentity[2],
       resp->requestingPortId.clockIdentity[3],
       resp->requestingPortId.clockIdentity[4],
       resp->requestingPortId.clockIdentity[5],
       resp->requestingPortId.clockIdentity[6],
       resp->requestingPortId.clockIdentity[7]
      ); 
  DBGM(" Requesting port number........ %d\n", resp->requestingPortId.portNumber);
}

void msgUnpackDelayResp(void *buf, MsgDelayResp *resp)
{
  DBGM("msgUnpackDelayResp:\n");
  resp->delayReceiptTimestamp.seconds           = flip32(*(UInteger32*)(((UInteger8*)buf) + 40));
  resp->delayReceiptTimestamp.nanoseconds       = flip32(*(Integer32*) (((UInteger8*)buf) + 44));
  resp->requestingSourceCommunicationTechnology =        *(UInteger8*) (((UInteger8*)buf) + 49);
  memcpy(resp->requestingSourceUuid,                                   (((UInteger8*)buf) + 50), 6);
  resp->requestingSourcePortId                  = flip16(*(UInteger16*)(((UInteger8*)buf) + 56));
  resp->requestingSourceSequenceId              = flip16(*(UInteger16*)(((UInteger8*)buf) + 58));

  DBGM(" delayReceiptTimestamp.secs.... %u\n", resp->delayReceiptTimestamp.seconds);
  DBGM(" delayReceiptTimestamp.nsecs... %d\n", resp->delayReceiptTimestamp.nanoseconds);
  DBGM(" requestingSourceCommTechnology %u\n",
       resp->requestingSourceCommunicationTechnology
      );
  DBGM(" requestingSourceUuid.......... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       resp->requestingSourceUuid[0],
       resp->requestingSourceUuid[1],
       resp->requestingSourceUuid[2],
       resp->requestingSourceUuid[3],
       resp->requestingSourceUuid[4],
       resp->requestingSourceUuid[5]
      );
  DBGM(" requestingSourcePortId........ %u\n", resp->requestingSourcePortId);
  DBGM(" requestingSourceSequenceId.... %u\n", resp->requestingSourceSequenceId);
}

void msgUnpackManagement(void *buf, MsgManagement *manage)
{
  DBGM("msgUnpackManagement :\n");
  manage->targetCommunicationTechnology =        *(UInteger8*) (((UInteger8*)buf) + 41);
  memcpy(manage->targetUuid,                                   (((UInteger8*)buf) + 42), 6);
  manage->targetPortId                  = flip16(*(UInteger16*)(((UInteger8*)buf) + 48));
  manage->startingBoundaryHops          = flip16(*(Integer16*) (((UInteger8*)buf) + 50));
  manage->boundaryHops                  = flip16(*(Integer16*) (((UInteger8*)buf) + 52));
  manage->managementMessageKey          =        *(UInteger8*) (((UInteger8*)buf) + 55);
  manage->parameterLength               = flip16(*(UInteger16*)(((UInteger8*)buf) + 58));

  DBGM(  " targetCommTechnology.......... %u\n", manage->targetCommunicationTechnology);
  DBGM(  " targetUuid.................... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
       manage->targetUuid[0], manage->targetUuid[1], manage->targetUuid[2],
       manage->targetUuid[3], manage->targetUuid[4], manage->targetUuid[5]
      );
  DBGM(  " targetPortId.................. %u\n", manage->targetPortId);
  DBGM(  " startingBoundaryHops.......... %d\n", manage->startingBoundaryHops);
  DBGM(  " boundaryHops.................. %d\n", manage->boundaryHops);
  DBGM(  " managementMessageKey.......... %u\n", manage->managementMessageKey);
  DBGM(  " parameterLength............... %u\n", manage->parameterLength);
  
  if(manage->managementMessageKey == PTP_MM_GET_FOREIGN_DATA_SET)
  {
    manage->recordKey                   = flip16(*(UInteger16*)(((UInteger8*)buf) + 62));
    DBGM(" recordKey..................... %u\n", manage->parameterLength);
  }
}

UInteger8 msgUnloadManagement(void          *buf,
                              MsgManagement *manage,
                              PtpClock      *ptpClock,
                              RunTimeOpts   *rtOpts
                             )
{
  TimeInternal internalTime;
  TimeRepresentation externalTime;
  
  switch(manage->managementMessageKey)
  {
  case PTP_MM_INITIALIZE_CLOCK:
    if(ptpClock->initializable)
      return PTP_INITIALIZING;
    break;
    
  case PTP_MM_GOTO_FAULTY_STATE:
    DBG("msgUnloadManagment: event FAULT_DETECTED (forced by management message)\n");
    return PTP_FAULTY;
    break;
    
  case PTP_MM_DISABLE_PORT:
    if(manage->targetPortId == 1)
    {
      DBG("msgUnloadManagment: event DESIGNATED_DISABLED\n");
      return PTP_DISABLED;
    }
    break;
    
  case PTP_MM_ENABLE_PORT:
    if(manage->targetPortId == 1)
    {
      DBG("msgUnloadManagment: event DESIGNATED_ENABLED\n");
      return PTP_INITIALIZING;
    }
    break;
    
  case PTP_MM_CLEAR_DESIGNATED_PREFERRED_MASTER:
    ptpClock->preferred = FALSE;
    DBG("msgUnloadManagment: set preffered master to FALSE\n");
    break;
      
  case PTP_MM_SET_DESIGNATED_PREFERRED_MASTER:
    ptpClock->preferred = TRUE;
    DBG("msgUnloadManagment: set preffered master to TRUE\n");
    break;
    
  case PTP_MM_DISABLE_BURST:
    DBG("msgUnloadManagment: ignoring Disable Burst command (not supported)\n");
    break;
    
  case PTP_MM_ENABLE_BURST:
    DBG("msgUnloadManagment: ignoring Enable Burst command (not supported)\n");
    break;
    
  case PTP_MM_SET_SYNC_INTERVAL:
    rtOpts->syncInterval                =        *(Integer8*)  (((UInteger8*)buf) + 63);
    DBG("msgUnloadManagment: set syncInterval to 0x%d\n", rtOpts->syncInterval);
    break;
    
  case PTP_MM_SET_SUBDOMAIN:
    memcpy(rtOpts->subdomainName,                              (((UInteger8*)buf) + 60), 16);
    DBG("msgUnloadManagment: set subdomainName to %s\n", rtOpts->subdomainName);
    break;
    
  case PTP_MM_SET_TIME:
    externalTime.seconds                = flip32(*(UInteger32*)(((UInteger8*)buf) + 60));
    externalTime.nanoseconds            = flip32(*(Integer32*) (((UInteger8*)buf) + 64));
    DBG("msgUnloadManagment: setting time to %d seconds, %d nanoseconds\n",
        externalTime.seconds,
        externalTime.nanoseconds
       );
    toInternalTime(&internalTime, &externalTime, &ptpClock->halfEpoch);
    setTime(&internalTime, ptpClock->current_utc_offset);
    break;
    
  case PTP_MM_UPDATE_DEFAULT_DATA_SET:
    if(!rtOpts->slaveOnly)
    {
      ptpClock->clock_stratum           =        *(UInteger8*) (((UInteger8*)buf) + 63);
    }
    memcpy(ptpClock->clock_identifier,                         (((UInteger8*)buf) + 64), 4);
    ptpClock->clock_v1_variance         = flip16(*(Integer16*) (((UInteger8*)buf) + 70));
    ptpClock->preferred                 =        *(UInteger8*) (((UInteger8*)buf) + 75);
    rtOpts->syncInterval                =        *(UInteger8*) (((UInteger8*)buf) + 79);
    memcpy(rtOpts->subdomainName,                              (((UInteger8*)buf) + 80), 16);
    break;
    
  case PTP_MM_UPDATE_GLOBAL_TIME_PROPERTIES:
    ptpClock->current_utc_offset        = flip16(*(Integer16*) (((UInteger8*)buf) + 62));
    ptpClock->leap_59                   =        *(UInteger8*) (((UInteger8*)buf) + 67);
    ptpClock->leap_61                   =        *(UInteger8*) (((UInteger8*)buf) + 71);
    ptpClock->epoch_number              = flip16(*(UInteger16*)(((UInteger8*)buf) + 74));
    break;
    
  default:
    break;
  }
  
  return ptpClock->port_state;
}

void msgUnpackManagementPayload(void          *buf, 
                                MsgManagement *manage
                               )
{
  switch(manage->managementMessageKey)
  {
  case PTP_MM_CLOCK_IDENTITY:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_CLOCK_IDENTITY\n");
    manage->payload.clockIdentity.clockCommunicationTechnology = *(UInteger8*)(((UInteger8*)buf) + 63);
    memcpy(manage->payload.clockIdentity.clockUuidField,                      (((UInteger8*)buf) + 64),
           PTP_UUID_LENGTH
          );
    manage->payload.clockIdentity.clockPortField       = flip16(*(UInteger16*)(((UInteger8*)buf) + 74));
    memcpy(manage->payload.clockIdentity.manufacturerIdentity,                (((UInteger8*)buf) + 76),
           MANUFACTURER_ID_LENGTH
          );
    break;
    
  case PTP_MM_DEFAULT_DATA_SET:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_DEFAULT_DATA_SET\n");
    manage->payload.defaultData.clockCommunicationTechnology = *(UInteger8*)(((UInteger8*)buf) + 63);
    memcpy(manage->payload.defaultData.clockUuidField,                      (((UInteger8*)buf) + 64),
           PTP_UUID_LENGTH
          ); 
    manage->payload.defaultData.clockPortField       = flip16(*(UInteger16*)(((UInteger8*)buf) + 74));
    manage->payload.defaultData.clockStratum                =  *(UInteger8*)(((UInteger8*)buf) + 79); /* 2010-09-06: AKB: Removed Uinteger16 */
    memcpy(manage->payload.defaultData.clockIdentifier,                     (((UInteger8*)buf) + 80),
           PTP_CODE_STRING_LENGTH
          );
    manage->payload.defaultData.clockVariance        = flip16(*(UInteger16*)(((UInteger8*)buf) + 86));
    manage->payload.defaultData.clockFollowupCapable         = *(UInteger8*)(((UInteger8*)buf) + 91);
    manage->payload.defaultData.preferred                    = *(UInteger8*)(((UInteger8*)buf) + 95);
    manage->payload.defaultData.initializable                = *(UInteger8*)(((UInteger8*)buf) + 99);
    manage->payload.defaultData.externalTiming               = *(UInteger8*)(((UInteger8*)buf) + 103);
    manage->payload.defaultData.isBoundaryClock              = *(UInteger8*)(((UInteger8*)buf) + 107);
    manage->payload.defaultData.syncInterval                 = *(UInteger8*)(((UInteger8*)buf) + 111);
    memcpy(manage->payload.defaultData.subdomainName,                       (((UInteger8*)buf) + 112),
           PTP_SUBDOMAIN_NAME_LENGTH
          );
    manage->payload.defaultData.numberPorts          = flip16(*(UInteger16*)(((UInteger8*)buf) + 130));
    manage->payload.defaultData.numberForeignRecords = flip16(*(UInteger16*)(((UInteger8*)buf) + 134));
    break;
    
  case PTP_MM_CURRENT_DATA_SET:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_CURRENT_DATA_SET\n");
    manage->payload.current.stepsRemoved                 = flip16(*(UInteger16*)(((UInteger8*)buf) + 62));
    manage->payload.current.offsetFromMaster.seconds     = flip32(*(UInteger32*)(((UInteger8*)buf) + 64));
    manage->payload.current.offsetFromMaster.nanoseconds = flip32(*(UInteger32*)(((UInteger8*)buf) + 68));
    manage->payload.current.oneWayDelay.seconds          = flip32(*(UInteger32*)(((UInteger8*)buf) + 72));
    manage->payload.current.oneWayDelay.nanoseconds      = flip32(*(Integer32*) (((UInteger8*)buf) + 76));
    break;
    
  case PTP_MM_PARENT_DATA_SET:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_PORT_DATA_SET\n");
    manage->payload.parent.parentCommunicationTechnology       = *(UInteger8*) (((UInteger8*)buf) + 63);
    memcpy(manage->payload.parent.parentUuid,                                  (((UInteger8*)buf) + 64),
           PTP_UUID_LENGTH
          );
    manage->payload.parent.parentPortId                 = flip16(*(UInteger16*)(((UInteger8*)buf) + 74));
    manage->payload.parent.parentLastSyncSequenceNumber = flip16(*(UInteger16*)(((UInteger8*)buf) + 74));
    manage->payload.parent.parentFollowupCapable        =        *(UInteger8*) (((UInteger8*)buf) + 83);
    manage->payload.parent.parentExternalTiming         =        *(UInteger8*) (((UInteger8*)buf) + 87);
    manage->payload.parent.parentVariance               = flip16(*(UInteger16*)(((UInteger8*)buf) + 90));
    manage->payload.parent.parentStats                  = *(UInteger8*)        (((UInteger8*)buf) + 85);
    manage->payload.parent.observedVariance             = flip16(*(Integer16*) (((UInteger8*)buf) + 98));
    manage->payload.parent.observedDrift                = flip32(*(Integer32*) (((UInteger8*)buf) + 100));
    manage->payload.parent.utcReasonable                =        *(UInteger8*) (((UInteger8*)buf) + 107);
    manage->payload.parent.grandmasterCommunicationTechnology =  *(UInteger8*) (((UInteger8*)buf) + 111);
    memcpy(manage->payload.parent.grandmasterUuidField,                        (((UInteger8*)buf) + 112),
           PTP_UUID_LENGTH
          );
    manage->payload.parent.grandmasterPortIdField       = flip16(*(UInteger16*)(((UInteger8*)buf) + 122));
    manage->payload.parent.grandmasterStratum           =        *(UInteger8*) (((UInteger8*)buf) + 127);
    memcpy(manage->payload.parent.grandmasterIdentifier,                       (((UInteger8*)buf) + 128),
           PTP_CODE_STRING_LENGTH
          );
    manage->payload.parent.grandmasterVariance          = flip16(*(Integer16*) (((UInteger8*)buf) + 134));
    manage->payload.parent.grandmasterPreferred         =        *(UInteger8*) (((UInteger8*)buf) + 139);
    manage->payload.parent.grandmasterIsBoundaryClock   =        *(UInteger8*) (((UInteger8*)buf) + 144);
    manage->payload.parent.grandmasterSequenceNumber    = flip16(*(UInteger16*)(((UInteger8*)buf) + 146));
    break;
    
  case PTP_MM_PORT_DATA_SET:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_FOREIGN_DATA_SET\n");
    manage->payload.port.returnedPortNumber             = flip16(*(UInteger16*)(((UInteger8*)buf) + 62));
    manage->payload.port.portState                      =        *(UInteger8*) (((UInteger8*)buf) + 67);
    manage->payload.port.lastSyncEventSequenceNumber    = flip16(*(UInteger16*)(((UInteger8*)buf) + 70));
    manage->payload.port.lastGeneralEventSequenceNumber = flip16(*(UInteger16*)(((UInteger8*)buf) + 74));
    manage->payload.port.portCommunicationTechnology    =        *(UInteger8*) (((UInteger8*)buf) + 79);
    memcpy(manage->payload.port.portUuidField,                                 (((UInteger8*)buf) + 80),
           PTP_UUID_LENGTH
          );
    manage->payload.port.portIdField                    = flip16(*(UInteger16*)(((UInteger8*)buf) + 90));
    manage->payload.port.burstEnabled                   =        *(UInteger8*) (((UInteger8*)buf) + 95);
    manage->payload.port.subdomainAddressOctets         =        *(UInteger8*) (((UInteger8*)buf) + 97);
    manage->payload.port.eventPortAddressOctets         =        *(UInteger8*) (((UInteger8*)buf) + 98);
    manage->payload.port.generalPortAddressOctets       =        *(UInteger8*) (((UInteger8*)buf) + 99);
    memcpy(manage->payload.port.subdomainAddress,                              (((UInteger8*)buf) + 100),
           SUBDOMAIN_ADDRESS_LENGTH
          );
    memcpy(manage->payload.port.eventPortAddress,                              (((UInteger8*)buf) + 106),
           PORT_ADDRESS_LENGTH
          );
    memcpy(manage->payload.port.generalPortAddress,                            (((UInteger8*)buf) + 110),
           PORT_ADDRESS_LENGTH
          );
    break;
  
  case PTP_MM_GLOBAL_TIME_DATA_SET:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_GLOBAL_TIME_DATA_SET\n");
    manage->payload.globalTime.localTime.seconds     = flip32(*(UInteger32*)(((UInteger8*)buf) + 60));
    manage->payload.globalTime.localTime.nanoseconds = flip32(*(Integer32*) (((UInteger8*)buf) + 64));
    manage->payload.globalTime.currentUtcOffset      = flip16(*(Integer16*) (((UInteger8*)buf) + 70));
    manage->payload.globalTime.leap59                =        *(UInteger8*) (((UInteger8*)buf) + 75);
    manage->payload.globalTime.leap61                =        *(UInteger8*) (((UInteger8*)buf) + 79);
    manage->payload.globalTime.epochNumber           = flip16(*(UInteger16*)(((UInteger8*)buf) + 82));
    break;
    
    
  case PTP_MM_FOREIGN_DATA_SET:
    DBGM("msgUnloadManagementPayload: managementMessageKey PTP_MM_FOREIGN_DATA_SET\n");
    manage->payload.foreign.returnedPortNumber           = flip16(*(UInteger16*)(((UInteger8*)buf) + 62));
    manage->payload.foreign.returnedRecordNumber         = flip16(*(UInteger16*)(((UInteger8*)buf) + 68));
    manage->payload.foreign.foreignMasterCommunicationTechnology = *(UInteger8*)(((UInteger8*)buf) + 71);
    memcpy(manage->payload.foreign.foreignMasterUuid,                           (((UInteger8*)buf) + 72),
           PTP_UUID_LENGTH
          );
    manage->payload.foreign.foreignMasterPortId          = flip16(*(UInteger16*)(((UInteger8*)buf) + 82));
    manage->payload.foreign.foreignMasterSyncs           = flip16(*(UInteger16*)(((UInteger8*)buf) + 66));
    break;
    
  case PTP_MM_NULL:
    DBGM("msgUnloadManagementPayload: managementMessageKey NULL\n");
    break;
    
  default:
    DBGM("msgUnloadManagementPayload: managementMessageKey ?\n");
    break;
  }
  
  return;
}

void msgPackHeader(void     *buf,
                   PtpClock *ptpClock
                  )
{
  *(Integer32*)(((UInteger8*)buf) + 0) =  shift16(flip16(VERSION_PTP),     0) 
                          | shift16(flip16(VERSION_NETWORK), 1);
  memcpy(      (((UInteger8*)buf) + 4),   ptpClock->subdomain_name, 16);
  *(Integer32*)(((UInteger8*)buf) + 20) = shift8(ptpClock->port_communication_technology, 1);
  memcpy(      (((UInteger8*)buf) + 22),  ptpClock->port_uuid_field, 6);
  
  if(ptpClock->external_timing)
    setFlag(   (((UInteger8*)buf) + 34), PTP_EXT_SYNC);
  if(ptpClock->clock_followup_capable)
    setFlag(   (((UInteger8*)buf) + 34), PTP_ASSIST);
  if(ptpClock->is_boundary_clock)
    setFlag(   (((UInteger8*)buf) + 34), PTP_BOUNDARY_CLOCK);
}

void msgPackV2Header(void     *buf,
                     PtpClock *ptpClock
                    )
{
  *(UInteger8*) (((UInteger8*)buf) + 0)  =  ptpClock->tx_transport_specific;
  *(UInteger8*) (((UInteger8*)buf) + 1)  =  2;

  /* Message length set by other pack routines */

  *(UInteger8*) (((UInteger8*)buf) + 4)  =  ptpClock->domain_number;
  *(UInteger8*) (((UInteger8*)buf) + 5)  =  0;  /* reserved2 */
  *(UInteger16*)(((UInteger8*)buf) + 6)  =  0;  /* clear flags (set to values by other pack routines) */

  /* Correction field set by other pack routines */

  *(Integer32*) (((UInteger8*)buf) + 16) =  0;  /* reserved3 */
  memcpy(       (((UInteger8*)buf) + 20),   ptpClock->port_clock_identity, 8);
  *(UInteger16*)(((UInteger8*)buf) + 28) =  flip16(ptpClock->port_id_field);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
#endif

}

void msgPackAnnounce(void                 *buf, 
                     Boolean               unicast,
                     V2TimeRepresentation *originTimestamp,
                     PtpClock             *ptpClock
                    )
{
  DBGM("msgPackAnnounce:\n");
  /* PTP Header */
  /* Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_ANNOUNCE_MESSAGE;

  /* Length */
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_ANNOUNCE_LENGTH); /* Length */

  /* Flags */
  *(UInteger16*)  (((UInteger8*)buf) + 6)  = 0;  /* Init both flag bytes to zero */
  if (ptpClock->leap_61)
     *(UInteger8*)(((UInteger8*)buf) + 7)  |= ANNOUNCE_LI_61;
  if (ptpClock->leap_59)
     *(UInteger8*)(((UInteger8*)buf) + 7)  |= ANNOUNCE_LI_59;
  if (ptpClock->current_utc_offset_valid)
     *(UInteger8*)(((UInteger8*)buf) + 7)  |= ANNOUNCE_CURRENT_UTC_OFFSET_VALID;
  if (ptpClock->ptp_timescale)
     *(UInteger8*)(((UInteger8*)buf) + 7)  |= ANNOUNCE_PTP_TIMESCALE;
  if (ptpClock->time_traceable)
     *(UInteger8*)(((UInteger8*)buf) + 7)  |= ANNOUNCE_TIME_TRACEABLE;
  if (ptpClock->frequency_traceable)
     *(UInteger8*)(((UInteger8*)buf) + 7)  |= ANNOUNCE_FREQUENCY_TRACEABLE;  

  *(Integer64*) (((UInteger8*)buf) + 8)  =  0;  /* correctionField */

  /* Sequence # */

  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(ptpClock->last_announce_tx_sequence_number);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_ALL_OTHERS_CONTROL;
  *(UInteger8*)   (((UInteger8*)buf) + 33) =  ptpClock->announce_interval;

  /* Timestamp */

  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(originTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(originTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(originTimestamp->nanoseconds);

  /* Current UTC Offset */

  *(UInteger16*)  (((UInteger8*)buf) + 44) =  flip16(ptpClock->current_utc_offset);

  /* reserved */

  *(UInteger8*)   (((UInteger8*)buf) + 46) =  0;

  /* Grandmaster Priority1, Clock Quality, Priority2, Identity, Steps removed
   * and Time source.  Used for remote to run best master clock algorithm.
   * Note, some of these fields are byte aligned on odd addresses, so 
   * for portability, some 16 bit values are copied a byte at a time
   */

  *(UInteger8*)   (((UInteger8*)buf) + 47) =  ptpClock->grandmaster_priority1;
  *(UInteger8*)   (((UInteger8*)buf) + 48) =  ptpClock->grandmaster_clock_quality.clockClass;
  *(Enumeration8*)(((UInteger8*)buf) + 49) =  ptpClock->grandmaster_clock_quality.clockAccuracy;
  *(UInteger16*)  (((UInteger8*)buf) + 50) 
    = flip16(ptpClock->grandmaster_clock_quality.offsetScaledLogVariance);
  *(UInteger8*)   (((UInteger8*)buf) + 52) =  ptpClock->grandmaster_priority2;
  memcpy (        (((UInteger8*)buf) + 53),   ptpClock->parent_clock_identity, 8);
  *(UInteger8*)   (((UInteger8*)buf) + 61) =  ptpClock->steps_removed >> 8;
  *(UInteger8*)   (((UInteger8*)buf) + 62) =  ptpClock->steps_removed & 0xFF;
  *(UInteger8*)   (((UInteger8*)buf) + 63) =  ptpClock->time_source;  

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
#endif

}

void msgPackV2Sync(void                 *buf, 
                   Boolean               unicast,
                   V2TimeRepresentation *originTimestamp,
                   PtpClock             *ptpClock
                  )
{
  DBGM("msgPackV2Sync:\n");
  /* PTP Header */
  /* Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_SYNC_MESSAGE;        /* Message type */
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_SYNC_LENGTH); /* Length */

  /* Flags and Log Mean message interval based on if unicast or not */

  if (ptpClock->clock_followup_capable)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  =  V2_TWO_STEP_FLAG;
  }
  else
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  =  0;
  }

  *(UInteger8*)   (((UInteger8*)buf) + 7)  =  0; /* Initialize second flag byte to zero */

  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  |= V2_UNICAST_FLAG; 
    *(UInteger8*) (((UInteger8*)buf) + 33) =  LOGMEAN_UNICAST;
  }
  else
  {
    *(UInteger8*) (((UInteger8*)buf) + 33) =  ptpClock->sync_interval;
  }

  *(Integer64*)   (((UInteger8*)buf) + 8)  =  0;                      /* correctionField */

  /* Sequence number: */
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(ptpClock->last_sync_tx_sequence_number);

  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_SYNC_CONTROL;        /* Control */



  /* Timestamp: */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(originTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(originTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(originTimestamp->nanoseconds);

  DBGM("msgPackV2Sync:\n");

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
#endif

}

void msgPackSync(void               *buf, 
                 Boolean             burst,
                 TimeRepresentation *originTimestamp,
                 PtpClock           *ptpClock
                )
{

/* PTP header fields */

  *(UInteger8*)(((UInteger8*)buf) + 20) = 1;  /* Event messageType */
  *(Integer32*)(((UInteger8*)buf) + 28) =   shift16(flip16(ptpClock->port_id_field), 0)
                            | shift16(flip16(ptpClock->last_sync_tx_sequence_number), 1);
  *(UInteger8*)(((UInteger8*)buf) +32) = PTP_SYNC_MESSAGE;  /* control */
  if(ptpClock->burst_enabled && burst)
    setFlag(   (((UInteger8*)buf) + 34), PTP_SYNC_BURST);
  else
    clearFlag( (((UInteger8*)buf) + 34), PTP_SYNC_BURST);
  if(ptpClock->parent_stats)
    setFlag(   (((UInteger8*)buf) + 34), PARENT_STATS);
  else
    clearFlag( (((UInteger8*)buf) + 34), PARENT_STATS);
  
/* Sync message specific fields */
  
  *(Integer32*)(((UInteger8*)buf) + 40) =   flip32(originTimestamp->seconds);
  *(Integer32*)(((UInteger8*)buf) + 44) =   flip32(originTimestamp->nanoseconds);
  *(Integer32*)(((UInteger8*)buf) + 48) =   shift16(flip16(ptpClock->epoch_number), 0)
                            | shift16(flip16(ptpClock->current_utc_offset), 1);
  *(Integer32*)(((UInteger8*)buf) + 52) =   shift8(ptpClock->grandmaster_communication_technology, 1);
  memcpy(      (((UInteger8*)buf) + 54),    ptpClock->grandmaster_uuid_field, 6);
  *(Integer32*)(((UInteger8*)buf) + 60) =   shift16(flip16(ptpClock->grandmaster_port_id_field), 0)
                            | shift16(flip16(ptpClock->grandmaster_sequence_number), 1);
  *(Integer32*)(((UInteger8*)buf) + 64) =   shift8(ptpClock->grandmaster_stratum, 3);
  memcpy(      (((UInteger8*)buf) + 68),    ptpClock->grandmaster_identifier, 4);
  *(Integer32*)(((UInteger8*)buf) + 72) =   shift16(flip16(ptpClock->grandmaster_v1_variance), 1);
  *(Integer32*)(((UInteger8*)buf) + 76) =   shift16(flip16(ptpClock->grandmaster_preferred), 0)
                            | shift16(flip16(ptpClock->grandmaster_is_boundary_clock), 1);
  *(Integer32*)(((UInteger8*)buf) + 80) =   shift16(flip16(ptpClock->sync_interval), 1);
  *(Integer32*)(((UInteger8*)buf) + 84) =   shift16(flip16(ptpClock->clock_v1_variance), 1);
  *(Integer32*)(((UInteger8*)buf) + 88) =   shift16(flip16(ptpClock->steps_removed), 1);
  *(Integer32*)(((UInteger8*)buf) + 92) =   shift8(ptpClock->clock_stratum, 3);
  memcpy(      (((UInteger8*)buf) + 96),    ptpClock->clock_identifier, 4);
  *(Integer32*)(((UInteger8*)buf) + 100) =  shift8(ptpClock->parent_communication_technology, 1);
  memcpy(      (((UInteger8*)buf) + 102),   ptpClock->parent_uuid, 6);
  *(Integer32*)(((UInteger8*)buf) + 108) =  shift16(flip16(ptpClock->parent_port_id), 1);
  *(Integer32*)(((UInteger8*)buf) + 112) =  shift16(flip16(ptpClock->observed_v1_variance), 1);
  *(Integer32*)(((UInteger8*)buf) + 116) =  flip32(ptpClock->observed_drift);
  *(Integer32*)(((UInteger8*)buf) + 120) =  shift8(ptpClock->utc_reasonable, 3);
}

void msgPackV2DelayReq(void                 *buf, 
                       Boolean               unicast,
                       V2TimeRepresentation *originTimestamp,
                       PtpClock             *ptpClock
                      )
{
  DBGM("msgPackV2DelayReq:\n");
  /* PTP Header */
  /* Message type, length, flags, Sequence, Control, log mean message interval */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_DELAY_REQ_MESSAGE;
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_DELAY_REQ_LENGTH);
  *(UInteger16*)  (((UInteger8*)buf) + 6)  =  0;
  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6) |= V2_UNICAST_FLAG;
  } 
  *(Integer64*)   (((UInteger8*)buf) + 8)  =  0;  /* correctionField */
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(ptpClock->sentDelayReqSequenceId);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_DELAY_REQ_CONTROL;
  *(UInteger8*)   (((UInteger8*)buf) + 33) =  LOGMEAN_DELAY_REQ;

  /* Timestamp */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(originTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(originTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(originTimestamp->nanoseconds);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
#endif

}

void msgPackV2PDelayReq(void                 *buf, 
                        Boolean               unicast,
                        V2TimeRepresentation *originTimestamp,
                        PtpClock             *ptpClock
                       )
{
  DBGM("msgPackV2PDelayReq:\n");
  /* PTP Header */
  /* Message type, length, flags, Sequence, Control, log mean message interval */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_PDELAY_REQ_MESSAGE;
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_PDELAY_REQ_LENGTH);
  *(UInteger16*)  (((UInteger8*)buf) + 6)  =  0;
  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6) |= V2_UNICAST_FLAG;
  } 
  *(Integer64*)   (((UInteger8*)buf) + 8)  =  0;  /* correctionField */
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(ptpClock->sentDelayReqSequenceId);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_ALL_OTHERS_CONTROL;
  *(UInteger8*)   (((UInteger8*)buf) + 33) =  LOGMEAN_PDELAY_REQ;

  /* Timestamp */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(originTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(originTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(originTimestamp->nanoseconds);

  /* Reserved */
  memset(         (((UInteger8*)buf) + 44),   0, 10);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
  debug_dump_ptp_v2_port_identity_field(buf);
#endif

}

void msgPackDelayReq(void               *buf, 
                     Boolean             burst,
                     TimeRepresentation *originTimestamp,
                     PtpClock           *ptpClock
                    )
{

/* PTP header fields */

  *(UInteger8*)(((UInteger8*)buf) + 20) =  1;  /* Event messageType */
  *(Integer32*)(((UInteger8*)buf) + 28) =  shift16(flip16(ptpClock->port_id_field), 0)
                           | shift16(flip16(ptpClock->sentDelayReqSequenceId), 1);
  *(UInteger8*)(((UInteger8*)buf) + 32) =  PTP_DELAY_REQ_MESSAGE;  /* control */
  if(ptpClock->burst_enabled && burst)
    setFlag(   (((UInteger8*)buf) + 34),   PTP_SYNC_BURST);
  else
    clearFlag( (((UInteger8*)buf) + 34),   PTP_SYNC_BURST);
  if(ptpClock->parent_stats)
    setFlag(   (((UInteger8*)buf) + 34),   PARENT_STATS);
  else
    clearFlag( (((UInteger8*)buf) + 34),   PARENT_STATS);

/* Delay request specific fields */  

  *(Integer32*)(((UInteger8*)buf) + 40) =  flip32(originTimestamp->seconds);
  *(Integer32*)(((UInteger8*)buf) + 44) =  flip32(originTimestamp->nanoseconds);
  *(Integer32*)(((UInteger8*)buf) + 48) =  shift16(flip16(ptpClock->epoch_number), 0)
                           | shift16(flip16(ptpClock->current_utc_offset), 1);
  *(Integer32*)(((UInteger8*)buf) + 52) =  shift8(ptpClock->grandmaster_communication_technology, 1);
  memcpy(      (((UInteger8*)buf) + 54),   ptpClock->grandmaster_uuid_field, 6);
  *(Integer32*)(((UInteger8*)buf) + 60) =  shift16(flip16(ptpClock->grandmaster_port_id_field), 0)
                           | shift16(flip16(ptpClock->grandmaster_sequence_number), 1);
  *(Integer32*)(((UInteger8*)buf) + 64) =  shift8(ptpClock->grandmaster_stratum, 3);
  memcpy(      (((UInteger8*)buf) + 68),   ptpClock->grandmaster_identifier, 4);
  *(Integer32*)(((UInteger8*)buf) + 72) =  shift16(flip16(ptpClock->grandmaster_v1_variance), 1);
  *(Integer32*)(((UInteger8*)buf) + 76) =  shift16(flip16(ptpClock->grandmaster_preferred), 0)
                           | shift16(flip16(ptpClock->grandmaster_is_boundary_clock), 1);
  *(Integer32*)(((UInteger8*)buf) + 80) =  shift16(flip16(ptpClock->sync_interval), 1);
  *(Integer32*)(((UInteger8*)buf) + 84) =  shift16(flip16(ptpClock->clock_v1_variance), 1);
  *(Integer32*)(((UInteger8*)buf) + 88) =  shift16(flip16(ptpClock->steps_removed), 1);
  *(Integer32*)(((UInteger8*)buf) + 92) =  shift8(ptpClock->clock_stratum, 3);
  memcpy(      (((UInteger8*)buf) + 96),   ptpClock->clock_identifier, 4);
  *(Integer32*)(((UInteger8*)buf) + 100) = shift8(ptpClock->parent_communication_technology, 1);
  memcpy(      (((UInteger8*)buf) + 102),  ptpClock->parent_uuid, 6);
  *(Integer32*)(((UInteger8*)buf) + 108) = shift16(flip16(ptpClock->parent_port_id), 1);
  *(Integer32*)(((UInteger8*)buf) + 112) = shift16(flip16(ptpClock->observed_v1_variance), 1);
  *(Integer32*)(((UInteger8*)buf) + 116) = flip32(ptpClock->observed_drift);
  *(Integer32*)(((UInteger8*)buf) + 120) = shift8(ptpClock->utc_reasonable, 3);
}

void msgPackV2FollowUp(void                 *buf, 
                       Boolean               unicast,
                       UInteger16            associatedSequenceId,
                       V2TimeRepresentation *preciseOriginTimestamp,
                       PtpClock             *ptpClock
                      )
{
  DBGM("msgPackV2FollowUp\n");
  /* PTP Header */
  /* Message type, length, flags, Sequence, Control, log mean message interval */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_FOLLOWUP_MESSAGE;
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_FOLLOWUP_LENGTH);
  *(Integer64*)   (((UInteger8*)buf) + 8)  =  0;  /* correctionField */
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(associatedSequenceId);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_FOLLOWUP_CONTROL;
  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  |= V2_UNICAST_FLAG; 
    *(UInteger8*) (((UInteger8*)buf) + 33) =  LOGMEAN_UNICAST;
  }
  else
  {
    *(UInteger8*) (((UInteger8*)buf) + 33) =  ptpClock->sync_interval;
  }

  /* Timestamp */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(preciseOriginTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(preciseOriginTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(preciseOriginTimestamp->nanoseconds);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
#endif

}

void msgPackFollowUp(void               *buf,
                     UInteger16          associatedSequenceId,
                     TimeRepresentation *preciseOriginTimestamp,
                     PtpClock           *ptpClock
                    )
{

/* PTP header fields */

  *(UInteger8*)(((UInteger8*)buf) + 20) =  2;  /* General messageType */
  *(Integer32*)(((UInteger8*)buf) + 28) =  shift16(flip16(ptpClock->port_id_field), 0)
                           | shift16(flip16(ptpClock->last_general_event_sequence_number), 1);
  *(UInteger8*)(((UInteger8*)buf) + 32) =  PTP_FOLLOWUP_MESSAGE;  /* control */
  clearFlag(   (((UInteger8*)buf) + 34),   PTP_SYNC_BURST);
  clearFlag(   (((UInteger8*)buf) + 34),   PARENT_STATS);

/* Follow Up fields */
  
  *(Integer32*)(((UInteger8*)buf) + 40) = shift16(flip16(associatedSequenceId), 1);
  *(Integer32*)(((UInteger8*)buf) + 44) = flip32(preciseOriginTimestamp->seconds);
  *(Integer32*)(((UInteger8*)buf) + 48) = flip32(preciseOriginTimestamp->nanoseconds);
}

void msgPackV2DelayResp(void                 *buf, 
                        Boolean               unicast,
                        V2MsgHeader          *header,
                        V2TimeRepresentation *delayReceiptTimestamp,
                        PtpClock             *ptpClock
                       )
{
  DBGM("msgPackV2DelayResp:\n");
  /* PTP Header */
  /* Message type, length, flags, Sequence, Control, log mean message interval */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_DELAY_RESP_MESSAGE;
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_DELAY_RESP_LENGTH);
  *(UInteger8*)   (((UInteger8*)buf) + 4)  =  header->domainNumber;
  *(UInteger8*)   (((UInteger8*)buf) + 6)  =  0;
  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  |= V2_UNICAST_FLAG; 
    *(UInteger8*) (((UInteger8*)buf) + 33) =  LOGMEAN_UNICAST;
  }
  else
  {
    *(UInteger8*) (((UInteger8*)buf) + 33) =  ptpClock->delay_req_interval;
  }
  *(UInteger8*)   (((UInteger8*)buf) + 7)  =  0; /* Initialize second flag byte to zero */

  *(UInteger32*)  (((UInteger8*)buf) + 8)  =  flip32((UInteger32)(header->correctionField >> 32));
  *(UInteger32*)  (((UInteger8*)buf) + 12) =  flip32((UInteger32)(header->correctionField & 0xFFFFFFFF));
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(header->sequenceId);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_DELAY_RESP_CONTROL;


  /* Timestamp */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(delayReceiptTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(delayReceiptTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(delayReceiptTimestamp->nanoseconds);

  /* requestingPortId, copy from Delay Request header */

  memcpy((((UInteger8*)buf) + 44),
         header->sourcePortId.clockIdentity,  
         8
        );
  *(UInteger16*)  (((UInteger8*)buf) + 52) = flip16(header->sourcePortId.portNumber);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
  debug_dump_ptp_v2_port_identity_field(buf);
#endif

}

void msgPackV2PDelayResp(void                 *buf, 
                         Boolean               unicast,
                         V2MsgHeader          *header,
                         V2TimeRepresentation *requestReceiptTimestamp,
                         PtpClock             *ptpClock
                        )
{
  /* PTP Header */
  /* Message type, length, flags, Sequence, Control, log mean message interval */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_PDELAY_RESP_MESSAGE;
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_PDELAY_RESP_LENGTH);
  *(UInteger8*)   (((UInteger8*)buf) + 4)  =  header->domainNumber;
  if (ptpClock->clock_followup_capable)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  =  V2_TWO_STEP_FLAG;
  }
  else
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  =  0;
  }

  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  |= V2_UNICAST_FLAG; 
  }

  *(UInteger64*)  (((UInteger8*)buf) + 8)  =  0; /*correctionField */
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(header->sequenceId);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_ALL_OTHERS_CONTROL;
  *(UInteger8*)   (((UInteger8*)buf) + 33) =  LOGMEAN_PDELAY_RESP;


  /* Timestamp */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(requestReceiptTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(requestReceiptTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(requestReceiptTimestamp->nanoseconds);

  /* requestingPortId, copy from PDelay Request message header */

  memcpy((((UInteger8*)buf) + 44),
         header->sourcePortId.clockIdentity,
         8
        );
  *(UInteger16*)  (((UInteger8*)buf) + 52) = flip16(header->sourcePortId.portNumber);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
  debug_dump_ptp_v2_port_identity_field(buf);
#endif
}

void msgPackV2PDelayRespFollowUp(void                 *buf, 
                                 Boolean               unicast,
                                 V2MsgHeader          *header,
                                 V2TimeRepresentation *responseOriginTimestamp,
                                 PtpClock             *ptpClock
                                )
{
  /* PTP Header */
  /* Message type, length, flags, Sequence, Control, log mean message interval */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  &= 0xF0;                   /* Clear previous Message type */
  *(UInteger8*)   (((UInteger8*)buf) + 0)  |= V2_PDELAY_RESP_FOLLOWUP_MESSAGE;
  *(UInteger16*)  (((UInteger8*)buf) + 2)  =  flip16(V2_PDELAY_RESP_FOLLOWUP_LENGTH);
  *(UInteger8*)   (((UInteger8*)buf) + 4)  =  header->domainNumber;

  *(UInteger8*)   (((UInteger8*)buf) + 6)  =  0;  /* Flags */
  if (unicast)
  {
    *(UInteger8*) (((UInteger8*)buf) + 6)  |= V2_UNICAST_FLAG; 
  }

  /*correctionField */
  *(UInteger32*)  (((UInteger8*)buf) + 8)  =  flip32((UInteger32)(header->correctionField >> 32)); 
  *(UInteger32*)  (((UInteger8*)buf) + 12) =  flip32((UInteger32)(header->correctionField & 0xFFFFFFFF)); 
  *(UInteger16*)  (((UInteger8*)buf) + 30) =  flip16(header->sequenceId);
  *(UInteger8*)   (((UInteger8*)buf) + 32) =  V2_ALL_OTHERS_CONTROL;
  *(UInteger8*)   (((UInteger8*)buf) + 33) =  LOGMEAN_PDELAY_RESP_FOLLOWUP;


  /* Timestamp */
  *(UInteger16*)  (((UInteger8*)buf) + 34) =  flip16(responseOriginTimestamp->epoch_number);
  *(UInteger32*)  (((UInteger8*)buf) + 36) =  flip32(responseOriginTimestamp->seconds);
  *(UInteger32*)  (((UInteger8*)buf) + 40) =  flip32(responseOriginTimestamp->nanoseconds);

  /* requestingPortId, copy from PDelay Request message header */

  memcpy((((UInteger8*)buf) + 44),
         header->sourcePortId.clockIdentity,
         8
        );
  *(UInteger16*)  (((UInteger8*)buf) + 52) = flip16(header->sourcePortId.portNumber);

#ifdef PTPD_DBG
  debug_dump_ptp_v2_header(buf);
  debug_dump_ptp_v2_port_identity_field(buf);
#endif
}

void msgPackDelayResp(void               *buf, 
                      MsgHeader          *header,
                      TimeRepresentation *delayReceiptTimestamp, 
                      PtpClock           *ptpClock
                     )
{

/* PTP header fields */

  *(UInteger8*)(((UInteger8*)buf) + 20) =  2;  /* General messageType */
  *(Integer32*)(((UInteger8*)buf) + 28) =  shift16(flip16(ptpClock->port_id_field), 0)
                           | shift16(flip16(ptpClock->last_general_event_sequence_number), 1);
  *(UInteger8*)(((UInteger8*)buf) + 32) =  PTP_DELAY_RESP_MESSAGE;  /* control */
  clearFlag(   (((UInteger8*)buf) + 34),   PTP_SYNC_BURST);
  clearFlag(   (((UInteger8*)buf) + 34),   PARENT_STATS);
  
/* Delay Request fields */

  *(Integer32*)(((UInteger8*)buf) + 40) =  flip32(delayReceiptTimestamp->seconds);
  *(Integer32*)(((UInteger8*)buf) + 44) =  flip32(delayReceiptTimestamp->nanoseconds);
  *(Integer32*)(((UInteger8*)buf) + 48) =  shift8(header->sourceCommunicationTechnology, 1);
  memcpy(      (((UInteger8*)buf) + 50),   header->sourceUuid, 6);
  *(Integer32*)(((UInteger8*)buf) + 56) =  shift16(flip16(header->sourcePortId), 0)
                           | shift16(flip16(header->sequenceId), 1);
}

UInteger16 msgPackManagement(void          *buf, 
                             MsgManagement *manage,
                             PtpClock      *ptpClock
                            )
{

/* PTP header fields */

  *(UInteger8*)(((UInteger8*)buf) + 20) = 2;  /* General messageType */
  *(Integer32*)(((UInteger8*)buf) + 28) = shift16(flip16(ptpClock->port_id_field), 0)
                          | shift16(flip16(ptpClock->last_general_event_sequence_number), 1);
  *(UInteger8*)(((UInteger8*)buf) + 32) = PTP_MANAGEMENT_MESSAGE;  /* control */
  clearFlag(   (((UInteger8*)buf) + 34),  PTP_SYNC_BURST);
  clearFlag(   (((UInteger8*)buf) + 34),  PARENT_STATS);

/* Management message fields */

  *(Integer32*)(((UInteger8*)buf) + 40) = shift8(manage->targetCommunicationTechnology, 1);
  memcpy(      (((UInteger8*)buf) + 42), manage->targetUuid, 6);
  *(Integer32*)(((UInteger8*)buf) + 48) = shift16(flip16(manage->targetPortId), 0)
                          | shift16(flip16(MM_STARTING_BOUNDARY_HOPS), 1);
  *(Integer32*)(((UInteger8*)buf) + 52) = shift16(flip16(MM_STARTING_BOUNDARY_HOPS), 0);
  
  *(UInteger8*)(((UInteger8*)buf) + 55) = manage->managementMessageKey;
  
  switch(manage->managementMessageKey)
  {
  case PTP_MM_GET_FOREIGN_DATA_SET:
    *(UInteger16*)(((UInteger8*)buf) + 62) = manage->recordKey;
    *(Integer32*) (((UInteger8*)buf) + 56) = shift16(flip16(4), 1);
    return 64;
    
  default:
    *(Integer32*) (((UInteger8*)buf) + 56) = shift16(flip16(0), 1);
    return 60;
  }
}

UInteger16 msgPackManagementResponse(void          *buf, 
                                     MsgHeader     *header,
                                     MsgManagement *manage,
                                     PtpClock      *ptpClock
                                    )
{
  TimeInternal internalTime;
  TimeRepresentation externalTime;

/* PTP header fields */
  
  *(UInteger8*)(((UInteger8*)buf) + 20) =  2;  /* messageType */
  *(Integer32*)(((UInteger8*)buf) + 28) =  shift16(flip16(ptpClock->port_id_field), 0)
                           | shift16(flip16(ptpClock->last_general_event_sequence_number), 1);
  *(UInteger8*)(((UInteger8*)buf) + 32) =  PTP_MANAGEMENT_MESSAGE;  /* control */
  clearFlag(   (((UInteger8*)buf) + 34),   PTP_SYNC_BURST);
  clearFlag(   (((UInteger8*)buf) + 34),   PARENT_STATS);

/* Management response message fields */

  *(Integer32*)(((UInteger8*)buf) + 40) =  shift8(header->sourceCommunicationTechnology, 1);
  memcpy(      (((UInteger8*)buf) + 42),   header->sourceUuid, 6);
  *(Integer32*)(((UInteger8*)buf) + 48) =  shift16(flip16(header->sourcePortId), 0)
                           | shift16(flip16(MM_STARTING_BOUNDARY_HOPS), 1);
  *(Integer32*)(((UInteger8*)buf) + 52) =  shift16(flip16(manage->startingBoundaryHops
                                           - manage->boundaryHops
                                           + 1
                                          ), 0);
  
  switch(manage->managementMessageKey)
  {
  case PTP_MM_OBTAIN_IDENTITY:
    *(UInteger8*)(((UInteger8*)buf) + 55) =  PTP_MM_CLOCK_IDENTITY;
    *(Integer32*)(((UInteger8*)buf) + 56) =  shift16(flip16(64), 1);
    *(Integer32*)(((UInteger8*)buf) + 60) =  shift8(ptpClock->clock_communication_technology, 3);
    memcpy(      (((UInteger8*)buf) + 64),   ptpClock->clock_uuid_field, 6);
    *(Integer32*)(((UInteger8*)buf) + 72) =  shift16(flip16(ptpClock->clock_port_id_field), 1);
    memcpy(      (((UInteger8*)buf) + 76),   MANUFACTURER_ID, 48);
    return 124;
    
  case PTP_MM_GET_DEFAULT_DATA_SET:
    *(UInteger8*)(((UInteger8*)buf) + 55) =  PTP_MM_DEFAULT_DATA_SET;
    *(Integer32*)(((UInteger8*)buf) + 56) =  shift16(flip16(76), 1);
    *(Integer32*)(((UInteger8*)buf) + 60) =  shift8(ptpClock->clock_communication_technology, 3);
    memcpy(      (((UInteger8*)buf) + 64),   ptpClock->clock_uuid_field, 6);
    *(Integer32*)(((UInteger8*)buf) + 72) =  shift16(flip16(ptpClock->clock_port_id_field), 1);
    *(Integer32*)(((UInteger8*)buf) + 76) =  shift8(ptpClock->clock_stratum, 3);
    memcpy(      (((UInteger8*)buf) + 80),   ptpClock->clock_identifier, 4);
    *(Integer32*)(((UInteger8*)buf) + 84) =  shift16(flip16(ptpClock->clock_v1_variance), 1);
    *(Integer32*)(((UInteger8*)buf) + 88) =  shift8(ptpClock->clock_followup_capable, 3);
    *(Integer32*)(((UInteger8*)buf) + 92) =  shift8(ptpClock->preferred, 3);
    *(Integer32*)(((UInteger8*)buf) + 96) =  shift8(ptpClock->initializable, 3);
    *(Integer32*)(((UInteger8*)buf) + 100) = shift8(ptpClock->external_timing, 3);
    *(Integer32*)(((UInteger8*)buf) + 104) = shift8(ptpClock->is_boundary_clock, 3);
    *(Integer32*)(((UInteger8*)buf) + 108) = shift8(ptpClock->sync_interval, 3);
    memcpy(((UInteger8*)buf) + 112, ptpClock->subdomain_name, 16);
    *(Integer32*)(((UInteger8*)buf) + 128) = shift16(flip16(ptpClock->number_ports), 1);
    *(Integer32*)(((UInteger8*)buf) + 132) = shift16(flip16(ptpClock->number_foreign_records), 1);
    return 136;
    
  case PTP_MM_GET_CURRENT_DATA_SET:
    *(UInteger8*)(((UInteger8*)buf) + 55) =  PTP_MM_CURRENT_DATA_SET;
    *(Integer32*)(((UInteger8*)buf) + 56) =  shift16(flip16(20), 1);
    *(Integer32*)(((UInteger8*)buf) + 60) =  shift16(flip16(ptpClock->steps_removed), 1);
    
    fromInternalTime(&ptpClock->offset_from_master, &externalTime, 0);
    *(Integer32*)(((UInteger8*)buf) + 64) =  flip32(externalTime.seconds);
    *(Integer32*)(((UInteger8*)buf) + 68) =  flip32(externalTime.nanoseconds);
    
    fromInternalTime(&ptpClock->one_way_delay, &externalTime, 0);
    *(Integer32*)(((UInteger8*)buf) + 72) =  flip32(externalTime.seconds);
    *(Integer32*)(((UInteger8*)buf) + 76) =  flip32(externalTime.nanoseconds);
    return 80;
    
  case PTP_MM_GET_PARENT_DATA_SET:
    *(UInteger8*)(((UInteger8*)buf) + 55) =  PTP_MM_PARENT_DATA_SET;
    *(Integer32*)(((UInteger8*)buf) + 56) =  shift16(flip16(90), 1);
    *(Integer32*)(((UInteger8*)buf) + 60) =  shift8(ptpClock->parent_communication_technology, 3);
    memcpy(      (((UInteger8*)buf) + 64),   ptpClock->parent_uuid, 6);
    *(Integer32*)(((UInteger8*)buf) + 72) =  shift16(flip16(ptpClock->parent_port_id), 1);
    *(Integer32*)(((UInteger8*)buf) + 76) =  shift16(flip16(ptpClock->parent_last_sync_sequence_number), 1);
    *(Integer32*)(((UInteger8*)buf) + 80) =  shift8(ptpClock->parent_followup_capable, 1);
    *(Integer32*)(((UInteger8*)buf) + 84) =  shift8(ptpClock->parent_external_timing, 3);
    *(Integer32*)(((UInteger8*)buf) + 88) =  shift16(flip16(ptpClock->parent_v1_variance), 1);
    *(Integer32*)(((UInteger8*)buf) + 92) =  shift8(ptpClock->parent_stats, 3);
    *(Integer32*)(((UInteger8*)buf) + 96) =  shift16(flip16(ptpClock->observed_v1_variance), 1);
    *(Integer32*)(((UInteger8*)buf) + 100) = flip32(ptpClock->observed_drift);
    *(Integer32*)(((UInteger8*)buf) + 104) = shift8(ptpClock->utc_reasonable, 3);
    *(Integer32*)(((UInteger8*)buf) + 108) = shift8(ptpClock->grandmaster_communication_technology, 3);
    memcpy(      (((UInteger8*)buf) + 112),  ptpClock->grandmaster_uuid_field, 6);
    *(Integer32*)(((UInteger8*)buf) + 120) = shift16(flip16(ptpClock->grandmaster_port_id_field), 1);
    *(Integer32*)(((UInteger8*)buf) + 124) = shift8(ptpClock->grandmaster_stratum, 3);
    memcpy(      (((UInteger8*)buf) + 128),  ptpClock->grandmaster_identifier, 4);
    *(Integer32*)(((UInteger8*)buf) + 132) = shift16(flip16(ptpClock->grandmaster_v1_variance), 1);
    *(Integer32*)(((UInteger8*)buf) + 136) = shift8(ptpClock->grandmaster_preferred, 3);
    *(Integer32*)(((UInteger8*)buf) + 140) = shift8(ptpClock->grandmaster_is_boundary_clock, 3);
    *(Integer32*)(((UInteger8*)buf) + 144) = shift16(flip16(ptpClock->grandmaster_sequence_number), 1);
    return 148;
    
  case PTP_MM_GET_PORT_DATA_SET:
    if(manage->targetPortId && manage->targetPortId != ptpClock->port_id_field)
    {
      *(UInteger8*)(((UInteger8*)buf) + 55) = PTP_MM_NULL;
      *(Integer32*)(((UInteger8*)buf) + 56) = shift16(flip16(0), 1);
      return 0;
    }
    
    *(UInteger8*)(((UInteger8*)buf) + 55) = PTP_MM_PORT_DATA_SET;
    *(Integer32*)(((UInteger8*)buf) + 56) = shift16(flip16(52), 1);
    *(Integer32*)(((UInteger8*)buf) + 60) = shift16(flip16(ptpClock->port_id_field), 1);
    *(Integer32*)(((UInteger8*)buf) + 64) = shift8(ptpClock->port_state, 3);
    *(Integer32*)(((UInteger8*)buf) + 68) = shift16(flip16(ptpClock->last_sync_tx_sequence_number), 1);
    *(Integer32*)(((UInteger8*)buf) + 72) = shift16(flip16(ptpClock->last_general_event_sequence_number), 1);
    *(Integer32*)(((UInteger8*)buf) + 76) = shift8(ptpClock->port_communication_technology, 3);
    memcpy(((UInteger8*)buf) + 80, ptpClock->port_uuid_field, 6);
    *(Integer32*)(((UInteger8*)buf) + 88) = shift16(flip16(ptpClock->port_id_field), 1);
    *(Integer32*)(((UInteger8*)buf) + 92) = shift8(ptpClock->burst_enabled, 3);
    *(Integer32*)(((UInteger8*)buf) + 96) = shift8(4, 1) | shift8(2, 2) | shift8(2, 3);
    memcpy(      (((UInteger8*)buf) + 100), ptpClock->subdomain_address, 4);
    memcpy(      (((UInteger8*)buf) + 106), ptpClock->event_port_address, 2);
    memcpy(      (((UInteger8*)buf) + 110), ptpClock->general_port_address, 2);
    return 112;
    
  case PTP_MM_GET_GLOBAL_TIME_DATA_SET:
    *(UInteger8*)(((UInteger8*)buf) + 55) = PTP_MM_GLOBAL_TIME_DATA_SET;
    *(Integer32*)(((UInteger8*)buf) + 56) = shift16(flip16(24), 1);
    
    getTime(&internalTime, ptpClock->current_utc_offset);
    fromInternalTime(&internalTime, &externalTime, ptpClock->halfEpoch);
    *(Integer32*)(((UInteger8*)buf) + 60) = flip32(externalTime.seconds);
    *(Integer32*)(((UInteger8*)buf) + 64) = flip32(externalTime.nanoseconds);
    
    *(Integer32*)(((UInteger8*)buf) + 68) = shift16(flip16(ptpClock->current_utc_offset), 1);
    *(Integer32*)(((UInteger8*)buf) + 72) = shift8(ptpClock->leap_59, 3);
    *(Integer32*)(((UInteger8*)buf) + 76) = shift8(ptpClock->leap_61, 3);
    *(Integer32*)(((UInteger8*)buf) + 80) = shift16(flip16(ptpClock->epoch_number), 1);
    return 84;
    
  case PTP_MM_GET_FOREIGN_DATA_SET:
    if((manage->targetPortId && manage->targetPortId != ptpClock->port_id_field)
      || !manage->recordKey || manage->recordKey > ptpClock->number_foreign_records)
    {
      *(UInteger8*)(((UInteger8*)buf) + 55) = PTP_MM_NULL;
      *(Integer32*)(((UInteger8*)buf) + 56) = shift16(flip16(0), 1);
      return 0;
    }
    
    *(UInteger8*)(((UInteger8*)buf) + 55) = PTP_MM_FOREIGN_DATA_SET;
    *(Integer32*)(((UInteger8*)buf) + 56) = shift16(flip16(28), 1);
    *(Integer32*)(((UInteger8*)buf) + 60) = shift16(flip16(ptpClock->port_id_field), 1);
    *(Integer32*)(((UInteger8*)buf) + 64) = shift16(flip16(manage->recordKey - 1), 1);
    *(Integer32*)(((UInteger8*)buf) + 68) = shift8(ptpClock->foreign[manage->recordKey - 1].foreign_master_communication_technology, 3);
    memcpy(((UInteger8*)buf) + 72, ptpClock->foreign[manage->recordKey - 1].foreign_master_uuid, 6);
    *(Integer32*)(((UInteger8*)buf) + 80) = shift16(flip16(ptpClock->foreign[manage->recordKey - 1].foreign_master_port_id), 1);
    *(Integer32*)(((UInteger8*)buf) + 84) = shift16(flip16(ptpClock->foreign[manage->recordKey - 1].foreign_master_syncs), 1);
    return 88;
    
  default:
    return 0;
  }
}

// eof msg.c
