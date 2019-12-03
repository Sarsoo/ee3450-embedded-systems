#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "MyDefines.h"
 
#include "TM4C123GH6PM.h"
 
void SetupPWM(void);
void SetupGPIO(void);
void Delay(int t);
 
int main()
{
    SetupPWM();
		SetupGPIO();
	
		uint32_t *sw1 = (uint32_t *)(((char*)GPIOF) + (_PIN4 << 2));
		uint32_t *sw2 = (uint32_t *)(((char*)GPIOF) + (_PIN0 << 2));
	
		uint32_t *cwturn = (uint32_t *)(((char*)GPIOA) + (_PIN3 << 2));
		uint32_t *acwturn = (uint32_t *)(((char*)GPIOA) + (_PIN2 << 2));
		*cwturn = 0xFF;
		*acwturn = 0x00;
		
		bool countup = true;
		bool turn_clockwise = true;
	
		*cwturn = 0xFF;
		*acwturn = 0x00;
		PWM0 -> ENABLE |= _PWM3;
		PWM1 -> ENABLE |= _PWM7;
		double cmp_percent = 0.9;	//Start at 10% Duty Cycle
		
		bool sw1currstate;
		bool sw1prevstate = false;
		
		bool sw2currstate;
		bool sw2prevstate = false;
    
		while(1){
				
				if(!(*sw1)) sw1currstate = true;
					else sw1currstate = false;
				if(!(*sw2)) sw2currstate = true;
					else sw2currstate = false;
			
				//DUTY CYCLE CHANGE
        if(!sw1currstate && sw1prevstate){	//Falling Edge Triggered
					if(cmp_percent == 1){ //count down at 100% duty cycle
						countup = false;
					}
					if(cmp_percent == 0){ //count up at 0% duty cycle
						countup = true;
					}
					if(countup == true){	//add 10% duty cycle
						cmp_percent -= 0.1;
					}else{								//remove 10% duty cycle
						cmp_percent += 0.1;
					}
					PWM1->_3_CMPB = 20000 - 20000*cmp_percent;
					PWM0->_1_CMPB = 20000 - 20000*cmp_percent;
				}
				
				//MOTOR DIRECTION CHANGE
				if(!sw2currstate && sw2prevstate){	//Falling Edge Triggered
					
					PWM0->_1_CMPB = 20000;	//Set Duty Cycle 0%
					PWM1->_3_CMPB = 20000;
					
					Delay(2000);	//Wait 2 Seconds
					
					if(turn_clockwise){
						*cwturn = 0x00;
						*acwturn = 0xFF;
						turn_clockwise = false;
					}else{
						*cwturn = 0xFF;
						*acwturn = 0x00;
						turn_clockwise = true;
					}
					PWM1->_3_CMPB = 20000*cmp_percent;	
					PWM0->_1_CMPB = 20000*cmp_percent;
				}
				sw1prevstate = sw1currstate;
				sw2prevstate = sw2currstate;
    }
}


void SetupGPIO(void){
	
		// Config for GPIO
    // 1. Enable Clock on GPIOF
    SYSCTL->RCGCGPIO = (_PORTA|_PORTB|_PORTF);
    // allow time for clock to stabilize
    while((SYSCTL->PRGPIO & (_PORTA|_PORTB|_PORTF)) != (_PORTA|_PORTB|_PORTF)){};
 
    // 2. Unlock GPIO
		GPIOF -> LOCK = 0x4C4F434B;
		GPIOF -> CR |= _PIN0;
    // 3. Clear AMSEL to disable analog
		//GPIOA -> AMSEL = 0x0;
		//GPIOB -> AMSEL = 0x0;	
		//GPIOF -> AMSEL = 0x0; 
    // 4. Config PCTL to select GPIO
		GPIOA -> PCTL = 0x0;
		GPIOB -> PCTL = 0x00400000;	
		GPIOF -> PCTL = 0x00005000;
    // 5. Set DIR to 0 for input, 1 for output
		GPIOA -> DIR |= _PIN2|_PIN3;
		GPIOB -> DIR |= _PIN5;
		GPIOF -> DIR |= _PIN3;
    // 6. Enable AFSEL bits to 1
		GPIOA -> AFSEL = 0x0;
		GPIOB -> AFSEL = _PIN5;
		GPIOF -> AFSEL = _PIN3;
    // 7. Set PUE bits to 1 to enable internal pull-up
		GPIOF -> PUR = _PIN0|_PIN4;
    // 8. Set DEN bits to 1 to enable data pins
		GPIOA -> DEN |= _PIN2|_PIN3;
		GPIOB -> DEN |= _PIN5;
		GPIOF -> DEN |= _PIN0|_PIN3|_PIN4;
}


 
void SetupPWM(void)
{
		int load = 20000;
    // 1. Enable Clock for PWM Module 1
    SYSCTL->RCGCPWM |= (_PWM_MODULE0|_PWM_MODULE1);
    while((SYSCTL->PRPWM & (_PWM_MODULE0|_PWM_MODULE1))==0){};
    
		// 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (1 << 20);       // RCC[20]=1:USEPWMDIV
    SYSCTL->RCC &= ~0x000E0000;     // RCC[19:17]=000 PWMDIV
		SYSCTL->RCC |= (_PWMDIV_4 << 17);    // RCC[19:17]=0x04 divider=/32	
    
		// 3. Disable PWM Generator 2
    PWM0->_1_CTL &= ~0x01; // Disable PWM Generator 2
		PWM1->_3_CTL &= ~0x01; // Disable PWM Generator 2
    
		// 4. Config LOAD, CMPn, GENn values
		PWM0->_1_LOAD = load;	// M0 GEN 1 B PWM 3 PB5 MOTOR
    PWM0->_1_CMPB = (load - load*0.1);
    PWM0->_1_GENB = (0x02 << 10 ) | (0x03 <<2);//0x080C	
			
		PWM1->_3_LOAD = load;	// M1 GEN 3 B PWM 7 PF3 LED
    PWM1->_3_CMPB = (load - load*0.1);
    PWM1->_3_GENB = (0x02 << 10 ) | (0x03 <<2);//0x080C	
		
		// 5. Enable PWM Generator 2
		PWM0->_1_CTL |= 0x01;
		PWM1->_3_CTL |= 0x01;
    
		// 6. Enable PWM5 Output
		
}

void Delay(int t)
{
    volatile int i, j;
    for (i = 0; i < t; i++)
        for (j = 0; j < 3180; j++)
            {};
}