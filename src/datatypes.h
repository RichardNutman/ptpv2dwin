/* src/datatypes.h */
/* General data structures, typedefs, etc. definitions for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (datatypes.h) contains Modifications (updates, corrections     */
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


#ifndef DATATYPES_H
#define DATATYPES_H

typedef struct {
  UInteger32 seconds;
  Integer32  nanoseconds;  
} TimeRepresentation;


typedef struct {
  UInteger16 epoch_number;
  UInteger32 seconds;
  Integer32  nanoseconds;  
} V2TimeRepresentation;

typedef struct {
  Integer32 seconds;
  Integer32 nanoseconds;  
} TimeInternal;

typedef struct {
  Integer32  interval;
  Integer32  left;
  Boolean    expire;
} IntervalTimer;

/* AKB: New types for IEEE 1588 v2 */

typedef struct {
  Octet            clockIdentity[8];
  UInteger16       portNumber;
} PortIdentity;

typedef struct {
  UInteger8        clockClass;
  Enumeration8     clockAccuracy;
  UInteger16       offsetScaledLogVariance;
} ClockQuality;

/* 1588 Version 1 common Message header */
typedef struct {
                                                      // Offset  Length (bytes)
  UInteger16   versionPTP;                            // 00       2
  UInteger16   versionNetwork;                        // 02       2
  Octet        subdomain[PTP_SUBDOMAIN_NAME_LENGTH];  // 04      16
  UInteger8    messageType;                           // 20       1
  UInteger8    sourceCommunicationTechnology;         // 21       1
  Octet        sourceUuid[PTP_UUID_LENGTH];           // 22       6
  UInteger16   sourcePortId;                          // 28       2
  UInteger16   sequenceId;                            // 30       2
  UInteger8    control;                               // 32       1
  UInteger8    reserved1;                             // 33       1
  Octet        flags[2];                              // 34       2
} MsgHeader;

/* 1588 Version 2 common Message header */
typedef struct {
                                                      // Offset  Length (bytes)
  UInteger8    transportSpecificAndMessageType;       // 00       1 (2 4-bit fields)
  UInteger8    reserved1AndVersionPTP;                // 01       1 (2 4-bit fields)
  UInteger16   messageLength;                         // 02       2
  UInteger8    domainNumber;                          // 04       1
  UInteger8    reserved2;                             // 05       1
  Octet        flags[2];                              // 06       2
  Integer64    correctionField;                       // 08       8
  UInteger32   reserved3;                             // 16       4
  PortIdentity sourcePortId;                          // 20      10
  UInteger16   sequenceId;                            // 30       2
  UInteger8    control;                               // 32       1
  UInteger8    logMeanMessageInterval;                // 33       1
} V2MsgHeader;

/* Version 1 Sync or Delay_Req message */
typedef struct {
  TimeRepresentation  originTimestamp;
  UInteger16          epochNumber;
  Integer16           currentUTCOffset;
  UInteger8           grandmasterCommunicationTechnology;
  Octet               grandmasterClockUuid[PTP_UUID_LENGTH];
  UInteger16          grandmasterPortId;
  UInteger16          grandmasterSequenceId;
  UInteger8           grandmasterClockStratum;
  Octet               grandmasterClockIdentifier[PTP_CODE_STRING_LENGTH];
  Integer16           grandmasterClockVariance;
  Boolean             grandmasterPreferred;
  Boolean             grandmasterIsBoundaryClock;
  Integer8            syncInterval;
  Integer16           localClockVariance;
  UInteger16          localStepsRemoved;
  UInteger8           localClockStratum;
  Octet               localClockIdentifer[PTP_CODE_STRING_LENGTH];
  UInteger8           parentCommunicationTechnology;
  Octet               parentUuid[PTP_UUID_LENGTH];
  UInteger16          parentPortField;
  Integer16           estimatedMasterVariance;
  Integer32           estimatedMasterDrift;
  Boolean             utcReasonable;
  
} MsgSync;

typedef MsgSync MsgDelayReq;

