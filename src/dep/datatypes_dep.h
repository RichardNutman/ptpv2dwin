/* src/dep/datatypes_dep.h */
/* Target specific data structures, typedefs, etc. definitions for PTP */
/* Copyright (c) 2005-2007 Kendall Correll */

/****************************************************************************/
/* Begin additional copyright and licensing information, do not remove      */
/*                                                                          */
/* This file (datattypes_dep.h) contains Modifications (updates, corrections*/
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


#ifndef DATATYPES_DEP_H
#define DATATYPES_DEP_H

#ifdef FALSE
#undef FALSE
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef __WINDOWS__
// Windows doesn't support ssize_t, they support it as uppercase
#define ssize_t SSIZE_T

// Windows compiler doesn't add Winsock for the standard library
// This pragma forces it to:
#pragma comment(lib, "ws2_32.lib")

// Windows handles raw sockets with the same structures and types as 
// IP sockets, but opens them as raw.  
// These defines map the linux raw socket types to regular socket type
#define sockaddr_ll     sockaddr_in
#define sll_family      sin_family
#define sll_ifindex     sin_ifindex;
#define sll_protocol    sin_protocol 

// Windows does not suppot bzero, mapping to memset
#ifndef bzero
#define bzero(dest,len) memset(dest,0,(size_t)len)
#endif

// Adding MIN and MAX
#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))


#ifndef socklen_t 
typedef int socklen_t;
#endif

#ifndef iovec
struct iovec
{
    void * iov_base;  /* BSD uses caddr_t (1003.1g requires void *) */
    size_t iov_len;   /* Must be size_t (1003.1g) */
};
#endif



#ifndef msghdr
struct msghdr {
    void         *msg_name;       /* optional address */
    socklen_t     msg_namelen;    /* size of address */
    struct iovec *msg_iov;        /* scatter/gather array */
    size_t        msg_iovlen;     /* # elements in msg_iov */
    void         *msg_control;    /* ancillary data, see below */
    socklen_t     msg_controllen; /* ancillary data buffer len */
    int           msg_flags;      /* flags on received message */
};

#ifndef timespec
struct timespec 
{
      time_t  tv_sec;         /* seconds */
      long    tv_nsec;        /* and nanoseconds */
};
#endif

#ifndef itimerspec
struct itimerspec 
{
   struct timespec  it_interval;
   struct timespec  it_value;
};
#endif


#endif

#else
// Non-Windows mappings to make windows happy and still compile
// Linux OK
#ifndef SOCKET
#define SOCKET Integer32
#endif

#endif

typedef enum {FALSE=0, TRUE} Boolean;
#ifndef __WINDOWS__
typedef char                 Octet;
#else
typedef unsigned char        Octet;       // AKB: Changed from char to unsigned char for Windows
#endif
typedef signed char          Integer8;
typedef signed short         Integer16;
typedef signed int           Integer32;
typedef signed long long     Integer64;   // AKB: added
typedef unsigned char        UInteger8;
typedef unsigned short       UInteger16;
typedef unsigned int         UInteger32;
typedef unsigned long long   UInteger64;  // AKB: added

// AKB: Added enumeration types

typedef unsigned char        Enumeration8;
typedef unsigned short       Enumeration16;
typedef unsigned int         Enumeration32;
typedef unsigned long long   Enumeration64;

typedef struct {
  Integer32  nsec_prev;
  Integer32  y;
} offset_from_master_filter;

typedef struct {
  Integer32  nsec_prev;
  Integer32  y;
  Integer32  s_exp;
} one_way_delay_filter;

// AKB 2010-09-05: Changed Sockets from Integer32 to SOCKET 
typedef struct {
  SOCKET        eventSock;                  /* Event port UDP socket */
  SOCKET        generalSock;                /* General port UDP socket */
  UInteger32    multicastAddr;              /* IP multicast address */
  UInteger32    unicastAddr;                /* Optional IP unicast destination address */
  UInteger32    pdelayMulticastAddr;        /* Address for V2 PDelay messages */
  UInteger32    rawIfIndex;                 /* Interface Index of raw socket */
  char          ifName[IFNAMSIZ];           /* Interface name (e.g. "eth0") */
  unsigned char portMacAddress[6];          /* Local Hardware Port MAC address */
} NetPath;

#endif

// eof datatypes_dep.h

