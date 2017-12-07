/*
 * circular_buffer.c
 * Author: m.chichikalov@outlook.com
 * TODO  - Add test with different data type buffer.
 */
#include <stdlib.h>
#include <stdio.h>
#include "cicle_buffer.h"

//*****************************************************************************
// static functions
static void c_b_modify_buffer(circular_buffer_t *, const TYPE_OF_ELEMENT_OF_BUFFER *);
// -- other I used in tests. Can't declare as static.
TYPE_OF_ELEMENT_OF_BUFFER* c_b_move_position(circular_buffer_t *pThis, TYPE_OF_ELEMENT_OF_BUFFER* pPosition);
void c_b_move_pHead(circular_buffer_t *pThis);
void c_b_move_pTail(circular_buffer_t *pThis);
TYPE_OF_ELEMENT_OF_BUFFER* c_b_get_pHead(circular_buffer_t *pThis);
TYPE_OF_ELEMENT_OF_BUFFER* c_b_get_pTail(circular_buffer_t *pThis);


//*****************************************************************************
int c_b_init(circular_buffer_t *pThis){
	pThis->pHead = pThis->buffer;
	pThis->pTail = pThis->buffer;
	return 1;
}

//*****************************************************************************
// Put one element in buffer. If not enough space return -1.
// After the data was put return available space in buffer.
//*****************************************************************************
int c_b_put(circular_buffer_t *pThis, void *pData){
	int free_space_in_buffer = c_b_get_free_space(pThis);
	if (free_space_in_buffer == 0)
		return -1;
	c_b_modify_buffer(pThis, (TYPE_OF_ELEMENT_OF_BUFFER *)pData);
	return (free_space_in_buffer - 1);
}

//*****************************************************************************
// Put string into buffer. If it has not enough space will return -1.
// String should be NULL determined.
// After the data was put return available space in buffer.
//*****************************************************************************
int c_b_put_string(circular_buffer_t *pThis, const char *str){
	int len = 0;
	while (str[len++] != 0x0){	}
	int free_space_in_buffer = c_b_get_free_space(pThis);
	if (len > free_space_in_buffer)
		return -1;
	for (int i=0; i<len; i++){
		c_b_modify_buffer(pThis, (TYPE_OF_ELEMENT_OF_BUFFER*) &str[i]);
	}
	return (free_space_in_buffer - len);
}

//*****************************************************************************
// todo Put "len" elements in buffer. If not enough space return -1.
// After the data was put return available space in buffer.
//*****************************************************************************
int c_b_put_mem(circular_buffer_t *pThis, TYPE_OF_ELEMENT_OF_BUFFER *pointer, int len){
	int free_space_in_buffer = c_b_get_free_space(pThis);
	if (len > free_space_in_buffer)
		return -1;
	for (int i=0; i<len; i++){
		c_b_modify_buffer(pThis, pointer++);
	}
	return (free_space_in_buffer - len);
}

//*****************************************************************************
static void c_b_modify_buffer(circular_buffer_t *pThis, const TYPE_OF_ELEMENT_OF_BUFFER *pData){
	*pThis->pHead = *pData;
	pThis->pHead = c_b_move_position(pThis, pThis->pHead);
}

//*****************************************************************************
inline TYPE_OF_ELEMENT_OF_BUFFER* c_b_get_pHead(circular_buffer_t *pThis){
	return pThis->pHead;
}

//*****************************************************************************
inline TYPE_OF_ELEMENT_OF_BUFFER* c_b_get_pTail(circular_buffer_t *pThis){
	return pThis->pTail;
}

//*****************************************************************************
TYPE_OF_ELEMENT_OF_BUFFER* c_b_move_position(circular_buffer_t *pThis, TYPE_OF_ELEMENT_OF_BUFFER *pPosition){
	pPosition ++;
	if (pPosition > (pThis->buffer + (LEN_BUFFER - 1)))
		pPosition = pThis->buffer;
	return pPosition;
}

//*****************************************************************************
void c_b_move_pHead(circular_buffer_t *pThis){
	pThis->pHead = c_b_move_position(pThis, pThis->pHead);
}

//*****************************************************************************
void c_b_move_pTail(circular_buffer_t *pThis){
	pThis->pTail = c_b_move_position(pThis, pThis->pTail);
}


//*****************************************************************************
// TODO now better!
TYPE_OF_ELEMENT_OF_BUFFER c_b_get(circular_buffer_t *pThis){
	TYPE_OF_ELEMENT_OF_BUFFER temp = *(pThis->pTail);
	if (c_b_get_free_space(pThis) != (LEN_BUFFER - 1))
		c_b_move_pTail(pThis);
	return temp;
}

//*****************************************************************************
// TODO definition
int c_b_get_mem(circular_buffer_t *pThis, TYPE_OF_ELEMENT_OF_BUFFER *pointer,  int value){
	int size = ((LEN_BUFFER - 1) - c_b_get_free_space(pThis));
	if (size < value)
		return 0;
	for (int i=0; i<value; i++){
		*(pointer++) = c_b_get(pThis);
	}
	return 1;
}

//*****************************************************************************
int c_b_get_string(circular_buffer_t *pThis, char *str){
	int pos_termination  = c_b_find_character(pThis, 0x00);
	if (pos_termination <= 1)
		return 0;
	for (int i=0; i < pos_termination; i++){
		*(str++) = *(pThis->pTail);
		c_b_move_pTail(pThis);
	}
	return 1;
}

//*****************************************************************************
int c_b_get_free_space(circular_buffer_t *pThis){
	int temp = ((pThis->pHead + 1 ) - pThis->pTail);
	if (temp > 0)
		return (LEN_BUFFER - temp);
	else
		return abs(temp);
}

//*****************************************************************************
int c_b_find_character(circular_buffer_t *pThis, char character){
	TYPE_OF_ELEMENT_OF_BUFFER* pTailPseudo = pThis->pTail;
	int position = 0;
	do {
		position++;
		if ((char)*pTailPseudo == character)
			return position;
		pTailPseudo = c_b_move_position(pThis, pTailPseudo);
	} while (pTailPseudo != pThis->pHead);
	return 0;
}


















