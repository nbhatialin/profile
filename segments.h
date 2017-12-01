/*
 *     segments.h
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     An interface to work with and store segment representations in a UM
 *     
 */ 
#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <stdlib.h>
#include <stdint.h>

#define T Segments_T
typedef struct T *T;
typedef uint32_t Umsegment_Id;

T new_segments(void *init_program);
Umsegment_Id add_segment(T segments, uint32_t length);
int program_size(T segments);
void replace_program(T segments, Umsegment_Id id);
uint32_t get_program_elem(T segments, uint32_t index);
void remove_segment(T segments, Umsegment_Id id);
uint32_t get_segment_elem(T segments, Umsegment_Id id, uint32_t index);
uint32_t put_segment_elem(T segments, Umsegment_Id id, uint32_t index, 
							uint32_t add);
void free_segments(T segments);

#undef Umsegment_Id
#undef T
#endif