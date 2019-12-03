#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
 
#include "TM4C123GH6PM.h"
#include "ez123G.h"
#include "MyDefines.h"

char str[100]; 

void Setup_GPIO(void);
uint32_t getWidth();
uint32_t measureD(void);
void Setup_Timer(void);

int main(void)
{
    uint16_t    i = 0;
	
		Setup_123G_80MHz();
		Setup_Timer();
		Setup_GPIO();
	
    PEZOBJ_LCD  lcd;
		lcd = ezLCD_Create();
    ezLCD_Connect_DataPort(lcd, GPIOD, PIN_3_0);
    ezLCD_Connect_ENPin(lcd, GPIOE, PIN1);
    ezLCD_Connect_RSPin(lcd, GPIOE, PIN2);
    ezLCD_Connect_RWPin(lcd, GPIOE, PIN3);
	
		double 			time;
		double      distance;
    uint32_t widthArray[3];
		uint32_t avgWidth;
    ezLCD_Start(lcd);
    ezLCD_LoadVerticalBargraphFonts(lcd);
		while(1){
				
				widthArray[0] = getWidth();
				//widthArray[1] = getWidth();
				//widthArray[2] = getWidth();
				//avgWidth = (widthArray[0]+widthArray[1]+widthArray[2])/3;
        time = widthArray[0] * (1.25E-8);
				distance = time * (34000/2);
				
				ezLCD_Position(lcd, 0, 0);
        sprintf(str,"%d: width: %2d", ++i, widthArray[0]);
				ezLCD_PrintString(lcd, str);
				ezLCD_Position(lcd, 1, 0);
				sprintf(str,"dist(cm): %6.2lf cm \n\r", distance);
				ezLCD_PrintString(lcd, str);
				//timer_waitMillis(100);
    }
		
}

uint32_t getWidth()
{
				uint32_t    pulseWidth = 0;
				uint32_t *trig = (uint32_t *)(((char*)GPIOF) + (_PIN3 << 2));
				// Set "Trig" pin to Low
				*trig = 0x00;
        timer_waitMillis(500);      // Waiting for 500ms
        // Set "Trig" pin to High for 10ns
				*trig = 0xFF;
        timer_waitMicros(10);       // Waiting for 10ns
        // Set "Trig" pin to Low
				*trig = 0x00;
        pulseWidth = measureD();    // Call measureD() to get the delta t on the "echo" pin
        // Calculate distance
				
				return pulseWidth;
}

uint32_t measureD(void)
{
    uint32_t highEdge,lowEdge;
    uint32_t deltaT;
 
    /*Capture firstEgde i.e. rising edge*/
    //1. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
		TIMER1 -> ICR = 1 << 2;
    //2. Waiting for capture rising edge event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
		while((TIMER1 -> RIS & (1 << 2)) != (1 << 2)){}
    //3. Read the highEdge from GPTMTAR (TIMER1->TAR) registers
		highEdge = TIMER1 -> TAR;
    /*Capture secondEgde i.e. falling edge*/
    //4. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
		TIMER1 -> ICR = 1 << 2;
    //5. Waiting for capture falling edge event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
		while((TIMER1 -> RIS & (1 << 2)) != (1 << 2)){}
    //6. Read the lowEdge from GPTMTAR (TIMERa->TAR) registers
		lowEdge = TIMER1 -> TAR;
    //7. Calculate deltaT = highEdge - lowEdge
    //   Note: the deltaT must large than zero, cannot be negative value
		if(highEdge > lowEdge){
				deltaT = highEdge - lowEdge;
		}else{
				deltaT = 0xFFFFFFFF - (lowEdge - highEdge);
		}
    return deltaT;
}

void Setup_Timer(void)
{
    //* PF2: T1CCP0 (PCTL=07) both edge (edge-time mode)
 
    // 1 . Enable Clock for TIMER Module
    SYSCTL->RCGCTIMER |= (_TIMER1);
    while ((SYSCTL->PRTIMER & (_TIMER1)) != (_TIMER1)) {}
    // 2. Disable TIMER
    TIMER1->CTL &= ~(_TIMERA_ENABLE);  // Disable TimerA & TimerB
    // 3. Configure TIMER
    TIMER1->CFG = 0x04;         // Split into two 16-bit timers
    // 4. Configure Timer n Mode: GPTMTAMR
    TIMER1->TAMR = (_TIMERA_COUNTDOWN|_TIMERA_EDGE_TIME|_TIMERA_CAPTURE);
    // 5. Configure Timer Event Mode: rising-, falling-, or both-edges
    TIMER1->CTL &= ~(0x03 << 2);
    TIMER1->CTL |=  (_TIMERA_BOTH_EDGES);  // Both edges
    // 6. Configure Load
    TIMER1->TAILR = 0xFFFF;
    //Set the prescaler to 0xFF
    TIMER1->TAPR = 0xFF;
    TIMER1->IMR = 0;
    // 7. Enable GPTM Timer
    TIMER1->CTL |= _TIMERA_ENABLE;  // Enable TimerB
}

void Setup_GPIO(void)
{
 
    // GPIO Initialization and Configuration
    // 1. Enable Clock to the GPIO Modules (SYSCTL->RCGCGPIO)
    SYSCTL->RCGCGPIO |= _PORTD|_PORTE|_PORTF;
    // allow time for clock to stabilize (SYSCTL->PRGPIO)
    while((SYSCTL->PRGPIO & (_PORTD|_PORTE|_PORTF)) != (_PORTD|_PORTE|_PORTF)){};
    // 2. Unlock PD7 and PF0 on TM4C123G; or PD7 on TM4C1294 (GPIO->LOCK and GPIO->CR)
    // 3. GPIO Analog Mode Select (GPIOAMSEL)
		GPIOD -> AMSEL = 0x0;
		GPIOE -> AMSEL = 0x0;	
		GPIOF -> AMSEL = 0x0;
    // 4. GPIO Port COntrol (GPIOPCTL)
		GPIOD -> PCTL = 0x0;
		GPIOE -> PCTL = 0x0;
		GPIOF -> PCTL = 0x00000700;
    // 5. Clear AFSEL bits to 0 to select regular I/O
		GPIOD -> AFSEL = 0x0;
		GPIOE -> AFSEL = 0x0;	
		GPIOF -> AFSEL |= _PIN2;
		// 6. GPIO Pin Direction (GPIODIR) 0 for input, 1 for output
		GPIOD -> DIR |= _PIN0|_PIN1|_PIN2|_PIN3; 
		GPIOE -> DIR |= _PIN1|_PIN2|_PIN3; 
		GPIOF -> DIR |= _PIN3; 
    // 7. Set PUR bits to 1 to enable internal pull-up resistor
    // 8. Set DEN bits to 1 to enable data pins
		GPIOD -> DEN |= _PIN0|_PIN1|_PIN2|_PIN3; 	
		GPIOE -> DEN |= _PIN1|_PIN2|_PIN3; 
		GPIOF -> DEN |= _PIN2|_PIN3; 
 
}