# GROUP10
EMBEDDED PROJECT:GRID FREQUENCY MEASUREMENT

PROBLEM STATEMENT:
Designing a real-time grid frequency measurement system using the Tiva C Series microcontroller. This system should be capable of accurately measuring the frequency of an AC power grid of 50 Hz with minimal delay and high accuracy. 

SOLUTION:
1)	By using a LV25P sensor and zero-crossing detection circuit to convert the AC grid signal to a pulse train, which marks each cycle’s start and allows safe measurement. 
2)	By configuring the Tiva C microcontroller’s timer to measure the interval between zero-crossing pulses and calculate frequency as the reciprocal of this period for real-time accuracy. 
3)	Indicating the frequency data on LED, with an alert system to signal any deviations from the expected range, helping to monitor grid stability.
   
BLOCK DIAGRAM:

![GRID FREQUENCY MEASUREMENT docx - Word 07-11-2024 22_13_43](https://github.com/user-attachments/assets/e0e25a4c-7312-42fb-abd3-60f8dd885ee1)

FLOWCHART:

![GRID FREQUENCY MEASUREMENT docx - Word 07-11-2024 23_01_43](https://github.com/user-attachments/assets/b86338cb-74ed-4b49-baa1-eace0b720d8f)

CODE:
#include <stdint.h>
#include "tm4c123gh6pm.h"

// Global Variables
uint32_t timePeriod;
uint32_t startTime, endTime;
float tick, period, freq;
const float timePerTick = 62.5e-9; 

// Function to Initialize Timer 1 for Edge-Time Mode
void Timer1_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x02;           
    while ((SYSCTL_PRTIMER_R & 0x02) == 0) {}

    TIMER1_CTL_R = 0x00;                 
    TIMER1_CFG_R = 0x04;                 
    TIMER1_TAMR_R = 0x17;                 
    TIMER1_CTL_R &= ~0x0C;                
    TIMER1_CTL_R |= 0x04;                 
    TIMER1_CTL_R |= 0x01;                
}

// Function to Initialize Port B for Timer Capture Input
void PortB_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x02;           
    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}

    GPIO_PORTB_AFSEL_R |= 0x10;           
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) | 0x00070000; 
    GPIO_PORTB_DEN_R |= 0x10;             
}

// Function to Initialize Port F for LEDs
void PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;            
    while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}

    GPIO_PORTF_DIR_R |= 0x0C;             
    GPIO_PORTF_DEN_R |= 0x0C;            
    GPIO_PORTF_DATA_R &= ~0x0C;          
}


uint32_t MeasurePeriod(void) {
    while ((TIMER1_RIS_R & 0x04) == 0) {} 
    startTime = TIMER1_TAR_R;
    TIMER1_ICR_R = 0x02;                 

    while ((TIMER1_RIS_R & 0x04) == 0) {} 
    endTime = TIMER1_TAR_R;
    TIMER1_ICR_R = 0x02;                  

    if (endTime > startTime) {
        return endTime - startTime;       
    } else {
        return (0xFFFF - startTime + endTime); 
    }
}


void Delay_ms(uint32_t delay) {
    volatile uint32_t i; 
    for (i = 0; i < (16000 * delay); i++) {}
}

// Main Program
int main(void) {
    PortB_Init();         
    PortF_Init();         
    Timer1_Init();       

    while (1) {
        timePeriod = MeasurePeriod();         
        tick = timePeriod;                    
        period = timePerTick * tick;          
        freq = 1 / period;                   
        if ((freq > 47.5) && (freq < 48.5)) { 
            GPIO_PORTF_DATA_R ^= 0x08;        
            GPIO_PORTF_DATA_R &= ~0x02;       
        } else {
            GPIO_PORTF_DATA_R ^= 0x02;        
            GPIO_PORTF_DATA_R &= ~0x08;       
        }
        Delay_ms(500);                        
    }
}
