/*
 *     sequence.h
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     An interface to work with and store segment representations in a UM
 *     
 */ 
#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdlib.h>
#include <stdint.h>

#define T Sequence_T
typedef struct T *T;
typedef void * elem_ptr;

int seq_length(T seq);
elem_ptr seq_get(T seq, int index);
void seq_addhi(T seq, elem_ptr to_add);
elem_ptr seq_remhi(T seq);
T new_seq(int hint);
void free_seq(T seq);
void seq_put(T seq, int index, elem_ptr insert);
void ensureCapacity(T seq);
#undef T
#undef elem_ptr
#endif