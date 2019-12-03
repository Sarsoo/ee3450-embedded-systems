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
    S_3,
} STATES;


// Golbal Variables
uint32_t *pSw1  = (uint32_t *)(((char*)GPIOD) +(_PIN6 << 2));  // GPIO pin for SW1
uint32_t *pSw2  = (uint32_t *)(((char*)GPIOF) +(_PIN0 << 2));  // GPIO pin for SW2
uint32_t *pLed1 = (uint32_t *)(((char*)GPIOF) +(_PIN1 << 2));  // GPIO pin for LED1
uint32_t *pLed2 = (uint32_t *)(((char*)GPIOF) +(_PIN3 << 2));  // GPIO pin for LED2

int main()
{
    // Local Variables
    uint16_t    cmp_1_0m = _____; // CMP value for Pulse Width 1 ms
    uint16_t    cmp_1_5m = _____; // CMP value for Pulse Width 1.5 ms
    uint16_t    cmp_2_0m = _____; // CMP value for Pulse Width 2 ms
    uint16_t    i = 0;
    bool        currSw1, preSw1 = _____;
    bool        currSw2, preSw2 = _____;
    bool        SW1, SW2;
    STATES      state = S_IDLE;

    Setup_123G_40MHz();
    //Setup_1294_60MHz();

    Setup_PWM();
    Setup_GPIO();

    while (1) {
        SW1 = SW2 = false;
        // Read current switches



        if (  &&   ){  // Detect the edge for Sw1
            SW1 = true;
        }

        if (  &&   ){  // Detect the edge for Sw2
            SW2 = true;
        }
        preSw1 = currSw1;   // Update current state to previous state
        preSw2 = currSw2;   // Update current state to previous state

        switch (state){ // Changed the state based on SW1 and SW2
            case S_IDLE:


                break;






        }
        i++;
        timer_waitMillis(100);  // This is the only delay function in the code.
    }
}
//------------------------------------------------------------------------------
void Setup_PWM(void)
{

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
    GPIOC->PCTL= 0x000000;
    // 5. Set DIR to 0 for input, 1 for output
    GPIOC -> DIR	|=  _PIN5;
		GPIOD -> DIR	=  0x0;
		GPIOF -> DIR	=  _PIN0|_PIN1|_PIN3;
    // 6. Enable AFSEL bits to 1
		GPIOC -> AFSEL |= _PIN5;
    // 7. Set PUE bits to 1 to enable internal pull-up
    GPIOD -> PDR= _PIN6;
		GPIOF -> PUR= _PIN0;
    // 8. Set DEN bits to 1 to enable data pins
    GPIOF->DEN= _PIN1 | _PIN4 | _PIN2; 
}
//------------------------------------------------------------------------------
