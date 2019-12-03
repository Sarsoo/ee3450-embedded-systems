#include "MyDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
 
#include "TM4C123GH6PM.h"

 typedef enum STATES {
    S_0,
    S_1,
    S_2,
    S_3,
}STATES;

void SetGPIO(void);
void GPIOF_Handler (void);
void Setup_GPIOIRQ(void);
 
volatile bool sw1;

 uint32_t *led1 =(uint32_t *) (((char*)GPIOF) +(_PIN1 << 2));
 uint32_t *led2 =(uint32_t *) (((char*)GPIOF) +(_PIN3 << 2));
 
typedef struct STATELIST{
	int led1;
	int led2;
	struct STATELIST *next;
	
}STATELIST;

int main()
{
    STATES currState = S_0;

		STATELIST state[] = {
													{0, 0, &state[1]},	//s0
													{0xFF, 0, &state[2]},	//s1
													{0xFF, 0xFF, &state[3]},	//s2
													{0, 0xFF, &state[0]},	//s3
												};
	
    SetGPIO();
    Setup_GPIOIRQ ();

//Moore State Machine		
while(1){
    switch (currState){
        case S_0:
            *led1 = 0x00;
            *led2 = 0x00;
            if(sw1){
                    sw1=false;
                currState = S_1;
            }
        break;
        case S_1:
            *led1 = 0x01;
            *led2 = 0x00;
        if(sw1){
                    sw1=false;
                currState = S_2;
            }
        break;
        case S_2:
            *led1 = 0x01;
            *led2 = 0x01;
        if(sw1){
                    sw1=false;
                currState = S_3;
            }
        break;
     case S_3:
            *led1 = 0x00;
            *led2 = 0x01;
    if(sw1){
                    sw1=false;
                currState = S_0;
            }
        break;        
 
      }
    }

//Mealy State Machine
while(1){
    switch (currState){
        case S_0:
            if(sw1){
                sw1=false;
								currState = S_1;
							  *led1 = 0x00;
								*led2 = 0x00;
            }
        break;
        case S_1:
						if(sw1){
                sw1=false;
                currState = S_2;
								*led1 = 0x01;
								*led2 = 0x00;
            }
        break;
        case S_2:
						if(sw1){
                sw1=false;
                currState = S_3;
								*led1 = 0x01;
								*led2 = 0x01;
            }
        break;
				case S_3:
						if(sw1){
                sw1=false;
                currState = S_0;
								*led1 = 0x00;
								*led2 = 0x01;
            }
        break;        
 
      }
    }
	
//MOORE STATE WITH LINKED LIST
STATELIST *pState = &state[0];		
while(1){
	*led1 = pState->led1;
	*led2 = pState->led2;
	if(sw1){
		
	}
}	
}
void SetGPIO(void)
{
    // Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = _PORTF;
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & _PORTF) != _PORTF){};
 
    // 2. Unlock GPIO

    // 3. Clear AMSEL to disable analog
    // 4. Config PCTL to select GPIO
        GPIOF->PCTL= 0x00;
    // 5. Set DIR to 0 for input, 1 for output
        GPIOF->DIR=  _PIN1 | _PIN2;
    // 6. Enable AFSEL bits to 1

    // 7. Set PUE bits to 1 to enable internal pull-up
        GPIOF->PUR= _PIN4;
    // 8. Set DEN bits to 1 to enable data pins
        GPIOF->DEN= _PIN1 | _PIN4 | _PIN2;    
}
 
void Setup_GPIOIRQ()
{
        uint16_t n,m,r,r2;
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
        n = 30/4;
      r = 21;
        m = 30/32; r2 = 30 % 32;
    NVIC->IP[n] |= (3 << r);
  // 7. Enable IRQ in the NVIC
    NVIC->ISER[m] |= (1 << r2);
}
     uint32_t *pn1 =(uint32_t *) (((char*)GPIOF) +(_PIN4 << 2));

void GPIOF_Handler()//Interrupt service
{
        GPIOF ->ICR |= _PIN4; // Clear Interrupt flag
        sw1=true;
    //*pn1 ^= _PIN1;

}