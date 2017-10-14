/* --------------- Includes ----------- */
#include "main.h"
#include <string.h>
#include <stdlib.h>

// definition commands word
#define _CMD_ON        "on?"
#define _CMD_OFF       "off?"
#define _CMD_HELP      "help"
#define _CMD_CLEAR     "clear"
#define _CMD_CAN       "can"
// arguments for can
	#define _SCMD_CAN_OPEN              "open"
		#define _SCMD_CAN_OPEN_LOOP     "loop"
		#define _SCMD_CAN_OPEN_LISTEN   "listen"
	#define _SCMD_CAN_CLOSE             "close"
	#define _SCMD_CAN_SET               "set"
#define _CMD_FILTER       "filter"
// arguments for can
	#define _SCMD_FILTER_SHOW   "show"
	#define _SCMD_FILTER_ON    "on"
	#define _SCMD_FILTER_OFF   "off"
	#define _SCMD_FILTER_SET   "set"

extern cicle_buffer_t* pc_b;
extern char * prompt_default;

void print_filters_conf(void);
void print_help(void);
void print_filters_help(void);
void print_can_help(void);
void print_filter_on_off(uint32_t filter);
void print_filter_set(uint32_t filter, const char* fr1, const char* fr2);
void int_to_ascii(uint32_t val, char* str);
uint32_t ascii_to_int(char* str);


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
			print_help();

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
				} else if ((strcmp (argv[i], _SCMD_FILTER_ON )  == 0)||
						   (strcmp (argv[i], _SCMD_FILTER_SET)  == 0)||
						   (strcmp (argv[i], _SCMD_FILTER_OFF)  == 0)) {
					uint32_t filter = atoi (argv[i+1]);
					if ((filter <= 14) && (filter != 0)) {
						filter -= 1;
						if ((strcmp (argv[i], _SCMD_FILTER_ON )  == 0)) filter += 30;
							if (strcmp (argv[i], _SCMD_FILTER_SET)  == 0){
								if (i+3 < argc){
									print_filter_set(filter, argv[i+2], argv[i+3]);
									break;
								} else {
									print ("Not enough arguments for command 'set'.\n\r");
									print ("Type '\033[33mfilter\033[0m' for help.\n\r");
									break;
								}
						}
						print_filter_on_off(filter);
						break;
					} else {
						print ("Specify filter's number.\n\r");
						break;
					}
				}
			} else {
					print_filters_help();
					break;
			}

//*****************************************************************************
//		CAN ->
		} else if (strcmp (argv[i], _CMD_CAN) == 0){ // can
			if (++i < argc) {

			} else {
					print_can_help();
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
void print_filter_set(uint32_t filter, const char* fr1, const char* fr2){

//	Check that filter diactivated
	CLEAR_BIT(CAN->FA1R, (1 << (filter)));
	WRITE_REG(CAN->sFilterRegister[filter].FR1, ascii_to_int((char*)fr1));
	WRITE_REG(CAN->sFilterRegister[filter].FR2, ascii_to_int((char*)fr2));
	SET_BIT  (CAN->FA1R, (1 << (filter)));
	print("Done!\n\r");

}


//*****************************************************************************
void print_filter_on_off(uint32_t filter){
	if (filter > 30) {
		filter -= 30;
		SET_BIT  (CAN->FA1R, (1 << (filter)));
	} else
		CLEAR_BIT(CAN->FA1R, (1 << (filter)));
	print("Done!\n\r");
}

//*****************************************************************************
void print_filters_conf(){
	char number[9];
	uint32_t state;

	for (uint32_t i = 0; i < 14; i++){
		uint32_t k = i + 1;
		state = 0;
		state = READ_BIT(CAN->FA1R, 1 << i);
		number[2] = 0x0;
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
	if    (val < 10)  	return  (val + '0')       ;
	else    		    return ((val -  10) + 'A');
}
//*****************************************************************************
uint8_t _ascii_to_4bit(uint8_t val){
	if      (val <= '9') 	return   (val - '0')         ; // 0...9
	else if (val <= 'F')   	return  ((val - 63 ) | 0x08) ; // A...F
	else                	return  ((val - 95 ) | 0x08) ; // a...f
}


//*****************************************************************************
void int_to_ascii(uint32_t val, char* str){
	char* pVal = (char*)(&val);
	uint8_t i;
	pVal += 3;
	for(i = 0; i < 8; i+=2)
	{
		str[i]   = _4bit_to_ascii(*pVal >> 4);
		str[i+1] = _4bit_to_ascii(*pVal);
		pVal--;
	}
	str[i] = 0x0;
}

//*****************************************************************************
// str - string xxxxxxxx in HEX format (32bit).
uint32_t ascii_to_int(char* str){
	uint32_t val = 0;
	uint8_t i;
	char* pVal = (char*)(&val);
	pVal += 3;
	for (i = 0; i < 8; i+=2){
		*pVal = ((_ascii_to_4bit(str[i]) << 4) | _ascii_to_4bit(str[i+1]));
		pVal--;
	}
	return val;
}


//*****************************************************************************
void print_filters_help(void){
	print ("\033[33m'filter show'\033[0m - list of all filters.\n\r");
	print ("\033[33m'filter on XX'\033[0m - turn on filter XX.\n\r");
	print ("\033[33m'filter off XX'\033[0m - turn off filter XX.\n\r");
	print ("\033[33m'filter set XX YYYYYYYY ZZZZZZZZ'\033[0m -\n\r");
	print ("\tset XX filter's configuration.\n\r");
	print ("\tYYYYYYYY - HEX value for FR1 register.\n\r");
	print ("\tZZZZZZZZ - HEX value for FR2 register.\n\r");
}
//*****************************************************************************
void print_can_help(void){
	print ("\033[33m'can open'\033[0m - open can.\n\r");
	print ("\033[33m'can close'\033[0m - close can.\n\r");
	print ("\033[33m'can set'\033[0m - set bitrate\n\r");
	print ("\033[33m'can open loop'\033[0m - open can in a loop-back mode.\n\r");
	print ("\033[33m'can open listen'\033[0m - open can in a listen-only mode.\n\r");
}
//*****************************************************************************
void print_help(void){
	print ("\033[33mHandyCan\033[0m v");
	print (_VER);
	print("\n\r");
}





















