/*
 * PWM0_interface.h
 *
 * Created: 16/02/2025
 *  Author:MahmoudAboheif
 */ 

#ifndef MCAL_PWM0_INC_PWM0_INTERFACE_H_
#define MCAL_PWM0_INC_PWM0_INTERFACE_H_


                /*************** APIS PROTOTYPES ***************/

void PWM0_voidInit       (void);
void PWM0_voidGeneratePWM(uint8 copy_uint8DutyCycle);
void PWM0_voidStop       (void);



#endif /* MCAL_PWM0_INC_PWM0_INTERFACE_H_ */
