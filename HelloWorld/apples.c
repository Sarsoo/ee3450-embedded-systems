#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
 
#include "TM4C123GH6PM.h"
void DelayMs(int s);
void Delay(int t);
 
int main(void)
{
     
    // GPIO Configuration
    // 1. Enable Clock on GPIOF[3:1] (SYSCTL_RCGCGPIO)
    SYSCTL->RCGCGPIO = 0x20;
    // allow time for clock to stabilize (SYSCTL_PRGPIO)
    while((SYSCTL->PRGPIO & 0x20) != 0x20){};
    // 2. Unlock GPIOC[3:0], GIOD[7] and GPIOF[0] on TM4C123G (GPIOn->LOCK and GPIOn->CR) NOT USING
    // 3. Clear AMSEL to disable analog
		GPIOF->AMSEL = 0x00;
    // 4. Config PCTL to select GPIO
		GPIOF->PCTL = 0x0000;
		//or
		//GPIOF->PCTL &= 0xFFF00FFF;
    // 5. Set DIR to 0 for input, 1 for output
    GPIOF->DIR = 0x08; // PF3,PF2,PF1 for Output
    // 6. Enable AFSEL bits to 1
		GPIOF->AFSEL = 0x00;
    // 7. Set PUR bits to 1 to enable internal pull-up
		GPIOF->PUR = 0x10;
    // 8. Set DEN bits to 1 to enable data pins
    GPIOF->DEN = 0x0E; // Enable digital pin on PF3,PF2,PF1
			
		int sample = GPIOF -> DATA;
		sample |= 0x0E;			
		//sample |= 0x08;
		GPIOF->DATA = sample;
		
}