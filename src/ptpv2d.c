/* src/ptpv2d.c */
/* Main entry point for ptpv2d PTP daeamon */
/* This file is a derivative work from ptpd.c
 * Copyright (c) 2005-2007 Kendall Correll
 */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (ptpv2d.c) contains Modifications (updates, corrections        */
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

#define _CRT_SECURE_NO_WARNINGS

#include "ptpd.h"

RunTimeOpts rtOpts;  /* statically allocated run-time configuration data */
#ifdef PTPD_DBG
int debugLevel; /* Global variable for enabling various debug printf statments */
#endif

#include <windows.h>

TCHAR* serviceName = TEXT("PTPv2d Service");
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;

#define SERVICE_DESCR_STRING	"PTPv2d Service"

static unsigned gRealArgc = 0;
static char *gRealArgv[16] = { NULL };

int original_code_start(int argc, char *argv[])
{
	PtpClock *ptpClock;
	Integer16 ret;
	
	//Sleep(20000);
	

	/* initialize run-time options to reasonable values */
	memset(&rtOpts, 0, sizeof(rtOpts));

	rtOpts.syncInterval = DEFAULT_SYNC_INTERVAL;
	rtOpts.announceInterval = DEFAULT_ANNOUNCE_INTERVAL;  // AKB: Added for V2
	memcpy(rtOpts.subdomainName,
		DEFAULT_PTP_DOMAIN_NAME,
		PTP_SUBDOMAIN_NAME_LENGTH
		);
	memcpy(rtOpts.clockIdentifier,
		IDENTIFIER_DFLT,
		PTP_CODE_STRING_LENGTH
		);
	rtOpts.clockVariance = DEFAULT_V1_CLOCK_VARIANCE;
	rtOpts.clockStratum = DEFAULT_CLOCK_STRATUM;
	rtOpts.unicastAddress[0] = 0;
	rtOpts.inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
	rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
	rtOpts.noResetClock = DEFAULT_NO_RESET_CLOCK;
	rtOpts.s = DEFAULT_DELAY_S;
	rtOpts.ap = DEFAULT_AP;
	rtOpts.ai = DEFAULT_AI;
	rtOpts.max_foreign_records = DEFAULT_MAX_FOREIGN_RECORDS;
	rtOpts.currentUtcOffset = DEFAULT_UTC_OFFSET;

#ifdef PTPD_DBG
	debugLevel = 0; /* Set all debug printing off unless requested by user */
#endif


	if(!(ptpClock = ptpdStartup(argc, argv, &ret, &rtOpts)))
	{
		// ptpdStartup did not return a pointer to the ptpClock structure,
		// something went wrong
		// Return code set by ptpdStartup function, pass
		// back to operating system.
		return ret;
	}

	// Check if Probe option selected (send one management message and then exit)
	// if not, then go ahead and start the PTP main code (protocol)

	if(rtOpts.probe)
	{
		probe(&rtOpts, ptpClock);
		ptpdShutdown();
		return 1;
	}
	else
	{
		if(rtOpts.ifaceName[0] == '\0')
		{
			/* do the multiple port protocol engine, if not bound to a single port */
			multiPortProtocol(&rtOpts, ptpClock);
			NOTIFY("main: self shutdown, probably due to an error\n");
		}
		else
		{
			/* do the single port protocol engine */
			protocol(&rtOpts, ptpClock);
			NOTIFY("main: self shutdown, probably due to an error\n");
		}
	}

	ptpdShutdown();

	return 0;
}



int controlLoop(int argc, char *argv[])
{
	while(TRUE) //main program loop
	{
		// Do work

		if(original_code_start(argc, argv) != 0)
		{
			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			break;
		}


		if(WaitForSingleObject(stopServiceEvent, 0) == WAIT_TIMEOUT)
		{
			; // Do nothing
		}
		else //shutdown!
		{
			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			break;
		}
	}

	return 1;
}


void WINAPI ServiceControlHandler(DWORD controlCode)
{
	switch(controlCode)
	{
	case SERVICE_CONTROL_INTERROGATE:
		break;

	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		SetEvent(stopServiceEvent);
		return;

	case SERVICE_CONTROL_PAUSE:
		break;

	case SERVICE_CONTROL_CONTINUE:
		break;

	default:
		if(controlCode >= 128 && controlCode <= 255)
		{
			// user defined control code
			break;
		}
		else
		{
			// unrecognised control code
			break;
		}
	}
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void WINAPI ServiceMain(DWORD argc, TCHAR* argv[])
{
	// initialise service status
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 15000;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceControlHandler);

	if(serviceStatusHandle)
	{

		/*
		Read service args */
		/*
		for (unsigned int argidx = 0; (argidx + 1) < argc; ++argidx)
		{
		if (string(argv[argidx]) == "-c") //config file
		{
		g_configfile = argv[argidx + 1];
		}
		else if (string(argv[argidx]) == "-") //log config file
		{
		g_logconfigfile = argv[argidx + 1];
		}
		}
		*/

		// service is starting
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		// do initialisation here
		stopServiceEvent = CreateEvent(0, FALSE, FALSE, 0);

		// running
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		controlLoop(gRealArgc, gRealArgv);

		// do cleanup here
		CloseHandle(stopServiceEvent);
		stopServiceEvent = 0;

		// service is now stopped
		serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
	}
}

void RunService()
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ serviceName, ServiceMain },
		{ 0, 0 }
	};

	StartServiceCtrlDispatcher(serviceTable);
}

