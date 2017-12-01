/*
 *     um_load.h
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     Exports just one function which takes in a file and parses its 
 *     informaion into a sequence, returning a void pointer to this seq
 *     
 */ 
#ifndef UM_LOAD_H
#define UM_LOAD_H

#include <stdlib.h>
#include <stdio.h>

void *load_um(FILE *filename);

#endif