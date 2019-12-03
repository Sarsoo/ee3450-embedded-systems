#ifndef __EZ_1294_LIB_H
#define __EZ_1294_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "Defines.h"

 

#if defined(TM4C123GH6PM)
    #include "TM4C123GH6PM.h"
#elif defined(TM4C1294NCPDT)
    #include "TM4C1294NCPDT.h"
#endif


typedef uint32_t clock_t;

#if defined(TM4C123GH6PM)
    void Setup_123G_80MHz(void);
#elif defined(TM4C1294NCPDT)
    void Setup_1294_80MHz(void);
#endif


void timer_waitMillis(uint32_t millis);
void timer_waitMicros(uint16_t micros);



#endif
