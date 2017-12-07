/*
 * cicle_buffer.h
 * Author: m.chichikalov@outlook.com
 */
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

//*****************************************************************************
// You should define the length of buffer in LEN_BUFFER
#ifndef LEN_BUFFER
	#define LEN_BUFFER                    256
#endif /* LEN_BUFFER */

#ifdef TEST_CHAR
	#define TYPE_OF_ELEMENT_OF_BUFFER     unsigned char
#endif /* TEST_CHAR */

#ifdef TEST_INT
	#define TYPE_OF_ELEMENT_OF_BUFFER     int
#endif /* TEST_INT */

#ifndef TYPE_OF_ELEMENT_OF_BUFFER
	#define TYPE_OF_ELEMENT_OF_BUFFER     char
#endif /* TYPE_OF_ELEMENT_OF_BUFFER */

//*****************************************************************************

typedef struct {
	TYPE_OF_ELEMENT_OF_BUFFER buffer[LEN_BUFFER];
	TYPE_OF_ELEMENT_OF_BUFFER *pHead;
	TYPE_OF_ELEMENT_OF_BUFFER *pTail;
}circular_buffer_t ;

int c_b_init(circular_buffer_t *buf);

/*
 * Function returns available free space in the buffer.
 */
int c_b_get_free_space(circular_buffer_t *buf);

/*
 *
 */
int c_b_find_character(circular_buffer_t *buf, char character);

/*
 * Copy one single element into the ring buffer.
 * Returns the ring buffer's free space.
 *
 * It is NOT possible function will not start copying and return -1.
 */
int c_b_put(circular_buffer_t *buf, void *element);

/*
 * Copy string str into the ring buffer.
 * Returns the ring buffer's free space.
 *
 * It is NOT possible to copy string longer than is available in the
 * buffer; If it happens, function does not start copying and return -1.
 */
int c_b_put_string(circular_buffer_t *buf, const char *str);

/*
 * Read string from the ring buffer.
 * If
 */
int c_b_get_string(circular_buffer_t *buf, char *str);

/*
 * Copy n (bytes) from a memory into the ring buffer.
 * Returns the ring buffer's free space.
 *
 * It is NOT possible to copy more data from src than is available in the
 * buffer; If it happens, function return -1.
 */

int c_b_put_mem   (circular_buffer_t *buf, TYPE_OF_ELEMENT_OF_BUFFER *pointer, int len);

/*
 * Read 1 element from the ring buffer.
 */
TYPE_OF_ELEMENT_OF_BUFFER c_b_get(circular_buffer_t *buf);

/*
 * Copy n (bytes) from the ring buffer into memory.
 *
 */
int c_b_get_mem(circular_buffer_t *pThis, TYPE_OF_ELEMENT_OF_BUFFER *pointer,  int value);
#endif /* CIRCULAR_BUFFER_H */