/* Version 2 Sync or Delay_Req message */

typedef struct {
                                                      // Offset  Length (bytes)
  V2TimeRepresentation originTimestamp;               // 34       10
} V2MsgSync;

typedef V2MsgSync V2MsgDelayReq;

/* Version 2 Announce message */
typedef struct {
                                                      // Offset  Length (bytes)
  V2TimeRepresentation originTimestamp;               // 34       10
  Integer16            currentUTCOffset;              // 44        2
  UInteger8            reserved;                      // 46        1
  UInteger8            grandmasterPriority1;          // 47        1
  ClockQuality         grandmasterClockQuality;       // 48        4
  UInteger8            grandmasterPriority2;          // 52        1
  Octet                grandmasterIdentity[8];        // 53        8
  UInteger16           stepsRemoved;                  // 61*       2
  Enumeration8         timeSource;                    // 63        1

/* *Note: stepsRemoved is a 16 bit field, but it is 
 * not 16 bit aligned in the announce message
 */
} MsgAnnounce;


/* Follow_Up message */
typedef struct {
  UInteger16          associatedSequenceId;
  TimeRepresentation  preciseOriginTimestamp;
  
} MsgFollowUp;

/* Version 2 Follow Up message */

typedef struct {
  V2TimeRepresentation preciseOriginTimestamp;
} V2MsgFollowUp;


/* Delay_Resp message */
typedef struct {
  TimeRepresentation  delayReceiptTimestamp;
  UInteger8           requestingSourceCommunicationTechnology;
  Octet               requestingSourceUuid[PTP_UUID_LENGTH];
  UInteger16          requestingSourcePortId;
  UInteger16          requestingSourceSequenceId;
  
} MsgDelayResp;

/* Other V2 messages */

typedef struct {
  V2TimeRepresentation receiveTimestamp;
  PortIdentity         requestingPortId;
} V2MsgDelayResp;
  

typedef struct {
  V2TimeRepresentation originTimestamp;
  Octet                reserved[10];
} V2MsgPDelayReq;

typedef struct {
  V2TimeRepresentation requestReceiptTimestamp;
  PortIdentity         requestingPortId;
} V2MsgPDelayResp;

typedef struct {
  V2TimeRepresentation responseOriginTimestamp;
  PortIdentity         requestingPortId;
} V2MsgPDelayRespFollowUp;

/* V2 Signaling and Management messages (note does not contina any TLVs) */

typedef struct {
  PortIdentity         targetPortIdentity;
  /* One or more TLVs afterwards */
} V2MsgSignaling;

typedef struct {
  PortIdentity         targetPortIdentity;
  UInteger8            startingBoundaryHops;
  UInteger8            boundaryHops;
  UInteger8            reservedAndActionField;
  /* One of more TLVs afterwards */
} V2MsgManagement;


