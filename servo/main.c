#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "MyDefines.h"
 
#include "TM4C123GH6PM.h"
 
void SetupPWM(void);
void SetupGPIO(void);

void Delay(int s)
{
		volatile int i, j;
	for (i = 0; i < s; i++){
		for(j = 0; j < 3180; j++){}}
}

#define CMP_0_5MS 19500
#define CMP_1_5MS 18500
#define CMP_2_5MS 17500
#define TO_RIGHT 0
int main()
{
	uint16_t cmp = CMP_1_5MS;
	bool	dir = TO_RIGHT;
	SetupPWM();
	SetupGPIO();
	
	while(1){
		if(dir == TO_RIGHT){
			cmp += 10;
			if( cmp >= CMP_0_5MS){
				cmp = CMP_0_5MS;
				dir = !dir;
			}
		}else{
				cmp -= 10;
				if(cmp <= CMP_2_5MS){
					cmp = CMP_2_5MS;
					dir = !dir;
				}
			}
		PWM1->_3_CMPB = cmp;
		Delay(10);	
	}
}

void SetupGPIO(){
		// Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = _PORTF;
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & _PORTF) != _PORTF){};
    // 2. Unlock GPIO
    // 3. Clear AMSEL to disable analog
		GPIOF -> AMSEL = 0x0; 
    // 4. Config PCTL to select GPIO
		GPIOF -> PCTL = 0x00005000;
    // 5. Set DIR to 0 for input, 1 for output
		GPIOF -> DIR = _PIN3;
    // 6. Enable AFSEL bits to 1
		GPIOF -> AFSEL = _PIN3;
    // 7. Set PUE bits to 1 to enable internal pull-up
    // 8. Set DEN bits to 1 to enable data pins
		GPIOF -> DEN = _PIN3;
}

void SetupPWM(void)
{
    // 1. Enable Clock for PWM Module 1
    SYSCTL->RCGCPWM |= _PWM_MODULE1;
    while((SYSCTL->PRPWM & _PWM_MODULE1)==0){};
    // 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (1 << 20);       // RCC[20]=1:USEPWMDIV
    SYSCTL->RCC &= ~0x000E0000;     // RCC[19:17]=000 PWMDIV
		SYSCTL->RCC |= (_PWMDIV_16 << 17);    // RCC[19:17]=0x04 divider=/32	
    // 3. Disable PWM Generator 2
    PWM1->_3_CTL &= ~0x01; // Disable PWM Generator 2
    // 4. Config LOAD, CMPn, GENn values
		PWM1->_3_LOAD = 20000;	// GEN 3 B PWM 7 PF3 MOTOR
    PWM1->_3_CMPB = 18500;
    PWM1->_3_GENB = (0x02 << 10 ) | (0x03 <<2);//0x080C	
		// 5. Enable PWM Generator 3
    PWM1->_3_CTL |= 0x01;
    // 6. Enable PWM7 Output
		PWM1 -> ENABLE |= _PWM7;
}