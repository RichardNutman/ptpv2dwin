/* src/dep/ledlib.c */
/* Simple library to set/clear MPC8313E-RDB LEDs from user mode */
/* NOTE: These functions could be used as a basis to turn on/off
 * LEDs on other target boards based on PTP status
 */

/****************************************************************************/
/* Begin copyright and licensing information, do not remove                 */
/*                                                                          */
/* This file (ledlib.c) contains original work by Alan K. Bartky            */
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

/**
 * @file ledlib.c
 * This file contains library functions for setting and clearing
 * status LEDs for use by the ptpv2d software.  It was originally
 * written and designed for the MPC8313E-RBD board, but hopefully 
 * should be useful as an GPL version 2 licensed open source
 * for other LEDs, or for use to directly manipulate other memory 
 * mapped functions available by some external physical address
 * by a Linux application.
 *
 * @par MPC8313E-RDB board LED information
 * The MPC8313E-RDB board has 8 LEDs where the least significant 6 bits are green
 * (0-63), and the upper 2 bits are yellow and red where red is the most signifcant
 * bit of the byte.
 *
 *
 * @par Usage for ioremap, out_8, iounmap
 * @code
 *   volatile void *p = ioremap(MY_HARD_REG_ADDR, 4096);
 *   ...
 *   out_8(p, state ^= 0x1);
 *   ...
 *   // When done with virutal pointer
 *   ...
 *   iounmap(p, 4096);
 * @nocode
 *
 * @par Copyright (for for ioremap, iounmap and out_8 functions)
 *  Copyright (C) 2003 Stephane Fillod
 *
 * @par Copyright (for LED manipulation functions)
 * This file (ledlib.c) contains original work by Alan K. Bartky
 * Copyright (c) 2008-2010 by Alan K. Bartky, all rights reserved
 *
 * @par License
 * This source code and its associated software algorithms are under        
 * copyright and for this file are licensed under the terms of the GNU      
 * General Public License as published by the Free Software Foundation;     
 * either version 2 of the License, or (at your option) any later version.
 *
 * @author Alan K. Bartky (LED library functions for MPC8313E-RDB)
 * @author Stephanie Filod (open source functions for ioremap, iounmap and out_8)
 *
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef LED_SUPPORT
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <memory.h>
#endif

#ifdef MPC8313E_RDB

// Per 8313EVB_INIT.cfg, LED/Status buffer is memory mapped @ 0xfa000000, size 32 KB

#define MEMSIZE         0x00008000
#define MEM_ADDR        0xfa000000

#define LED_BIT_7       0x80
#define LED_BIT_6       0x40
#define LED_BIT_5       0x20
#define LED_BIT_4       0x10
#define LED_BIT_3       0x08
#define LED_BIT_2       0x04
#define LED_BIT_1       0x02
#define LED_BIT_0       0x01


#define LEDS_ALL_ON     0x00
#define LEDS_ALL_OFF    0xFF

#define RED_LED         7
#define RED_LED_ON      0x7F
#define RED_LED_OFF     0x80

#define YELLOW_LED      6
#define YELLOW_LED_ON   0xBF
#define YELLOW_LED_OFF  0x40

#define GREEN_LED       5
#define GREEN_LED_ON    0xDF
#define GREEN_LED_OFF   0x20

#define STATUS_LED      4
#define STATUS_LED_ON   0xEF
#define STATUS_LED_OFF  0x10

#endif

#ifdef LED_SUPPORT
/* Global vars */
volatile unsigned char * led_ptr;
unsigned char            led_value;


/**
 * Function to output value to memory mapped location
 *
 * @par Copyright
 *  Copyright (C) 2003 Stephane Fillod
 * 
 * @param addr[out]
 * Virtual address of uprotected memory mapped location to write 8 bit unsigned
 * byte value to
 *
 * @param val[in] Unsigned 8 bit value to write
 *
 */
