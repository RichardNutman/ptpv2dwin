/* src/dep/constants_dep.h */
/* Target specific constant definitions for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (constants_dep.h) contains Modifications (updates, corrections */
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
 * @file constants_dep.c
 * Platform dependent constants, defines, includes and macros
 *
 * @par Original Copyright
 * This file is a derivative work from constants_dep.h
 * Copyright (c) 2005-2007 Kendall Correll 
 *
 * @par Modifications and enhancements Copyright
 * Modifications Copyright (c) 2007-2010 by Alan K. Bartky, all rights
 * reserved
 *
 * @par
 * This file (constants_dep.c) contains Modifications (updates, corrections      
 * comments and addition of initial support for IEEE 1588 version 1, IEEE 
 * version 2 and IEEE 802.1AS PTP) and other features by Alan K. Bartky.
 * 
 * @par License
 * These modifications and their associated software algorithms are under 
 * copyright and for this file are licensed under the terms of the GNU   
 * General Public License as published by the Free Software Foundation;   
 * either version 2 of the License, or (at your option) any later version.
 */
#ifndef CONSTANTS_DEP_H
#define CONSTANTS_DEP_H

#if !defined(linux) && !defined(__NetBSD__) && !defined(__FreeBSD__) && !defined(__WINDOWS__)
#error Not ported to this architecture, please update.
#endif

#ifdef	linux
#include<netinet/in.h>
#include<net/if.h>
#include<net/if_arp.h>
#define IFACE_NAME_LENGTH         IF_NAMESIZE
#define NET_ADDRESS_LENGTH        INET_ADDRSTRLEN

#define IFCONF_LENGTH 10

#include <netpacket/packet.h>
#include <net/ethernet.h>       /* the L2 protocols */

#include<endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define PTPD_LSBF
#elif __BYTE_ORDER == __BIG_ENDIAN
#define PTPD_MSBF
#endif
#endif /* linux */

#ifdef	__WINDOWS__
//#include<winsock.h>
#include<winsock2.h>
#include<Ws2tcpip.h>
#include <iphlpapi.h>
#include<assert.h>
#include<WinBase.h>
#include<io.h>

#ifndef IF_NAMESIZE              // Should be defined in Windows netioapi.h
#define IF_NAMESIZE         256  // Windows maps interface names to NDIS names which are 256 bytes long
#endif

#ifndef IFNAMSIZ
#define IFNAMSIZ            IF_NAMESIZE  // defined in Windows netioapi.h
#endif



#define IFACE_NAME_LENGTH   IF_NAMESIZE

//#define INET_ADDRSTRLEN     16
#define NET_ADDRESS_LENGTH  INET_ADDRSTRLEN

#define IFCONF_LENGTH       10

// AKB 2010-09-05: Windows uses normal sockets and opens them as raw
// These constants are to map linux compatible raw
// socket constants
#define PF_PACKET AF_INET
#define AF_PACKET AF_INET

/* Windows only runs little endian */
#define PTPD_LSBF

#define PTPD_NO_DAEMON  1 // Windows code doesn't support background mode

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#pragma comment(lib, "IPHLPAPI.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

// Disable warnings about reopen function:
#define _CRT_SECURE_NO_WARNINGS

#endif /* __WINDOWS__*/

#if defined(__NetBSD__) || defined(__FreeBSD__)
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <net/if.h>
# include <net/if_dl.h>
# include <net/if_types.h>
# if defined(__FreeBSD__)
#  include <net/ethernet.h>
#  include <sys/uio.h>
# else
#  include <net/if_ether.h>
# endif
# include <ifaddrs.h>
# define IFACE_NAME_LENGTH         IF_NAMESIZE
# define NET_ADDRESS_LENGTH        INET_ADDRSTRLEN

# define IFCONF_LENGTH 10

# define adjtimex ntp_adjtime

# include <machine/endian.h>
# if BYTE_ORDER == LITTLE_ENDIAN
#   define PTPD_LSBF
# elif BYTE_ORDER == BIG_ENDIAN
#   define PTPD_MSBF
# endif
#endif


#ifdef CONFIG_MPC831X
#define ADJ_FREQ_MAX  524288
#else
#define ADJ_FREQ_MAX  512000
#endif

#define DEFAULT_UTC_OFFSET           33 /* V2: TAI = UTC plus 33 seconds as of 1/1/2006 */

#ifdef __WINDOWS__
/* System Dependent (AKB: Moved from constants.h to constants_dep.h */
#define DEFAULT_SYNC_INTERVAL        0
#define DEFAULT_ANNOUNCE_INTERVAL    2  // AKB: Added for PTP V2
#define DEFAULT_V1_CLOCK_VARIANCE    (-4000)  /* AKB: renamed for V2, changed for HW clock */
#define DEFAULT_V2_CLOCK_VARIANCE    ( 4000)  /* AKB: added   for V2, changed for HW clock */
#define DEFAULT_CLOCK_STRATUM        4
#define DEFAULT_INBOUND_LATENCY      0       /**< Default inbound  latency in nanoseconds */
#define DEFAULT_OUTBOUND_LATENCY     0       /**< Default outbound latency in nanoseconds */
#define DEFAULT_NO_RESET_CLOCK       FALSE
#define DEFAULT_AP                   10
#define DEFAULT_AI                   1000
#define DEFAULT_DELAY_S              6
#define DEFAULT_MAX_FOREIGN_RECORDS  5

