/*
 * GLCD.c
 *
 * Created: 23/05/2025
 * Author: MahmoudAboheif
 */ 

#include "GLCD.h"
#if GLCD_USE_TIMERS_FOR_DELAY == 1
	#include "GPT.h"
#else
	#include <util/delay.h>
#endif

uint8 guint8_Curr_X_Address = 0;
uint8 guint8_Curr_Y_Address = 0;
uint8 guint8_Curr_Page = 0;

void GLCD_Init(void) {
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
		GPT_Init(gaStrGPT_Config);
	#endif
	/* Initialize Port Directions */
	DIO_SetPinDirection(GLCD_CTRL_PORT, GLCD_ENABLE_PIN, DIO_OUTPUT);
	DIO_SetPinDirection(GLCD_CTRL_PORT, GLCD_RS_PIN, DIO_OUTPUT);
	DIO_SetPinDirection(GLCD_CTRL_PORT, GLCD_RW_PIN, DIO_OUTPUT);
	DIO_SetPinDirection(GLCD_CTRL_PORT, GLCD_RESET_PIN, DIO_OUTPUT);
	DIO_SetPinDirection(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_1, DIO_OUTPUT);
	DIO_SetPinDirection(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_2, DIO_OUTPUT);

	DIO_SetPortDirection(GLCD_DATA_PORT, DIO_OUTPUT);
	/* Keep reset pin inactive (high) */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RESET_PIN, DIO_HIGH);
	/* Initialization of GLCD Sequence */
	GLCD_SelectPage(GLCD_BOTH_PAGES);
	GLCD_SendInstruction(GLCD_DISPLAY_OFF);
	GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + 0);  /* line=0 */
	guint8_Curr_X_Address = 0;
	GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + 0);  /* column=0 */
	guint8_Curr_Y_Address = 0;
	GLCD_SendInstruction(GLCD_SET_Z_ADDRESS_PREFIX + 0);  /* start_line=0 */
	GLCD_SendInstruction(GLCD_DISPLAY_ON);
	GLCD_SelectPage(GLCD_PAGE_0);
	guint8_Curr_Page = 0;
}

void GLCD_SelectPage(GLCD_PageSelectType Page) {
	switch (Page) {
		case GLCD_NO_PAGES:
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_1, DIO_LOW);
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_2, DIO_LOW);
			break;
		case GLCD_PAGE_0:
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_1, DIO_HIGH);
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_2, DIO_LOW);
			break;
		case GLCD_PAGE_1:
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_1, DIO_LOW);
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_2, DIO_HIGH);
			break;
		case GLCD_BOTH_PAGES:
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_1, DIO_HIGH);
			DIO_WritePin(GLCD_CTRL_PORT, GLCD_PAGE_SELECT_PIN_2, DIO_HIGH);
			break;
		default: break;
	}
}

void GLCD_SendInstruction(GLCD_CommandType Instruction) {
	/* Select Instruction Register */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RS_PIN, GLCD_INSTRUCTION_REG);
	/* Select Write Operation */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RW_PIN, GLCD_WRITE_MODE);
	/* Send Command on Data Pins */
	DIO_WritePort(GLCD_DATA_PORT, Instruction);
	/* Enable Pulse of min. width 1us */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_ENABLE_PIN, DIO_HIGH);
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
		GPT_Delay_us(&gaStrGPT_Config[0], 5);
	#else
		_delay_us(5);
	#endif
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_ENABLE_PIN, DIO_LOW);
}

void GLCD_SendData(uint8 Data) {
	/* Select Data Register */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RS_PIN, GLCD_DATA_REG);
	/* Select Write Operation */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RW_PIN, GLCD_WRITE_MODE);
	/* Send Command on Data Pins */
	DIO_WritePort(GLCD_DATA_PORT, Data);
	/* Enable Pulse of min. width 1us */
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_ENABLE_PIN, DIO_HIGH);
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
		GPT_Delay_us(&gaStrGPT_Config[0], 5);
	#else
		_delay_us(5);
	#endif
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_ENABLE_PIN, DIO_LOW);
}

void GLCD_Reset(void) {
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RESET_PIN, DIO_LOW);
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
		GPT_Delay_us(&gaStrGPT_Config[0], 10);
	#else
		_delay_us(10);
	#endif
	DIO_WritePin(GLCD_CTRL_PORT, GLCD_RESET_PIN, DIO_HIGH);
}

void GLCD_DisplaySpecialPattern(uint64 Pattern) {
	for(uint8 i = 0; i < 5; i++) {
		GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + guint8_Curr_X_Address);
		GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + guint8_Curr_Y_Address);
		GLCD_SendData((Pattern>>(i*8U)) & 0x00000000FFUL);
		guint8_Curr_Y_Address++;
		if (guint8_Curr_Y_Address >= GLCD_MAX_PAGE_PIXEL_WIDTH && guint8_Curr_Page == 0) {
			GLCD_SelectPage(GLCD_PAGE_1);
			guint8_Curr_Y_Address = 0;
		}
	}
}

