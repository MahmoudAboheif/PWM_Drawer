/*
 * main.c
 *
 * Created: 1/06/2025
 *  Author: MahmoudAboheif 
 */ 
 #include "ATmega32A_Config.h"
 #include "App.h"
 #include "GLCD.h"
 #include "TMR1_interface.h"
 #include "TMR1_config.h"
 #include "TMR1_private.h"
 #include "TMR1_register.h"
 #include "GI_interface.h"
 #include "PWM0_interface.h"
 #include "PWM0_private.h"
 #include "PWM0_register.h"

 #include <util/delay.h>
 #include <string.h>
 #include <stdio.h>

 int main(void)
 {
	 // Configure PD6 as input (ICP1 pin)
	 DIO_SetPinDirection(DIO_PORT_D, 6, DIO_INPUT);
	 
	 // Configure PB3 as output for PWM generation (OC0)
	 DIO_SetPinDirection(DIO_PORT_B, 3, DIO_OUTPUT);
	 
	 // Enable global interrupts
	 GI_voidEnable();
	 
	 // Initialize Input Capture
	 InputCapture_Init();
	 
	 // Initialize PWM for testing
	 PWM0_voidInit();
	 PWM0_voidGeneratePWM(90);	//Enter the duty cycle u want to test
	 
	 // Initialize Timer1 for Input Capture
	 TMR1_InputCapture_Init();
	 
	 // Initialize GLCD
	 GLCD_Init();
	 GLCD_ClearDisplay();
	 
	 while(1)
	 {
		 MeasureSignal();
		 Display_SignalMeasurement();
		 Display_Signal();
		 
		 _delay_ms(500);
	 }
 }