/* features, only change to reflect changes in implementation */
#define CLOCK_FOLLOWUP     FALSE  // Windows implementation doesn't support HW timestamping
#define CLOCK_FOLLOWUP_RAW FALSE
#define INITIALIZABLE      TRUE
#define BURST_ENABLED      FALSE
#define EXTERNAL_TIMING    FALSE
#define BOUNDARY_CLOCK     FALSE
#define NUMBER_PORTS       1


#elif defined CONFIG_MPC831X

/* System Dependent (AKB: Moved from constants.h to constants_dep.h */
/* Defines for MPC831X implementation with Hardware timestampinng

#define DEFAULT_SYNC_INTERVAL        0
#define DEFAULT_ANNOUNCE_INTERVAL    2  // AKB: Added for PTP V2
#define DEFAULT_V1_CLOCK_VARIANCE    (-1000)  /* AKB: renamed for V2, changed for HW clock */
#define DEFAULT_V2_CLOCK_VARIANCE    ( 1000)  /* AKB: added   for V2, changed for HW clock */
#define DEFAULT_CLOCK_STRATUM        4
#define DEFAULT_INBOUND_LATENCY      0       /**< Default inbound  latency in nanoseconds */
#define DEFAULT_OUTBOUND_LATENCY     0       /**< Default outbound latency in nanoseconds */
#define DEFAULT_NO_RESET_CLOCK       FALSE
#define DEFAULT_AP                   2
#define DEFAULT_AI                   10
#define DEFAULT_DELAY_S              6
#define DEFAULT_MAX_FOREIGN_RECORDS  5

/* features, only change to reflect changes in implementation */
#define CLOCK_FOLLOWUP     TRUE 
#define CLOCK_FOLLOWUP_RAW TRUE
#define INITIALIZABLE      TRUE
#define BURST_ENABLED      FALSE
#define EXTERNAL_TIMING    FALSE
#define BOUNDARY_CLOCK     FALSE
#define NUMBER_PORTS       1

#else

/* System Dependent (AKB: Moved from constants.h to constants_dep.h */
#define DEFAULT_SYNC_INTERVAL        0
#define DEFAULT_ANNOUNCE_INTERVAL    2  // AKB: Added for PTP V2
#define DEFAULT_V1_CLOCK_VARIANCE    (-4000)  /* AKB: renamed for V2, changed for HW clock */
#define DEFAULT_V2_CLOCK_VARIANCE    ( 4000)  /* AKB: added   for V2, changed for HW clock */
#define DEFAULT_CLOCK_STRATUM        4
#define DEFAULT_INBOUND_LATENCY      0       /**< Default inbound  latency in nanoseconds */
#define DEFAULT_OUTBOUND_LATENCY     0       /**< Default outbound latency in nanoseconds */
#define DEFAULT_NO_RESET_CLOCK       FALSE
#define DEFAULT_AP                   10
#define DEFAULT_AI                   1000
#define DEFAULT_DELAY_S              6
#define DEFAULT_MAX_FOREIGN_RECORDS  5

/* features, only change to reflect changes in implementation */
#define CLOCK_FOLLOWUP     TRUE   // Linux implementation does support followup on IP sockets
#define CLOCK_FOLLOWUP_RAW FALSE  // Linux implementation does not yet support timestamps on raw sockets
#define INITIALIZABLE      TRUE
#define BURST_ENABLED      FALSE
#define EXTERNAL_TIMING    FALSE
#define BOUNDARY_CLOCK     FALSE
#define NUMBER_PORTS       1

#endif


/* UDP/IPv4 dependent */

#define SUBDOMAIN_ADDRESS_LENGTH  4
#define PORT_ADDRESS_LENGTH       2

#define PACKET_SIZE  384  // AKB: Big enough for all packets and nice round binary number

#define PTP_EVENT_PORT    319  // 0x013F
#define PTP_GENERAL_PORT  320  // 0x0140

#define DEFAULT_PTP_DOMAIN_ADDRESS     "224.0.1.129"  // 0xE0-00-01-81
#define ALTERNATE_PTP_DOMAIN1_ADDRESS  "224.0.1.130"  // 0xE0-00-01-82
#define ALTERNATE_PTP_DOMAIN2_ADDRESS  "224.0.1.131"  // 0xE0-00-01-83
#define ALTERNATE_PTP_DOMAIN3_ADDRESS  "224.0.1.132"  // 0xE0-00-01-84
#define DEFAULT_PTP_PDELAY_ADDRESS     "224.0.0.107"  // 0xE0-00-00-6B

#define HEADER_LENGTH             40
#define SYNC_PACKET_LENGTH        124
#define DELAY_REQ_PACKET_LENGTH   124
#define FOLLOW_UP_PACKET_LENGTH   52
#define DELAY_RESP_PACKET_LENGTH  60
#define MANAGEMENT_PACKET_LENGTH  136

#define MM_STARTING_BOUNDARY_HOPS  0x7fff

/* others */

#define SCREEN_BUFSZ  256     // AKB: Increased to handle more stats (may cause screen wrap)
#define SCREEN_MAXSZ  144

#endif

// eof constants_dep.h
