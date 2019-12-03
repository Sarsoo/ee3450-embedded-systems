#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
 
#include "MyDefines.h"
#include <TM4C123GH6PM.h>
 
void Delay(int s)
{
     volatile   int i, j;
    for (i = 0; i < s; i++)
        for (j = 0; j < 3180; j++){}
}
 
void SetupGPIO(void);
 
typedef struct STEPS{
				struct STEPS* prev;
        uint8_t pinout1;
				uint8_t pinout2;
				struct STEPS* next;
} STEPS;

#define CW  0
#define CCW 1
void Stepper(int s, bool direction);
 
STEPS step[]={
	{&step[3], _PIN0, _PIN1, &step[1]},
	{&step[2], _PIN1, _PIN2, &step[2]},
	{&step[1], _PIN2, _PIN3, &step[3]},
	{&step[0], _PIN3, _PIN0, &step[0]},
};
 
int main()
{
        int i = 0;
     
        SetupGPIO();
     
        while(1){
					
            for ( i = 0; i < 10; i++){
                    Stepper(10, CW);
                    Delay(10);
            }
						/*
            for (i = 0; i < 10; i++){
                    Stepper(10, CCW);
                Delay(10);             
            }*/
        }
         
}
 
void SetupGPIO(void)
{
    // GPIO Initialization and Configuration
    // 1. Enable Clock on GPIOs
    SYSCTL->RCGCGPIO |= _PORTD;
    // allow time for clock to stabilize
    while ((SYSCTL->PRGPIO & _PORTD) != _PORTD) {};
    // 2. Unlock PD7 and/or PF0 for TM4C 123G
    // 3. Config AMSEL to disable analog function
    // 4. Config PCTL to select 0-GPIO
    // 5. Set AFSEL bits to 0			
    // 6. Set DIR to 0 for input, 1 for output
		GPIOD -> DIR = _PIN0|_PIN1|_PIN2|_PIN3;
    // 7. Set PUR/PDR/ODR bits to 1 to enable internal pull-up/-down resistir and/or open-drain
    // 8. Set DEN bits to 1 to enable all
		GPIOD -> DEN = _PIN0|_PIN1|_PIN2|_PIN3;
 
}
void Stepper(int s, bool direction)
{
    int i;
    STEPS* p = step;
 
    for (i = 0; i< s; i++){
            if (direction == CW){
                // p pointed to next element of step array
								GPIOD -> DATA |= ((p->pinout1)|(p->pinout2));
								Delay(10);
                GPIOD -> DATA &= ~((p->pinout1)|(p->pinout2));
								p = p->next;
            } else {
                GPIOD -> DATA |= ((p->pinout1)|(p->pinout2));
								Delay(10);
                GPIOD -> DATA &= ~((p->pinout1)|(p->pinout2));
								p = p->prev;
                 
            }
            // Port = p->pinout;
         
    }
}
 