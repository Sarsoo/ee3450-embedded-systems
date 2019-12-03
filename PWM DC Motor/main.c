#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "MyDefines.h"
 
#include "TM4C123GH6PM.h"
 
void SetupPWM(void);
void SetupGPIO(void);
void SetupPWMDEFINE(void);
void SetupGPIODEFINE(void);
 
int main()
{
    SetupPWM();
		SetupGPIO();
 
		int cmp = 0;
		volatile int i;
	
    while(1){
        // Your Code Here
				//GPIOF -> DATA = 0x01;
			
				PWM1 -> _3_CMPB = cmp;
				PWM1 -> _2_CMPA = cmp++;
				
				if(cmp > 9999) cmp = 0;
			
				for(i = 0; i < 500; i++){}
    }
}

int mainDEFINE(){
	
	uint32_t *sw1 = (uint32_t *)(((char*)GPIOF) + (_PIN4 << 2));
	uint32_t *led = (uint32_t *)(((char*)GPIOF) + (_PIN2 << 2));
	
	SetupPWMDEFINE();
	SetupGPIODEFINE();
	
	while(1){
		if(!(*sw1)){
			*led = 0xFF;
		}else{
			*led = 0x00;
		}
	}
}

void SetupGPIODEFINE(void){
	
		// Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = _PORTF;
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & _PORTF) != _PORTF){};
 
    // 2. Unlock GPIO
		GPIOF -> LOCK = 0x4C4F434B;
		GPIOF -> CR |= 0x01;
    // 3. Clear AMSEL to disable analog
		GPIOF -> AMSEL = 0x0; 
    // 4. Config PCTL to select GPIO
		GPIOF -> PCTL = 0x5000;
    // 5. Set DIR to 0 for input, 1 for output
		GPIOF -> DIR = _PIN2 | _PIN3;
    // 6. Enable AFSEL bits to 1
		GPIOF -> AFSEL = _PIN3;
    // 7. Set PUE bits to 1 to enable internal pull-up
		GPIOF -> DATA = _PIN4;
    // 8. Set DEN bits to 1 to enable data pins
		GPIOF -> DEN = _PIN2 | _PIN3;
			
}

void SetupGPIO(void){
	
		// Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = 0x20;
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & 0x20) != 0x20){};
 
    // 2. Unlock GPIO
		GPIOF -> LOCK = 0x4C4F434B;
		GPIOF -> CR |= 0x01;
    // 3. Clear AMSEL to disable analog
		GPIOF -> AMSEL = 0x0; 
    // 4. Config PCTL to select GPIO
		GPIOF -> PCTL = 0x00005005;
    // 5. Set DIR to 0 for input, 1 for output
		GPIOF -> DIR = 0x09;
    // 6. Enable AFSEL bits to 1
		GPIOF -> AFSEL = 0x09;
    // 7. Set PUE bits to 1 to enable internal pull-up
    // 8. Set DEN bits to 1 to enable data pins
		GPIOF -> DEN = 0x09;
}

void SetupPWMDEFINE(void)
{
		int load = 40000;
    // 1. Enable Clock for PWM Module 1
    SYSCTL->RCGCPWM |= _PWM_MODULE1;
    while((SYSCTL->PRPWM & _PWM_MODULE1)!= _PWM_MODULE1){};
    // 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (1 << 20);       // RCC[20]=1:USEPWMDIV
    SYSCTL->RCC &= ~0x000E0000;     // RCC[19:17]=000 PWMDIV
		SYSCTL->RCC |= (_PWMDIV_2 << 17);    // RCC[19:17]=0x04 divider=/32	
    // 3. Disable PWM Generator 2
    PWM1->_2_CTL &= ~0x01; // Disable PWM Generator 2
		PWM1->_3_CTL &= ~0x01; // Disable PWM Generator 3
    // 4. Config LOAD, CMPn, GENn values
			
		PWM1->_2_LOAD = load;	// GEN 2 A PWM 4 PF0 MOTOR
    PWM1->_2_CMPA = load/2;
    PWM1->_2_GENA = _PWM_LEFT_ALIG_CMPBD;//0x080C
			
		PWM1->_3_LOAD = load;	// GEN 3 B PWM 7 PF3 LED
    PWM1->_3_CMPB = load/2;
    PWM1->_3_GENB = (0x02 << 10 ) | (0x03 <<2);//0x080C	
		// 5. Enable PWM Generator 2
    PWM1->_2_CTL |= 0x01;
		PWM1->_3_CTL |= 0x01;
    // 6. Enable PWM5 Output
    //PWM1->ENABLE |= 1 << 4; // Enable PWM4
		//PWM1->ENABLE |= 1 << 7; // Enable PWM7
		PWM1 -> ENABLE |= 0x90;
}
 
void SetupPWM(void)
{
		int load = 10000;
    // 1. Enable Clock for PWM Module 1
    SYSCTL->RCGCPWM |= 0x02;
    while((SYSCTL->PRPWM & 0x02)==0){};
    // 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (1 << 20);       // RCC[20]=1:USEPWMDIV
    SYSCTL->RCC &= ~0x000E0000;     // RCC[19:17]=000 PWMDIV
		SYSCTL->RCC |= (0x03 << 17);    // RCC[19:17]=0x04 divider=/32	
    // 3. Disable PWM Generator 2
    PWM1->_2_CTL &= ~0x01; // Disable PWM Generator 2
		PWM1->_3_CTL &= ~0x01; // Disable PWM Generator 3
    // 4. Config LOAD, CMPn, GENn values
			
		PWM1->_2_LOAD = load;	// GEN 2 A PWM 4 PF0 MOTOR
    PWM1->_2_CMPA = load/2;
    PWM1->_2_GENA = (0x02 << 6 ) | (0x03 <<2);//0x080C
			
		PWM1->_3_LOAD = load;	// GEN 3 B PWM 7 PF3 LED
    PWM1->_3_CMPB = load/2;
    PWM1->_3_GENB = (0x02 << 10 ) | (0x03 <<2);//0x080C	
		// 5. Enable PWM Generator 2
    PWM1->_2_CTL |= 0x01;
		PWM1->_3_CTL |= 0x01;
    // 6. Enable PWM5 Output
    //PWM1->ENABLE |= 1 << 4; // Enable PWM4
		//PWM1->ENABLE |= 1 << 7; // Enable PWM7
		PWM1 -> ENABLE |= 0x90;
}