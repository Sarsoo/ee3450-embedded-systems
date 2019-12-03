#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <TM4C123GH6PM.h>       // The Header File for EK-TM4C123GXL LaunchPad
// #include <TM4C1294NCPDT.h>   // The Header File for EK-TM4C1294XL LaunchPad
#include "ez123G.h"
#include "MyDefines.h"          // Your Definitions for the System Configuration

#define _TIMER_CFG_1632B_TIMER16B           (0x4)
#define _TIMERA_CAPTURE_EVENT_INT_CLR       (1 << 2)
#define _TIMERA_CAPTURE_EVENT_INT           (1 << 2)
#define _TIMERB_CAPTURE_EVENT_INT_CLR       (1 << 10)
#define _TIMERB_CAPTURE_EVENT_INT           (1 << 10)

float SYSTEM_CLOCK_FREQ = 80E6;

typedef enum EDGE_EVENT{
        FALLING_EDGE,
        RISING_EDGE

} EDGE_EVENT;

void        Setup_PWM(void);
void        Setup_Timer(void);
void        Setup_GPIO(void);
void        Setup_UART(void);

uint32_t    MeasurePeriod(void);
uint32_t    MeasurePulseWidth(void);
void        UART_PrintString(char * s);

typedef struct PWM_CONTROL{
    uint16_t    load;
    uint16_t    cmp;
} PWM_CONTROL;

PWM_CONTROL Pwm[]={
                   {18150,  4628},
                   {6665,   3032},
                   {12490,  8118},
                   {49996,  37497},
                   {8328,   2914}
};

// On-board switch
uint32_t    *Sw = (uint32_t *) (((char*)GPIOF)+ (_PIN4 << 2));

int main()
{
    PEZOBJ_LCD          lcd;
    volatile uint32_t   T;
    volatile uint32_t   t;
    uint32_t            freq;
    double              duty;
    char                str[100];
    int                 idx = 0;
    uint8_t             i = 0;
    bool                preSw = true;
    uint16_t            currSw;

    Setup_123G_80MHz();     // Setup System Clock to 80 MHz

    Setup_PWM();
    Setup_Timer();
    Setup_GPIO();

    lcd = ezLCD_Create();
    ezLCD_Connect_DataPort(lcd, GPIOD, PIN_3_0);
    ezLCD_Connect_ENPin(lcd,    GPIOE, PIN1);
    ezLCD_Connect_RSPin(lcd,    GPIOE, PIN2);
    ezLCD_Connect_RWPin(lcd,    GPIOE, PIN3);

    ezLCD_Start(lcd);
    ezLCD_ClearDisplay(lcd);

    PWM0->_1_CTL &= ~0x01; // Disable PWM Module 0, Generator 1
    PWM1->_3_CTL &= ~0x01; // Disable PWM Module 1, Generator 3
    PWM0->_1_LOAD = Pwm[idx].load;
    PWM0->_1_CMPB = Pwm[idx].cmp;
    PWM1->_3_LOAD = Pwm[idx].load;
    PWM1->_3_CMPB = Pwm[idx].cmp;
    PWM0->_1_CTL |= 0x01; // Disable PWM Module 0, Generator 1
    PWM1->_3_CTL |= 0x01; // Disable PWM Module 1, Generator 3

    while(1){

        currSw = *Sw;
        if (preSw && (!currSw)){
            if (++idx >= (sizeof(Pwm) / sizeof(PWM_CONTROL))) idx = 0;
            PWM0->_1_CTL &= ~0x01; // Disable PWM Module 0, Generator 1
            PWM1->_3_CTL &= ~0x01; // Disable PWM Module 1, Generator 3
            PWM0->_1_LOAD = Pwm[idx].load;
            PWM0->_1_CMPB = Pwm[idx].cmp;
            PWM1->_3_LOAD = Pwm[idx].load;
            PWM1->_3_CMPB = Pwm[idx].cmp;
            PWM0->_1_CTL |= 0x01; // Disable PWM Module 0, Generator 1
            PWM1->_3_CTL |= 0x01; // Disable PWM Module 1, Generator 3
            timer_waitMillis(100);
        }
        preSw = currSw;

        T = MeasurePeriod();
        t = MeasurePulseWidth();
        freq = 1 / (T * (1.25E-8));
        duty = (t * 100) / T ;
        sprintf(str, "%02d: f%d = %d Hz  ", i, idx, freq);
        ezLCD_Position(lcd, 0,0);
        ezLCD_PrintString(lcd, str);
        sprintf(str, "Duty = %.1f %%  ", duty);
        ezLCD_Position(lcd, 1,0);
        ezLCD_PrintString(lcd, str);
        i = (++i) % 100;
        timer_waitMillis(500);
    }

}

