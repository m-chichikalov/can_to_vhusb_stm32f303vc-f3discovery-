/*
 * cicle_buffer.c
 * Author: m.chichikalov@outlook.com
 */

#include "cicle_buffer.h"

//*****************************************************************************
// static functions
static void c_b_modify_buffer(cicle_buffer_t* pThis, char val);
static void c_b_set_p_actual(cicle_buffer_t* pThis, char* new_pointer);
static void c_b_set_p_used(cicle_buffer_t* pThis, char* new_pointer);

//*****************************************************************************
int c_b_init(cicle_buffer_t* pThis){
	pThis->p_actual = pThis->buffer;
	pThis->p_used   = pThis->buffer;
	return 1;
}

//*****************************************************************************
void c_b_put_into(cicle_buffer_t* pThis, const char* str){
	int i = 0;
	do  {
			if ((pThis->p_actual + 1) == pThis->p_used)
				break;
			c_b_modify_buffer(pThis, str[i++]);
		} while (str[i-1] != (char)0x0);
}

//*****************************************************************************
static void c_b_modify_buffer(cicle_buffer_t* pThis, char val){
	*pThis->p_actual = val;
	c_b_set_p_actual(pThis, (pThis->p_actual + 1));
}

//*****************************************************************************
inline char* c_b_get_p_actual(cicle_buffer_t* pThis){
	return pThis->p_actual;
}

//*****************************************************************************
inline char* c_b_get_p_used(cicle_buffer_t* pThis){
	return pThis->p_used;
}

//*****************************************************************************
inline void c_b_set_p_actual(cicle_buffer_t* pThis, char* index){
	pThis->p_actual = index;
	if (pThis->p_actual > (pThis->buffer + (LEN_BUFFER - 1)))
			pThis->p_actual = pThis->buffer;
}

//*****************************************************************************
inline void c_b_set_p_used(cicle_buffer_t* pThis, char* index){
	pThis->p_used = index;
	if (pThis->p_used > (pThis->buffer + (LEN_BUFFER - 1)))
			pThis->p_used = pThis->buffer;
}


//*****************************************************************************
char c_b_get_from(cicle_buffer_t* pThis){
	char temp = *(pThis->p_used);
	if ((pThis->p_actual) == (pThis->p_used)){
		return 0x0;
	}
	else {
	c_b_set_p_used(pThis, (pThis->p_used + 1));
	return temp;
	}
}


