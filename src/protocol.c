/* src/protocol.c */
/* Main protocol handling functions for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (protocol.c) contains Modifications (updates, corrections      */
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

// Local function prototypes:

Boolean doInit(RunTimeOpts*, PtpClock*);
void doState(RunTimeOpts*, PtpClock*);
void toState(UInteger8, RunTimeOpts*, PtpClock*);

void handle(RunTimeOpts*, PtpClock*);

void handleSync(MsgHeader*,    // Pointer to V1 unpacked message header
	V2MsgHeader*,  // Pointer to V2 unpakced message header
	Octet*,        // Pointer to start of raw PTP message
	ssize_t,       // Length of PTP message
	TimeInternal*, // Reception time (internal format) of Sync message
	Boolean,       // isFromSelf TRUE/FALSE
	RunTimeOpts*,  // Pointer to run time options structure
	PtpClock*      // Pointer to main data structure
	);

void handleFollowUp(MsgHeader*,
	V2MsgHeader*,
	Octet*,
	ssize_t,
	Boolean,
	RunTimeOpts*,
	PtpClock*
	);

void handleDelayReq(MsgHeader*,
	V2MsgHeader*,
	Octet*,
	ssize_t,
	TimeInternal*,
	Boolean,
	RunTimeOpts*,
	PtpClock*
	);

void handleDelayResp(MsgHeader*,
	V2MsgHeader*,
	Octet*,
	ssize_t,
	Boolean,
	RunTimeOpts*,
	PtpClock*
	);

void handleManagement(MsgHeader*,
	Octet*,
	ssize_t,
	Boolean,
	RunTimeOpts*,
	PtpClock*
	);

void handleAnnounce(V2MsgHeader  *header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	);


void handlePDelayReq(V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	);


void handlePDelayResp(V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	);

void handlePDelayRespFollowUp(V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	);

void handleSyncTxComplete(TimeInternal*, RunTimeOpts*, PtpClock*);
void handleDelayReqTxComplete(TimeInternal*, RunTimeOpts*, PtpClock*);
void handlePDelayRespTxComplete(TimeInternal*, RunTimeOpts*, PtpClock*);


void issueSync(RunTimeOpts*, PtpClock*);
void issueFollowup(TimeInternal*, RunTimeOpts*, PtpClock*);
void issueDelayReq(RunTimeOpts*, PtpClock*);
void issueDelayResp(TimeInternal*, MsgHeader*, V2MsgHeader*, RunTimeOpts*, PtpClock*);
void issueManagement(MsgHeader*, MsgManagement*, RunTimeOpts*, PtpClock*);

void issueAnnounce(RunTimeOpts *rtOpts,                    // AKB: added for v2
	PtpClock    *ptpClock
	);

void issuePDelayResp(TimeInternal*, V2MsgHeader*, RunTimeOpts*, PtpClock*);
void issuePDelayRespFollowup(TimeInternal*, RunTimeOpts*, PtpClock*);

MsgSync *     addForeign(Octet*, MsgHeader*, PtpClock*);
MsgAnnounce * addV2Foreign(Octet*, V2MsgHeader*, PtpClock*);  // AKB: added for v2

#ifdef CONFIG_MPC831X
void checkTxCompletions(RunTimeOpts*,PtpClock*);
#endif

void multiPortProtocol(RunTimeOpts *rtOpts,  // Global Run Time Options
	PtpClock    *ptpClock // Pointer to array of ptpClock structures
	)
{
	int           i;
	PtpClock *    currentPtpClockData;
	TimeInternal  event_wait_time;
	int           ret;


	currentPtpClockData = ptpClock;
	for(i = 0; i < MAX_PTP_PORTS; i++)
	{
		DBGV("multiPortProtocol: initializing port %d\n", (i + 1));
		toState(PTP_INITIALIZING, rtOpts, currentPtpClockData);
		if(!doInit(rtOpts, currentPtpClockData))
		{
			// doInit Failed!  Exit 
			return;
		}
		currentPtpClockData++;
	}

	for(;;)
	{
		currentPtpClockData = ptpClock;
		clearTime(&event_wait_time);

		for(i = 0; i < MAX_PTP_PORTS; i++)
		{
			if(currentPtpClockData->port_state != PTP_INITIALIZING)
			{
				DBGV("multiPortProtocol: doState port %d\n", (i + 1));
				// Not INIITIALIZING state, execute state machine
				doState(rtOpts, currentPtpClockData);
			}
			else
			{
				// State INITIALIZING, run doInit function, check if OK
				DBGV("multiPortProtocol: doInit port %d\n", (i + 1));
				if(!doInit(rtOpts, currentPtpClockData))
				{
					// doInit Failed!  Exit 
					DBGV("multiPortProtocol: doInit failed, port %d\n", (i + 1));
					return;
				}
			}

			// If poll timeout active, set event wait time to the lowest value
			// of all poll timeouts 

			if(currentPtpClockData->tx_complete_poll_timeout.nanoseconds > 0)
			{
				if(
					(event_wait_time.nanoseconds == 0)
					|| (currentPtpClockData->tx_complete_poll_timeout.nanoseconds <
					event_wait_time.nanoseconds
					)
					)
				{
					event_wait_time.nanoseconds
						= currentPtpClockData->tx_complete_poll_timeout.nanoseconds;
				}
			}

			currentPtpClockData++;
		}
		// Scan for work on each port complete, wait for message or timeout
		// via call to netSelectAll

		if(event_wait_time.nanoseconds == 0)
		{
			DBGV("multiPortProtocol: netSelectAll, no timeout\n");
			ret = netSelectAll(0, ptpClock);  // Wait until main timeout or event
		}
		else
		{
			DBGV("multiPortProtocol: netSelectAll, timeout %d nanoseconds\n",
				event_wait_time.nanoseconds
				);
			ret = netSelectAll(&event_wait_time, ptpClock); // Wait specified amount of time
		}
		if(ret < 0)
		{
			//
			// If return code is negative, then there
			// was a socket failure
			//
			PERROR("multiPortProtocol: failed to poll sockets");
			return;
		}
	}
}


/* loop forever. doState() has a switch for the actions and events to be
 * checked for 'port_state'. the actions and events may or may not change
 * 'port_state' by calling toState(), but once they are done we loop around
 * again and perform the actions required for the new 'port_state'.
 */

void protocol(RunTimeOpts * rtOpts,
	PtpClock *    ptpClock
	)
{
	int ret;

	// Main entry point for protocol state machine

	DBG("protocol: event POWERUP\n");


	// Setup INITIALIZING state 

	toState(PTP_INITIALIZING, rtOpts, ptpClock);
	if(!doInit(rtOpts, ptpClock))
	{
		// doInit Failed!  Exit 
		return;
	}

	for(;;)  // Forever loop
	{
		if(ptpClock->port_state != PTP_INITIALIZING)
		{
			// Not INIITIALIZING state, execute state machine
			doState(rtOpts, ptpClock);
		}
		else
		{
			// State INITIALIZING, run doInit function, check if OK
			if(!doInit(rtOpts, ptpClock))
			{
				// doInit Failed!  Exit 
				return;
			}
		}

		if(ptpClock->message_activity)
		{
			DBGV("protocol: message_activity was TRUE\n");
		}
		else
		{
			DBGV("protocol: message_activity was FALSE\n");
		}

		// Scan for work on each port complete, wait for message or timeout
		// via call to netSelectAll

		if(ptpClock->tx_complete_poll_timeout.nanoseconds == 0)
		{
			ret = netSelect(0,
				&ptpClock->netPath
				);  // Wait until main timeout or event
		}
		else
		{
			ret = netSelect(&ptpClock->tx_complete_poll_timeout,
				&ptpClock->netPath
				); // Wait specified amount of time
		}

		if(ret < 0)
		{
			//
			// If return code is negative, then there
			// was a socket failure
			//
			PERROR("protocol: failed to poll sockets");
			return;
		}

	}
}

