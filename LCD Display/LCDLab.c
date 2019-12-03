#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
  
#include "TM4C123GH6PM.h"
#include "ez123G.h"
 
char str[100];
 
int main(void)
{
    PEZOBJ_LCD  lcd; 
    uint16_t           i = 0;
		int array[]= {10, 35, 65, 43, 32, 86, 44,  9, 18};
		
    // Enable GPIOD[3:0] and GPIOE[3:1]
    // Config for GPIOD and GPIOE
    // 1. Enable Clock on GPIOs
    SYSCTL->RCGCGPIO |= 0x18;
    // allow time for clock to stabilize
    while ((SYSCTL->PRGPIO & 0x18) != 0x18) {};
    // 2. Unlock PD7
    GPIOD->LOCK = 0x4C4F434B;
		GPIOD->CR |= 0x80; 
     
    // 3. Config AMSEL to disable analog function
		GPIOD	-> AMSEL = 0x00;
		GPIOE -> AMSEL = 0x00;
    // 4. Config PCTL to select GPIO
		GPIOD	-> PCTL = 0x00;
		GPIOE -> PCTL = 0x00;	
    // 5. Set DIR to 0 for input, 1 for output
		GPIOD	-> DIR |= 0x0F;
		GPIOE -> DIR |= 0x0E;
 
    // 6. Set AFSEL bits to 0
		GPIOD	-> AFSEL = 0x00;
		GPIOE -> AFSEL = 0x00;
    // 7. Set PUE bits to 1 to enable internal pull-up (Skipped)
    // 8. Set DEN bits to 1 to enable data pins
		GPIOD	-> DEN |= 0x0F;
		GPIOE -> DEN |= 0x0E;
 
     
    lcd = ezLCD_Create();
    ezLCD_Connect_DataPort(lcd, GPIOD, PIN_3_0);
    ezLCD_Connect_ENPin(lcd, GPIOE, PIN1);
    ezLCD_Connect_RSPin(lcd, GPIOE, PIN2);
    ezLCD_Connect_RWPin(lcd, GPIOE, PIN3);
     
    ezLCD_Start(lcd);
    ezLCD_LoadVerticalBargraphFonts(lcd);
     
    ezLCD_ClearDisplay(lcd);
    
		int j = 0;
		float a;
		int s = 0, M = array[0], m = array[0];
		int total = 0;
		while(j <= 8){
			ezLCD_ClearDisplay(lcd);
			s += array[j];
			if(array[j] > M) M = array[j];
			if(array[j] < m) m = array[j];
			a = s / (j + 1);
			ezLCD_Position(lcd, 0, 0);
			sprintf(str, "S=%d", s);
			ezLCD_PrintString(lcd, str);
			ezLCD_Position(lcd, 0, 8);
			sprintf(str, "M=%d", M);
			ezLCD_PrintString(lcd, str);
			ezLCD_Position(lcd, 1, 0);
			sprintf(str, "A=%.2f", a);
			ezLCD_PrintString(lcd, str);
			ezLCD_Position(lcd, 1, 8);
			sprintf(str, "m=%d", m);
			ezLCD_PrintString(lcd, str);
			timer_waitMillis(500);
			j++;
		}
}