/*
 * CLI_Functionality.h
 *
 *  Created on: 4 окт. 2017 г.
 *      Author: user
 */

#ifndef CLI_FUNCTIONALITY_H_
#define CLI_FUNCTIONALITY_H_

#define LENGTH_BUFFER                         64
/*-------------------- buffer for DMA USART1-TX --------------------*/

extern char bufDMAtoUSART[LENGTH_BUFFER];

typedef struct {
	char buffer[LENGTH_BUFFER];
	char* pDMA;
	char* pActual;
}bufDMAtoUSART_t;

#define START_BUFFER                          (uint32_t)(bufDMAtoUSART)
#define END_BUFFER                            (uint32_t)(bufDMAtoUSART + (LENGTH_BUFFER - 1))


// print to stream callback
void print (const char * str);

// execute callback
int execute (int argc, const char * const * argv);

#endif /* CLI_FUNCTIONALITY_H_ */