Boolean doInit(                       // doInit function
	RunTimeOpts * rtOpts,  // Pointer to run time options
	PtpClock *    ptpClock // Pointer to PTP clock structure
	)
{
	unsigned int microseconds;

	DBG("doInit: manufacturerIdentity: %s\n", MANUFACTURER_ID);

	/* Setup pointers to payload area of input and output buffers */

	ptpClock->msgObuf = &(ptpClock->outputBuffer[16]);
	ptpClock->msgIbuf = &(ptpClock->inputBuffer[16]);

	/* initialize networking */

	netShutdown(&ptpClock->netPath); // Shutdown net (precaution if already running)


	if(!netInit(                     // Setup networking (sockets, etc.) test if OK
		&ptpClock->netPath,  // Pointer Network path string (e.g. eth0)
		rtOpts,              // Pointer to run time options
		ptpClock             // Pointer to PTP clockstructure
		))
	{

		// netInit failed!  Print error messge, change state to faulty
		// and return FALSE

		PERROR("doInit: failed to initialize network\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return FALSE;
	}

	/* netInit OK.  initialize other stuff */

	initData(rtOpts, ptpClock);      // Initialize Data

	if(ptpClock->port_id_field == 1)  // AKB: Only init common timer on call from 1st port init
	{
		if(rtOpts->syncInterval < 0)
		{
			// syncInterval < 0, calc microseconds (-1:500,000, -2:250,000, etc.)
			microseconds = (1000000 / (1 << (abs(rtOpts->syncInterval))));
			DBGV("doInit: sync_interval = %d, initTimer %uusecs/tick\n",
				rtOpts->syncInterval,
				microseconds
				);
			initTimer(
				0,            // 0 seconds
				microseconds  // u-secs based on sync interval
				);
		}
		else
		{
			DBGV("doInit: sync_interval = %d, initTimer 1 second/tick\n",
				rtOpts->syncInterval
				);
			initTimer(1, 0);        // Initialize Timer ticks to 1 per second
		}
	}


	initClock(rtOpts, ptpClock);     // Initialize Clock


	m1(ptpClock);

	if(rtOpts->ptpv2)
	{
		msgPackV2Header(ptpClock->msgObuf, // Create message header
			ptpClock
			);
	}
	else
	{
		msgPackHeader(ptpClock->msgObuf, // Create message header
			ptpClock
			);
	}

	// If debug enabled, print out PTP Clock data:

	DBG("doInit: ptpClock Data:\n");

	DBG(" sync message interval...: %d\n", PTP_SYNC_INTERVAL_TIMEOUT(ptpClock->sync_interval));
	DBG(" clock identifier........: %s\n", ptpClock->clock_identifier);
	DBG(" 256*log2(clock variance): %d\n", ptpClock->clock_v1_variance);
	DBG(" clock stratum...........: %d\n", ptpClock->clock_stratum);
	DBG(" clock preferred?........: %s\n", ptpClock->preferred ? "yes" : "no");
	DBG(" bound interface name....: %s\n", rtOpts->ifaceName);
	DBG(" communication technology: %d\n", ptpClock->port_communication_technology);
	DBG(" uuid....................: %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
		ptpClock->port_uuid_field[0],
		ptpClock->port_uuid_field[1],
		ptpClock->port_uuid_field[2],
		ptpClock->port_uuid_field[3],
		ptpClock->port_uuid_field[4],
		ptpClock->port_uuid_field[5]
		);
	DBG(" PTP subdomain name......: %s\n", ptpClock->subdomain_name);
	DBG(" subdomain address.......: %hhx.%hhx.%hhx.%hhx\n",
		ptpClock->subdomain_address[0],
		ptpClock->subdomain_address[1],
		ptpClock->subdomain_address[2],
		ptpClock->subdomain_address[3]
		);
	DBG(" event port address......: %hhx %hhx\n",
		ptpClock->event_port_address[0],
		ptpClock->event_port_address[1]
		);
	DBG(" general port address....: %hhx %hhx\n",
		ptpClock->general_port_address[0],
		ptpClock->general_port_address[1]
		);

	// Initialization complete, change from INITIALIZATION to LISTENING state

	toState(PTP_LISTENING, rtOpts, ptpClock);

	return TRUE;
}

/* handle actions and events for 'port_state' */

void doState(RunTimeOpts *rtOpts,
	PtpClock    *ptpClock
	)
{
	UInteger8 state;

	ptpClock->message_activity = FALSE;  // Set messsage activity to FALSE

	// 1st part of state machine, first check if there was a record
	// update to the PTP Clock record if LISTENING, PASSIVE, SLAVE
	// or MASTER state.  If something changed in this state,
	// then the best master clock (BMC) algorithm is run to see
	// if we need to change state.

	DBGV("doState: Port state: %d\n", ptpClock->port_state);

	switch(ptpClock->port_state)
	{
	case PTP_LISTENING:
	case PTP_PASSIVE:
	case PTP_SLAVE:
	case PTP_MASTER:

		if(ptpClock->record_update)         // Test if record update
		{
			// record udpate is TRUE, set it to FALSE, run Best Master Clock
			// (BMC) algorithm, and check if state needs to change.

			DBGV("doState: Record Update TRUE, invoking BMC algorithm\n");
			ptpClock->record_update = FALSE;  // Clear record update boolean

			// Based on run time options, run either version 1 or version 2 BMC algorithm

			if(rtOpts->ptpv2)
			{
				state = v2bmc(                    // run version 2 BMC algorithm, get suggested state
					ptpClock->foreign,
					rtOpts,
					ptpClock
					);
			}
			else
			{
				state = bmc(                    // run version 1 BMC algorithm, get suggested state
					ptpClock->foreign,
					rtOpts,
					ptpClock
					);
			}
			if(state != ptpClock->port_state) // If Suggested state is new, change state
			{
				DBG("doState: After BMC check, State change from %d to %d\n",
					ptpClock->port_state,
					state
					);
				// State Change, execute toState function to change states
				toState(
					state,    // State to change to
					rtOpts,   // Pointer to run time options
					ptpClock  // Pointer to PTP Clock data structure
					);
			}
		}
		break;

	default:
		break;
	}

	// Main State machine processing:

	switch(ptpClock->port_state)
	{
	case PTP_FAULTY:
		/*
		 * FAULTY STATE:
		 * imaginary troubleshooting, this code does not do any troublshooting, so
		 * instead, we fake that the fault was cleared and go back to INITIALIZING
		 * state.
		 */

		DBG("doState: imaginary event FAULT_CLEARED\n");
		toState(PTP_INITIALIZING, rtOpts, ptpClock);
		return;

	case PTP_LISTENING:
	case PTP_PASSIVE:
	case PTP_UNCALIBRATED:
	case PTP_SLAVE:
		//
		// FAULTY, PASSIVE, UNCALIBRATED AND SLAVE STATES:
		//
		// Call handle routine to see if there is a packet to process.
		// handle function calls other functions as appropritate if a message
		// is received. 
		//
		handle(rtOpts, ptpClock);

		if(timerExpired(SYNC_RECEIPT_TIMER, ptpClock->itimer, ptpClock->port_id_field))
		{
			DBG("doState: event SYNC_RECEIPT_TIMEOUT_EXPIRES\n");
			ptpClock->number_foreign_records = 0;
			ptpClock->foreign_record_i = 0;
			if(!rtOpts->slaveOnly && ptpClock->clock_stratum != 255)
			{
				//
				// Not set to Slave only option and Stratum not set to 255 (slave only)
				// Sync Receipt has expired so no longer receiving
				// sync messages, switch to MASTER state
				//
				m1(ptpClock);
				toState(PTP_MASTER, rtOpts, ptpClock);
			}
			else
			{
				//
				// Run time options set to Slave only option 
				// or Stratum set to 255 (slave only)
				// Change state to LISTENING, if not already set to LISTENING.
				//
				if(ptpClock->port_state != PTP_LISTENING)
				{
					toState(PTP_LISTENING, rtOpts, ptpClock);
				}
			}
		}

		break;

	case PTP_MASTER:
		DBGV("doState: Port state: PTP_MASTER\n");
		if(timerExpired(SYNC_INTERVAL_TIMER, ptpClock->itimer, ptpClock->port_id_field))
		{
			DBGV("doState: event SYNC_INTERVAL_TIMEOUT_EXPIRES\n");
			ptpClock->sentSync = FALSE;
			issueSync(rtOpts, ptpClock);
		}


		if(timerExpired(ANNOUNCE_INTERVAL_TIMER, ptpClock->itimer, ptpClock->port_id_field))
		{
			DBGV("doState: event ANNOUNCE_INTERVAL_TIMEOUT_EXPIRES\n");
			issueAnnounce(rtOpts, ptpClock);
		}

		handle(rtOpts, ptpClock);

		break;

	case PTP_DISABLED:
		DBGV("doState: Port state: DISABLED\n");
		handle(rtOpts, ptpClock);
		break;

	default:
		DBG("doState: ignoring unrecognized port state %d\n", ptpClock->port_state);
		break;
	}
}

/* perform actions required when leaving 'port_state' and entering 'state' */

void toState(UInteger8     state,
	RunTimeOpts * rtOpts,
	PtpClock *    ptpClock
	)
{
	ptpClock->message_activity = TRUE;

	/* leaving state tasks */
	switch(ptpClock->port_state)
	{
	case PTP_MASTER:
		//
		// Leaving MASTER state to some other state, stop Sync Interval
		// (transmit) timer and start up Sync Receipt timer:
		//
		timerStop(SYNC_INTERVAL_TIMER,      // Stop Sync Interval timer 
			ptpClock->itimer);
		timerStop(ANNOUNCE_INTERVAL_TIMER,  // Stop Announce Interval timer 
			ptpClock->itimer);

		timerStart(SYNC_RECEIPT_TIMER,      // Start Sync Receipt timer
			PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->sync_interval),
			ptpClock->itimer);
		break;

	case PTP_SLAVE:
		//
		// Leaving slave state to some other state, initialize the clock
		//
		initClock(rtOpts, ptpClock);
		break;

	default:
		break;
	}

	/* entering state tasks */
	switch(state)
	{
	case PTP_INITIALIZING:
		//
		// Entering INITIALIZING state:
		// stop the Sync receipt timer and set state to INITIALIZING
		//
		DBG("toState: entering state PTP_INITIALIZING\n");


		timerStop(SYNC_RECEIPT_TIMER,
			ptpClock->itimer);
		ptpClock->port_state = PTP_INITIALIZING;
		break;

	case PTP_FAULTY:
		//
		// Entering FAULTY state:
		// stop the Sync receipt timer and set state to FAULTY
		//
		DBG("toState: entering state PTP_FAULTY\n");


		timerStop(SYNC_RECEIPT_TIMER,
			ptpClock->itimer);
		ptpClock->port_state = PTP_FAULTY;
		break;

	case PTP_DISABLED:
		//
		// Entering DISABLED state:
		// stop the Sync receipt timer and set state to DISABLED
		//
		DBG("toState: entering state change to PTP_DISABLED\n");

#ifdef CONFIG_MPC831X
		/* Set Yellow LED and set meter to max */
		yellow_alarm(TRUE);
		led_meter(255);
#endif

		timerStop(SYNC_RECEIPT_TIMER,
			ptpClock->itimer);
		ptpClock->port_state = PTP_DISABLED;
		break;

	case PTP_LISTENING:
		//
		// Entering LISTENING state:
		// Start the Sync receipt timer and set state to LISTENING
		//
		DBG("toState: entering state PTP_LISTENING\n");


		timerStart(SYNC_RECEIPT_TIMER,
			PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->sync_interval),
			ptpClock->itimer);

		ptpClock->port_state = PTP_LISTENING;
		break;

	case PTP_MASTER:
		//
		// Entering MASTER state:
		// First check if port state is not equal to PRE_MASTER
		//
		DBG("toState: entering state PTP_MASTER\n");


		if(ptpClock->port_state != PTP_PRE_MASTER)
		{
			//
			// Port state is not PRE_MASTER:
			// Start the Sync and Announce interval (transmit) timers
			//
			DBGV("toState: starting SYNC interval timer, interval: %d\n",
				ptpClock->sync_interval
				);
			timerStart(SYNC_INTERVAL_TIMER,
				PTP_SYNC_INTERVAL_TIMEOUT(ptpClock->sync_interval),
				ptpClock->itimer
				);

			if(rtOpts->ptpv2)
			{
				/* Also start announce timer if running PTP version 2 */

				DBGV("toState: starting ANNOUNCE interval timer, interval: %d\n",
					ptpClock->announce_interval
					);
				timerStart(ANNOUNCE_INTERVAL_TIMER,
					PTP_SYNC_INTERVAL_TIMEOUT(ptpClock->announce_interval),
					ptpClock->itimer
					);
			}
		}
		//
		// Stop the Sync receipt timer and set port state to MASTER
		//
		timerStop(SYNC_RECEIPT_TIMER,
			ptpClock->itimer);
		timerStop(ANNOUNCE_RECEIPT_TIMER,
			ptpClock->itimer);

		ptpClock->port_state = PTP_MASTER;
		break;

	case PTP_PASSIVE:
		//
		// Entering PASSIVE state:
		// set port state to PASSIVE
		//
		DBG("toState: entering state PTP_PASSIVE\n");
		ptpClock->port_state = PTP_PASSIVE;
		break;

	case PTP_UNCALIBRATED:
		//
		// Entering UNCALIBRATED state:
		// set port state to UNCALIBRATED
		//
		DBG("toState: entering state PTP_UNCALIBRATED\n");
		ptpClock->port_state = PTP_UNCALIBRATED;
		break;

	case PTP_SLAVE:
		DBG("toState: entering state PTP_SLAVE\n");



		initClock(rtOpts, ptpClock);

		/* R is chosen to allow a few syncs before we first get a one-way delay estimate */
		/* this is to allow the offset filter to fill for an accurate initial clock reset */

		ptpClock->Q = 0;
		ptpClock->R = getRand(&ptpClock->random_seed) % 4 + 4;

		DBG("toState: Q = %d, R = %d\n", ptpClock->Q, ptpClock->R);

		ptpClock->waitingForFollow = FALSE;
		clearTime(&ptpClock->t3_delay_req_tx_time);
		clearTime(&ptpClock->t4_delay_req_rx_time);

		timerStart(SYNC_RECEIPT_TIMER,
			PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->sync_interval),
			ptpClock->itimer);

		ptpClock->port_state = PTP_SLAVE;
		break;

	default:
		DBG("toState: ignoring set to unrecognized state %d!\n", state);
		break;

	}  // End state switch statement


	//
	// Test if display statisitics option is TRUE.
	// If true, display statisitics after this state change
	//
	if(rtOpts->displayStats)
	{
		displayStats(rtOpts, ptpClock);
	}
}

