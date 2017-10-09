/* --------------- Includes ----------- */
#include "main.h"
#include "CLI_Functionality.h"
#include <string.h>
#include <stdlib.h>

// definition commands word
#define _CMD_HELP      "help"
#define _CMD_CLEAR     "clear"
#define _CMD_CLR       "clear_port"
#define _CMD_SET       "set_port"
// arguments for set/clear
	#define _SCMD_PB           "port_b"
	#define _SCMD_PD           "port_d"
#define _CMD_CAN       "can"
// arguments for can
	#define _SCMD_CAN_INIT     "-init"
	#define _SCMD_CAN_STOP     "-stop"
	#define _SCMD_CAN_         "-"

#define _NUM_OF_CMD 4
#define _NUM_OF_SETCLEAR_SCMD 2

//available  commands
char * keyworld [] = {_CMD_HELP, _CMD_CLEAR, _CMD_SET, _CMD_CLR};
// 'set/clear' command arguments
char * set_clear_key [] = {_SCMD_PB, _SCMD_PD};

// array for comletion
char * compl_world [_NUM_OF_CMD + 1];

extern cicle_buffer_t* pc_b;
//extern      microrl_t* prl ;
extern char * prompt_default;



//*****************************************************************************
void print (const char * str)
{ //put string into the buffer
	c_b_put_into(pc_b, str);
//	enable TXE interrupt to send these data
	LL_USART_EnableIT_TXE(USART1);
}

void execute (int argc, const char * const * argv) {
	int i = 0;
	while (i < argc) {
		if (strcmp (argv[i], _CMD_HELP) == 0) { // help
			print ("microrl v");
			print (_VER);
			print("\n\r");
		} else if (strcmp (argv[i], _CMD_CLEAR) == 0) { // clear
			print ("\033[2J");    // ESC seq for clear entire screen
			print ("\033[H");     // ESC seq for move cursor at left-top corner
		} else if ((strcmp (argv[i], _CMD_SET) == 0) || (strcmp (argv[i], _CMD_CLR) == 0)) {
			if (++i < argc) {
				int val = strcmp (argv[i-1], _CMD_CLR);
				unsigned char * port = NULL;
				int pin = 0;
				if (strcmp (argv[i], _SCMD_PD) == 0) {
	//						port = (unsigned char *)&PORTD;
				} else if (strcmp (argv[i], _SCMD_PB) == 0) {
	//						port = (unsigned char *)&PORTB;
				} else {
					print ("only '");
					print (_SCMD_PB);
					print ("' and '");
					print (_SCMD_PD);
					print ("' support\n\r");
					break;
				}
				if (++i < argc) {
					pin = atoi (argv[i]);
	//						set_port_val (port, pin, val);
					break;
				} else {
					print ("specify pin number, use Tab\n\r");
					break;
				}
			} else {
					print ("specify port, use Tab\n\r");
					break;
			}
		} else if (strcmp (argv[i], _CMD_CAN) == 0){ //
		} else {
			print ("command: '");
			print ((char*)argv[i]);
			print ("' Not found.\n\r");
		}
		i++;
	}
	print(prompt_default);
}