void Setup_PWM(void)
{
    //PF1 M1PWM5 GEN 2
		//PF3 M1PWM7 GEN 3
		int load = 8;
    // 1. Enable Clock for PWM Module 1
    SYSCTL->RCGCPWM |= _PWM_MODULE1;
    while((SYSCTL->PRPWM & _PWM_MODULE1)!= _PWM_MODULE1){};
    // 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (0 << 20);       // RCC[20]=1:USEPWMDIV
    //SYSCTL->RCC &= ~0x000E0000;     // RCC[19:17]=000 PWMDIV
		//SYSCTL->RCC |= (_PWMDIV_2 << 17);    // RCC[19:17]=0x04 divider=/32	
    // 3. Disable PWM Generator 2
		PWM1->_2_CTL &= ~0x01; // Disable PWM Generator 3
		PWM1->_3_CTL &= ~0x01; // Disable PWM Generator 3
    // 4. Config LOAD, CMPn, GENn values
		PWM1->_2_LOAD = load;	// GEN 2 B PWM 5 PF1 LED
    PWM1->_2_CMPB = load/2;
    PWM1->_2_GENB = _PWM_RIGHT_ALIG_CMPBD;//0x080C		
			
		PWM1->_3_LOAD = load;	// GEN 3 B PWM 7 PF3 PWM OUT
    PWM1->_3_CMPB = load/2;
    PWM1->_3_GENB = _PWM_RIGHT_ALIG_CMPBD;//0x080C	
		// 5. Enable PWM Generator 2
		PWM1->_3_CTL |= 0x01;
		PWM1->_3_CTL |= 0x01;
    // 6. Enable PWM5 Output
		PWM1 -> ENABLE |= _PWM5;
		PWM1 -> ENABLE |= _PWM7;

}
void Setup_Timer(void)
{
    // 1 . Enable Clock for TIMER
    SYSCTL->RCGCTIMER |= _TIMER1;
    while ((SYSCTL->PRTIMER & (_TIMER1)) != (_TIMER1)) {}
    // 2. Disable TIMER
    TIMER1->CTL &= ~(_TIMERA_ENABLE);
    // 3. Configure TIMER
    TIMER1->CFG = _TIMER_CFG_1632B_TIMER16B;
    // 4. Configure Timer n Mode: GPTMTAMR
    TIMER1->TAMR = (_TIMERA_COUNTDOWN|_TIMERA_EDGE_TIME|_TIMERA_CAPTURE);
    // 5. Configure Timer Event Mode
    TIMER1->CTL &= ~(0x03 << 2);
    TIMER1->CTL |=  _TIMERA_BOTH_EDGES;
    // 6. Configure Load
    TIMER1->TAILR = 0xffff; //0x00000000;
    //Set the prescaler to 0xff
    TIMER1->TAPR = 0xff;
    TIMER1->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;
    TIMER1->IMR = 0;
    // 7. Enable GPTM Timer
    TIMER1->CTL |= _TIMERA_ENABLE;  // Enable Timer

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
    // 4. GPIO Port Control (GPIOPCTL)
		GPIOD -> PCTL = 0x0;
		GPIOE -> PCTL = 0x0;
		GPIOF -> PCTL = 0x00005750;
    // 5. Clear AFSEL bits to 0 to select regular I/O
		GPIOD -> AFSEL = 0x0;
		GPIOE -> AFSEL = 0x0;	
		GPIOF -> AFSEL |= _PIN1|_PIN2|_PIN3;
		// 6. GPIO Pin Direction (GPIODIR) 0 for input, 1 for output
		GPIOD -> DIR |= _PIN0|_PIN1|_PIN2|_PIN3; 
		GPIOE -> DIR |= _PIN1|_PIN2|_PIN3; 
		GPIOF -> DIR |= _PIN1|_PIN3; 
    // 7. Set PUR bits to 1 to enable internal pull-up resistor
		GPIOF -> PUR |= _PIN4;	
		// 8. Set DEN bits to 1 to enable data pins
		GPIOD -> DEN |= _PIN0|_PIN1|_PIN2|_PIN3; 	
		GPIOE -> DEN |= _PIN1|_PIN2|_PIN3; 
		GPIOF -> DEN |= _PIN1|_PIN2|_PIN3|_PIN4; 
}

