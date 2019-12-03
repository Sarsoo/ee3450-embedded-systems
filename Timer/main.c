#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
 
#include "TM4C123GH6PM.h"
#include "MyDefines.h"
#include "ez123G.h"
void initGPIO(void);
void initTimer(void);

uint32_t deltaT;

void UltraSonic();
uint32_t MeasureD();
 
int main(void)
{			
		Setup_123G_80MHz(); 	//Setup System Clock to 80MHz
		initTimer();
		initGPIO();
		
		for(;;){
		
		}
}

// trig->pf3 for 123G 
uint32_t *pf3;
void UltraSonic(){
	uint32_t pulseWidth;
	//Set "Trig" pin to low for 500ms
	*pf3 = 00;
	timer_waitMillis(500);	//Waiting for 500 ms
	//Set "trig" pin to high for  10 us
	*pf3 = _PIN3; //*pf3 = 0xFF
	timer_waitMicros(10);
	//Call MeasureD()
	pulseWidth = MeasureD();
	//Calculate deltaT
	
	//Calculate distance in cm
}

uint32_t MeasureD(){
	uint32_t highEdge, lowEdge;
	uint32_t pulseCounter;
	
	//Capture first edge -- rising edge
	
	//Clear TimerA Capture mode event by clearing bit on ICR register
	TIMER1->ICR |= (1 << 2); //CAECINT
	//Waiting for capture signal by check RIS register
	while ((TIMER1->RIS & (1<<2)) != (1<<2)){}
	//read the high Edge from the TAR register
	highEdge = (TIMER1->TAR) | (TIMER1->TAPS << 16);
		
	//capture second edge -- falling edge
	//Clear TimerA Capture mode event by clearing bit on ICR register
	TIMER1->ICR |= (1 << 2); //CAECINT
	//Waiting for capture signal by check RIS register
	while ((TIMER1->RIS & (1<<2)) != (1<<2)){}
	//read the lowEdge from the TAR register
	lowEdge = (TIMER1->TAR) | (TIMER1->TAPS << 16);
	if(highEdge > lowEdge)
		deltaT = highEdge - lowEdge;
	else
		deltaT = highEdge + (0xFFFFFF)- lowEdge;
	
	return pulseCounter;
}

void initTimer(void){
	//Enable Clock On Timer Module
	SYSCTL->RCGCTIMER |= _TIMER1;
	while((SYSCTL->PRTIMER & (_TIMER1)) != (_TIMER1)){}
	//Disable Timer A/B
	TIMER1->CTL &= ~_TIMERA_ENABLE;
	//Configure Timer
	TIMER1->CFG = 0x04;	//Split into two 16-bit timer
	//Configure Timer A Mode
	TIMER1->TAMR = _TIMERA_COUNTDOWN | _TIMERA_CAPTURE | _TIMERA_EDGE_TIME;
	//Configure Timer Event Mode
	TIMER1->CTL &= ~(0x03 << 2);
	TIMER1->CTL |= _TIMERA_BOTH_EDGES;
	//Config Load
	TIMER1->TAILR = 0xffff;
	//Set the prescaler to 15
	TIMER1->TAPR = 0xff;
	TIMER1->IMR = 0; // IMR = interrupt mask = 0, disabled all timer IRQ
	//Enable TImer A
	TIMER1->CTL |= _TIMERA_ENABLE;
}
void initGPIO(void){
	
	  // GPIO Configuration
    // 1. Enable Clock on GPIOF[3:1] (SYSCTL_RCGCGPIO)
    SYSCTL->RCGCGPIO = 0x20;
    // allow time for clock to stabilize (SYSCTL_PRGPIO)
    while((SYSCTL->PRGPIO & 0x20) != 0x20){};
    // 2. Unlock GPIOC[3:0], GIOD[7] and GPIOF[0] on TM4C123G (GPIOn->LOCK and GPIOn->CR) NOT USING
		GPIOF->LOCK = 0x4C4F434B;
		GPIOF->CR |= 0x01;
    // 3. Clear AMSEL to disable analog
		GPIOF->AMSEL = 0x00;
    // 4. Config PCTL to select GPIO
		GPIOF->PCTL = 0x0000;
		//or
		//GPIOF->PCTL &= 0xFFF00FFF;
    // 5. Set DIR to 0 for input, 1 for output
    GPIOF->DIR = 0x0E; // PF3,PF2,PF1 for Output
    // 6. Enable AFSEL bits to 1
		GPIOF->AFSEL = 0x00;
    // 7. Set PUR bits to 1 to enable internal pull-up
		GPIOF->PUR = 0x11;
    // 8. Set DEN bits to 1 to enable data pins
    GPIOF->DEN = 0x1F; // Enable digital pin on PF3,PF2,PF1
}