void InstallService(DWORD argc, TCHAR* argv[])
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);

	if(serviceControlManager)
	{
		TCHAR path[_MAX_PATH + 1];
		TCHAR newPath[_MAX_PATH + 1];
		if(GetModuleFileName(0, path, sizeof(path) / sizeof(path[0])) > 0)
		{
			//have to wrap service name and params in quotes,
			//or it doesn't work with long file names
			strcpy(newPath, "\"");
			strcat(newPath, path);
			strcat(newPath, "\"");

			for(unsigned i = 2; i < argc; i++)
			{
				strcat(newPath, " ");
				strcat(newPath, argv[i]);
			}

			/*
			Optional args
			fullpath += " -l \"";
			fullpath += g_logconfigfile.c_str();
			fullpath += "\" -c \"";
			fullpath += g_configfile.c_str();
			fullpath += "\"";
			*/

			//std::cout << "Installing service" << fullpath << std::endl;
			printf("Installing service %s\n", newPath);

			SC_HANDLE service = CreateService(serviceControlManager,
				serviceName, serviceName,
				SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, newPath,
				0, 0, 0, 0, 0);
			if(service)
			{
				SC_ACTION act[3]; //action to perform when service fails
				act[0].Delay = 60000; //60 seconds
				act[0].Type = SC_ACTION_RESTART;
				act[1].Delay = 60000; //60 seconds
				act[1].Type = SC_ACTION_RESTART;
				//avoid infinite re-start/fail loop
				act[2].Delay = 60000; //60 seconds
				act[2].Type = SC_ACTION_NONE;

				SERVICE_FAILURE_ACTIONS svfailact;
				svfailact.cActions = 3; //number of elements in action array
				svfailact.lpsaActions = (SC_ACTION*)(&act);
				svfailact.lpRebootMsg = NULL;
				svfailact.lpCommand = NULL;
				svfailact.dwResetPeriod = 86400; //fail counter reset time (= 1 day)

				BOOL result = ChangeServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, &svfailact);
				if(result)
				{
					//std::cout << "Service installed";
					printf("Service installed\n");
				}
				else
				{
					//std::cout << "Error encountered installing service";
					printf("Error encountered installing service\n");
				}

				SERVICE_DESCRIPTION sdBuf;
				// Set the description visible in the Service Manager, which is an optional config
				sdBuf.lpDescription = SERVICE_DESCR_STRING;
				if(ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &sdBuf))
				{
					//std::cout << "Service description updated OK\n";
					printf("Service description updated OK\n");
				}
				else
				{
					//std::cout << "ERROR: could not update service description\n";
					printf("ERROR: coult not update service description\n");
				}
				CloseServiceHandle(service);
			}
			else
			{
				//std::cout << "Service installed failed: " << GetLastError();
				printf("Service installed failed: %d\n", GetLastError());
			}
		}
		CloseServiceHandle(serviceControlManager);
	}
}

void UninstallService()
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

	if(serviceControlManager)
	{
		SC_HANDLE service = OpenService(serviceControlManager,
			serviceName, SERVICE_QUERY_STATUS | DELETE);
		if(service)
		{
			SERVICE_STATUS serviceStatus;
			if(QueryServiceStatus(service, &serviceStatus))
			{
				if(serviceStatus.dwCurrentState == SERVICE_STOPPED)
				{
					DeleteService(service);
					//std::cout << "Service uninstalled";
					printf("Service uninstalled\n");
				}
				//else std::cout << "Error uninstalling service, service not stopped";
				printf("Error uninstalling service, service not stopped\n");
			}
			CloseServiceHandle(service);
		}
		CloseServiceHandle(serviceControlManager);
	}
}




