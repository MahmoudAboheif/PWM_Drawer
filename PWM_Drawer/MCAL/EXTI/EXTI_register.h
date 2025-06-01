/*
 * EXTI_register.h
 *
 * Created: 31/01/2025
 *  Author:MahmoudAboheif
 */

#ifndef MCAL_EXTI_INC_EXTI_REGISTER_H_
#define MCAL_EXTI_INC_EXTI_REGISTER_H_


                                       /* EXTERNAL INTERRUPT REGISTERS */

// MCU Control Register
#define MCUCR_REG			(*(volatile uint8*)0x55)
#define ISC00               0
#define ISC01               1
#define ISC10               2
#define ISC11               3

// MCU Control and Status Register
#define MCUCSR_REG			(*(volatile uint8*)0x54)
#define ISC2                6

// General Interrupt Control Register
#define GICR_REG			(*(volatile uint8*)0x5B)
#define INT0                6
#define INT1                7
#define INT2                5

// General Interrupt Flag Register
#define GIFR_REG			(*(volatile uint8*)0x5A)
#define INTF0               6
#define INTF1               7
#define INTF2               5


#endif /* MCAL_EXTI_INC_EXTI_REGISTER_H_ */
