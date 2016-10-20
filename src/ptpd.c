/* ptpd.c */

#include "ptpd.h"

RunTimeOpts rtOpts;  /* statically allocated run-time configuration data */

int main(int argc, char **argv)
{
  PtpClock *ptpClock;
  Integer16 ret;
  
  /* initialize run-time options to reasonable values */ 

  memset(&rtOpts, 0, sizeof(rtOpts));

  rtOpts.syncInterval  = DEFAULT_SYNC_INTERVAL;
  memcpy(rtOpts.subdomainName, 
         DEFAULT_PTP_DOMAIN_NAME, 
         PTP_SUBDOMAIN_NAME_LENGTH
        );
  memcpy(rtOpts.clockIdentifier,
         IDENTIFIER_DFLT,
         PTP_CODE_STRING_LENGTH
        );
  rtOpts.clockVariance     = DEFAULT_CLOCK_VARIANCE;
  rtOpts.clockStratum      = DEFAULT_CLOCK_STRATUM;
  rtOpts.unicastAddress[0] = 0;
  rtOpts.inboundLatency.nanoseconds  = DEFAULT_INBOUND_LATENCY;
  rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
  rtOpts.noResetClock                = DEFAULT_NO_RESET_CLOCK;
  rtOpts.s  = DEFAULT_DELAY_S;
  rtOpts.ap = DEFAULT_AP;
  rtOpts.ai = DEFAULT_AI;
  rtOpts.max_foreign_records = DEFAULT_MAX_FOREIGN_RECORDS;
  rtOpts.currentUtcOffset    = DEFAULT_UTC_OFFSET;
  
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

