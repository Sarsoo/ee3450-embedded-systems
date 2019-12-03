#ifndef __MYDEFINES_H
#define __MYDEFINES_H

#define _PIN0 0x01
#define _PIN1 0x02
#define _PIN2 0x04
#define _PIN3 0x08
#define _PIN4 0x10
#define _PIN5 0x20
#define _PIN6 0x40
#define _PIN7 0x80

#define _PORTA 0x0001
#define _PORTB 0x0002
#define _PORTC 0x0004
#define _PORTD 0x0008
#define _PORTE 0x0010
#define _PORTF 0x0020


#define _PWM_MODULE0 0x01
#define _PWM_MODULE1 0x02

#define _PWM0 0x01
#define _PWM1 0x02
#define _PWM2 0x04
#define _PWM3 0x08
#define _PWM4 0x10
#define _PWM5 0x20
#define _PWM6 0x40
#define _PWM7 0x80

#define _PWMDIV_2 0x0
#define _PWMDIV_4 0x1
#define _PWMDIV_8 0x2
#define _PWMDIV_16 0x3
#define _PWMDIV_32 0x4
#define _PWMDIV_64 0x5

#define _PWM_LEFT_ALIG_CMPAD ((0x02 <<  6) | (0x03 << 2))
#define _PWM_LEFT_ALIG_CMPBD ((0x02 << 10) | (0x03 << 2))

#endif