/* check for and handle received messages */
void handle(RunTimeOpts * rtOpts, PtpClock *    ptpClock)
{
	int            ret;
	ssize_t        length;
	Boolean        isFromSelf;
	TimeInternal   time = { 0, 0 };
	TimeInternal   handle_timeout;
	UInteger16     current_sequence;

	DBGV("handle:\n");

	//
	// First check if there is already message activity
	//
	if(!ptpClock->message_activity)
	{
		//
		// No messge acitivity, call netSelect to poll the socket
		// for any messages.  In this case, we specify a
		// null timeout, so if nothing is available to process
		// the call to select() within netSelect will
		// wait until a message is received, or the timeout
		// thread is scheduled and that will also then shortly
		// later cause the select() function to return (i.e.
		// wake up).
		// pending

		// AKB: New multiport code support, set timeout to zero as we
		// now wait for events in the main loop.  All we want to 
		// do here is to check if anything to process and
		// return immediately.

		clearTime(&handle_timeout);

		ret = netSelect(&handle_timeout, &ptpClock->netPath);

		/* AKB: Commented out old code

		 ifdef CONFIG_MPC831X
		 if (ptpClock->tx_complete_poll_timeout.nanoseconds > 0)
		 {
		 DBGV("handle: Calling netSelect tx packet complete poll timeout\n");
		 ret = netSelect(&ptpClock->tx_complete_poll_timeout,
		 &ptpClock->netPath
		 );
		 }
		 else
		 {
		 DBGV("handle: Calling netSelect with NULL timeout\n");
		 ret = netSelect(0,
		 &ptpClock->netPath
		 );
		 }
		 else
		 DBGV("handle: Calling netSelect with NULL timeout\n");
		 ret = netSelect(0, &ptpClock->netPath);
		 endif

		 */

		if(ret < 0)
		{
			//
			// If return code is negative, then there
			// was a socket failure, change state
			// to faulty and return
			//
			PERROR("handle: failed to poll sockets");
			toState(PTP_FAULTY, rtOpts, ptpClock);
			return;
		}
		//
		// If Poll of socket returns zero, then there
		// are no messages to handle, so return
		//
		else
		{
			if(!ret)
			{
				DBGV("handle: nothing to process, returning\n");
				return;
			}
			DBGV("handle: TRUE return from netSelect\n");
		}
		//
		// If poll of socket is a positive number, then
		// there is a message to process, continue.
		//
	}

	DBGV("handle: message to process, checking Event Socket\n");

	//
	// Check if event message needs to be processed
	//

	length = netRecvEvent(ptpClock->msgIbuf,
		&time,
		&ptpClock->netPath,
		ptpClock->current_utc_offset
		);
	if(length < 0)
	{
		//
		// Got negative number on call to netRecvEvent, 
		// socket failed, change to FAULTY state
		//
		PERROR("handle: failed to receive on the event socket");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}
	else
	{
		if(!length)
		{
			//
			// If we are here, then netRecvEvent returned zero
			// Check for general event using netRecvGeneral
			//
			DBGV("handle: message to process, checking General Socket\n");
			length = netRecvGeneral(ptpClock->msgIbuf, &ptpClock->netPath);
			if(length < 0)
			{
				PERROR("handle: failed to receive on the general socket");
				toState(PTP_FAULTY, rtOpts, ptpClock);
				return;
			}
			else
			{
				if(!length)
				{
					//
					// If we are here, then both the UDP Event and General
					// socket reads returned 0.
					return;					
				}
			}
		}
	}

	ptpClock->message_activity = TRUE;

	if(!msgPeek(ptpClock->msgIbuf, length))
	{
		return;
	}

	if(length < HEADER_LENGTH)
	{
		PERROR("handle: message shorter than header length\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	/* Get PTP Version of current message */
	ptpClock->current_msg_version = msgGetPtpVersion(ptpClock->msgIbuf);
	DBGV("handle: msgGetPtpVersion: %d\n",
		ptpClock->current_msg_version
		);

	if(ptpClock->current_msg_version == 1)
	{
		/* PTP version 1 */
		DBGV("handle: Processing PTP version 1 message\n");
		DBGV("handle: Unpacking received message header\n");

		/* Unpack common version 1 PTP header fields: */
		msgUnpackHeader(ptpClock->msgIbuf, &ptpClock->msgTmpHeader);

		DBGV("handle: event Receipt of Message\n");
		DBGV("handle:   type..... %d\n",
			ptpClock->msgTmpHeader.control
			);
		DBGV("handle:   uuid..... %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
			ptpClock->msgTmpHeader.sourceUuid[0],
			ptpClock->msgTmpHeader.sourceUuid[1],
			ptpClock->msgTmpHeader.sourceUuid[2],
			ptpClock->msgTmpHeader.sourceUuid[3],
			ptpClock->msgTmpHeader.sourceUuid[4],
			ptpClock->msgTmpHeader.sourceUuid[5]
			);
		DBGV("handle:   sequence. %d\n",
			ptpClock->msgTmpHeader.sequenceId
			);

		/* Convert message type to V2 for subsequent joint V1/V2 processing */

		switch(ptpClock->msgTmpHeader.control)
		{
		case PTP_SYNC_MESSAGE:                            /* 0x00 */
			ptpClock->v2_msg_type = V2_SYNC_MESSAGE;       /* 0x0  */
			break;

		case PTP_DELAY_REQ_MESSAGE:                       /* 0x01 */
			ptpClock->v2_msg_type = V2_DELAY_REQ_MESSAGE;  /* 0x1  */
			break;

		case PTP_FOLLOWUP_MESSAGE:                        /* 0x02 */
			ptpClock->v2_msg_type = V2_FOLLOWUP_MESSAGE;   /* 0x8  */
			break;

		case PTP_DELAY_RESP_MESSAGE:                      /* 0x03 */
			ptpClock->v2_msg_type = V2_DELAY_RESP_MESSAGE; /* 0x9  */
			break;

		case PTP_MANAGEMENT_MESSAGE:                      /* 0x04 */
			ptpClock->v2_msg_type = V2_MANAGEMENT_MESSAGE; /* 0xD  */
			break;

		default:
			/* Invalid Version 1 message type */
			/* Temp for Debug, print message and return */
			DBG("handle: Invalid PTP version 1 control field: %u\n",
				ptpClock->msgTmpHeader.control
				);
		}

		/* isFromSelf is used to identify PTP packets that are echoed back
		 * to us as the IP/UDP socket is programmed to echo back all packets
		 * that are IP multicast.  This is used to then get the timestamp
		 * of the sent packet in certain cases (e.g. when Master
		 * has sent a sync, when it sees a Sync that is "from self"
		 * it uses that to get the timestamp for the preciseOriginTimestamp
		 * of the follow up packet)
		 */

		isFromSelf = ptpClock->msgTmpHeader.sourceCommunicationTechnology
			== ptpClock->port_communication_technology
			&& ptpClock->msgTmpHeader.sourcePortId
			== ptpClock->port_id_field
			&& !memcmp(ptpClock->msgTmpHeader.sourceUuid,
			ptpClock->port_uuid_field,
			PTP_UUID_LENGTH
			);
		current_sequence = ptpClock->msgTmpHeader.sequenceId;
	}
	else if(ptpClock->current_msg_version == 2)
	{
		/* PTP version 2 */
		DBGV("handle: Processing PTP version 2 message\n");
		DBGV("handle: Unpacking received message header\n");

		/* Unpack common version 2 PTP header fields: */
		msgUnpackV2Header(ptpClock->msgIbuf, &ptpClock->v2MsgTmpHeader);

		ptpClock->v2_msg_type
			= ptpClock->v2MsgTmpHeader.transportSpecificAndMessageType & 0x0F;
		ptpClock->rx_transport_specific
			= ptpClock->v2MsgTmpHeader.transportSpecificAndMessageType >> 4;

		DBGV("handle: event Receipt of Message\n");
		DBGV("handle:   messageType.......... %u\n", ptpClock->v2_msg_type);
		DBGV("handle:   transportSpecific.... %u\n", ptpClock->rx_transport_specific);
		DBGV("handle:   sequenceId........... %4.4x\n", ptpClock->v2MsgTmpHeader.sequenceId);


		isFromSelf = (ptpClock->v2MsgTmpHeader.sourcePortId.portNumber
			== ptpClock->port_id_field)
			&& !memcmp(ptpClock->v2MsgTmpHeader.sourcePortId.clockIdentity,
			ptpClock->port_clock_identity,
			8
			);
		current_sequence = ptpClock->v2MsgTmpHeader.sequenceId;

	}

	else /* Unknown PTP version */
	{
		/* Temp for DEBUG, ignore any future versions */
		DBG("handle: Unkown version!!  Ignoring message\n");
		return;
	}

	DBGV("handle: isFrom self is %s\n", isFromSelf ? "TRUE" : "FALSE");


	DBGV("handle:   time..... %us %dns\n",
		time.seconds,
		time.nanoseconds
		);

	/* subtract the inbound latency adjustment if it is not a loop back and the
	 * time stamp seems reasonable
	 */

	if(!isFromSelf && time.seconds > 0)
	{
		subTime(&time, &time, &rtOpts->inboundLatency);
	}

	switch(ptpClock->v2_msg_type)
	{
	case V2_SYNC_MESSAGE:  /* V1 control: 0x00, V2 type: 0x0 */
		DBGV("handle: SYNC_MESSAGE, length: %d\n",
			length
			);
		handleSync(&ptpClock->msgTmpHeader,
			&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			&time,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	case V2_FOLLOWUP_MESSAGE: /* V1 control: 0x02, V2 type: 0x8 */
		DBGV("handle: FOLLOWUP_MESSAGE, length: %d\n",
			length
			);
		handleFollowUp(&ptpClock->msgTmpHeader,
			&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	case V2_DELAY_REQ_MESSAGE: /* V1 control: 0x01, V2 type: 0x1 */
		DBGV("handle: DELAY_REQ_MESSAGE, length: %d\n",
			length
			);
		if(getSeconds(&time) != 0)
		{
		}
		else
		{
			DBG("handle: SYNC_MESSAGE, time == 0, ignoring\n");
			return;
		}
		handleDelayReq(&ptpClock->msgTmpHeader,
			&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			&time,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	case V2_DELAY_RESP_MESSAGE: /* V1 control: 0x03, V2 type: 0x9 */
		DBGV("handle: DELAY_RESP_MESSAGE, length: %d\n",
			length
			);
		handleDelayResp(&ptpClock->msgTmpHeader,
			&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	case PTP_MANAGEMENT_MESSAGE: /* V1 control: 0x04, V2 type: 0xD */
		DBGV("handle: MANAGEMENT_MESSAGE, length: %d\n",
			length
			);
		if(ptpClock->current_msg_version == 1)
		{
			handleManagement(&ptpClock->msgTmpHeader,
				ptpClock->msgIbuf,
				length,
				isFromSelf,
				rtOpts,
				ptpClock
				);
		}
		else
		{
			DBG("handle: Version 2 management message not supported\n");
		}
		break;

	case V2_ANNOUNCE_MESSAGE:  /* V2 type: 0xb */
		DBGV("handle: ANNOUNCE_MESSAGE, length: %d\n",
			length
			);
		handleAnnounce(&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			&time,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;


	case V2_PDELAY_REQ_MESSAGE: /* V2 type: 0x2 */
		DBGV("handle: PDELAY_REQ_MESSAGE, length: %d\n",
			length
			);
		handlePDelayReq(&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			&time,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	case V2_PDELAY_RESP_MESSAGE: /* V2 type: 0x3 */
		DBGV("handle: PDELAY_RESP_MESSAGE, length: %d\n",
			length
			);
		handlePDelayResp(&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			&time,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	case V2_PDELAY_RESP_FOLLOWUP_MESSAGE: /* V2 type: 0xA */
		DBGV("handle: PDELAY_RESP_FOLLOWUP MESSAGE, length: %d\n",
			length
			);
		handlePDelayRespFollowUp(&ptpClock->v2MsgTmpHeader,
			ptpClock->msgIbuf,
			length,
			isFromSelf,
			rtOpts,
			ptpClock
			);
		break;

	default:
		DBG("handle: unrecognized message\n");
		break;
	}
}


void handleSyncTxComplete(
	TimeInternal *time,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	/* Inbound Sync message is from us (echoed back from the socket
	 * or detected as a transmit complete by polling the driver).
	 * Check if we are clock follow up message capable and if
	 * so, add any outbound latency to the reported Sync message
	 * transmission time, then build and send a follow up message.
	 */
	ptpClock->sentSync = FALSE;

	if(ptpClock->clock_followup_capable)
	{
		addTime(time, time, &rtOpts->outboundLatency);
		issueFollowup(time, rtOpts, ptpClock);
	}
}

void handlePDelayRespTxComplete(
	TimeInternal *time,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	/* Inbound Sync message is from us (echoed back from the socket
	 * or detected as a transmit complete by polling the driver).
	 * Check if we are clock follow up message capable and if
	 * so, add any outbound latency to the reported Sync message
	 * transmission time, then build and send a follow up message.
	 */
	ptpClock->sentPDelayResp = FALSE;

	if(ptpClock->clock_followup_capable)
	{
		addTime(time, time, &rtOpts->outboundLatency);
		issuePDelayRespFollowup(time, rtOpts, ptpClock);
	}
}


void handleAnnounce(V2MsgHeader  *header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	MsgAnnounce *announce;  /* V2 announce  message */
	UInteger16   sequence_delta;

	if(length < V2_ANNOUNCE_LENGTH)
	{
		PERROR("handleAnnounce: short announce message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	switch(ptpClock->port_state)
	{
	case PTP_FAULTY:
	case PTP_INITIALIZING:
	case PTP_DISABLED:
		DBG("handleAnnounce: FAULTY, INITIALIZING or DISABLED, disregard\n");
		return;

	case PTP_UNCALIBRATED:
	case PTP_SLAVE:
		if(isFromSelf)
		{
			DBGV("handleAnnounce: SLAVE or UNCALIBRATED, ignore from self\n");
			return;
		}

		DBGV("handleAnnounce: SLAVE or UNCALIBRATED:\n");
		DBGV("handleAnnounce: looking for id %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
			ptpClock->parent_clock_identity[0],
			ptpClock->parent_clock_identity[1],
			ptpClock->parent_clock_identity[2],
			ptpClock->parent_clock_identity[3],
			ptpClock->parent_clock_identity[4],
			ptpClock->parent_clock_identity[5],
			ptpClock->parent_clock_identity[6],
			ptpClock->parent_clock_identity[7]
			);

		if(header->sourcePortId.portNumber == ptpClock->parent_port_id
			&& !memcmp(header->sourcePortId.clockIdentity, ptpClock->parent_clock_identity, 8)
			)
		{
			DBGV("handleAnnounce: announce is from current parent\n");
			/* Check sequence number, making sure we handle the transition from
			 * FFFF to 0000.  This is done by checking the absolute difference by
			 * subtracting 2 16 bit unsigned values and checking
			 */
			sequence_delta = header->sequenceId
				- ptpClock->parent_last_announce_sequence_number;

			if(sequence_delta == 0)
			{
				DBG("handleAnnounce: Duplicate sequence %u received!, ignoring\n",
					header->sequenceId
					);
				return;
			}

			/* AKB: Commenting out, need better algorigthm for detecting out of range
			 * as we cannot have any false detections
			 *
			 if  (sequence_delta > 0x7FFF)
			 {
			 DBG("handleAnnounce: Out of order sequence, received: %u, last: %u\n",
			 header->sequenceId,
			 ptpClock->parent_last_announce_sequence_number
			 );
			 return;
			 }
			 */

			if(sequence_delta != 1)
			{
				/* 1 or more announce Messages possibly lost/discarded on the net
				 * or this is the first one we have received
				 */
				DBG("handleAnnounce: Expecting sequence: %u, received: %u\n",
					(UInteger16)(ptpClock->parent_last_announce_sequence_number + 1),
					header->sequenceId
					);
			}

			/* AKB: Calling V2 to V1 conversion (may use in future version of this software */

			convert_v2_header_to_v1(&ptpClock->v2MsgTmpHeader,
				&ptpClock->msgTmpHeader
				);
			/* addV2Foreign() takes care of msgUnpackAnnounce() */


			ptpClock->record_update = TRUE;
			announce = addV2Foreign(ptpClock->msgIbuf,
				&ptpClock->v2MsgTmpHeader,
				ptpClock
				);

			v2_s1(header, announce, ptpClock);

			ptpClock->parent_last_announce_sequence_number = header->sequenceId;

		}


	case PTP_MASTER:
	default:
		if(!isFromSelf)
		{
			/* Inbound Announce message is from somone else, indicate record
			 * update is true and call add foreign to check if we know
			 * about this foreign master, or if not, add it to the table
			 * if there is room.
			 */

			/* AKB: for now, calling addForeign with converted V2 to V1 header, will probably
			 * need to change this in the future for fully handling V2 announce messages in
			 * their native format
			 */
			DBGV("handleAnnounce: state %d, announce is from outside\n",
				ptpClock->port_state
				);

			convert_v2_header_to_v1(&ptpClock->v2MsgTmpHeader,
				&ptpClock->msgTmpHeader
				);
			ptpClock->record_update = TRUE;

			DBGV("handleAnnounce: call add foreign\n");

			addV2Foreign(ptpClock->msgIbuf,
				&ptpClock->v2MsgTmpHeader,
				ptpClock
				);
		}
		else
		{
			/* Inbound Announce message is from us (echoed back from the socket).
			 * Nothing to do if we see our own announce as there is no follow up
			 * needed.
			 */
			DBGV("handleAnnouce: from self, ignoring\n");
		}
		break;
	}
}

short temp_debug_issue_delay_counter = 0;

void handleSync(MsgHeader    *header,
	V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	MsgSync      *sync;
	V2MsgSync    *v2sync;
	TimeInternal  originTimestamp;
	Boolean       sync_source_ok;
	UInteger16    sequence_delta;
	Boolean       current_sequence;

	DBGV("handleSync: length = %d\n", length);

	if(
		((ptpClock->current_msg_version == 1) && (length < SYNC_PACKET_LENGTH))
		|| length < V2_SYNC_LENGTH
		)
	{
		PERROR("handleSync: short sync message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	if(!isNonZeroTime(time))
	{
		DBG("handleSync: time is zero, ignoring\n");
		return;
	}

	switch(ptpClock->port_state)
	{
	case PTP_FAULTY:
	case PTP_INITIALIZING:
	case PTP_DISABLED:
		DBG("handleSync: FAULTY, INITIALIZING or DISABLED disregard\n");
		return;

	case PTP_UNCALIBRATED:
	case PTP_SLAVE:
		if(isFromSelf)
		{
			DBGV("handleSync: ignore from self\n");
			return;
		}

		if(getFlag(header->flags, PTP_SYNC_BURST)
			&& !ptpClock->burst_enabled
			)
		{
			return;
		}

		if(ptpClock->current_msg_version == 1)
		{
			/* Version 1 SYNC message: */
			DBGV("handleSync: looking for uuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
				ptpClock->parent_uuid[0], ptpClock->parent_uuid[1], ptpClock->parent_uuid[2],
				ptpClock->parent_uuid[3], ptpClock->parent_uuid[4], ptpClock->parent_uuid[5]);

			sequence_delta = header->sequenceId
				- ptpClock->parent_last_sync_sequence_number;

			sync_source_ok =
				(header->sourceCommunicationTechnology                  // Check technology same
				== ptpClock->parent_communication_technology
				&& header->sourcePortId                                   // Check source port equal
				== ptpClock->parent_port_id
				&& !memcmp(header->sourceUuid,                            // Check UUID equal
				ptpClock->parent_uuid,
				PTP_UUID_LENGTH
				)
				);
		}
		else
		{
			/* Version 2 SYNC message: */
			DBGV("handleSync: looking for id %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
				ptpClock->parent_clock_identity[0],
				ptpClock->parent_clock_identity[1],
				ptpClock->parent_clock_identity[2],
				ptpClock->parent_clock_identity[3],
				ptpClock->parent_clock_identity[4],
				ptpClock->parent_clock_identity[5],
				ptpClock->parent_clock_identity[6],
				ptpClock->parent_clock_identity[7]
				);

			sequence_delta = v2_header->sequenceId
				- ptpClock->parent_last_sync_sequence_number;

			sync_source_ok =
				(v2_header->sourcePortId.portNumber                      // Check source port equal
				== ptpClock->parent_port_id
				&& !memcmp(v2_header->sourcePortId.clockIdentity,          // Check clock identity equal
				ptpClock->parent_clock_identity,
				8
				)
				);
		}
		if(sync_source_ok)
		{

			/* Source of the sync message is OK, check if the sequence number is OK */

			if(sequence_delta == 0)
			{
				DBG("handleSync: Possible duplicate sequence %u received!, ignoring\n",
					header->sequenceId
					);
				return;
			}

			/* AKB: Commenting this out for now, as we could become a slave at anytime

			if  (sequence_delta > 0x7FFF)
			{
			DBG("handleSync: Out of order sequence, received: %u, last: %u\n",
			header->sequenceId,
			ptpClock->parent_last_sync_sequence_number
			);
			return;
			}
			*/

			if(sequence_delta != 1)
			{
				/* 1 or more sync Messages may have been lost/discarded on the net
				 * or this is the first sync message we have received
				 */
				DBG("handleSync: Expecting sequence: %u, received: %u\n",
					(UInteger16)(ptpClock->parent_last_sync_sequence_number + 1),
					header->sequenceId
					);
			}


			if(ptpClock->current_msg_version == 1)
			{
				clearTime(&ptpClock->sync_correction);
				clearTime(&ptpClock->followup_correction);
				/* addForeign() takes care of msgUnpackSync() */
				ptpClock->record_update = TRUE;
				sync = addForeign(ptpClock->msgIbuf,
					&ptpClock->msgTmpHeader,
					ptpClock);

				if(sync->syncInterval != ptpClock->sync_interval)
				{
					DBGV("handleSync: message's sync interval is %d, but clock's is %d\n",
						sync->syncInterval,
						ptpClock->sync_interval
						);
					/* spec recommends handling a sync interval discrepancy as a fault */
				}
			}
			else
			{
				v2sync = &ptpClock->msgTmp.v2sync;
				msgUnpackV2Sync(msgIbuf, v2sync);

				/* Store Version 2 sequence number
				 * (for Version 1, this is done when s1 is called)
				 */

				ptpClock->parent_last_sync_sequence_number = v2_header->sequenceId;

				/* Get correction field, change to Internal time */

				v2CorrectionToInternalTime(&ptpClock->sync_correction,
					v2_header->correctionField
					);

			}

			/* Copy local time of reception of this SYNC message to
			 * sync_rx_time
			 */

			copyTime(&ptpClock->t2_sync_rx_time,  // Destination
				time                         // Source
				);

			if(ptpClock->current_msg_version == 1)
			{
				ptpClock->waitingForFollow = getFlag(header->flags, PTP_ASSIST);
			}
			else
			{
				ptpClock->waitingForFollow = (v2_header->flags[0] & V2_TWO_STEP_FLAG
					) == V2_TWO_STEP_FLAG;
			}

			if(!ptpClock->waitingForFollow)
			{
				if(ptpClock->current_msg_version == 1)
				{
					toInternalTime(&originTimestamp,
						&sync->originTimestamp,
						&ptpClock->halfEpoch);
				}
				else
				{
					v2ToInternalTime(&originTimestamp,
						&v2sync->originTimestamp
						);

				}
				updateOffset(&originTimestamp,
					&ptpClock->t2_sync_rx_time,
					&ptpClock->ofm_filt,
					rtOpts,
					ptpClock);

				updateClock(rtOpts, ptpClock);
			}
			else
			{
				ptpClock->waitingForFollow = TRUE;
			}

			if(ptpClock->current_msg_version == 1)
			{
				s1(header, sync, ptpClock);
			}

			if(!(--ptpClock->R))
			{
				if(++temp_debug_issue_delay_counter < 1000)
					issueDelayReq(rtOpts, ptpClock);

				ptpClock->Q = 0;
				ptpClock->R = getRand(&ptpClock->random_seed)
					% (PTP_DELAY_REQ_INTERVAL - 2)
					+ 2;

				DBGV("handleSync: Q = %d, R = %d\n", ptpClock->Q, ptpClock->R);
			}

			DBGV("handleSync: SYNC_RECEIPT_TIMER reset\n");
			timerStart(SYNC_RECEIPT_TIMER,
				PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->sync_interval),
				ptpClock->itimer);
		}
		else
		{
			DBG("handleSync: Not from current master\n");
		}

	case PTP_MASTER:
	default:
		DBGV("handleSync: MASTER, LISTENING, PASSIVE states\n");
		if(ptpClock->current_msg_version == 1)
		{
			DBGV("handleSync: Version 1 message\n");
			/* Version 1 SYNC message, check communication technology (not checked for
			 * version 2)
			 */
			if(!(header->sourceCommunicationTechnology == ptpClock->clock_communication_technology
				|| header->sourceCommunicationTechnology == PTP_DEFAULT
				|| ptpClock->clock_communication_technology == PTP_DEFAULT
				)
				)
			{
				DBG("handleSync: Ignoring Invalid Version 1 Communication technology\n");
				DBG("handleSync: Message Communication Technology: %d\n",
					header->sourceCommunicationTechnology
					);
				DBG("handleSync: Local Communication Technology:   %d\n",
					ptpClock->clock_communication_technology
					);
				DBG("handleSync: PTP_DEFAULT:                      %d\n",
					PTP_DEFAULT
					);
				return;
			}

			DBGV("handleSync: Version 1 Communication technology OK\n");
			/* Version 1 message communication technolgy is OK,
			 * Check if message is from self (looped back via socket)
			 * If not, then this is a message from the outside world
			 * and for version 1, Sync messages are used for both synchronization
			 * and for "announcing" grandmaster information and
			 * we then run the version 1 foreign update routine to
			 * update database and also see later if we need to change state
			 */
			if(!isFromSelf)
			{
				/* Not from self, Version 1 and the inbound Sync message is from somone else,
				 * indicate record update is true and call add foreign to
				 * check if we know about this foreign master, or if not, add it to the table
				 * if there is room.
				 */
				ptpClock->record_update = TRUE;
				addForeign(ptpClock->msgIbuf, &ptpClock->msgTmpHeader, ptpClock);
				return;

			}
			else
			{
				/* Is from self, check if this is the sequence currently in process */
				current_sequence = (header->sequenceId == ptpClock->last_sync_tx_sequence_number);
			}
		}
		else
		{
			/* Version 2 message */
			if(isFromSelf)
			{
				/* Is from self, check if this is the sequence currently in process */
				current_sequence = (v2_header->sequenceId == ptpClock->last_sync_tx_sequence_number);
			}
		}

		if(isFromSelf)
		{

			/* Inbound Version 1 or 2 Sync message is from us (echoed back from the socket).
			 * Check if we are clock follow up message capable and if
			 * so, add any outbound latency to the reported Sync message
			 * transmission time, then build and send a follow up message.
			 */
			if(ptpClock->sentSync == TRUE)
			{
				/* Check to make sure that the Sync message is the one we expect,
				 * if not, it may be an old one that we are just now getting
				 * the frame looped back.
				 */

				if(current_sequence)
				{
					handleSyncTxComplete(time, rtOpts, ptpClock);
				}
				else
				{
					DBG("handleSync: isFromSelf, but old sequence number, ignore\n");
				}
			}
			else
			{
				DBGV("handleSync: from self, sentSync == FALSE, assume Follow up sent\n");
			}
		}
		break;
	}
}

void handleFollowUp(MsgHeader   *header,
	V2MsgHeader *v2_header,
	Octet       *msgIbuf,
	ssize_t      length,
	Boolean      isFromSelf,
	RunTimeOpts *rtOpts,
	PtpClock    *ptpClock
	)
{
	MsgFollowUp   *follow;
	V2MsgFollowUp *v2follow;
	TimeInternal   preciseOriginTimestamp;
	Boolean        followup_expected;

	if(	((ptpClock->current_msg_version == 1) && (length < FOLLOW_UP_PACKET_LENGTH)) || length < V2_FOLLOWUP_LENGTH)
	{
		PERROR("handleFollowUp: short follow up message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	switch(ptpClock->port_state)
	{
	case PTP_SLAVE:
		if(isFromSelf)
		{
			DBGV("handleFollowUp: ignore from self\n");
			return;
		}

		if(ptpClock->current_msg_version == 1)
		{

			/* Version 1 Follow up message */

			if(getFlag(header->flags, PTP_SYNC_BURST) && !ptpClock->burst_enabled)
				return;

			DBGV("handleFollowUp: looking for uuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
				ptpClock->parent_uuid[0], ptpClock->parent_uuid[1], ptpClock->parent_uuid[2],
				ptpClock->parent_uuid[3], ptpClock->parent_uuid[4], ptpClock->parent_uuid[5]);

			follow = &ptpClock->msgTmp.follow;
			msgUnpackFollowUp(ptpClock->msgIbuf, follow);

			followup_expected
				= (ptpClock->waitingForFollow
				&& follow->associatedSequenceId == ptpClock->parent_last_sync_sequence_number
				&& header->sourceCommunicationTechnology == ptpClock->parent_communication_technology
				&& header->sourcePortId == ptpClock->parent_port_id
				&& !memcmp(header->sourceUuid, ptpClock->parent_uuid, PTP_UUID_LENGTH)
				);
		}
		else
		{

			/* Version 2 Follow up messsage */

			DBGV("handleFollowUp: Version 2 message, calling unpack function\n");

			v2follow = &ptpClock->msgTmp.v2follow;

			msgUnpackV2FollowUp(ptpClock->msgIbuf, v2follow);

			DBGV("handleFollowUp: looking for id %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
				ptpClock->parent_clock_identity[0],
				ptpClock->parent_clock_identity[1],
				ptpClock->parent_clock_identity[2],
				ptpClock->parent_clock_identity[3],
				ptpClock->parent_clock_identity[4],
				ptpClock->parent_clock_identity[5],
				ptpClock->parent_clock_identity[6],
				ptpClock->parent_clock_identity[7]
				);

			DBGV("handleFollowUp: message id     %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
				v2_header->sourcePortId.clockIdentity[0],
				v2_header->sourcePortId.clockIdentity[1],
				v2_header->sourcePortId.clockIdentity[2],
				v2_header->sourcePortId.clockIdentity[3],
				v2_header->sourcePortId.clockIdentity[4],
				v2_header->sourcePortId.clockIdentity[5],
				v2_header->sourcePortId.clockIdentity[6],
				v2_header->sourcePortId.clockIdentity[7]
				);

			DBGV("handleFollowUp: expecting port %4.4x, is %4.4x\n",
				ptpClock->parent_port_id,
				v2_header->sourcePortId.portNumber
				);

			DBGV("handleFollowUp: expecting sequence %4.4x, is %4.4x\n",
				ptpClock->parent_last_sync_sequence_number,
				v2_header->sequenceId
				);


			followup_expected
				= (ptpClock->waitingForFollow
				&& (v2_header->sequenceId == ptpClock->parent_last_sync_sequence_number)
				&& (v2_header->sourcePortId.portNumber == ptpClock->parent_port_id)
				&& !memcmp(v2_header->sourcePortId.clockIdentity,
				ptpClock->parent_clock_identity,
				8
				)
				);
		}

		if(followup_expected)
		{
			/* Follow Up message is expected and OK */

			DBGV("handleFollowUp: expected and OK\n");

			ptpClock->waitingForFollow = FALSE;
			if(ptpClock->current_msg_version == 1)
			{

				/* V1: Convert time to internal time, update the offset from Master and then
				 * update the clock.
				 */

				toInternalTime(&preciseOriginTimestamp,
					&follow->preciseOriginTimestamp,
					&ptpClock->halfEpoch);

				/* AKB: Copy precise Origin Timestamp from Follow up message to
				 * Sync Send time for statistics purposes
				 */
				copyTime(&ptpClock->t1_sync_tx_time,  // Destination
					&preciseOriginTimestamp      // Source
					);

			}
			else
			{
				/* V2: Convert time to internal time, update the offset from Master and then
				 * update the clock.
				 */

				v2ToInternalTime(&preciseOriginTimestamp,
					&v2follow->preciseOriginTimestamp);
				/* AKB: Copy precise Origin Timestamp from Follow up message to
				 * Sync Send time for statistics purposes
				 */
				copyTime(&ptpClock->t1_sync_tx_time,  // Destination
					&preciseOriginTimestamp      // Source
					);

				/* Get correction field, change to Internal time */

				v2CorrectionToInternalTime(&ptpClock->followup_correction,
					v2_header->correctionField
					);


			}
			updateOffset(&preciseOriginTimestamp,    /* SYNC send time from Master Follow up */
				&ptpClock->t2_sync_rx_time, /* Previously stored SYNC rx time */
				&ptpClock->ofm_filt,        /* Filtered Offset from Master */
				rtOpts,                    /* Pointer to Run time options structure */
				ptpClock                   /* Pointer to PTP data structure */
				);

			updateClock(rtOpts, ptpClock);
		}
		else
		{
			DBG("handleFollowUp: unwanted\n");
			DBG(" waitingForFollow:     %s\n",
				ptpClock->waitingForFollow ? "TRUE" : "FALSE"
				);
			DBG(" header    Sequence:   %u\n",
				v2_header->sequenceId
				);
			DBG(" last sync Sequence:   %u\n",
				ptpClock->parent_last_sync_sequence_number
				);
		}
		break;

	default:
		DBG("handleFollowUp: disregard\n");
		return;
	}
}

void handleDelayReqTxComplete(TimeInternal *time,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	DBGV("handleDelayReqTxComplete:\n");

	/*
	 * First determine if we are sending Delay Requests or PDelay Requests
	 */

	if(rtOpts->pdelay)
	{
		/* Running pdelay requests */
		/*
		 * Get time value and add any outbound latency to store into
		 * Delay request send time.
		 */
		copyTime(&ptpClock->t1_pdelay_req_tx_time,
			time
			);

		addTime(&ptpClock->t1_pdelay_req_tx_time, // Result (x+y)
			&ptpClock->t1_pdelay_req_tx_time, // x
			&rtOpts->outboundLatency);        // y

		/*
		 * Check if PDelay response and optional follow up
		 * message was received and processed
		 * prior to getting an indication that the TX timestamp
		 * for the PDelay Request was processed.  If so,
		 * then update the delay time and clear out the
		 * message timestamps.
		 */
		if(ptpClock->t3_pdelay_resp_tx_time.seconds)// Time from responder's PDELAY RESP FOLLOWUP
		{
			updatePathDelay(&ptpClock->owd_filt,
				rtOpts,
				ptpClock
				);

			/* Delay Req/Resp handshake complete, clear flag */
			ptpClock->sentDelayReq = FALSE;
		}
	}
	else
	{

		/* Running Delay requests (not PDelay requests) */

		/*
		 * Get time value and add any outbound latency to store into
		 * Delay request send time.
		 */
		copyTime(&ptpClock->t3_delay_req_tx_time, // destination
			time                             // source
			);

		addTime(&ptpClock->t3_delay_req_tx_time,
			&ptpClock->t3_delay_req_tx_time,
			&rtOpts->outboundLatency);

		/*
		 * Check if Delay response message was received and processed
		 * prior to getting an indication that the TX timestamp
		 * for the Delay Request was processed.  If so,
		 * then update the delay time and clear out the
		 * message timestamps.
		 */

		if(ptpClock->t4_delay_req_rx_time.seconds)
		{
			updateDelay(&ptpClock->t3_delay_req_tx_time,
				&ptpClock->t4_delay_req_rx_time,
				&ptpClock->owd_filt,
				rtOpts,
				ptpClock
				);

			clearTime(&ptpClock->t3_delay_req_tx_time);
			clearTime(&ptpClock->t4_delay_req_rx_time);
			clearTime(&ptpClock->delay_resp_correction);

			/* Delay Req/Resp handshake complete, clear flag */
			ptpClock->sentDelayReq = FALSE;
		}
	}
}

void handleDelayReq(MsgHeader    *header,
	V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	DBGV("handleDelayReq: message length: %d\n", length);
	if(((ptpClock->current_msg_version == 1) && (length < DELAY_REQ_PACKET_LENGTH))
		|| length < V2_DELAY_REQ_LENGTH	)
	{
		PERROR("handleDelayReq: short delay request message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	if(!isNonZeroTime(time))
	{
		DBG("handleDelayReq: time is zero, ignoring\n");
		return;
	}

	switch(ptpClock->port_state)
	{
	case PTP_MASTER:
		if(isFromSelf)
		{
			DBGV("handleDelayReq: MASTER state, ignore from self\n");
			return;
		}

		if(ptpClock->current_msg_version == 1)
		{

			/* Version 1 Delay request is received from the outside world and we are MASTER,
			 * so we need to respond with a Delay Response as long as the communication technology
			 * checks out OK
			 */
			DBGV("handleDelayReq: MASTER state, V1 delay response received\n");
			if(!(header->sourceCommunicationTechnology == ptpClock->clock_communication_technology
				|| header->sourceCommunicationTechnology == PTP_DEFAULT
				|| ptpClock->clock_communication_technology == PTP_DEFAULT))
			{
				DBGV("handleDelayReq: Invalid communication technology\n");
				return;
			}
		}
		DBGV("handleDelayReq: MASTER state, calling issueDelayResp\n");
		issueDelayResp(time, header, v2_header, rtOpts, ptpClock);
		break;

	case PTP_SLAVE:

		if(isFromSelf)
		{
			DBG("handleDelayReq: SLAVE state, handle Transmit complete from self\n");
#ifdef CONFIG_MPC831X
			ptpClock->tx_delay_req_time_pending = FALSE;
#endif
			handleDelayReqTxComplete(time,
				rtOpts,
				ptpClock
				);
		}
		break;

	default:
		DBG("handleDelayReq: disregard\n");
		return;
	}
}

void handleDelayResp(MsgHeader   *header,
	V2MsgHeader *v2_header,
	Octet       *msgIbuf,
	ssize_t      length,
	Boolean      isFromSelf,
	RunTimeOpts *rtOpts,
	PtpClock    *ptpClock
	)
{
	MsgDelayResp   *resp;
	V2MsgDelayResp *v2resp;
	Boolean         delay_response_ok;

	DBGV("handleDelayResp: message length: %d\n", length);
	if(((ptpClock->current_msg_version == 1) && (length < DELAY_RESP_PACKET_LENGTH))
		|| length < V2_DELAY_RESP_LENGTH)
	{
		PERROR("handleDelayResp: short delay response message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	switch(ptpClock->port_state)
	{
	case PTP_SLAVE:
		if(isFromSelf || !ptpClock->sentDelayReq)
		{
			DBGV("handleDelayResp: ignore from self or delay request not sent\n");
			return;
		}

		if(ptpClock->current_msg_version == 1)
		{
			DBGV("handleDelayResp: version 1 delay response received\n");
			resp = &ptpClock->msgTmp.resp;
			msgUnpackDelayResp(ptpClock->msgIbuf, resp);
			delay_response_ok =
				(resp->requestingSourceSequenceId == ptpClock->sentDelayReqSequenceId
				&& resp->requestingSourceCommunicationTechnology == ptpClock->port_communication_technology
				&& resp->requestingSourcePortId == ptpClock->port_id_field
				&& !memcmp(resp->requestingSourceUuid, ptpClock->port_uuid_field, PTP_UUID_LENGTH)
				&& header->sourceCommunicationTechnology == ptpClock->parent_communication_technology
				&& header->sourcePortId == ptpClock->parent_port_id
				&& !memcmp(header->sourceUuid, ptpClock->parent_uuid, PTP_UUID_LENGTH)
				);
		}
		else
		{
			DBGV("handleDelayResp: version 2 delay response received\n");
			v2resp = &ptpClock->msgTmp.v2resp;
			msgUnpackV2DelayResp(ptpClock->msgIbuf, v2resp);
			delay_response_ok =
				(v2_header->sequenceId == ptpClock->sentDelayReqSequenceId
				&& v2resp->requestingPortId.portNumber == ptpClock->port_id_field
				&& !memcmp(v2resp->requestingPortId.clockIdentity,
				ptpClock->port_clock_identity,
				8
				)
				&& v2_header->sourcePortId.portNumber == ptpClock->parent_port_id
				&& !memcmp(v2_header->sourcePortId.clockIdentity,
				ptpClock->parent_clock_identity,
				8
				)
				);
		}
		if(delay_response_ok)
		{
			DBGV("handleDelayResp: delay_response_ok\n");
			if(ptpClock->current_msg_version == 1)
			{
				toInternalTime(&ptpClock->t4_delay_req_rx_time,
					&resp->delayReceiptTimestamp,
					&ptpClock->halfEpoch
					);
				clearTime(&ptpClock->delay_resp_correction);
			}
			else
			{
				v2ToInternalTime(&ptpClock->t4_delay_req_rx_time,
					&v2resp->receiveTimestamp
					);
				/* Get correction field, change to Internal time */

				v2CorrectionToInternalTime(&ptpClock->delay_resp_correction,
					v2_header->correctionField
					);

			}

			if(ptpClock->t3_delay_req_tx_time.seconds)
			{
				updateDelay(&ptpClock->t3_delay_req_tx_time,
					&ptpClock->t4_delay_req_rx_time,
					&ptpClock->owd_filt,
					rtOpts,
					ptpClock
					);

				clearTime(&ptpClock->t3_delay_req_tx_time);
				clearTime(&ptpClock->t4_delay_req_rx_time);
				clearTime(&ptpClock->delay_resp_correction);

				/* Delay Req/Resp handshake complete, clear flag */
				ptpClock->sentDelayReq = FALSE;

			}
		}
		else
		{
			DBG("handleDelayResp: unwanted\n");
		}
		break;

	default:
		DBG("handleDelayResp: disregard\n");
		return;
	}
}


void handlePDelayReq(V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	DBGV("handlePDelayReq: message length: %d\n", length);
	if(length < V2_PDELAY_REQ_LENGTH)
	{
		PERROR("handlePDelayReq: short pdelay request message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	if(!isNonZeroTime(time))
	{
		DBG("handlePDelayReq: time is zero, ignoring\n");
		return;
	}

	if(ptpClock->port_state >= PTP_LISTENING)
	{
		if(isFromSelf)
		{
			DBG("handlePDelayReq: SLAVE or MASTER state, handle from self\n");


			copyTime(&ptpClock->t1_pdelay_req_tx_time, // Destination
				time                              // Source
				);

			addTime(&ptpClock->t1_pdelay_req_tx_time,
				&ptpClock->t1_pdelay_req_tx_time,
				&rtOpts->outboundLatency);

			if(ptpClock->t4_pdelay_resp_rx_time.seconds)
			{
				updatePathDelay(&ptpClock->owd_filt,
					rtOpts,
					ptpClock
					);

				/* PDelay Request, Response and optional Response Follow up Complete, clear flag */
				ptpClock->sentDelayReq = FALSE;
			}
		}
		else
		{
			/* PDelay request is from network (not looped back to us) */
			/* Issue PDelay response and setup for PDelay response followup */
			DBGV("handlePDelayReq: Not from self, calling issuePDelayResp\n");

			/* Copy Inbound Unpacked PDelay Request header info so we can generate
			 * the PDelay Response Follow up later after confirmation of transmission
			 * of the PDelay Response.
			 */

			memcpy(&ptpClock->currentRxPDelayReqHeader,
				v2_header,
				sizeof(ptpClock->currentRxPDelayReqHeader)
				);

			issuePDelayResp(time, v2_header, rtOpts, ptpClock);
		}
	}
	else
	{
		/* State is less than LISTENING (DISABLED, INITIALIZING or FAULTY) */
		DBGV("handlePdelayReq: Not in a valid state to process message\n");
	}
}


void handlePDelayResp(V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	TimeInternal *time,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	Boolean              port_id_ok;
	Boolean              current_sequence;
	V2MsgPDelayResp     *v2presp;


	DBGV("handlePDelayResp: message length: %d\n", length);
	if(length < V2_PDELAY_RESP_LENGTH)
	{
		PERROR("handlePDelayResp: short pdelay response message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	if(!isNonZeroTime(time))
	{
		DBG("handlePDelayResp: time is zero, ignoring\n");
		return;
	}


	if(ptpClock->port_state >= PTP_LISTENING)
	{
		if(isFromSelf)
		{
			DBG("handlePDelayResp: SLAVE or MASTER state, handle from self\n");


			ptpClock->t3_pdelay_resp_tx_time.seconds = time->seconds;
			ptpClock->t3_pdelay_resp_tx_time.nanoseconds = time->nanoseconds;

			addTime(&ptpClock->t3_pdelay_resp_tx_time,
				&ptpClock->t3_pdelay_resp_tx_time,
				&rtOpts->outboundLatency);

			issuePDelayRespFollowup(&ptpClock->t3_pdelay_resp_tx_time,
				rtOpts,
				ptpClock
				);


		}
		else
		{
			/* PDelay response is from network (not looped back to us) */
			DBGV("handlePDelayResp: Not from self\n");

			v2presp = &ptpClock->msgTmp.v2presp;
			msgUnpackV2PDelayResp(ptpClock->msgIbuf, v2presp);

			port_id_ok =
				(v2presp->requestingPortId.portNumber == ptpClock->port_id_field
				&& !memcmp(v2presp->requestingPortId.clockIdentity,
				ptpClock->port_clock_identity,
				8
				)
				);

			current_sequence =
				(v2_header->sequenceId == ptpClock->sentDelayReqSequenceId);

			/* Check error conditions */

			if((!rtOpts->pdelay)     // Make sure PDelay is configured
				|| (!port_id_ok)         // Make sure requesting port field matches us
				|| ((ptpClock->receivedPDelayResp  // Check for multiple responses to current request
				&& current_sequence
				)
				)
				)
			{
				PERROR("handlePDelayResp: unrequested, invalid or multiple pdelay response message\n");
				toState(PTP_FAULTY, rtOpts, ptpClock);
				return;
			}
			if(!current_sequence)
			{
				DBG("handlePDelayResp: mismatch in sequence ID, assume old, discarding\n");
				return;
			}

			/* If we are here in the code then:
			 *  We are configured for PDelay (instead of Delay)
			 *  Port Id field is OK
			 *  The sequence ID is current (matches the one sent out in PDelay Request)
			 *  Last check, see if we have completed a PDelay Request/Response/Follow-up for this
			 *  sequence
			 */

			if(!ptpClock->sentDelayReq)
			{
				PERROR("handlePDelayResp: multiple pdelay response messages received\n");
				toState(PTP_FAULTY, rtOpts, ptpClock);
				return;
			}

			/* If we are here, then the PDelay Response is OK and expected.
			 * Set Received PDelay Response Flag (for multiple response test
			 * and continue processing
			 */

			DBGV("handlePDelayResp: version 2 pdelay response OK\n");
			ptpClock->receivedPDelayResp = TRUE;
			ptpClock->waitingForPDelayRespFollow = ((v2_header->flags[0] & V2_TWO_STEP_FLAG)
				== V2_TWO_STEP_FLAG
				);

			/* Copy Pdelay response receive time into T4 */

			ptpClock->t4_pdelay_resp_rx_time.seconds = time->seconds;
			ptpClock->t4_pdelay_resp_rx_time.nanoseconds = time->nanoseconds;

			/* Copy Pdelay request receive time (T2) from message (note: may be T3-T2 or zero
			 * Depending on options from the PDelay Responder).
			 */

			v2ToInternalTime(&ptpClock->t2_pdelay_req_rx_time,
				&v2presp->requestReceiptTimestamp
				);

			/* Get correction field, change to Internal time */

			v2CorrectionToInternalTime(&ptpClock->pdelay_resp_correction,
				v2_header->correctionField
				);

			if(!ptpClock->waitingForPDelayRespFollow)
			{
				DBGV("handlePDelayResp: One step PDelay Response received\n");

				/* This is a one step PDelay Response (no follow up expected) */
				/* This means we calculate the Path delay now assuming we
				 * have the transmit complete of the PDelay Response done
				 */

				/* Make sure time reasonable (seconds != 0) */
				if(ptpClock->t1_pdelay_req_tx_time.seconds)
				{

					updatePathDelay(&ptpClock->owd_filt,
						rtOpts,
						ptpClock
						);

					/* PDelay Req/Resp handshake complete, clear flag */
					ptpClock->sentDelayReq = FALSE;  // Note: Used for both Delay and Pdelay Request protocol
				}
			}
		}
	}
	else
	{
		/* State is less than LISTENING (DISABLED, INITIALIZING or FAULTY) */
		DBGV("handlePdelayResp: Not in a valid state to process message\n");
	}
}

void handlePDelayRespFollowUp(V2MsgHeader  *v2_header,
	Octet        *msgIbuf,
	ssize_t       length,
	Boolean       isFromSelf,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	Boolean                  port_id_ok;
	Boolean                  current_sequence;
	V2MsgPDelayRespFollowUp *v2pfollow;


	DBGV("handlePDelayRespFollowUp: message length: %d\n", length);
	if(length < V2_PDELAY_RESP_FOLLOWUP_LENGTH)
	{
		PERROR("handlePDelayRespFollowUp: short pdelay response follow up message\n");
		toState(PTP_FAULTY, rtOpts, ptpClock);
		return;
	}

	if(ptpClock->port_state >= PTP_LISTENING)
	{
		if(isFromSelf)
		{
			DBGV("handlePDelayRespFollowup: state >= LISTENING, ignore from self\n");
			return;
		}
		else
		{
			/* PDelay response is from network (not looped back to us) */
			DBGV("handlePDelayRespFollowUp: Not from self\n");

			v2pfollow = &ptpClock->msgTmp.v2pfollow;
			msgUnpackV2PDelayRespFollowUp(ptpClock->msgIbuf, v2pfollow);

			port_id_ok =
				(v2pfollow->requestingPortId.portNumber == ptpClock->port_id_field
				&& !memcmp(v2pfollow->requestingPortId.clockIdentity,
				ptpClock->port_clock_identity,
				8
				)
				);

			current_sequence =
				(v2_header->sequenceId == ptpClock->sentDelayReqSequenceId);

			/* Check error conditions */

			if((!rtOpts->pdelay)     // Make sure PDelay is configured
				|| (!port_id_ok)         // Make sure requesting port field matches us
				)
			{
				PERROR("handlePDelayRespFollowUp: unrequested or invalid message\n");
				toState(PTP_FAULTY, rtOpts, ptpClock);
				return;
			}
			if(!current_sequence)
			{
				DBG("handlePDelayRespFollowUp: mismatch in sequence ID, assume old\n");
				return;
			}

			/* If we are here in the code then:
			 *  We are configured for PDelay (instead of Delay)
			 *  Port Id field is OK
			 *  The sequence ID is current (matches the one sent out in PDelay Request)
			 *  Last check, see if we have completed a PDelay Request/Response/Follow-up for this
			 *  sequence
			 */

			if(!ptpClock->sentDelayReq)
			{
				PERROR("handlePDelayRespFollowUp: multiple or unexpected rx follow up\n");
				toState(PTP_FAULTY, rtOpts, ptpClock);
				return;
			}

			/* If we are here, then the PDelay Response Follow Up is OK and expected.
			 * continue processing
			 */

			DBGV("handlePDelayRespFollow: version 2 follow up OK\n");

			/* Copy Pdelay request receive time (T2) from message (note: may be T3-T2 or zero
			 * Depending on options from the PDelay Responder).
			 */

			v2ToInternalTime(&ptpClock->t3_pdelay_resp_tx_time,
				&v2pfollow->responseOriginTimestamp
				);

			/* Get correction field, change to Internal time */

			v2CorrectionToInternalTime(&ptpClock->pdelay_followup_correction,
				v2_header->correctionField
				);

			if(ptpClock->waitingForPDelayRespFollow)
			{
				DBGV("handlePDelayRespFollow: follow up expected\n");

				/* Make sure time reasonable (seconds != 0) */
				if(ptpClock->t1_pdelay_req_tx_time.seconds)
				{

					updatePathDelay(&ptpClock->owd_filt,
						rtOpts,
						ptpClock
						);

					/* PDelay Req/Resp handshake complete, clear flag */
				}
				ptpClock->sentDelayReq = FALSE;  // Note: Used for both Delay and Pdelay Request protocol
			}
			else
			{
				DBG("handlePDelayRespFollow: Received but not waiting for one\n");
				/* AKB: TBD on if this is an ignore or FAULTY condition, for now
				 * Ignoring message
				 */
			}
		}
	}
	else
	{
		/* State is less than LISTENING (DISABLED, INITIALIZING or FAULTY) */
		DBG("handlePdelayResp: Not in a valid state to process message\n");
	}
}


void handleManagement(MsgHeader *   header,
	Octet *       msgIbuf,
	ssize_t       length,
	Boolean       isFromSelf,
	RunTimeOpts * rtOpts,
	PtpClock *    ptpClock
	)
{
	MsgManagement *manage;

	UInteger8 state;

	if(ptpClock->port_state == PTP_INITIALIZING)
		return;

	manage = &ptpClock->msgTmp.manage;
	msgUnpackManagement(ptpClock->msgIbuf, manage);

	if((manage->targetCommunicationTechnology == ptpClock->clock_communication_technology
		&& !memcmp(manage->targetUuid, ptpClock->clock_uuid_field, PTP_UUID_LENGTH))
		|| ((manage->targetCommunicationTechnology == PTP_DEFAULT
		|| manage->targetCommunicationTechnology == ptpClock->clock_communication_technology)
		&& !sum(manage->targetUuid, PTP_UUID_LENGTH)))
	{
		switch(manage->managementMessageKey)
		{
		case PTP_MM_OBTAIN_IDENTITY:
		case PTP_MM_GET_DEFAULT_DATA_SET:
		case PTP_MM_GET_CURRENT_DATA_SET:
		case PTP_MM_GET_PARENT_DATA_SET:
		case PTP_MM_GET_PORT_DATA_SET:
		case PTP_MM_GET_GLOBAL_TIME_DATA_SET:
		case PTP_MM_GET_FOREIGN_DATA_SET:
			issueManagement(header, manage, rtOpts, ptpClock);
			break;

		default:
			ptpClock->record_update = TRUE;
			state = msgUnloadManagement(ptpClock->msgIbuf, manage, ptpClock, rtOpts);
			if(state != ptpClock->port_state)
				toState(state, rtOpts, ptpClock);
			break;
		}
	}
	else
	{
		DBG("handleManagement: unwanted\n");
	}
}

/* Functions to pack and send various messages */
void issueAnnounce(RunTimeOpts * rtOpts,
	PtpClock *    ptpClock
	)
{
	TimeInternal         internalTime;
	V2TimeRepresentation v2OriginTimestamp;
	int ret;
	int packet_length;  /* AKB: added for v2 support */

	/* Only issue an announce if we are running PTP version 2 */
	if(!rtOpts->ptpv2)
	{
		return;
	}

	++ptpClock->last_announce_tx_sequence_number;
	ptpClock->grandmaster_sequence_number = ptpClock->last_announce_tx_sequence_number;

	getTime(&internalTime, ptpClock->current_utc_offset); // Get current time

	DBGV("issueAnnounce: prepping V2 ANNOUNCE message\n");
	v2FromInternalTime(&internalTime,
		&v2OriginTimestamp,
		ptpClock->halfEpoch,
		0                     // TEMP FOR DEBUG: fixed epoch number of 0
		);

	msgPackAnnounce(ptpClock->msgObuf,    // buffer
		FALSE,                // unicast flag (TEMP FOR DEBUG, always set)
		&v2OriginTimestamp,
		ptpClock
		);
	packet_length = V2_ANNOUNCE_LENGTH;




	ret = netSendGeneral(ptpClock->msgObuf,
		packet_length,
		&ptpClock->netPath,
		FALSE
		);
	

	if(!ret)
	{
		DBG("issueAnnounce: unable to sent announce message, return = %d\n", ret);
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}
	else
	{
		DBGV("issueAnnounce: sent announce message\n");
	}
}

void issueSync(RunTimeOpts * rtOpts,
	PtpClock *    ptpClock
	)
{
	TimeInternal         internalTime;
	TimeRepresentation   originTimestamp;
	V2TimeRepresentation v2OriginTimestamp;
	int ret;
	int packet_length;  /* AKB: added for v2 support */

	++ptpClock->last_sync_tx_sequence_number;
	ptpClock->grandmaster_sequence_number = ptpClock->last_sync_tx_sequence_number;
	ptpClock->sentSync = TRUE;

	getTime(&internalTime, ptpClock->current_utc_offset);               // Get current time

	if(rtOpts->ptpv2)
	{
		DBGV("issueSync: prepping V2 SYNC message\n");
		v2FromInternalTime(&internalTime,
			&v2OriginTimestamp,
			ptpClock->halfEpoch,
			0                     // TEMP FOR DEBUG: fixed epoch number of 0
			);


		msgPackV2Sync(ptpClock->msgObuf,    // buffer
			FALSE,                // unicast flag (TEMP FOR DEBUG, always set)
			&v2OriginTimestamp,
			ptpClock
			);
		packet_length = V2_SYNC_LENGTH;
	}
	else
	{
		DBGV("issueSync: prepping V1 SYNC message\n");
		fromInternalTime(&internalTime, &originTimestamp, ptpClock->halfEpoch);
		msgPackSync(ptpClock->msgObuf, FALSE, &originTimestamp, ptpClock);
		packet_length = SYNC_PACKET_LENGTH;
	}



	DBGV("issueSync: netSendEvent, sequence: %d\n",
		ptpClock->last_sync_tx_sequence_number
		);
	ret = netSendEvent(ptpClock->msgObuf,
		packet_length,
		&ptpClock->netPath,
		FALSE
		);
	

	if(!ret)
	{
		DBG("issueSync: Network send error: %d\n",
			ret
			);
		ptpClock->sentSync = FALSE;
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}
	else
	{
		DBGV("issueSync: sent sync message\n");
	}
}

void issueFollowup(TimeInternal *time,    // Transmit Time from last sent Sync message
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	int ret;
	int packet_length;

	TimeRepresentation   preciseOriginTimestamp;
	V2TimeRepresentation v2PreciseOriginTimestamp;

	ptpClock->sentSync = FALSE;

	if(rtOpts->ptpv2)
	{
		DBGV("issueFollowUp: prepping V2 FOLLOW UP message\n");
		v2FromInternalTime(time,
			&v2PreciseOriginTimestamp,
			ptpClock->halfEpoch,
			0                     // TEMP FOR DEBUG: fixed epoch number of 0
			);


		msgPackV2FollowUp(ptpClock->msgObuf,    // buffer
			FALSE,                // unicast flag (TEMP FOR DEBUG, always set)
			ptpClock->last_sync_tx_sequence_number,
			&v2PreciseOriginTimestamp,
			ptpClock
			);
		packet_length = V2_FOLLOWUP_LENGTH;
	}
	else
	{
		DBGV("issueFollowUp: prepping V1 FOLLOW UP message\n");
		++ptpClock->last_general_event_sequence_number;
		fromInternalTime(time, &preciseOriginTimestamp, ptpClock->halfEpoch);

		msgPackFollowUp(ptpClock->msgObuf,
			ptpClock->last_sync_tx_sequence_number,
			&preciseOriginTimestamp,
			ptpClock
			);
		packet_length = FOLLOW_UP_PACKET_LENGTH;
	}



	ret = netSendGeneral(ptpClock->msgObuf,
		packet_length,
		&ptpClock->netPath,
		FALSE
		);
	
	if(!ret)
	{
		DBG("issueFollowup: Network send error: %d\n",
			ret
			);
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}
	else
	{
		DBGV("issueFollowup: sent followup message\n");
	}
}

void issueDelayReq(RunTimeOpts *rtOpts,
	PtpClock    *ptpClock
	)
{
	TimeInternal         internalTime;
	TimeRepresentation   originTimestamp;
	V2TimeRepresentation v2OriginTimestamp;
	int                  ret;
	UInteger16           length;
	Boolean              pdelay;

	ptpClock->sentDelayReq = TRUE;
	ptpClock->receivedPDelayResp = FALSE;

	getTime(&internalTime, ptpClock->current_utc_offset);
	pdelay = rtOpts->pdelay;

	if(rtOpts->ptpv2)
	{
		DBGV("issueDelayReq: building and sending V2 message\n");
		v2FromInternalTime(&internalTime,
			&v2OriginTimestamp,
			ptpClock->halfEpoch,
			ptpClock->epoch_number
			);
		if(pdelay)
		{
			/* PDelay request instead of Delay request */
			/* Delay request */
			DBGV("issueDelayReq: building sending Pdelay Request message\n");
			ptpClock->sentDelayReqSequenceId = ++ptpClock->last_delay_req_tx_sequence_number;
			msgPackV2PDelayReq(ptpClock->msgObuf,   // buf, 
				FALSE,               // unicast,
				&v2OriginTimestamp,  // originTimestamp,
				ptpClock             // ptpClock
				);
			length = V2_PDELAY_REQ_LENGTH;
		}
		else
		{
			/* Delay request */
			DBGV("issueDelayReq: building sending Delay Request message\n");
			ptpClock->sentDelayReqSequenceId = ++ptpClock->last_delay_req_tx_sequence_number;
			msgPackV2DelayReq(ptpClock->msgObuf,   // buf, 
				TRUE,               // unicast,
				&v2OriginTimestamp,  // originTimestamp,
				ptpClock             // ptpClock
				);
			length = V2_DELAY_REQ_LENGTH;
		}
	}
	else
	{
		DBGV("issueDelayReq: building and sending V1 message\n");
		fromInternalTime(&internalTime, &originTimestamp, ptpClock->halfEpoch);
		ptpClock->sentDelayReqSequenceId = ++ptpClock->last_delay_req_tx_sequence_number;
		msgPackDelayReq(ptpClock->msgObuf, FALSE, &originTimestamp, ptpClock);
		length = DELAY_REQ_PACKET_LENGTH;
	}


	DBGV("issueDelayReq: calling netSendEvent\n");
	ret = netSendEvent(ptpClock->msgObuf,
		length,
		&ptpClock->netPath,
		pdelay
		);


	if(!ret)
	{
		DBG("issueDelayReq: error sending message, return code: %d\n", ret);
		ptpClock->sentDelayReq = FALSE;
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}
	else
	{
		DBGV("issueDelayReq: sent delay request message\n");
	}
}

void issueDelayResp(TimeInternal *time,
	MsgHeader    *header,
	V2MsgHeader  *v2header,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	int ret;

	TimeRepresentation   delayReceiptTimestamp;
	V2TimeRepresentation v2DelayReceiptTimestamp;
	UInteger16           length;

	++ptpClock->last_general_event_sequence_number;

	/* Check version of inbound message and respond back in the same version format */

	if(ptpClock->current_msg_version == 1)
	{

		fromInternalTime(time, &delayReceiptTimestamp, ptpClock->halfEpoch);
		msgPackDelayResp(ptpClock->msgObuf, header, &delayReceiptTimestamp, ptpClock);
		length = DELAY_RESP_PACKET_LENGTH;
	}
	else
	{
		v2FromInternalTime(time,
			&v2DelayReceiptTimestamp,
			ptpClock->halfEpoch,
			ptpClock->epoch_number
			);
		msgPackV2DelayResp(ptpClock->msgObuf,       // buf, 
			FALSE,                   // unicast,
			v2header,                // header,
			&v2DelayReceiptTimestamp, // delayReceiptTimestamp,
			ptpClock                 // ptpClock
			);
		length = V2_DELAY_RESP_LENGTH;
	}

	
	ret = netSendGeneral(ptpClock->msgObuf,
		length,
		&ptpClock->netPath,
		FALSE
		);
	
	if(!ret)
	{
		DBG("issueDelayResp: Network send error: %d\n",
			ret
			);
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}

	else
	{
		DBGV("issueDelayResp: sent delay response message\n");
	}
}

void issuePDelayResp(TimeInternal *time,
	V2MsgHeader  *v2header,
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	int                  ret;
	V2TimeRepresentation v2PDelayReceiptTimestamp;
	UInteger16           length;

	ptpClock->sentPDelayResp = TRUE;

	ptpClock->last_pdelay_resp_tx_sequence_number = v2header->sequenceId;

	v2FromInternalTime(time,
		&v2PDelayReceiptTimestamp,
		ptpClock->halfEpoch,
		ptpClock->epoch_number
		);
	msgPackV2PDelayResp(ptpClock->msgObuf,        // buf, 
		FALSE,                    // unicast,
		v2header,                 // header,
		&v2PDelayReceiptTimestamp, // delayReceiptTimestamp,
		ptpClock                  // ptpClock
		);
	length = V2_PDELAY_RESP_LENGTH;


	ret = netSendEvent(ptpClock->msgObuf,
		length,
		&ptpClock->netPath,
		TRUE
		);

	if(!ret)
	{
		DBG("issuePDelayResp: Network send error: %d\n",
			ret
			);
		ptpClock->sentPDelayResp = FALSE;
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}

	else
	{
		DBGV("issuePDelayResp: sent delay response message\n");
	}
}

void issuePDelayRespFollowup(TimeInternal *time,    // Transmit Time from PDelay Response message
	RunTimeOpts  *rtOpts,
	PtpClock     *ptpClock
	)
{
	int                  ret;
	int                  packet_length;
	V2TimeRepresentation v2ResponseOriginTimestamp;

	ptpClock->sentPDelayResp = FALSE;


	DBGV("issuePDelayRespFollowUp: prepping V2 PDELAY RESPONSE FOLLOW UP message\n");
	v2FromInternalTime(time,
		&v2ResponseOriginTimestamp,
		ptpClock->halfEpoch,
		0                     // TEMP FOR DEBUG: fixed epoch number of 0
		);

	msgPackV2PDelayRespFollowUp(ptpClock->msgObuf,                  // buf 
		FALSE,                              // unicast
		&ptpClock->currentRxPDelayReqHeader, // PDELAY REQ header,
		&v2ResponseOriginTimestamp,          // timestamp
		ptpClock                            // ptpClock
		);

	packet_length = V2_PDELAY_RESP_FOLLOWUP_LENGTH;


	ret = netSendGeneral(ptpClock->msgObuf,
		packet_length,
		&ptpClock->netPath,
		TRUE
		);
	
	if(!ret)
	{
		DBG("issuePDelayRespFollowup: Network send error: %d\n",
			ret
			);
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}
	else
	{
		DBGV("issueFollowup: sent followup message\n");
	}
}


void issueManagement(MsgHeader     *header,
	MsgManagement *manage,
	RunTimeOpts   *rtOpts,
	PtpClock      *ptpClock
	)
{
	UInteger16 length;
	int        ret;

	++ptpClock->last_general_event_sequence_number;

	if(!(length = msgPackManagementResponse(ptpClock->msgObuf, header, manage, ptpClock)))
	{
		return;
	}



	ret = netSendGeneral(ptpClock->msgObuf,
		length,
		&ptpClock->netPath,
		FALSE
		);
	
	if(!ret)
	{
		toState(PTP_FAULTY, rtOpts, ptpClock);
	}
	else
	{
		DBGV("issueManagement: sent management message\n");
	}
}

/* IEEE V1 add or update an entry in the foreign master data set,
 * return pointer to unpacked SYNC message
 */

MsgAnnounce * addV2Foreign(Octet       *buf,        // Buffer to raw data 
	V2MsgHeader *header,     // Unpacked PTP header
	PtpClock    *ptpClock    // Pointer to main PTP data structure
	)
{
	int i, j;
	Boolean found = FALSE;

	DBGV("addV2Foreign: add or update record\n");

	j = ptpClock->foreign_record_best;  /* Get index of current best master */

	/* Scan foreign master database */

	for(i = 0; i < ptpClock->number_foreign_records; ++i)
	{
		/* Check Clock Identity and Port ID to see if there
		 * is a need to add a new foreign record.
		 */
		if(header->sourcePortId.portNumber                      // Check Source ID equal
			== ptpClock->foreign[j].foreign_master_port_id
			&& !memcmp(header->sourcePortId.clockIdentity,           // Check Clock ID equal
			ptpClock->foreign[j].foreign_master_clock_identity,
			8
			)
			)
		{
			/* Match found, bump the number of announces messages received from this foreign
			 * master
			 */
			++ptpClock->foreign[j].foreign_master_announces;
			found = TRUE;
			DBGV("addV2Foreign: updated record %d\n", j);
			break;
		}

		/* Match not found, keep looking */
		j = (j + 1) % ptpClock->number_foreign_records;
	}

	/* Check if record found, if not add it if there is room */

	if(!found)
	{
		/* Have not seen this foreign master */
		/* Check if number of foreign records is less than max (still room) */
		if(ptpClock->number_foreign_records < ptpClock->max_foreign_records)
		{
			/* Still less than max, bump number of total records */
			++ptpClock->number_foreign_records;
		}

		/* Set current index based on loop above (found or not) */

		j = ptpClock->foreign_record_i;

		/* Store communication technology, Port ID and Clock Identity */


		ptpClock->foreign[j].foreign_master_port_id =
			header->sourcePortId.portNumber;
		memcpy(ptpClock->foreign[j].foreign_master_clock_identity,
			header->sourcePortId.clockIdentity,
			8
			);

		DBG("addV2Foreign: new foreign record: %d, number of records: %d\n",
			ptpClock->foreign_record_i,
			ptpClock->number_foreign_records
			);
		DBG("  Port ID....................... %d\n",
			ptpClock->foreign[j].foreign_master_port_id
			);
		DBG("  Foreign Master Clock Identity. %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
			ptpClock->foreign[j].foreign_master_clock_identity[0],
			ptpClock->foreign[j].foreign_master_clock_identity[1],
			ptpClock->foreign[j].foreign_master_clock_identity[2],
			ptpClock->foreign[j].foreign_master_clock_identity[3],
			ptpClock->foreign[j].foreign_master_clock_identity[4],
			ptpClock->foreign[j].foreign_master_clock_identity[5],
			ptpClock->foreign[j].foreign_master_clock_identity[6],
			ptpClock->foreign[j].foreign_master_clock_identity[7]
			);

		ptpClock->foreign_record_i = (ptpClock->foreign_record_i + 1)
			% ptpClock->max_foreign_records;
	}

	msgUnpackV2Header(buf, &ptpClock->foreign[j].v2_header);/* Store PTP Header in foreign record */
	msgUnpackAnnounce(buf, &ptpClock->foreign[j].announce); /* Store Announce data in record  */

	/* AKB: BMC, m1, s1, etc. still only handles V1 messages stored in the
	 * foreign master table.  Convert Announce message PTP header and data to V1 and store
	 */

	convert_v2_header_to_v1(&ptpClock->foreign[j].v2_header,
		&ptpClock->foreign[j].header
		);

	convert_v2_announce_to_v1_sync(&ptpClock->foreign[j].v2_header,
		&ptpClock->foreign[j].announce,
		&ptpClock->foreign[j].sync
		);

	return &ptpClock->foreign[j].announce;  /* Return pointer to unpacked Announce message */
}

/* IEEE V1 add or update an entry in the foreign master data set,
 * return pointer to unpacked SYNC message
 */

MsgSync * addForeign(Octet *buf, MsgHeader *header, PtpClock *ptpClock)
{
	int i, j;
	Boolean found = FALSE;

	DBGV("addForeign: add or update record\n");

	j = ptpClock->foreign_record_best;  /* Get index of current best master */

	for(i = 0; i < ptpClock->number_foreign_records; ++i)
	{
		/* Check source communication technology, UUID and Port ID to see if there
		 * is a match or not
		 */
		if(header->sourceCommunicationTechnology
			== ptpClock->foreign[j].foreign_master_communication_technology
			&& header->sourcePortId
			== ptpClock->foreign[j].foreign_master_port_id
			&& !memcmp(header->sourceUuid,
			ptpClock->foreign[j].foreign_master_uuid,
			PTP_UUID_LENGTH
			)
			)
		{
			/* Match found, bump the number of sync messages received */
			++ptpClock->foreign[j].foreign_master_syncs;
			found = TRUE;
			DBGV("addForeign: updated record %d\n", j);
			break;
		}

		/* Match not found, keep looking */
		j = (j + 1) % ptpClock->number_foreign_records;
	}

	/* Check if record found, if not add it if there is room */

	if(!found)
	{
		/* Have not seen this foreign master */
		/* Check if number of foreign records is less than max (still room) */
		if(ptpClock->number_foreign_records < ptpClock->max_foreign_records)
		{
			/* Still less than max, bump number of total records */
			++ptpClock->number_foreign_records;
		}

		/* Set current index based on loop above (found or not) */

		j = ptpClock->foreign_record_i;

		/* Store communication technology, Port ID and UUID */

		ptpClock->foreign[j].foreign_master_communication_technology =
			header->sourceCommunicationTechnology;
		ptpClock->foreign[j].foreign_master_port_id =
			header->sourcePortId;
		memcpy(ptpClock->foreign[j].foreign_master_uuid,
			header->sourceUuid,
			PTP_UUID_LENGTH
			);

		DBG("addForeign: new foreign record: %d, number of records: %d\n",
			ptpClock->foreign_record_i,
			ptpClock->number_foreign_records
			);
		DBG("  Master communication technology. %d\n",
			ptpClock->foreign[j].foreign_master_communication_technology
			);
		DBG("  Port ID......................... %d\n",
			ptpClock->foreign[j].foreign_master_port_id
			);
		DBG("  Foreign Master UUID.............. %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
			ptpClock->foreign[j].foreign_master_uuid[0],
			ptpClock->foreign[j].foreign_master_uuid[1],
			ptpClock->foreign[j].foreign_master_uuid[2],
			ptpClock->foreign[j].foreign_master_uuid[3],
			ptpClock->foreign[j].foreign_master_uuid[4],
			ptpClock->foreign[j].foreign_master_uuid[5]
			);

		ptpClock->foreign_record_i = (ptpClock->foreign_record_i + 1)
			% ptpClock->max_foreign_records;
	}

	msgUnpackHeader(buf, &ptpClock->foreign[j].header);  /* Store PTP Header in foreign record */
	msgUnpackSync(buf, &ptpClock->foreign[j].sync);    /* Store Sync message data in record  */

	return &ptpClock->foreign[j].sync;  /* Return pointer to unpacked Sync message */
}

// eof protocol.c

