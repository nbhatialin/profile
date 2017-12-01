/*
 *     um.c
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     The implementation of the UM - definitions of all of the instructions
 *     in addition to new, free, length, and next methods
 *     
 */ 
#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "bitpack.h"

#include "um.h"

#define T Um_T
#define max_bound 4294967296
#define eof 26
#define num_registers 8

struct T {
       	Segments_T segments;
       	register_val registers[num_registers];
        counter_type program_counter; 
};

/*
* Creates a new um variable. Take in a void * (information hiding wow!) 
* of the program to init with and returns a Um_T
*/
T new_um(void *program)
{
        T um_new;
        NEW(um_new);
        um_new->segments = new_segments(program);
        um_new->program_counter = 0;
        for (int i = 0; i < num_registers; i++) {
                um_new->registers[i] = 0;            
        }
        return um_new;
}

/*
* Returns the length of the program, for use by execute_um
*/
int program_length(T um) 
{
        return program_size(um->segments);
}

/*
* Frees all allocated memory from um.
*/
void free_um(T um)
{
        free_segments(um->segments);
        FREE(um);
}

/*
* pulls the next word, to be called after an instructions so the program 
* has iterated
*/
uint32_t next_program_elem(T um) {
        return get_program_elem(um->segments, um->program_counter);
}

/*
* Cmov: Takes in the um and 3 register indexes. If the register at c isnt 0,
* copies register at b into register at a.
*/
void cmov(T um, int a, int b, int c)
{
        if (um->registers[c] != 0) {
                um->registers[a] = um->registers[b];
        }
        um->program_counter++;
}

/*
* Sload: Takes in the um and 3 register indexes. loads segments(b,c) into 
* register at a.
*/
void sload(T um, int a, int b, int c)
{
        um->registers[a] = get_segment_elem(um->segments, um->registers[b], 
                um->registers[c]);
        um->program_counter++;
} 

/*
* Sstore: Takes in the um and 3 register indexes. puts value in c into 
* segments[a,b]
*/
void sstore(T um, int a, int b, int c)
{
        put_segment_elem(um->segments, um->registers[a], um->registers[b], 
                um->registers[c]);

        um->program_counter++;
}

/*
* Add: Takes in the um and 3 register indexes. adds values at b and c and 
* stores that result in a
*/
void add(T um, int a, int b, int c)
{
        um->registers[a] = (um->registers[b] + um->registers[c]) % max_bound;
        um->program_counter++;
}

/*
* Multiply: Takes in the um and 3 register indexes. multiplies values at b and c
* and stores that result in a
*/
void mul(T um, int a, int b, int c)
{
        um->registers[a] = (um->registers[b] * um->registers[c]) % max_bound;
        um->program_counter++;
}

/*
* Divide: Takes in the um and 3 register indexes. divides values b by val c 
* and stores that result in a. Fails if c is 0
*/
void divide(T um, int a, int b, int c)
{
        if (um->registers[c] == 0) {
                printf("poor\n");
                exit(1);
        }
        um->registers[a] = (um->registers[b] / um->registers[c]);
        um->program_counter++;
}

/*
* Nand: Takes in the um and 3 register indexes. nands values b & c 
* and stores that result in a.
*/
void nand(T um, int a, int b, int c)
{
        um->registers[a] = ~(um->registers[b] & um->registers[c]);
        um->program_counter++;
}

/*
* Halt: Takes in the um and 3 register indexes. frees all memory and exits
* peacefully
*/
void halt(T um, int a, int b, int c)
{
        (void)a;
        (void)b;
        (void)c;
        free_um(um);
        exit(0);
}

/*
* Map: Takes in the um and 3 register indexes. Adds a new segment with length
* c and stores the id in b
*/
void map(T um, int a, int b, int c)
{
        (void)a;
        um->registers[b] = add_segment(um->segments, um->registers[c]);
        um->program_counter++;
}

/*
* UnMap: Takes in the um and 3 register indexes. Removes the segment with id c
*/
void unmap(T um, int a, int b, int c)
{
        (void)a;
        (void)b;
        remove_segment(um->segments, um->registers[c]);
        um->program_counter++;
}

/*
* Out: Takes in the um and 3 register indexes. Prints the val in c as a char on
* stdout, it must be less than 256
*/
void out(T um, int a, int b, int c)
{
        (void)a;
        (void)b;
        assert(um->registers[c] < 256);
        putc(um->registers[c], stdout);
        um->program_counter++;
}

/*
* In: Takes in the um and 3 register indexes. Gets a char on stdin, it must be
* less than 256. Puts this val in c, or 255 if input is eof char
*/
void in(T um, int a, int b, int c)
{
        (void)a;
        (void)b;
        char input = getc(stdin);
        if (input == eof) {
                um->registers[c] = (max_bound - 1);
        }
        else {
                um->registers[c] = input;
        }
        um->program_counter++;
}

/*
* Loadp: Takes in the um and 3 register indexes. Replaces the program segment
* with segment of id b and sets the program counter to c
*/
void loadp(T um, int a, int b, int c)
{
        (void)a;
        if (um->registers[b] != 0) {
                replace_program(um->segments, um->registers[b]);
        }
        um->program_counter = um->registers[c];
}

/*
* Loadv: Takes in the um and 3 register indexes. Sets the value of a equal to
* the value stored in the rightmost 25 bits of the current word.
*/
void lv(T um, int a)
{
        uint32_t word = get_program_elem(um->segments, um->program_counter);
        uint32_t value = Bitpack_getu(word, 25, 0);
        um->registers[a] = value;
        um->program_counter++;
}

#undef T
