#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
 
#include "TM4C123GH6PM.h"
void DelayMs(int s);
void initGPIO(void);

int main(void)
{ 
		initGPIO();
		
		int numbers[16] = {
			0x3F,
			0x06,
			0x5B,
			0x4F,
			0x66,
			0x6D,
			0x7D,
			0x07,
			0x7F,
			0x6F,
			0x77,
			0x7C,
			0x39,
			0x5E,
			0x79,
			0x71
		};
		
		int a = 0;
		GPIOA -> DATA = (numbers[a] << 2) & 0x1C;
		GPIOC -> DATA = (numbers[a] << 1) & 0xF0;
		
		bool currSW2state; 
		bool prevSW2state = 0; 

		for(;;){
			
			currSW2state = !(GPIOF -> DATA & 0x01); 	
			
			if(currSW2state && !prevSW2state){
				a++;
				if(a == 16) a = 0;
				GPIOA -> DATA = (numbers[a] << 2) & 0x1C;
				GPIOC -> DATA = (numbers[a] << 1) & 0xF0;
			}
			
			if(!(GPIOF -> DATA & 0x10)){ 
				a = 0;
				GPIOA -> DATA = (numbers[a] << 2) & 0x1C;
				GPIOC -> DATA = (numbers[a] << 1) & 0xF0;
			}
			
			prevSW2state = currSW2state;
		
		}
}

void initGPIO(){
	// GPIO Configuration
    // 1. Enable Clock on Port F (SYSCTL_RCGCGPIO)
    SYSCTL->RCGCGPIO = 0x2D;
    // allow time for clock to stabilize (SYSCTL_PRGPIO)
    while((SYSCTL->PRGPIO & 0x2D) != 0x2D){};
    // 2. Unlock GIOD[7] and GPIOF[0] on TM4C123G (GPIOn->LOCK and GPIOn->CR)
    GPIOF->LOCK = 0x4C4F434B;
		GPIOD->LOCK = 0x4C4F434B;
    GPIOF->CR |= 0x01;
	  GPIOD->CR |= 0x80;
    // 3. Clear AMSEL to disable analog
    //GPIOF->AMSEL = 0x00;
			//GPIOC->AMSEL = 0x00;
			//GPIOA->AMSEL = 0x00;
			//GPIOD->AMSEL = 0x00;
    // 4. Config PCTL to select GPIO
    //GPIOF->PCTL = 0x00;
			//GPIOA->PCTL = 0x00;
			//GPIOC->PCTL = 0x00;
			//GPIOD->PCTL = 0x00;
    // 5. Set DIR to 0 for input, 1 for output
		GPIOA->DIR |= 0x1C;
		GPIOC->DIR |= 0xF0;
		GPIOD->DIR |= 0x80;
		GPIOF->DIR = 0x0E; // PF3,PF2,PF1 for Output
		// 6. Clear AFSEL bits to 0 to select regular I/O
    //GPIOF->AFSEL = 0x00;
		//GPIOA->AFSEL = 0x00;
		//GPIOC->AFSEL = 0x00;
		//GPIOD->AFSEL = 0x00;
    // 7. Set PUR bits to 1 to enable internal pull-up resistor
    GPIOF->PUR = 0x11;
    // 8. Set DEN bits to 1 to enable data pins
		GPIOA->DEN = 0x1C;
		GPIOC->DEN |= 0xF0;
		GPIOD->DEN = 0x80;
		GPIOF->DEN = 0x1F; // Enable all digital pins on PortF (PF3,PF2,PF1,PF0)
}
