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

#define S Sequence_T
typedef struct S *S;
typedef void * elem_ptr;

int seq_length(S seq);
elem_ptr seq_get(S seq, int index);
void seq_addhi(S seq, elem_ptr to_add);
elem_ptr seq_remhi(S seq);
S new_seq(int hint);
void free_seq(S *seq);
void seq_put(S seq, int index, elem_ptr insert);
void ensureCapacity(S seq);
#undef S
#undef elem_ptr
#endif