/* Management message */
typedef union
{
  struct ClockIdentity
  {
    UInteger8   clockCommunicationTechnology;
    Octet       clockUuidField[PTP_UUID_LENGTH];
    UInteger16  clockPortField;
    Octet       manufacturerIdentity[MANUFACTURER_ID_LENGTH];
  } clockIdentity;
  
  struct DefaultData
  {
    UInteger8   clockCommunicationTechnology;
    Octet       clockUuidField[PTP_UUID_LENGTH];
    UInteger16  clockPortField;
    UInteger8   clockStratum;
    Octet       clockIdentifier[PTP_CODE_STRING_LENGTH];
    Integer16   clockVariance;
    Boolean     clockFollowupCapable;
    Boolean     preferred;
    Boolean     initializable;
    Boolean     externalTiming;
    Boolean     isBoundaryClock;
    Integer8    syncInterval;
    Octet       subdomainName[PTP_SUBDOMAIN_NAME_LENGTH];
    UInteger16  numberPorts;
    UInteger16  numberForeignRecords;
  } defaultData;
  
  struct Current
  {
    UInteger16          stepsRemoved;
    TimeRepresentation  offsetFromMaster;
    TimeRepresentation  oneWayDelay;
  } current;
  
  struct Parent
  {
    UInteger8   parentCommunicationTechnology;
    Octet       parentUuid[PTP_UUID_LENGTH];
    UInteger16  parentPortId;
    UInteger16  parentLastSyncSequenceNumber;
    Boolean     parentFollowupCapable;
    Boolean     parentExternalTiming;
    Integer16   parentVariance;
    Boolean     parentStats;
    Integer16   observedVariance;
    Integer32   observedDrift;
    Boolean     utcReasonable;
    UInteger8   grandmasterCommunicationTechnology;
    Octet       grandmasterUuidField[PTP_UUID_LENGTH];
    UInteger16  grandmasterPortIdField;
    UInteger8   grandmasterStratum;
    Octet       grandmasterIdentifier[PTP_CODE_STRING_LENGTH];
    Integer16   grandmasterVariance;
    Boolean     grandmasterPreferred;
    Boolean     grandmasterIsBoundaryClock;
    UInteger16  grandmasterSequenceNumber;
  } parent;
  
  struct Port
  {
    UInteger16  returnedPortNumber;
    UInteger8   portState;
    UInteger16  lastSyncEventSequenceNumber;
    UInteger16  lastGeneralEventSequenceNumber;
    UInteger8   portCommunicationTechnology;
    Octet       portUuidField[PTP_UUID_LENGTH];
    UInteger16  portIdField;
    Boolean     burstEnabled;
    UInteger8   subdomainAddressOctets;
    UInteger8   eventPortAddressOctets;
    UInteger8   generalPortAddressOctets;
    Octet       subdomainAddress[SUBDOMAIN_ADDRESS_LENGTH];
    Octet       eventPortAddress[PORT_ADDRESS_LENGTH];
    Octet       generalPortAddress[PORT_ADDRESS_LENGTH];
  } port;
  
  struct GlobalTime
  {
    TimeRepresentation  localTime;
    Integer16           currentUtcOffset;
    Boolean             leap59;
    Boolean             leap61;
    UInteger16          epochNumber;
  } globalTime;
  
  struct Foreign
  {
    UInteger16  returnedPortNumber;
    UInteger16  returnedRecordNumber;
    UInteger8   foreignMasterCommunicationTechnology;
    Octet       foreignMasterUuid[PTP_UUID_LENGTH];
    UInteger16  foreignMasterPortId;
    UInteger16  foreignMasterSyncs;
  } foreign;
  
} MsgManagementPayload;

typedef struct {
  UInteger8   targetCommunicationTechnology;
  Octet       targetUuid[PTP_UUID_LENGTH];
  UInteger16  targetPortId;
  Integer16   startingBoundaryHops;
  Integer16   boundaryHops;
  UInteger8   managementMessageKey;
  UInteger16  parameterLength;
  UInteger16  recordKey;
  
  MsgManagementPayload payload;

} MsgManagement;

/* Foreign Master database record */
typedef struct
{
  UInteger8   foreign_master_communication_technology;
  Octet       foreign_master_uuid[PTP_UUID_LENGTH];
  UInteger16  foreign_master_port_id;
  UInteger16  foreign_master_syncs;
  
  MsgHeader   header;           // V1 PTP header data from Sync message
  MsgSync     sync;             // V1 Sync data from Sync message

  /* AKB: Added for v2 support */
  V2MsgHeader v2_header;        // V2 PTP header data from Announce message
  MsgAnnounce announce;         // V2 Announce data from Announce message
  Octet       foreign_master_clock_identity[8];
  UInteger16  foreign_master_announces;

} ForeignMasterRecord;

