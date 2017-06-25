/*----------------------------------------------------------------------------*-

  ttrd2-03a-t0401a-v001a_heartbeat_sw_u_task.h (Release 2017-02-22a)

  ----------------------------------------------------------------------------
   
  - See ttrd2-03a-t0401a-v001a_heartbeat_sw_u_task.c for details.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2016 SafeTTy Systems Ltd.

  This code forms part of a Time-Triggered Reference Design (TTRD) that is 
  documented in the following book: 

   Pont, M.J. (2016) 
   "The Engineering of Reliable Embedded Systems (Second Edition)", 
   Published by SafeTTy Systems Ltd. ISBN: 978-0-9930355-3-1.

  Both the TTRDs and the above book ("ERES2") describe patented 
  technology and are subject to copyright and other restrictions.

  This code may be used without charge: [i] by universities and colleges on 
  courses for which a degree up to and including 'MSc' level (or equivalent) 
  is awarded; [ii] for non-commercial projects carried out by individuals 
  and hobbyists.

  Any and all other use of this code and / or the patented technology 
  described in ERES2 requires purchase of a ReliabiliTTy Technology Licence:
  http://www.safetty.net/technology/reliabilitty-technology-licences

  Please contact SafeTTy Systems Ltd if you require clarification of these 
  licensing arrangements: http://www.safetty.net/contact

  CorrelaTTor, DecomposiTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy,  
  SafeTTy, SafeTTy Systems, TriplicaTTor and WarranTTor are registered 
  trademarks or trademarks of SafeTTy Systems Ltd in the UK & other countries.

-*----------------------------------------------------------------------------*/

#ifndef __HEARTBEAT_TASK_H__
#define __HEARTBEAT_TASK_H__

#include "main.h"

#define LED1_PIN        GPIO_PIN_5 
#define LED2_PIN        GPIO_PIN_6 
#define LED3_PIN        GPIO_PIN_7 
#define LED1_PORT       GPIOF
#define LED2_PORT       GPIOF
#define LED3_PORT       GPIOF

void LED_Init(void);
void LED1_Open(void);
void LED1_Close(void);
void LED1_Toggle(void);
void LED2_Open(void);
void LED2_Close(void);
void LED2_Toggle(void);
void LED3_Open(void);
void LED3_Close(void);
void LED3_Toggle(void);

// ------ Public function prototypes -----------------------------------------

void HEARTBEAT_SW_U_Init(void);

void HEARTBEAT_SW_U_Update1(void);
void HEARTBEAT_SW_U_Update2(void);

#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/

