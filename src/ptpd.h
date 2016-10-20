/* src/ptpd.h */
/* Exported functions from all PTP modules that are not target specific */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (ptpd.h) contains Modifications (updates, corrections          */
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



#ifndef PTPD_H
#define PTPD_H

#include "constants.h"
#include "dep/constants_dep.h"
#include "dep/datatypes_dep.h"
#include "datatypes.h"
#include "dep/ptpd_dep.h"


/* arith.c */
UInteger32 crc_algorithm(Octet *buf, Integer16 length);

UInteger32 sum(Octet *buf, Integer16 length);

void v2FromInternalTime(TimeInternal         *internal, // signed secs,   signed nanoseconds
                        V2TimeRepresentation *external, // unsigned secs, signed nanoseconds
                        Boolean               halfEpoch,// current date past year 2038
                        UInteger16            epoch     // Epoch number (even further in future)
                     );


void fromInternalTime(TimeInternal       *internal, // signed secs,   signed nanoseconds
                      TimeRepresentation *external, // unsigned secs, signed nanoseconds
                      Boolean halfEpoch             // Half Epoch flag (for time past year 2038)
                     );


void toInternalTime(TimeInternal *internal, TimeRepresentation *external, Boolean *halfEpoch);

void v2ToInternalTime(TimeInternal *internal, V2TimeRepresentation *external);

void v2CorrectionToInternalTime(TimeInternal *internal, Integer64 external);

void addTime(TimeInternal *result, TimeInternal *x, TimeInternal *y); // x+y

void subTime(TimeInternal *result, TimeInternal *x, TimeInternal *y); // x-y

void clearTime(TimeInternal *time);

void halveTime(TimeInternal *time);

void copyTime(TimeInternal *destination, TimeInternal *source);

Integer32 getSeconds(TimeInternal *time);

Integer64 getNanoseconds(TimeInternal *time);

Boolean isNonZeroTime(TimeInternal *time);


/* bmc.c */
UInteger8 bmc(ForeignMasterRecord*,RunTimeOpts*,PtpClock*);
void m1(PtpClock*);
void s1(MsgHeader*,MsgSync*,PtpClock*);
void initData(RunTimeOpts*,PtpClock*);

/* v2bmc.c */
#ifdef PTPD_DBG
/* AKB: Debug Function to dump data set info
 */
void debug_dump_data_set_info(PtpClock *ptpClock);
#endif
UInteger8 v2bmc(ForeignMasterRecord*,RunTimeOpts*,PtpClock*);
void v2_s1(V2MsgHeader*,MsgAnnounce*,PtpClock*);


/* probe.c */
void probe(RunTimeOpts*,PtpClock*);

/* protocol.c */
void protocol(RunTimeOpts*,PtpClock*);
void multiPortProtocol(RunTimeOpts*,PtpClock*);

/* v2utils.c */
/* AKB: added for Version 2 support */

/* V2 Clause 7.5.2.2.2 */
void convert_eui48_to_eui64(Octet * eui48,
                            Octet * eui64
                           );

/* V2 Clause 18.3.1 */
void convert_v2_domain_to_v1_subdomain(UInteger8  domain,
                                       Octet     *subdomain
                                      );

/* V2 Clause 18.3.2, Table 96 */
int v1_stratum_to_v2_clockClass (UInteger8 stratum);

/* V2 Clause 18.3.2, Table 97 and Clause 18.3.3 Table 99 */
UInteger8 v2_clockClass_to_v1_stratum (UInteger8 clock_class, UInteger8 priority1);


/* V2 Clause 18.3.3, Table 98 */
int v1_preferred_to_v2_priority1 (Boolean preferred);


/* V2 Clause 18.3.3, Table 99 */
Boolean v2_priority1_to_v1_preferred(UInteger8 priority1);


/* V2 Clause 18.3.4, Table 100 */
int v1_clock_identifier_to_v2(Octet     *identifier,
                              UInteger8 *clockAccuracy,
                              UInteger8 *timeSource
                             );

/* V2 Clause 18.3.4, Table 101 */
int v2_clockAccuracy_to_v1_clock_identifier(UInteger8  accuracy,
                                            Octet     *identifier
                                           );

/* V2 Clause 18.3.5, Table 102 */
Boolean convert_priority2_to_v1_boundaryClock (UInteger8 priority2);

/* V2 Clause 18.3.5, Table 103 */
Boolean convert_v1_boundaryClock_to_priority2 (Boolean boundary_clock);

/* V2 Clause 18.3.6, Table 104 */
void convert_v2_message_type_to_v1(UInteger8  v2_msg_type,
                                   UInteger8 *v1_msg_type,
                                   UInteger8 *v1_control
                                 );

/* V2 Clause 18.3.6, Table 104 */
UInteger8 convert_v1_control_to_v2_message_type(UInteger8  v1_control);

/* V2 Clause 18.3.7 and Clause 7.5.2.2.3, table 4 */
void convert_v1_uuid_to_v2_clockIdentity(Octet * uuid,
                                         Octet * clock_identity
                                        );

/* V2 Clause 18.3.7 and Clause 7.5.2.2.3, table 4 */
void convert_v2_clockIdentity_to_v1_uuid(Octet * clock_identity,
                                         Octet * uuid
                                        );

void convert_v2_header_to_v1(V2MsgHeader *v2_header,
                             MsgHeader   *v1_header
                            );

void convert_v2_announce_to_v1_sync(V2MsgHeader *v2_header,
                                    MsgAnnounce *announce,
                                    MsgSync     *sync
                                   );

#endif

// eof ptpd.h

