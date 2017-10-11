/* --------------- Includes ----------- */
#include "main.h"
#include <string.h>
#include <stdlib.h>

// definition commands word
#define _CMD_HELP      "help"
#define _CMD_CLEAR     "clear"
#define _CMD_CAN       "can"
// arguments for can
	#define _SCMD_CAN_INIT     "-init"
	#define _SCMD_CAN_STOP     "-stop"
	#define _SCMD_CAN_         "-"
#define _CMD_FILTER       "filter"
// arguments for can
	#define _SCMD_FILTER_SHOW   "show"
	#define _SCMD_FILTER_ON    "on"
	#define _SCMD_FILTER_OFF   "off"
	#define _SCMD_FILTER_SET   "set"

#define _NUM_OF_CMD 4
#define _NUM_OF_FILTER_SCMD 2

extern cicle_buffer_t* pc_b;
extern char * prompt_default;

void print_filters_conf(void);
void print_filter_on_off(uint32_t filter);
void int_to_ascii(uint32_t val, char* str);


//*****************************************************************************
void print (const char * str)
{ //put string into the buffer
	c_b_put_into(pc_b, str);
//	enable TXE interrupt to send these data
	LL_USART_EnableIT_TXE(USART1);
}

//*****************************************************************************
void execute (int argc, const char * const * argv) {
	int i = 0;
	while (i < argc) {

//*****************************************************************************
//		HELP.
		if (strcmp (argv[i], _CMD_HELP) == 0) { // help
			print ("HandyCan v");
			print (_VER);
			print("\n\r");

//*****************************************************************************
//		CLEAR.
		} else if (strcmp (argv[i], _CMD_CLEAR) == 0) { // clear
			print ("\033[2J");    // ESC seq for clear entire screen
			print ("\033[H");     // ESC seq for move cursor at left-top corner

//*****************************************************************************
//		FILTER ->
		} else if (strcmp (argv[i], _CMD_FILTER) == 0){ // Filters
			if (++i < argc) {
				if        (strcmp (argv[i], _SCMD_FILTER_SHOW) == 0) {
					print_filters_conf();
					break;
				} else if ((strcmp (argv[i], _SCMD_FILTER_ON )  == 0)|
						   (strcmp (argv[i], _SCMD_FILTER_SET)  == 0)|
						   (strcmp (argv[i], _SCMD_FILTER_OFF)  == 0)) {
					uint32_t filter = atoi (argv[i+1]);
					if ((filter <= 14) && (filter != 0)) {
						if ((strcmp (argv[i], _SCMD_FILTER_ON)  == 0))
							filter += 30;
						print_filter_on_off(filter);
						break;
					} else {
						print ("Specify filter's number.\n\r");
						break;
					}
				}
			} else {
					print ("'filter show' - list of all filters.\n\r");
					print ("'filter on XX' - turn on filter XX.\n\r");
					print ("'filter off XX' - turn off filter XX.\n\r");
					print ("'filter set XX' - turn off filter XX.\n\r");
					break;
			}

//*****************************************************************************
//		CAN ->
		} else if (strcmp (argv[i], _CMD_CAN) == 0){ // can
			if (++i < argc) {

			} else {
					print ("'can open' - open can.\n\r");
					print ("'can close' - close can.\n\r");
					print ("'can set ' - set bitrate\n\r");
					print ("'can open loop' - open can in a loop-back mode.\n\r");
					print ("'can open listen' - open can in a listen-only mode.\n\r");
					break;
			}

//*****************************************************************************
//		COMMAND NOT FOUND.
		} else {
			print ("Command: '");
			print ((char*)argv[i]);
			print ("' not found.\n\r");
		}
		i++;
	}
	print(prompt_default);
}


//*****************************************************************************
void print_filter_on_off(uint32_t filter){
	if (filter > 30) {
		filter -= 30;
		SET_BIT  (CAN->FA1R, (1 << (filter - 1)));
	} else
		CLEAR_BIT(CAN->FA1R, (1 << (filter - 1)));
	print("Done\n\r");
}

//*****************************************************************************
void print_filters_conf(){
	char number[8];
	uint32_t state;

	for (uint32_t i = 0; i < 14; i++){
		uint32_t k = i + 1;
		state = (uint32_t)0x0;
		state = READ_BIT(CAN->FA1R, 1 << i);
		number[2] = 0x00;
		number[0] = ' ';
		if (state != 0) print("\033[33m");
		print("Filter ");

		if (k > 9)	{
			number[0] = 1 + '0';
			number[1] = (k - 10) + '0';
		} else number[1] = k + '0';
		print((const char*)&number);

		print(" is ");
		if (state == 0) print("not ");
		print("active. ");

		if (READ_BIT(CAN->FFA1R, 1 << i) != 0)
			print("FIFO_1 ");
		else
			print("FIFO_0 ");

		print("\n\r");

		if (READ_BIT(CAN->FS1R, 1 << i) != 0)
			print("\t32-Bits filter. ");
		else
			print("\t16-Bits filters. ");
		if (READ_BIT(CAN->FM1R, 1 << i) != 0)
			print("Identifier LIST.");
		else
			print("Identifier MASK.");
		print("\n\r");

		print("\tFR1 - 0x");
		int_to_ascii(CAN->sFilterRegister[i].FR1, (char*)&number);
		print((const char*)&number);

		print("\tFR2 - 0x");
		int_to_ascii(CAN->sFilterRegister[i].FR2, (char*)&number);
		print((const char*)&number);
		print("\n\r");

		if (state != 0) print("\033[0m");


	}
}

//*****************************************************************************
uint8_t _4bit_to_ascii(uint8_t val){
	val &= 0x0f;
	if (val < 10)  	return val + '0';
	else    		return ((val - 10) + 'A');
}


//*****************************************************************************
void int_to_ascii(uint32_t val, char* str){
	char* pVal = (char*)(&val);
	pVal += 3;
	for(uint8_t i = 0; i < 8; i+=2)
	{
		str[i]   = _4bit_to_ascii(*pVal >> 4);
		str[i+1] = _4bit_to_ascii(*pVal);
		pVal--;
	}
}






