uint32_t MeasurePeriod(void)
{
    volatile uint32_t        edge1;
    volatile uint32_t        edge2;
    uint32_t        load;

    // **** Capture firstEgde i.e. rising edge ****
    TIMER1->CTL &= ~(_TIMERA_ENABLE);
    TIMER1->CTL &=  ~(0x03);
    TIMER1->CTL |=  _TIMERA_POSITIVE_EDGE;  // Capture on the falling- or rising- edgs
    TIMER1->CTL |= _TIMERA_ENABLE;

    //1. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register (CnERIS bit)
    TIMER1->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;
    //2. Waiting for capture event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER1->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};
    //3. Read the capture value from GPTMTAR (TIMER1->TAR) register
    edge1 = TIMER1->TAR;

    // **** Capture secondEgde i.e. falling edge ****
    //4. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
    TIMER1->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;

    //5. Waiting for capture falling edge event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER1->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};

    //6. Read the capture value from GPTMTAR (TIMERa->TAR) register
    edge2 = TIMER1->TAR;

    //7. Calculate deltaT = highEdge - lowEdge
    //   Note: the deltaT must large than zero, cannot be negative value
		if(edge1 > edge2){
				return edge1 - edge2;
		}else{
				return (0xFFFFFFFF - (edge2 - edge1));
		}
}

uint32_t MeasurePulseWidth(void)
{
    uint32_t        edge1;
    uint32_t        edge2;
    static uint32_t load;

    // **** Capture first Rising-Edge ****
    TIMER1->CTL &= ~(_TIMERA_ENABLE);
    TIMER1->CTL &= ~(0x03);
    TIMER1->CTL |= _TIMERA_POSITIVE_EDGE; // Capture on the Rising-Edge
    TIMER1->CTL |= _TIMERA_ENABLE;

    //1. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
    TIMER1->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;
    //2. Waiting for capture event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER1->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};
    //3. Read the capture value from GPTMTAR (TIMER1->TAR) register
    edge1 = TIMER1->TAR;

    TIMER1->CTL &=  ~(0x03);
    TIMER1->CTL |=  _TIMERA_NEGATIVE_EDGE; // Capture on the Falling-Edge


    // **** Capture second Egde i.e. falling edge ****
    //4. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
    TIMER1->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;

    //5. Waiting for capture falling edge event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER1->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};

    //6. Read the capture value from GPTMTAR (TIMERa->TAR) register
    edge2 = TIMER1->TAR;

    //7. Calculate deltaT = highEdge - lowEdge
    //   Note: the deltaT must large than zero, cannot be negative value
		if(edge1 > edge2){
				return edge1 - edge2;
		}else{
				return (0xFFFFFFFF - (edge2 - edge1));
		}


}

