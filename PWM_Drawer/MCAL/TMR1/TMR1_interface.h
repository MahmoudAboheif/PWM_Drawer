/*
 * TMR1_interface.h
 *
 * Created: 20/02/2025
 * Author: MahmoudAboheif
 */

#ifndef TMR1_INTERFACE_H_
#define TMR1_INTERFACE_H_

//modes

#define TMR1_NORMAL_OVER_FLOW	    		0
#define TMR1_CTC_COMPARE_MATCH_A			1
#define TMR1_CTC_COMPARE_MATCH_b			2
#define TMR1_INPUT_CAPT		            	3

                        /*************** APIS PROTOTYPES ***************/

void TMR1_voidInit       (void);
void TMR1_voidStart      (void);
void TMR1_voidStop       (void);
void TMR1_voidSetCallBack(void(*copy_pFunAction)(void), uint8 copy_uint8InterruptSource);

#endif /* TMR1_INTERFACE_H_ */
