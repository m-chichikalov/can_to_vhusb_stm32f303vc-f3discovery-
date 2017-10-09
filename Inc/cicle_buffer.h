/*
 * cicle_buffer.h
 * Author: m.chichikalov@outlook.com
 */
#ifndef CICLE_BUFFER_H_
#define CICLE_BUFFER_H_

//*****************************************************************************
// You should define the length of buffer in LEN_BUFFER
#define LEN_BUFFER               256
//
//
//
//
//
//
//*****************************************************************************

//typedef enum
//{
//  false = 0,
//  true = !false
//}bollean;

typedef struct {
	char buffer[LEN_BUFFER];
	char* p_actual;
	char* p_used;
}cicle_buffer_t;


int c_b_init(cicle_buffer_t* pThis);
char* c_b_put_into(cicle_buffer_t* pThis, const char* str);
char c_b_get_from(cicle_buffer_t* pThis);
char* c_b_get_p_actual(cicle_buffer_t* pThis);
char* c_b_get_p_used(cicle_buffer_t* pThis);
void c_b_set_p_actual(cicle_buffer_t* pThis, char* new_pointer);
void c_b_set_p_used(cicle_buffer_t* pThis, char* new_pointer);


//*****************************************************************************
// static functions
static void c_b_modify_buffer(cicle_buffer_t* pThis, char val);

#endif /* CICLE_BUFFER_H_ */
