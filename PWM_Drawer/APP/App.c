/*
 * App.c
 *
 * Created: 1/06/2025
 *  Author: MahmoudAboheif
 */ 

#include "App.h"
#include "BIT_MATH.h"

// Global variables for Input Capture
volatile unsigned long long capture_value1 = 0;
volatile unsigned long long capture_value2 = 0;
volatile unsigned long long capture_value3 = 0;
volatile unsigned char capture_state = 0;
volatile unsigned long long timer1_overflow_count = 0;

// global variables
unsigned long long Time_On;
unsigned long long Time_Off;
unsigned long long totalTime;
float duty_cycle;
float freq;

void InputCapture_Init(void)
{
	// Enable Timer1 Input Capture Interrupt
	TIMSK_REG |= (1 << 5);  // TICIE1 = bit 5
	
	// Enable Timer1 Overflow Interrupt
	TIMSK_REG |= (1 << 2);  // TOIE1 = bit 2
	
	// Set Input Capture to trigger on rising edge initially
	TCCR1B_REG |= (1 << 6); // ICES1 = bit 6
	
	// Enable Input Capture Noise Canceler (optional)
	TCCR1B_REG |= (1 << 7); // ICNC1 = bit 7
}

void TMR1_InputCapture_Init(void)
{
	// Configure Timer1 for Normal mode with prescaler 64
	// TCCR1A = 0 (Normal mode)
	TCCR1A_REG = 0;
	
	// Set prescaler to 64 (CS12=0, CS11=1, CS10=1)
	TCCR1B_REG |= (1 << 1) | (1 << 0);  // CS11 and CS10
	
	// Enable Input Capture features
	TCCR1B_REG |= (1 << 6);  // ICES1 - Rising edge trigger
	TCCR1B_REG |= (1 << 7);  // ICNC1 - Noise canceler
	
	// Reset timer
	TCNT1_uint16_REG = 0;
	timer1_overflow_count = 0;
}

void MeasureSignal(void)
{
	if(capture_state >= 3) // We have captured 3 edges
	{
		// Calculate total period (time between first and third capture)
		totalTime = ((capture_value3 - capture_value1) * 4);
		
		// Calculate ON time (time between first and second capture - rising to falling)
		Time_On = ((capture_value2 - capture_value1) * 4);
		
		// Calculate OFF time
		Time_Off = totalTime - Time_On;
		
		// Calculate frequency in Hz
		if(totalTime > 0)
		{
			freq = 1000000.0 / (float)totalTime; // Convert from ?s to Hz
			duty_cycle = ((float)Time_On * 100.0) / (float)totalTime;
		}
		else
		{
			freq = 0;
			duty_cycle = 0;
		}
		
		// Reset for next measurement
		capture_state = 0;
		timer1_overflow_count = 0;
		TCNT1_uint16_REG = 0;
	}
}

void Display_SignalMeasurement(void)
{
	char nummm[16];
	
	GLCD_ClearDisplay();

	/*************************************   DISPLAY FREQUENCY   *************************************/
	GLCD_GoToLine(0);
	sprintf(nummm,"freq=%0.2f Hz",freq);
	GLCD_DisplayString(nummm);
	
	/*************************************   DISPLAY DUTY_CYCLE   *************************************/
	GLCD_GoToLine(2);
	sprintf(nummm,"duty=%0.1f%%",duty_cycle);
	GLCD_DisplayString(nummm);
	
	/*************************************   DISPLAY TOTAL_TIME   *************************************/
	GLCD_GoToLine(4);
	sprintf(nummm,"T=%0.2f ms",(float)(totalTime/1000.0));
	GLCD_DisplayString(nummm);
}

void Display_Signal(void)
{
	/*************************************   DISPLAY Signal   *************************************/
	GLCD_GoToLine(6);
	
	// Fixed calculation for duty cycle representation
	int dutyRef = (int)((duty_cycle * 8.0) / 100.0);
	
	// Ensure dutyRef is within bounds
	if(dutyRef > 8) dutyRef = 8;
	if(dutyRef < 0) dutyRef = 0;
	
	// Draw first pulse
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_RISING_EDGE);
	for(char index = 0; index < dutyRef; index++){
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_HIGH_LINE);
	}
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_FALLING_EDGE);
	for(char index = 0; index < (8-dutyRef); index++){
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_LOW_LINE);
	}
	
	// Draw second pulse
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_RISING_EDGE);
	for(char index = 0; index < dutyRef; index++){
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_HIGH_LINE);
	}
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_FALLING_EDGE);
	for(char index = 0; index < (8-dutyRef); index++){
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_LOW_LINE);
	}
}

// ISR for Timer1 Input Capture
void __vector_6(void) __attribute__ ((signal));
void __vector_6(void)
{
	unsigned long long current_capture;
	
	// Calculate current capture value including overflows
	current_capture = (timer1_overflow_count * 65536ULL) + ICR1_REG;
	
	switch(capture_state)
	{
		case 0: // First capture (rising edge)
		capture_value1 = current_capture;
		capture_state = 1;
		// Change to falling edge detection
		TCCR1B_REG &= ~(1 << 6); // Clear ICES1
		break;
		
		case 1: // Second capture (falling edge)
		capture_value2 = current_capture;
		capture_state = 2;
		// Change back to rising edge detection
		TCCR1B_REG |= (1 << 6);  // Set ICES1
		break;
		
		case 2: // Third capture (next rising edge)
		capture_value3 = current_capture;
		capture_state = 3;
		break;
		
		default:
		capture_state = 0;
		break;
	}
}

// ISR for Timer1 Overflow
void __vector_9(void) __attribute__ ((signal));
void __vector_9(void)
{
	timer1_overflow_count++;
}