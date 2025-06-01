/*
 * TMR1_program.c
 *
 * Created: 20/02/2025
 * Author: MahmoudAboheif
 */


/* UTILES_LIB */
#include "STD_TYPES.h"
#include "BIT_MATH.h"

/* MCAL */
#include "TMR1_interface.h"
#include "TMR1_config.h"
#include "TMR1_private.h"
#include "TMR1_register.h"


static void(*PRV_pFunCallBackOVF)(void) = NULL;
static void(*PRV_pFunCallBackCTCA)(void) = NULL;

#define TMR1_MODE TMR1_NORMAL_MODE_0

void TMR1_voidInit(void)
{

	// Select Mode = Normal Mode 0
	CLR_BIT(TCCR1A_REG, WGM10);
	CLR_BIT(TCCR1A_REG, WGM11);
	CLR_BIT(TCCR1B_REG, WGM12);
	CLR_BIT(TCCR1B_REG, WGM13);
	
	// Init Timer With Preload Value
	TCNT1_uint16_REG = 0;
	
	// Enable OVF Interrupt
	SET_BIT(TIMSK_REG, TOIE1);

}


void TMR1_voidStart(void)
{
	//Select Prescaler Value = 64
	SET_BIT(TCCR1B_REG, CS10);
	SET_BIT(TCCR1B_REG, CS11);
	CLR_BIT(TCCR1B_REG, CS12);
}


void TMR1_voidStop(void)
{
	CLR_BIT(TCCR1B_REG, CS10);
	CLR_BIT(TCCR1B_REG, CS11);
	CLR_BIT(TCCR1B_REG, CS12);
}


void TMR1_voidSetCallBack(void(*copy_pFunAction)(void), uint8 copy_uint8InterruptSource)
{
	if(copy_pFunAction != NULL)
	{
		switch(copy_uint8InterruptSource)
		{
			case TMR1_NORMAL_OVER_FLOW:
			PRV_pFunCallBackOVF = copy_pFunAction;
			break;
			
			case TMR1_CTC_COMPARE_MATCH_A:
			PRV_pFunCallBackCTCA = copy_pFunAction;
			break;
		}
	}
	else
	{
		//return Error State
	}
}

