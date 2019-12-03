#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "MyDefines.h"
 
#include "TM4C123GH6PM.h"
 
void SetupGPIO(void);
void SetupGPIOIRQ(void);
void GPIOF_Handler(void);
 
int main()
{
		SetupGPIO();
		SetupGPIOIRQ();
	
    while(1){
        
    }
}

void SetupGPIO(void){
	
		// Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = _PORTF;
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & _PORTF) != _PORTF){};
    // 2. Unlock GPIO
    // 3. Clear AMSEL to disable analog
		GPIOF -> AMSEL = 0x0; 
    // 4. Config PCTL to select GPIO
		GPIOF -> PCTL = 0x0;
    // 5. Set DIR to 0 for input, 1 for output
		GPIOF -> DIR = _PIN1;
    // 6. Enable AFSEL bits to 1
		GPIOF -> AFSEL = 0x0;
    // 7. Set PUE bits to 1 to enable internal pull-up
		GPIOF -> PUR = _PIN4;
    // 8. Set DEN bits to 1 to enable data pins
		GPIOF -> DEN = _PIN4|_PIN1;
}

void SetupGPIOIRQ()
{
		uint32_t n, r, m, r2;
    // 1. Disable IRQ in GPIOIM register
    GPIOF->IM &= ~(_PIN4);
    // 2. Configure IRQ Type (0=edge, 1=level) in the GPIOIS register
    GPIOF->IS &= ~(_PIN4);
  // 3. Configure GPIOIBE (0=single edge, 1=both edge),
  //    GPIOIEV (0=low level or falling edge, 1=high level or rising edge)
    GPIOF->IBE &= ~(_PIN4);
    GPIOF->IEV &= ~(_PIN4);
  // 4. Clear the GPIORIS register
    GPIOF->ICR |= _PIN4;
  // 5. Enable IRQ in the GPIOIM register
    GPIOF->IM |= _PIN4;
  // 6. Set priority in the NVIC
		n = 30 / 4;
	r = 21; // remainder:0 -> r = 5, :1-> r=13, :2-> r=21, :3-> r=29
		m = 30 / 32; r2 = 30 % 32;
    NVIC->IP[n] |= (3 << r);
  // 7. Enable IRQ in the NVIC
    NVIC->ISER[m] |= (1 << r2);
}

uint32_t* pn1 = (uint32_t*)(((char*)GPIOF) + (_PIN1 << 2));
void GPIOF_Handler(){
		GPIOF->ICR |= _PIN4; //clear interrupt flag
		*pn1 ^= _PIN1;
	
}