/*
 *     um.h
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     The interface of the UM including all of its instructions
 *     
 */ 
#ifndef UM_H
#define UM_H

#include <stdlib.h>
#include <stdint.h>

#include "segments.h"

#define T Um_T
typedef struct T *T;
typedef uint32_t register_val;
typedef uint32_t counter_type;

T new_um(void *program);
int program_length(T um);
uint32_t next_program_elem(T um);
void free_um(T um);

void cmov(Um_T um, int a, int b, int c);
void sload(Um_T um, int a, int b, int c); 
void sstore(Um_T um, int a, int b, int c);
void add(Um_T um, int a, int b, int c);
void mul(Um_T um, int a, int b, int c);
void divide(Um_T um, int a, int b, int c);
void nand(Um_T um, int a, int b, int c);
void halt(T um, int a, int b, int c);
void map(T um, int a, int b, int c);
void unmap(T um, int a, int b, int c);
void out(T um, int a, int b, int c);
void in(T um, int a, int b, int c);
void loadp(Um_T um, int a, int b, int c);
void lv(T um, int a);

#undef register_val
#undef T
#endif