#ifdef __PPC__
inline void out_8(volatile unsigned char *addr, unsigned val)
{
        __asm__ __volatile__("stb%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}
/* etc., cf asm/io.h */
#else
inline void out_8(volatile unsigned char *addr, unsigned val)
{
        *addr = val & 0xff;
}
#endif

/**
 * Function to create a virtual pointer to a 32 bit physical
 * address
 *
 * @par Copyright
 *  Copyright (C) 2003 Stephane Fillod
 *
 * @param physaddr Physical memory pointer in unsigned long format to create virtual pointer to
 * @param length Length of segment of physical address to create the virtual pointer
 * @return Returns value from munmap function call.
 */
volatile void * ioremap(unsigned long physaddr, unsigned size)
{
    static int axs_mem_fd = -1;
    unsigned long page_addr, ofs_addr, reg, pgmask;
    void* reg_mem = NULL;

    /*
     * looks like mmap wants aligned addresses?
     */
    pgmask = getpagesize()-1;
    page_addr = physaddr & ~pgmask;
    ofs_addr  = physaddr & pgmask;

    /*
     * Don't forget O_SYNC, esp. if address is in RAM region.
     * Note: if you do know you'll access in Read Only mode,
     *    pass O_RDONLY to open, and PROT_READ only to mmap
     */
    if (axs_mem_fd == -1) {
        axs_mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
        if (axs_mem_fd < 0) {
                perror("AXS: can't open /dev/mem");
                return NULL;
        }
    }

    /* memory map */
    reg_mem = mmap(
        (caddr_t)reg_mem,
        size+ofs_addr,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        axs_mem_fd,
        page_addr
    );
    if (reg_mem == MAP_FAILED) {
        perror("AXS: mmap error");
        close(axs_mem_fd);
        return NULL;
    }

    reg = (unsigned long )reg_mem + ofs_addr;
    return (volatile void *)reg;
}

/**
 * Function to unmap a pointer to protected memory
 *
 * @par Copyright
 *  Copyright (C) 2003 Stephane Fillod
 *
 * @param start Virtual pointer to currently mapped protected memory
 * @param length Length of currently mapped protected memory as pointed to by start parameter
 * @return Returns value from munmap function call.
 */
int iounmap(volatile void *start, size_t length)
{
    unsigned long ofs_addr;
    ofs_addr = (unsigned long)start & (getpagesize()-1);

    /* do some cleanup when you're done with it */
    return munmap((unsigned char*)start-ofs_addr, length+ofs_addr);
}

#endif

/**
 * Function to turn all LEDs on or off.
 * @param on_or_off
 * If non-zero, turn all LEDs on
 * If zero, turn all LEDs off
 */
void all_leds (unsigned char on_or_off)
{
#ifdef LED_SUPPORT
   if (on_or_off)
   {
      led_value = LEDS_ALL_ON;
   }
   else
   {
      led_value = LEDS_ALL_OFF;
   }
   out_8(led_ptr,led_value);
#endif
}

/**
 * Function to set red alarm LED state
 * @param on_or_off
 * If non-zero, turn Red LED on, green and yellow LEDs off.
 * If zero, turn Red LED off
 */
void red_alarm (unsigned char on_or_off)
{
#ifdef LED_SUPPORT
   if (on_or_off)
   {
      /* Turn RED LED on, turn Green
       * and Yellow LEDs off
       */
      led_value &= RED_LED_ON;
      led_value |= GREEN_LED_OFF;
      led_value |= YELLOW_LED_OFF;
   }
   else
   {
      led_value |= RED_LED_OFF;
   }
   out_8(led_ptr,led_value);
#endif
}

/**
 * Function to set yellow alarm LED state
 * @param on_or_off
 * If non-zero, turn yellow LED on, green LED off, red LED left unchanged
 * If zero, turn yellow LED off
 */
void yellow_alarm (unsigned char on_or_off)
{
#ifdef LED_SUPPORT
   if (on_or_off)
   {
      /* Turn LED on, turn bit off */
      led_value &= YELLOW_LED_ON;
      led_value |= GREEN_LED_OFF;
   }
   else
   {
      led_value |= YELLOW_LED_OFF;
   }
   out_8(led_ptr,led_value);
#endif
}

/**
 * Function to set green alarm LED state
 * @param on_or_off
 * If non-zero, turn green LED on, yellow LED off, red LED left unchanged
 * If zero, turn green LED off
 */
void green_alarm (unsigned char on_or_off)
{
#ifdef LED_SUPPORT
   if (on_or_off)
   {
      /* Turn LED on, turn bit off */
      led_value &= GREEN_LED_ON;
      led_value |= YELLOW_LED_OFF;
   }
   else
   {
      led_value |= GREEN_LED_OFF;
   }
   out_8(led_ptr,led_value);
#endif /* LED_SUPPORT */
}

/**
 * Function to use least significant 4 bits of LED register to act
 * like a VU meter that goes up or down depending on passed value
 * @param value Meter value to set.  
 * For this meter, the smaller the value the more LEDs light
 * up.  Giving:
 * @par
 * value > 80: all LEDs off
 * @par
 * value <= 80 and value >40 : One LED  on
 * @par
 * value <= 40 and value >20 : Two LEDs on
 * @par
 * value <= 20 and value >10 : Three LEDs on
 * @par
 * value <= 10 : Four LEDs on
 *
 * @par
 * This is useful for PTP as we use the meter to indicate offset
 * from grandmaster.  The lower the number, the better
 * emulating a "cell phone bar" like application where
 * the better the signal the "more bars" you have.
 * 
 */
void led_meter(unsigned char value)
{
#ifdef LED_SUPPORT
  unsigned char led_meter_value;

  led_meter_value = 0;  /* Assume VU meter min value */

  if (value <= 80)
  {
    led_meter_value |= 1;
    if (value <= 40)
    {
      led_meter_value |= 2;
      if (value <= 20)
      {
         led_meter_value |= 4;
         if (value <= 10)
         {
           led_meter_value |= 8;
         }
      }
    }
  }

  /* Clear led value lower 4 bits */

  led_value &= 0xF0;

  led_meter_value ^= 0x0F;  /* Invert bits for setting LED */

  led_value |= led_meter_value;

  out_8(led_ptr,led_value);
#endif
}

/** 
 * Function to initialize LEDs to allow to calls to yellow alarm,
 * green_alarm, etc.
 */
void init_leds(void)
{
#ifdef LED_SUPPORT
	/* Create user mode pointer to LED register */

	led_ptr = (unsigned char *) ioremap(MEM_ADDR, MEMSIZE);

        /* Turn off all LEDs except the green status (running) for initial status */

        led_value = STATUS_LED_ON;

        out_8(led_ptr,led_value);
#endif
}

// eof ledlib.c


