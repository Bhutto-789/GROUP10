#include <stdint.h>
#include "tm4c123gh6pm.h"

// Global Variables
uint32_t timePeriod;
uint32_t startTime, endTime;
float tick, period, freq;
const float timePerTick = 62.5e-9; // 62.5 ns for a 16 MHz clock

// Function to Initialize Timer 1 for Edge-Time Mode
void Timer1_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x02;           // Enable Timer 1 clock
    while ((SYSCTL_PRTIMER_R & 0x02) == 0) {}

    TIMER1_CTL_R = 0x00;                  // Disable Timer 1
    TIMER1_CFG_R = 0x04;                  // Configure Timer 1 as 16-bit
    TIMER1_TAMR_R = 0x17;                 // Timer A in capture mode, edge-time
    TIMER1_CTL_R &= ~0x0C;                // Clear edge control bits (rising edge)
    TIMER1_CTL_R |= 0x04;                 // Set capture on rising edge
    TIMER1_CTL_R |= 0x01;                 // Enable Timer 1A
}

// Function to Initialize Port B for Timer Capture Input
void PortB_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x02;            // Enable clock for Port B
    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}

    GPIO_PORTB_AFSEL_R |= 0x10;           // Enable alternate function for PB4
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) | 0x00070000; // Configure PB4 as T1CCP0
    GPIO_PORTB_DEN_R |= 0x10;             // Enable digital functionality on PB4
}

// Function to Initialize Port F for LEDs
void PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;            // Enable clock for Port F
    while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}

    GPIO_PORTF_DIR_R |= 0x0C;             // Set PF3 (green LED) and PF2 (red LED) as output
    GPIO_PORTF_DEN_R |= 0x0C;             // Enable digital functionality for PF3 and PF2
    GPIO_PORTF_DATA_R &= ~0x0C;           // Turn off both LEDs initially
}

// Function to Measure Time Period Between Rising Edges
uint32_t MeasurePeriod(void) {
    while ((TIMER1_RIS_R & 0x04) == 0) {} // Wait for capture event
    startTime = TIMER1_TAR_R;
    TIMER1_ICR_R = 0x02;                  // Clear capture flag

    while ((TIMER1_RIS_R & 0x04) == 0) {} // Wait for the next capture event
    endTime = TIMER1_TAR_R;
    TIMER1_ICR_R = 0x02;                  // Clear capture flag

    if (endTime > startTime) {
        return endTime - startTime;       // Normal case
    } else {
        return (0xFFFF - startTime + endTime); // Overflow case
    }
}

// Function for Software Delay in ms
void Delay_ms(uint32_t delay) {
    volatile uint32_t i;  // Declare `i` as a volatile variable
    for (i = 0; i < (16000 * delay); i++) {}
}

// Main Program
int main(void) {
    PortB_Init();         // Initialize Port B for capture input
    PortF_Init();         // Initialize Port F for LEDs
    Timer1_Init();        // Initialize Timer 1 for capture

    while (1) {
        timePeriod = MeasurePeriod();         // Measure time period between rising edges
        tick = timePeriod;                    // Capture ticks
        period = timePerTick * tick;          // Calculate period (seconds)
        freq = 1 / period;                    // Calculate frequency (Hz)

        if ((freq > 47.5) && (freq < 48.5)) { // Check if frequency is near 48 Hz
            GPIO_PORTF_DATA_R ^= 0x08;        // Toggle green LED (PF3)
            GPIO_PORTF_DATA_R &= ~0x02;       // Turn off red LED (PF2)
        } else {
            GPIO_PORTF_DATA_R ^= 0x02;        // Toggle red LED (PF2)
            GPIO_PORTF_DATA_R &= ~0x08;       // Turn off green LED (PF3)
        }
        Delay_ms(500);                        // Delay for LED blink
    }
}
