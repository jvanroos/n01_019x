/*
 * n01
 *
 * Memory.h
 *
 */

#ifndef _INC_MEMORY_H
#define _INC_MEMORY_H

/* Include Files */

/* Define */

/* Struct */

/* Function Prototypes */
void *mem_alloc(const DWORD size);
void *mem_calloc(const DWORD size);
void mem_free(void **mem);

#ifdef _DEBUG
void mem_debug(void);
#endif

#endif