void GLCD_DisplayCharacter(char Data) {
	if ((guint8_Curr_Page == 0) | (guint8_Curr_Y_Address < GLCD_MAX_PAGE_PIXEL_WIDTH - 7U)) {
		/* Enough space to write character */
		switch (Data) {
			case 'A': case 'a': GLCD_DisplaySpecialPattern(GLCD_CHAR_A); break;
			case 'F': case 'f': GLCD_DisplaySpecialPattern(GLCD_CHAR_F); break;
			case 'R': case 'r': GLCD_DisplaySpecialPattern(GLCD_CHAR_R); break;
			case 'E': case 'e': GLCD_DisplaySpecialPattern(GLCD_CHAR_E); break;
			case 'Q': case 'q': GLCD_DisplaySpecialPattern(GLCD_CHAR_Q); break;
			case 'U': case 'u': GLCD_DisplaySpecialPattern(GLCD_CHAR_U); break;
			case 'N': case 'n': GLCD_DisplaySpecialPattern(GLCD_CHAR_N); break;
			case 'C': case 'c': GLCD_DisplaySpecialPattern(GLCD_CHAR_C); break;
			case 'Y': case 'y': GLCD_DisplaySpecialPattern(GLCD_CHAR_Y); break;
			case 'D': case 'd': GLCD_DisplaySpecialPattern(GLCD_CHAR_D); break;
			case 'T': case 't': GLCD_DisplaySpecialPattern(GLCD_CHAR_T); break;
			case 'K': case 'k': GLCD_DisplaySpecialPattern(GLCD_CHAR_K); break;
			case 'H': case 'h': GLCD_DisplaySpecialPattern(GLCD_CHAR_H); break;
			case 'Z': case 'z': GLCD_DisplaySpecialPattern(GLCD_CHAR_Z); break;
			case 'M': case 'm': GLCD_DisplaySpecialPattern(GLCD_CHAR_M); break;
			case 'S': case 's': GLCD_DisplaySpecialPattern(GLCD_CHAR_S); break;
			case 'I': case 'i': GLCD_DisplaySpecialPattern(GLCD_CHAR_I); break;
			case '%': GLCD_DisplaySpecialPattern(GLCD_CHAR_PERCENT); break;
			case '=': GLCD_DisplaySpecialPattern(GLCD_CHAR_EQUAL); break;
			case '.': GLCD_DisplaySpecialPattern(GLCD_CHAR_POINT); break;
			case ' ': GLCD_DisplaySpecialPattern(GLCD_CHAR_SPACE); break;
			case '-': GLCD_DisplaySpecialPattern(GLCD_CHAR_NEGATIVE); break;
			case '0': GLCD_DisplaySpecialPattern(GLCD_CHAR_0); break;
			case '1': GLCD_DisplaySpecialPattern(GLCD_CHAR_1); break;
			case '2': GLCD_DisplaySpecialPattern(GLCD_CHAR_2); break;
			case '3': GLCD_DisplaySpecialPattern(GLCD_CHAR_3); break;
			case '4': GLCD_DisplaySpecialPattern(GLCD_CHAR_4); break;
			case '5': GLCD_DisplaySpecialPattern(GLCD_CHAR_5); break;
			case '6': GLCD_DisplaySpecialPattern(GLCD_CHAR_6); break;
			case '7': GLCD_DisplaySpecialPattern(GLCD_CHAR_7); break;
			case '8': GLCD_DisplaySpecialPattern(GLCD_CHAR_8); break;
			case '9': GLCD_DisplaySpecialPattern(GLCD_CHAR_9); break;
			default: break;
		}
		GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + guint8_Curr_X_Address);
		GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + guint8_Curr_Y_Address);
		GLCD_SendData(0);
		guint8_Curr_Y_Address++;
		if (guint8_Curr_Y_Address >= GLCD_MAX_PAGE_PIXEL_WIDTH && guint8_Curr_Page == 0) {
			GLCD_SelectPage(GLCD_PAGE_1);
			guint8_Curr_Y_Address = 0;
		}
	}
}

void GLCD_DisplayString(char* Data_Ptr) {
	while (*Data_Ptr != '\0') {	
		GLCD_DisplayCharacter(*Data_Ptr);
		Data_Ptr++;
	}
}

void GLCD_GoToLine(uint8 Line) {
	if (Line < GLCD_MAX_LINES) {
		GLCD_SelectPage(GLCD_PAGE_0);
		guint8_Curr_X_Address = Line;
		GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + guint8_Curr_X_Address);
		guint8_Curr_Y_Address = 0;
		GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + guint8_Curr_Y_Address);
	}
}

void GLCD_ClearDisplay(void) {
	for (uint8 Line = 0; Line < GLCD_MAX_LINES; Line++) {
		GLCD_GoToLine(Line);
		GLCD_SelectPage(GLCD_BOTH_PAGES);
		for (uint8 i = 0; i < GLCD_MAX_PAGE_PIXEL_WIDTH; i++) {
			GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + i);
			GLCD_SendData(0);
		}
	}
	GLCD_Reset();
	GLCD_SelectPage(GLCD_PAGE_0);
	guint8_Curr_Page = 0;
	GLCD_GoToLine(0);
}