int main(int argc, TCHAR* argv[])
{
	int argidx = 1;
	BOOLEAN runasservice = TRUE;
	if(argc > 1 && lstrcmpi(argv[argidx], TEXT("install")) == 0)
	{
		++argidx;
		/*
		for (; (argidx + 1) < argc; ++argidx)
		{
		if (std::string(argv[argidx]) == "-c") //config file
		{
		g_configfile = argv[argidx + 1];
		}
		else if (string(argv[argidx]) == "-") //log config file
		{
		g_logconfigfile = argv[argidx + 1];
		}
		}*/
		InstallService(argc, argv);
		return 0;
	}
	else if(argc > 1 && lstrcmpi(argv[argidx], TEXT("uninstall")) == 0)
	{
		UninstallService();
		return 0;
	}
	else if(argc > 1 && lstrcmpi(argv[argidx], TEXT("debug")) == 0) //don't run as a service
	{
		runasservice = FALSE;
		++argidx;
	}

	if(!runasservice)
	{
		controlLoop(argc, argv);
	}
	else
	{
		//Sleep(10000);


		// Copy arguments into gloabsl
		for(unsigned i = 0; i < argc; i++)
		{
			gRealArgv[i] = malloc(256);
			strncpy(gRealArgv[i], argv[i], 255);
		}

		gRealArgc = argc;


		RunService();
	}

	return 0;
}

#if 0
int main(int argc, char **argv)
{
	PtpClock *ptpClock;
	Integer16 ret;

	/* initialize run-time options to reasonable values */ 
	memset(&rtOpts, 0, sizeof(rtOpts));

	rtOpts.syncInterval                = DEFAULT_SYNC_INTERVAL;
	rtOpts.announceInterval            = DEFAULT_ANNOUNCE_INTERVAL;  // AKB: Added for V2
	memcpy(rtOpts.subdomainName,
		DEFAULT_PTP_DOMAIN_NAME,
		PTP_SUBDOMAIN_NAME_LENGTH
		);
	memcpy(rtOpts.clockIdentifier,
		IDENTIFIER_DFLT,
		PTP_CODE_STRING_LENGTH
		);
	rtOpts.clockVariance               = DEFAULT_V1_CLOCK_VARIANCE;
	rtOpts.clockStratum                = DEFAULT_CLOCK_STRATUM;
	rtOpts.unicastAddress[0]           = 0;
	rtOpts.inboundLatency.nanoseconds  = DEFAULT_INBOUND_LATENCY;
	rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
	rtOpts.noResetClock                = DEFAULT_NO_RESET_CLOCK;
	rtOpts.s                           = DEFAULT_DELAY_S;
	rtOpts.ap                          = DEFAULT_AP;
	rtOpts.ai                          = DEFAULT_AI;
	rtOpts.max_foreign_records         = DEFAULT_MAX_FOREIGN_RECORDS;
	rtOpts.currentUtcOffset            = DEFAULT_UTC_OFFSET;
	rtOpts.ptp8021AS                   = FALSE;  // AKB: Added for 802.1AS (PTP over Ethernet)

#ifdef PTPD_DBG
	debugLevel = 0; /* Set all debug printing off unless requested by user */
#endif

#ifdef CONFIG_MPC831X
	//
	// For running on MPC831X (Freescale system with hardware timestamping):
	// Multiport protocol not fully suported yet, so force default to eth1 interface 
	// which is the best port to use on the MPC8313E-RDB board
	// NOTE: this interface can be overriden by the user at startup
	// time using the -b option when starting ptpv2d
	//
	memset( rtOpts.ifaceName, 0,      IFACE_NAME_LENGTH);
	strncpy((char * )rtOpts.ifaceName, "eth1", IFACE_NAME_LENGTH);
#endif

	if( !(ptpClock = ptpdStartup(argc, argv, &ret, &rtOpts)) )
	{
		// ptpdStartup did not return a pointer to the ptpClock structure,
		// something went wrong
		// Return code set by ptpdStartup function, pass
		// back to operating system.
		return ret;
	}

	// Check if Probe option selected (send one management message and then exit)
	// if not, then go ahead and start the PTP main code (protocol)

	if(rtOpts.probe)
	{
		probe(&rtOpts, ptpClock);
		ptpdShutdown();
		return 1;
	}
	else
	{
		if (rtOpts.ifaceName[0] == '\0')
		{
			/* do the multiple port protocol engine, if not bound to a single port */
			multiPortProtocol(&rtOpts, ptpClock);
			NOTIFY("main: self shutdown, probably due to an error\n");
		}
		else
		{
			/* do the single port protocol engine */
			protocol(&rtOpts, ptpClock);
			NOTIFY("main: self shutdown, probably due to an error\n");
		}
	}

	ptpdShutdown();

	return 1;
}
#endif

// eof ptpv2d.c

