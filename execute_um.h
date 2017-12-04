/*
 *     execute_um.h
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     The interface of the UM including all of its instructions
 *     
 */ 
#ifndef EXECUTE_UM_H
#define EXECUTE_UM_H

#include <stdlib.h>
#include <stdint.h>

#define T Um_T
typedef struct T *T;
typedef uint32_t register_val;
typedef uint32_t counter_type;
typedef uint32_t Umsegment_Id;


#undef register_val
#undef counter_type
#undef Umsegment_Id
#undef T
#endif