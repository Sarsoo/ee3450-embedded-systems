#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <TM4C123GH6PM.h>
//#include <TM4C1294NCPDT.h"
#include "MyDefines.h"  // Your Definitions Header File
#include "ez123G.h"
//#include "ez1294.h"

void Setup_PWM(void);
void Setup_GPIO(void);

typedef enum STATES{
    S_IDLE,
    S_0,
    S_1,
    S_2,
} STATES;


// Golbal Variables
uint32_t *pSw1  = (uint32_t *)(((char*)GPIOD) +(_PIN6 << 2));  // GPIO pin for SW1
uint32_t *pSw2  = (uint32_t *)(((char*)GPIOF) +(_PIN0 << 2));  // GPIO pin for SW2
uint32_t *pLed1 = (uint32_t *)(((char*)GPIOF) +(_PIN1 << 2));  // GPIO pin for LED1
uint32_t *pLed2 = (uint32_t *)(((char*)GPIOF) +(_PIN3 << 2));  // GPIO pin for LED2

int main()
{
    // Local Variables
    uint16_t    cmp_1_0m = 23750; // CMP value for Pulse Width 1 ms
    uint16_t    cmp_1_5m = 23125; // CMP value for Pulse Width 1.5 ms
    uint16_t    cmp_2_0m = 22500; // CMP value for Pulse Width 2 ms
    uint16_t    i = 0;
    bool        currSw1, preSw1 = 0;
    bool        currSw2, preSw2 = 1;
    bool        SW1, SW2;
    STATES      state = S_IDLE;

    Setup_123G_40MHz();
    //Setup_1294_60MHz();

    Setup_PWM();
    Setup_GPIO();
	
    while (1) {
        SW1 = SW2 = false;
        // Read current switches
				if(*pSw1)	currSw1 = 1;
					else currSw1 = 0;
				if(!(*pSw2))	currSw2 = 1;
					else currSw2 = 0;


        if (currSw1  && !preSw1){  // Detect the edge for Sw1
            SW1 = true;
        }

        if (currSw2  && !preSw2){  // Detect the edge for Sw2
            SW2 = true;
        }
        preSw1 = currSw1;   // Update current state to previous state
        preSw2 = currSw2;   // Update current state to previous state

        switch (state){ // Changed the state based on SW1 and SW2
            case S_IDLE:
								PWM0->_3_CMPB = cmp_1_0m;
								if(SW1)	state = S_0;
								if(SW2)	state = S_IDLE;
								
								*pLed1 = 0x0;
								*pLed2 = 0x0;
                
						break;
						case S_0:
								PWM0->_3_CMPB = cmp_1_0m;
								if(SW1)	state = S_1;
								if(SW2)	state = S_2;
								*pLed2 = 0x0;
						
								if(i%2 == 0){	//200ms delay
										//blink led1
										if(!(*pLed1 & _PIN1)) *pLed1 = 0xFF;
											else *pLed1 = 0x00;
								}
								
						break;
						case S_1:
								PWM0->_3_CMPB = cmp_1_5m;
								if(SW1)	state = S_2;
								if(SW2)	state = S_IDLE;
														
								if(i%2 == 0){	//200ms delay
										//blink led1 and led2
										if(!(*pLed1 & _PIN1)) *pLed1 = 0xFF;
											else *pLed1 = 0x00;
										if(!(*pLed2 & _PIN3)) *pLed2 = 0xFF;
											else *pLed2 = 0x00;
								}
						
						break;
						case S_2:
								PWM0->_3_CMPB = cmp_2_0m;
								if(SW1)	state = S_IDLE;
								if(SW2)	state = S_0;
								*pLed1 = 0x0;
						
								if(i%2 == 0){	//200ms delay
										//blink led2
										if(!(*pLed2 & _PIN3)) *pLed2 = 0xFF;
											else *pLed2 = 0x00;
								}
						
						break;
        }
        i++;
        timer_waitMillis(100);  // This is the only delay function in the code.
    }
}
//------------------------------------------------------------------------------
void Setup_PWM(void)
{
		// 1. Enable Clock for PWM Module 1
    SYSCTL->RCGCPWM |= _PWM_MODULE0;
    while((SYSCTL->PRPWM & _PWM_MODULE0)==0){};
    // 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (1 << 20);       // RCC[20]=1:USEPWMDIV
    SYSCTL->RCC &= ~0x000E0000;     // RCC[19:17]=000 PWMDIV
		SYSCTL->RCC |= (_PWMDIV_32 << 17);    // RCC[19:17]=0x04 divider=/32	
    // 3. Disable PWM Generator 2
    PWM0->_3_CTL &= ~0x01; // Disable PWM Generator 2
    // 4. Config LOAD, CMPn, GENn values
		PWM0->_3_LOAD = 25000;	// GEN 3 B PWM 7 PF3 MOTOR
    PWM0->_3_CMPB = 23750;
    PWM0->_3_GENB = _PWM_LEFT_ALIG_CMPBD;//0x080C	
		// 5. Enable PWM Generator 3
    PWM0->_3_CTL |= 0x01;
    // 6. Enable PWM7 Output
		PWM0 -> ENABLE |= _PWM7;
}
//------------------------------------------------------------------------------
void Setup_GPIO(void)
{
		// Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = _PORTC|_PORTD|_PORTF;
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & (_PORTC|_PORTD|_PORTF)) != (_PORTC|_PORTD|_PORTF)){};
    // 2. Unlock GPIO
		GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= 0x01;
    // 3. Clear AMSEL to disable analog
    // 4. Config PCTL to select GPIO
    GPIOC->PCTL	|= 0x400000;	//M0PWM7 GEN3
    // 5. Set DIR to 0 for input, 1 for output
    GPIOC -> DIR	|=  _PIN5;
		GPIOD -> DIR	=  0x0;
		GPIOF -> DIR	|=  _PIN1|_PIN3;
    // 6. Enable AFSEL bits to 1
		GPIOC -> AFSEL |= _PIN5;
		GPIOD -> AFSEL = 0x0;
		GPIOF -> AFSEL = 0x0;
    // 7. Set PUE bits to 1 to enable internal pull-up
    GPIOD -> PDR |= _PIN6;
		GPIOF -> PUR |= _PIN0;
    // 8. Set DEN bits to 1 to enable data pins
    GPIOC->DEN	|= _PIN5;
		GPIOD->DEN	|= _PIN6;
		GPIOF->DEN	|= _PIN0 | _PIN1 | _PIN3;			
}
//------------------------------------------------------------------------------
