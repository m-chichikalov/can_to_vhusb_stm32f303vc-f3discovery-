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
#define _CMD_FILTER     "filter"
// arguments for can
	#define _SCMD_FILTER_SHOW  "show"
	#define _SCMD_FILTER_ON    "on"
	#define _SCMD_FILTER_OFF   "off"
	#define _SCMD_FILTER_SET   "set"

extern cicle_buffer_t* pc_b;
extern char * prompt_default;

typedef struct {
	uint32_t name;
	uint32_t Sjw_Ts2_Ts1_Brp;
}bittime;


//Table from http://www.bittiming.can-wiki.info/
bittime set_bitrate[10] = {
                           { 0x0010, 0x001c0095},
		                   { 0x0020, 0x001c004a},
		                   { 0x0050, 0x001c001d},
		                   { 0x0100, 0x001c000e},
		                   { 0x0125, 0x001c000b},
		                   { 0x0250, 0x001c0005},
		                   { 0x0500, 0x001c0002},
		                   { 0x0800, 0x001b0001},
		                   { 0x1000, 0x00050002},
		                   { 0x0083, 0x001c0011}};

void print_filters_conf(void);
void print_help(void);
void print_filters_help(void);
void print_can_help(void);
void print_can_set(const char* btr);
void print_can_close(void);
void print_can_open(uint32_t can_mode);
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
			print ("\033[20,H");     // ESC seq for move cursor at left-top corner
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
				if ((strcmp (argv[i], _SCMD_CAN_CLOSE)  == 0))
				{ print_can_close(); break; }
				else if ((strcmp (argv[i], _SCMD_CAN_SET)  == 0))
				{ print_can_set(argv[i+1]); break; }
				else if ((strcmp (argv[i], _SCMD_CAN_OPEN)  == 0))
				{
					uint32_t can_mode;
					if (argc != (i + 2))
						can_mode = CAN_MODE_NORMAL;
					else if ((strcmp (argv[i + 1], _SCMD_CAN_OPEN_LOOP)  == 0))
						can_mode = CAN_MODE_SILENT_LOOPBACK;
					else if ((strcmp (argv[i + 1], _SCMD_CAN_OPEN_LISTEN)  == 0))
						can_mode = CAN_MODE_SILENT;
					print_can_open(can_mode);
					break;
				}
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
	if ((strlen(fr1)== 8) & (strlen(fr2) == 8))
	{
		CLEAR_BIT(CAN->FA1R, (1 << (filter)));
		WRITE_REG(CAN->sFilterRegister[filter].FR1, ascii_to_int((char*)fr1));
		WRITE_REG(CAN->sFilterRegister[filter].FR2, ascii_to_int((char*)fr2));
		SET_BIT  (CAN->FA1R, (1 << (filter)));
		print("Done!\n\r");
	} else
		print("Wrong length of the identifier. Should be xxxxxxxx - 8bytes.\n\r");
}
//*****************************************************************************
//    can SET xxxxxxxx  -  set xxxxxxxx in register CAN_BTR
//*****************************************************************************
void print_can_set(const char* btr){
	uint8_t len = strlen(btr);
	if ( !((len == 1) | (len == 8)) ){
		print("Wrong the identifier after command '\033[33mset\033[0m'.\n\r");
	}
	else if (Can_Init_Mode() == SUCCESS){
	uint32_t temp = ascii_to_int((char*)btr);
	if (len == 1)
		temp = set_bitrate[temp].Sjw_Ts2_Ts1_Brp;
	temp &= 0x037F03FF;
	WRITE_REG(CAN->BTR, temp);
	print("Done!\n\r");
	print("Now CAN in INITIALIZATION mode. Use '\033[33mcan open\033[0m' to turn it on.\n\r");
	print("Type '\033[33mcan\033[0m' for help.\n\r");
	} else {
		print("Something went wrong...\n\r");
	}
}
//*****************************************************************************
void print_can_close(void){
	if (Can_DeInit() == SUCCESS)
	print("Done! CAN is in SLEEP mode.\n\r");
	else {
		if (IS_BIT_SET(CAN->MSR, CAN_MSR_INAK))
			print("Upss... CAN already in INITIALIZATION mode.\n\r");
	}
}
//*****************************************************************************
void print_can_open(uint32_t can_mode){
	if (Can_Init_Mode() == SUCCESS){
		CAN->BTR &= 0x3FFFFFFF;
		CAN->BTR |= can_mode;
		if (Can_Normal_Mode() == SUCCESS)
			print("Done! CAN is in NORMAL mode.\n\r");
	}
	else
		print("Something went wrong...\n\r");
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
//   str - string from X to XXXXXXXX  in HEX format.
//   return (32bit).
//*****************************************************************************
uint32_t ascii_to_int(char* str){
	uint32_t val = 0;
	uint8_t i;
	char* pVal = (char*)(&val);
	pVal += 3;
	for (i = 0; i < 8; i+=2){
		if (str[i] == 0x0) break;
		*pVal  = (_ascii_to_4bit(str[i]) << 4);
		if (str[i+1] == 0x0)
			{ i++; break; }
		*pVal |= _ascii_to_4bit(str[i+1]);
		pVal--;
	}
	val = ( val >> ((8-i)*4) );
	return val;
}
//*****************************************************************************
void print_filters_help(void){
	print ("'\033[33mfilter show\033[0m' - list of all filters.\n\r");
	print ("'\033[33mfilter on XX\033[0m' - turn on filter XX.\n\r");
	print ("'\033[33mfilter off XX\033[0m' - turn off filter XX.\n\r");
	print ("'\033[33mfilter set XX YYYYYYYY ZZZZZZZZ\033[0m' -\n\r");
	print ("\tset XX filter's configuration.\n\r");
	print ("\tYYYYYYYY - HEX value for FR1 register.\n\r");
	print ("\tZZZZZZZZ - HEX value for FR2 register.\n\r");
}
//*****************************************************************************
void print_can_help(void){
	print ("'\033[33mcan open\033[0m' - open can.\n\r");
	print ("'\033[33mcan open loop\033[0m' - open can in a loop-back mode.\n\r");
	print ("'\033[33mcan open listen\033[0m' - open can in a listen-only mode.\n\r");
	print ("'\033[33mcan close\033[0m' - close can.\n\r");
	print ("'\033[33mcan SET XXXXXXXX\033[0m' - set register BTR for any needed bitrate.\n\r");
	print ("\tXXXXXXXX - HEX value, only first 30 bits are relevant. \n\r");
	print ("'\033[33mcan set X\033[0m' - set bitrate from list there x equal...\n\r");
	print ("\t'\033[33m0\033[0m' - 10kB\n\r");
	print ("\t'\033[33m1\033[0m' - 20kB\n\r");
	print ("\t'\033[33m2\033[0m' - 50kB\n\r");
	print ("\t'\033[33m3\033[0m' - 100kB\n\r");
	print ("\t'\033[33m4\033[0m' - 125kB\n\r");
	print ("\t'\033[33m5\033[0m' - 250kB\n\r");
	print ("\t'\033[33m6\033[0m' - 500kB\n\r");
	print ("\t'\033[33m7\033[0m' - 800kB\n\r");
	print ("\t'\033[33m8\033[0m' - 1  MB\n\r");
	print ("\t'\033[33m9\033[0m' - 83.33kB\n\r");
}
//*****************************************************************************
void print_help(void){
	print ("\033[33mHandyCan\033[0m v");
	print (_VER);
	print("\n\r");
}





















