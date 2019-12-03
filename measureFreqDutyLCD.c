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

float SYSTEM_CLOCK_FREQ = ________;

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
uint32_t    *Sw = (uint32_t *) (((char*)_____)+ (______ << 2));

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
    ezLCD_Connect_DataPort(lcd, GPIO____, PIN_3_0);
    ezLCD_Connect_ENPin(lcd,    GPIO____, PIN7);
    ezLCD_Connect_RSPin(lcd,    GPIO____, PIN6);
    ezLCD_Connect_RWPin(lcd,    GPIO____, PIN4);

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
        freq = ____________ ;
        duty = ____________ ;
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
ENB

    // 1. Enable Clock for PWM Module
    SYSCTL->RCGCPWM |= (_______);
    while((SYSCTL->PRPWM &  (_______)) !=  (_______)){};
    // 2. Enable and Setup Clock Divider for PWM
    SYSCTL->RCC |= (1 << 20);               // RCC[20]=1:USEPWMDIV
    SYSCTL->RCC &= ~(0x07 << 17);           // RCC[19:17]=000 PWMDIV
    SYSCTL->RCC |= (________ << 17);       // RCC[19:17]=0x04 divider=/32
    // 3. Disable PWM Generator
    // 4. Configure LOAD (Period), CMP (Initial Duty), GEN (PWM Mode) values
    // 5. Enable PWM Generator
    // 6. Enable PWM Output

}
void Setup_Timer(void)
{
    // 1 . Enable Clock for TIMER
    SYSCTL->RCGCTIMER |= ________;
    while ((SYSCTL->PRTIMER & (________)) != (________)) {}
    // 2. Disable TIMER
    TIMER_->CTL &= ~(________);
    // 3. Configure TIMER
    TIMER_->CFG = _TIMER_CFG_1632B_TIMER16B;
    // 4. Configure Timer n Mode: GPTMTAMR
    TIMER_->TAMR = ________;
    // 5. Configure Timer Event Mode
    TIMER_->CTL &= ~________;
    TIMER_->CTL |=  ________;
    // 6. Configure Load
    TIMER_->TAILR = 0xffff; //0x00000000;
    //Set the prescaler to 0xff
    TIMER_->TAPR = 0xff;
    TIMER_->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;
    TIMER_->IMR = 0;
    // 7. Enable GPTM Timer
    TIMER_->CTL |= ________;  // Enable Timer

}
void Setup_GPIO(void)
{

    // GPIO Initialization and Configuration
    // 1. Enable Clock to the GPIO Modules (SYSCTL->RCGCGPIO)
    SYSCTL->RCGCGPIO |= (_______);
    // allow time for clock to stabilize (SYSCTL->PRGPIO)
    while((SYSCTL->PRGPIO & (_______)) != (_______)){};
    // 2. Unlock PD7 and/or PF0 on TM4C123G (GPIO->LOCK and GPIO->CR)
    //GPIOF->LOCK = 0x4C4F434B;
    //GPIOF->CR |= _PIN0;

    //GPIOD_AHB->LOCK = 0x4C4F434B;
    //while( (GPIOD_AHB->LOCK & 0x01) == 0x01) {};
    //*(((char*)GPIOD_AHB)+0x524) = 0xFF;

    // 3. GPIO Analog Mode Select (GPIOAMSEL)
    // 4. GPIO Port COntrol (GPIOPCTL)
    // 5. Clear AFSEL bits to 0 to select regular I/O
    // 6. GPIO Pin Direction (GPIODIR) 0 for input, 1 for output
    // 7. Set PUR bits to 1 to enable internal pull-up resistor
    // 8. Set DEN bits to 1 to enable data pins

}

uint32_t MeasurePeriod(void)
{
    volatile uint32_t        edge1;
    volatile uint32_t        edge2;
    uint32_t        load;

    // **** Capture firstEgde i.e. rising edge ****
    TIMER_->CTL &= ~(_TIMERA_ENABLE);
    TIMER_->CTL &=  ~(_TIMERA_EVENT);
    TIMER_->CTL |=  ________;  // Capture on the falling- or rising- edgs
    TIMER_->CTL |= _TIMERA_ENABLE;

    //1. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register (CnERIS bit)
    TIMER_->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;
    //2. Waiting for capture event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER_->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};
    //3. Read the capture value from GPTMTAR (TIMER1->TAR) register
    edge1 = TIMER_->TAR;

    // **** Capture secondEgde i.e. falling edge ****
    //4. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
    TIMER_->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;

    //5. Waiting for capture falling edge event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER_->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};

    //6. Read the capture value from GPTMTAR (TIMERa->TAR) register
    edge2 = TIMER_->TAR;

    //7. Calculate deltaT = highEdge - lowEdge
    //   Note: the deltaT must large than zero, cannot be negative value




}
uint32_t MeasurePulseWidth(void)
{
    uint32_t        edge1;
    uint32_t        edge2;
    static uint32_t load;

    // **** Capture first Rising-Edge ****
    TIMER_->CTL &= ~(_TIMERA_ENABLE);
    TIMER_->CTL &= ~(_TIMERA_EVENT);
    TIMER_->CTL |= ________; // Capture on the Rising-Edge
    TIMER_->CTL |= _TIMERA_ENABLE;

    //1. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
    TIMER_->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;
    //2. Waiting for capture event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER_->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};
    //3. Read the capture value from GPTMTAR (TIMER1->TAR) register
    edge1 = TIMER_->TAR;

    TIMER_->CTL &=  ~(_TIMERA_EVENT);
    TIMER_->CTL |=  ________; // Capture on the Falling-Edge


    // **** Capture second Egde i.e. falling edge ****
    //4. Clear GPTM Timer A Capture Mode Event by writing 1 to
    //   corresponding bit on GPTMICR (TIMER1->ICR) register
    TIMER_->ICR = _TIMERA_CAPTURE_EVENT_INT_CLR;

    //5. Waiting for capture falling edge event by check the GPTM Raw Interrupt Status
    //   GPTMRIS (TIMER1->RIS) register
    while ((TIMER_->RIS & _TIMERA_CAPTURE_EVENT_INT) != _TIMERA_CAPTURE_EVENT_INT) {};

    //6. Read the capture value from GPTMTAR (TIMERa->TAR) register
    edge2 = TIMER_->TAR;

    //7. Calculate deltaT = highEdge - lowEdge
    //   Note: the deltaT must large than zero, cannot be negative value



}