/* main program data structure */
typedef struct {
  /* Default data set */
  /* V1: */
  UInteger8    clock_communication_technology;
  Octet        clock_uuid_field[PTP_UUID_LENGTH];
  UInteger16   clock_port_id_field;
  UInteger8    clock_stratum;
  Octet        clock_identifier[PTP_CODE_STRING_LENGTH]; /* V1: ATOM,NTP,HAND,INIT,DFLT */
  Integer16    clock_v1_variance;  /* V1 uses Integer16 for variance */
  Boolean      clock_followup_capable;
  Boolean      preferred;
  Boolean      initializable;
  Boolean      external_timing;
  Boolean      is_boundary_clock;
  Integer8     sync_interval;                             /* V2 & V1 */
  Octet        subdomain_name[PTP_SUBDOMAIN_NAME_LENGTH];
  UInteger16   number_ports;                              /* V2 & V1, static */
  UInteger16   number_foreign_records;

  /* AKB: added for V2 */
  /* Static */
  Boolean       two_step_flag;
  Octet         v2_clock_identity[8]; 
  /* Dynamic */
  ClockQuality  clock_quality;  /* V2, contains class, accuracy and log variance */
  /* Configurable */
  UInteger8     priority1;
  UInteger8     priority2;
  UInteger8     domain_number;
  Boolean       slave_only;

  
  /* Current data set */
  UInteger16    steps_removed;          /* V1 & V2 */
  TimeInternal  offset_from_master;     /* V1 & V2 */
  TimeInternal  one_way_delay;
  TimeInternal  mean_path_delay;        /* V2 */
  
  /* Parent data set */
  UInteger8     parent_communication_technology;
  Octet         parent_uuid[PTP_UUID_LENGTH];
  UInteger16    parent_port_id;                /* V1 & V2 */
  UInteger16    parent_last_sync_sequence_number;
  Boolean       parent_followup_capable;
  Boolean       parent_external_timing;
  Integer16     parent_v1_variance;
  Boolean       parent_stats;                  /* V1 & V2 */
  Integer16     observed_v1_variance;
  Integer32     observed_drift;                /* V1 & V2 observedParentClockPhaseChangeRate */
  Boolean       utc_reasonable;
  UInteger8     grandmaster_communication_technology;
  Octet         grandmaster_uuid_field[PTP_UUID_LENGTH];
  UInteger16    grandmaster_port_id_field;
  UInteger8     grandmaster_stratum;
  Octet         grandmaster_identifier[PTP_CODE_STRING_LENGTH];
  Integer16     grandmaster_v1_variance;
  Boolean       grandmaster_preferred;
  Boolean       grandmaster_is_boundary_clock;
  UInteger16    grandmaster_sequence_number;
  /* AKB: Added for V2: */
  Octet         parent_clock_identity[8];      /* V2 uses EUI-64 */
  UInteger16    parent_v2_variance;            /* V2 uses UInteger16 */
  Octet         grandmaster_clock_identity[8]; /* V2 uses EUI-64 */
  UInteger16    observed_v2_variance;          /* V2 uses UInteger16 */
  UInteger8     grandmaster_priority1;
  UInteger8     grandmaster_priority2;
  ClockQuality  grandmaster_clock_quality;     /* V2, includes Class, Accuracy and Variance */
  UInteger16    parent_last_announce_sequence_number;  
  
  /* Global time properties data set */
  Integer16     current_utc_offset;       /* V1 & V2 */
  Boolean       leap_59;                  /* V1 & V2 */
  Boolean       leap_61;                  /* V1 & V2 */
  UInteger16    epoch_number;
  /* AKB: Added for V2: */
  Boolean       current_utc_offset_valid;
  Boolean       time_traceable;
  Boolean       frequency_traceable;
  Boolean       ptp_timescale;
  Enumeration8  time_source; 
  
  /* Port configuration data set */
  UInteger8     port_state;                                       /* V1 & V2 */
  UInteger16    last_sync_tx_sequence_number;
  UInteger16    last_general_event_sequence_number;
  Octet         subdomain_address[SUBDOMAIN_ADDRESS_LENGTH];
  Octet         event_port_address[PORT_ADDRESS_LENGTH];
  Octet         general_port_address[PORT_ADDRESS_LENGTH];
  UInteger8     port_communication_technology;
  Octet         port_uuid_field[PTP_UUID_LENGTH];
  UInteger16    port_id_field;
  Boolean       burst_enabled;
  /* AKB: Added for V2: */
  Octet         port_clock_identity[8];  /* V2 uses EUI-64 */
  Integer8      delay_req_interval;
  TimeInternal  peer_mean_path_delay;
  Integer8      announce_interval;
  Integer8      announce_receipt_timeout;
  Enumeration8  delay_mechanism;
  Integer8      pdelay_req_interval;
  UInteger8     version_number;
  Integer8      sync_receipt_timeout; /* IEEE 802.1AS has both a sync and announce timeout */
  UInteger16    last_announce_tx_sequence_number;
  UInteger16    last_delay_req_tx_sequence_number;  // Used for both delay and pdelay
  UInteger16    last_pdelay_req_rx_sequence_number;
  UInteger16    last_pdelay_resp_tx_sequence_number;
  UInteger8     current_msg_version;
  UInteger8     v2_msg_type;
  UInteger8     rx_transport_specific;

  /* Foreign master data set */
  ForeignMasterRecord *foreign;
  
  /* Other things we need for the protocol */
  Boolean halfEpoch;
  
  Integer16  max_foreign_records;
  Integer16  foreign_record_i;
  Integer16  foreign_record_best;
  Boolean    record_update;
  UInteger32 random_seed;
  
  MsgHeader   msgTmpHeader;
  V2MsgHeader v2MsgTmpHeader;  /* AKB: Added for V2 support */
  
  union {
    MsgSync                 sync;
    MsgFollowUp             follow;
    MsgDelayReq             req;
    MsgDelayResp            resp;
    MsgManagement           manage;
    MsgAnnounce             announce;  /* AKB: Added V2 messages */
    V2MsgSync               v2sync;
    V2MsgFollowUp           v2follow;
    V2MsgDelayReq           v2req;
    V2MsgDelayResp          v2resp;
    V2MsgPDelayReq          v2preq;
    V2MsgPDelayResp         v2presp;
    V2MsgPDelayRespFollowUp v2pfollow;
    V2MsgSignaling          v2signal;
    V2MsgManagement         v2manage;
  } msgTmp;

  V2MsgHeader currentRxPDelayReqHeader;  /* AKB: used to create PDELAY response and followup */

  /* Pointers to payload area of buffers (to allow adding MAC header) */

  Octet * msgObuf;  
  Octet * msgIbuf;

  /* Actual storage for characters */
  
  Octet outputBuffer[(PACKET_SIZE+16)];  // Packet size plus size for MAC header
  Octet inputBuffer[(PACKET_SIZE+16)];
  
  TimeInternal  master_to_slave_delay;
  TimeInternal  slave_to_master_delay;

  /* Storage for message timestamp calculations */
  
  /* Delay Request mechanism, version 1 and version 2 PTP */
  TimeInternal  t1_sync_tx_time;      // Time from master from SYNC (1 step) or FOLLOW UP (2 step)
  TimeInternal  t2_sync_rx_time;      // Time at slave of inbound SYNC message
  TimeInternal  t3_delay_req_tx_time; // Time at slave of outbound DELAY REQ
  TimeInternal  t4_delay_req_rx_time; // Time from master from DELAY RESP

  TimeInternal  t1_pdelay_req_tx_time;  // Time at requestor of outbound PDELAY REQ
  TimeInternal  t2_pdelay_req_rx_time;  // Time from responder, reported in PDELAY RESP
  TimeInternal  t3_pdelay_resp_tx_time; // Time from responder, reported in PDELAY RESP FOLLOWUP
  TimeInternal  t4_pdelay_resp_rx_time; // Time at requestor

  /* Receive correction time in Internal time for V2 calculations */
  TimeInternal  sync_correction;
  TimeInternal  followup_correction;
  TimeInternal  delay_resp_correction;
  TimeInternal  pdelay_resp_correction;
  TimeInternal  pdelay_followup_correction;

  // AKB: For new filter to set initial time based on delta time calculations
  TimeInternal  t1_sync_delta_time;  // time between transmitted sync messages
  TimeInternal  t2_sync_delta_time;  // time between received    sync message

  Boolean       pdelay_resp_rx_two_step_flag;  // Used to select proper equation to use 
  
  UInteger16  Q;
  UInteger16  R;
  
  Boolean     sentDelayReq;     // Used for both Delay and PDelay request

  UInteger16  sentDelayReqSequenceId;

  Boolean     waitingForFollow; // Indicates two step Sync received, waiting for RX follow up
  Boolean     waitingForPDelayRespFollow; // Indicates two step PDelay Response received

  Boolean     sentSync;         // Sync Transmitted from Application

  Boolean     sentPDelayResp;   // PDelay Response transmitted from application

  Boolean     receivedPDelayResp;
  
  offset_from_master_filter  ofm_filt;
  one_way_delay_filter       owd_filt;
  
  Boolean message_activity;
  
  IntervalTimer  itimer[TIMER_ARRAY_SIZE];
  
  NetPath netPath;

  /* Clock control */
  Integer32     baseAdjustValue;      // AKB: Added to support setting/calc of base value
  Integer32     lastAdjustValue;      // AKB: for storing calculated adjust value

#ifdef CONFIG_MPC831X
  UInteger32    timerAddValue;        // AKB: Added to support changing HW clock frequency
  Boolean       tx_time_pending;      // AKB: Added to poll for HW transmit time
  Boolean       tx_sync_time_pending;
  Boolean       tx_delay_req_time_pending;
  Boolean       tx_pdelay_resp_time_pending;
#endif
  /* AKB moved to support raw sockets besides MPC831X */
  TimeInternal  tx_complete_poll_timeout;  // AKB: Added to support raw sockets


  /* Misc. additions for V2 support */
  UInteger8     tx_transport_specific; /* For setting of field in first byte of all V2 messages */
  
} PtpClock;

/* program options set at run-time */
typedef struct {
  Integer8      syncInterval;
  Octet         subdomainName[PTP_SUBDOMAIN_NAME_LENGTH];
  Octet         clockIdentifier[PTP_CODE_STRING_LENGTH];
  UInteger32    clockVariance;
  UInteger8     clockStratum;
  Boolean       clockPreferred;
  Integer16     currentUtcOffset;
  UInteger16    epochNumber;
  Octet         ifaceName[IFACE_NAME_LENGTH];
  Boolean       noResetClock;
  Boolean       noAdjust;
  Boolean       displayStats;
  Boolean       csvStats;
  Octet         unicastAddress[NET_ADDRESS_LENGTH];
  Integer16     ap, ai;               // P/I Filter values
  Integer16     s;                    // Filter "stiffness"
  TimeInternal  inboundLatency, outboundLatency;
  Integer16     max_foreign_records;
  Boolean       slaveOnly;
  Boolean       probe;
  UInteger8     probe_management_key;
  UInteger16    probe_record_key;
  Boolean       halfEpoch;
  Boolean       ptpv2;                // AKB: Flag added to support Version 2 Y/N
  Boolean       pdelay;               // AKB: Flag added to support Pdelay Y/N
  Integer32     baseAdjustValue;      // AKB: Added to support setting/calc of base value
  Boolean       rememberAdjustValue;  // AKB: Added to support Slave remembering masters clock
  Integer8      announceInterval;     // AKB: Added to support V2 announce message transmit timer

  Boolean       nonDaemon;            // AKB: Added to split parser from startup function
                                      // nonDaemon (TRUE == command mode (non-daemon)
                                      // FALSE == daemon mode)

  int           noClose;       // noClose option for daemon function (send output to file option
                               // sets this variable to 1)

#ifdef CONFIG_MPC831X
  UInteger32    hwClockPeriod;        // AKB: Added to support changing HW clock frequency

#endif
  
} RunTimeOpts;

#endif

// eof src/datatypes.h

