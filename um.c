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
#include "um.h"
#include "seq.h"
#include "uarray.h"
#include "except.h"

#define T Um_T
#define max_bound 4294967296
#define eof 26
#define num_registers 8
#define program_id 0
#define word_size 4

Except_T Too_large = {"Too many segments added"};

struct T {
        Seq_T program;
        Seq_T storage;
        Seq_T avail_id;
       	register_val registers[num_registers];
        counter_type program_counter; 
};

/*
* Our segment - mapped indicated whether the id space is free
*/
typedef struct seg 
{
        UArray_T segment;
        int mapped;
} *seg;

/*
* Creates a new um variable. Take in a void * (information hiding wow!) 
* of the program to init with and returns a Um_T
*/
T new_um(void *program)
{
        T um_new;
        NEW(um_new);

        um_new->storage = Seq_new(8);
        um_new->avail_id = Seq_new(10);
        um_new->program = program;

        Seq_addhi(um_new->storage, um_new->program);
        
        um_new->program_counter = 0;
        for (int i = 0; i < num_registers; i++) {
                um_new->registers[i] = 0;            
        }
        return um_new;
}

/*
* Frees all allocated memory from um.
*/
void free_um(T um)
{
        /* Replaced:
        free_segments(um->segments);
        FREE(um); 
        */

        //free_seq_elems(um->program);
        int pro_length = Seq_length(um->program);
        uint32_t *pro_elem;
        for (int i = 0; i < pro_length; i++) {
                pro_elem = Seq_remhi(um->program);
                FREE(pro_elem);
        }


        Seq_free(&um->program); //Stays

        //free_mapped(um->storage);
        int stor_length = Seq_length(um->storage);
        for (int i = 1; i < stor_length; i++) {
                seg stor_seg = Seq_get(um->storage, i);
                if (stor_seg->mapped == 1) {
                        UArray_free(&(stor_seg->segment));
                }
                FREE(stor_seg);
        }

        Seq_free(&um->storage); // Stays

        //free_seq_elems(um->avail_id);
        int id_length = Seq_length(um->avail_id);
        uint32_t *id_elem;
        for (int i = 0; i < id_length; i++) {
                id_elem = Seq_remhi(um->avail_id);
                FREE(id_elem);
        }

        Seq_free(&um->avail_id); // Same
      
        FREE(um);
}

/*
* pulls the next word, to be called after an instructions so the program 
* has iterated
*/
uint32_t next_program_elem(T um) {
        /*Replaced
        return get_program_elem(um->segments, um->program_counter);
        */
        // Param bail Only use um->program_counter once, pass as index
        uint32_t *elem = Seq_get(um->program, um->program_counter);
        return *elem;
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
        /* Replaced:
        um->registers[a] = get_segment_elem(um->segments, um->registers[b], 
                um->registers[c]);
                */

        // Paramaters
        uint32_t id = um->registers[b];
        uint32_t index = um->registers[c];
        //check_unmap(segments, id);
        if (Seq_length(um->storage) <= (int)id) {
                printf("poor\n");
                exit(1);
        }
        if (id != program_id) {
                seg check_seg = Seq_get(um->storage, id);
                if (check_seg->mapped == 0) {
                        printf("poor\n");
                        exit(1);
                }
        }

        if (id == program_id) {
                uint32_t *index_val = Seq_get(um->program, index);
                um->registers[a] = *index_val;
        } else {
                seg get_seg = Seq_get(um->storage, id);
                UArray_T get_segment = get_seg->segment;
                uint32_t *index_val = UArray_at(get_segment, index);
                um->registers[a] = *index_val;
        }

        um->program_counter++;
} 

/*
* Sstore: Takes in the um and 3 register indexes. puts value in c into 
* segments[a,b]
*/
void sstore(T um, int a, int b, int c)
{
        /*Replaced 
        put_segment_elem(um->segments, um->registers[a], um->registers[b], 
                um->registers[c]);
        */

        // Paramaters
        uint32_t id = um->registers[a];
        uint32_t index = um->registers[b];
        uint32_t add = um->registers[c];

        // check_unmap(segments, id);
        if (Seq_length(um->storage) <= (int)id) {
                printf("poor\n");
                exit(1);
        }
        if (id != program_id) {
                seg check_seg = Seq_get(um->storage, id);
                if (check_seg->mapped == 0) {
                        printf("poor\n");
                        exit(1);
                }
        }

        if (id == program_id) {
                uint32_t *to_free = Seq_get(um->program, index);
                *to_free = add; 
        }
        else {
                seg get_seg = Seq_get(um->storage, id);
                UArray_T get_segment = get_seg->segment;
                uint32_t *old_val = UArray_at(get_segment, index);
                *old_val = add;
        }

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
        /*Replaced
        um->registers[b] = add_segment(um->segments, um->registers[c]);
        */

        // Paramaters 
        uint32_t length = um->registers[c];

        UArray_T new_segment = UArray_new(length, word_size);
        if (Seq_length(um->avail_id) == 0) {
                seg add_seg;
                NEW(add_seg);
                add_seg->mapped = 1;
                add_seg->segment = new_segment;
                Seq_addhi(um->storage, add_seg);
                um->registers[b] = (Seq_length(um->storage) - 1);

        }
        else {
                Umsegment_Id *id_ptr = Seq_remhi(um->avail_id);
                Umsegment_Id id = *id_ptr;

                seg get_seg = Seq_get(um->storage, id);
                get_seg->segment = new_segment;
                get_seg->mapped = 1;

                FREE(id_ptr);
                um->registers[b] = id;
        }

        um->program_counter++;
}

/*
* UnMap: Takes in the um and 3 register indexes. Removes the segment with id c
*/
void unmap(T um, int a, int b, int c)
{
        (void)a;
        (void)b;
        /*Replaced
        remove_segment(um->segments, um->registers[c]);
        */

        // Paramaters
        uint32_t id = um->registers[c];

        //check_access(segments, id);
        if (id == program_id || Seq_length(um->storage) <= (int)id) {
                printf("poor\n");
                exit(1);
        }
        seg check_seg = Seq_get(um->storage, id);
        if (check_seg->mapped == 0) {
                printf("poor\n");
                exit(1);
        }

        /* Freeing the UArray when we need to add something to its spot */
        seg rem_segment = Seq_get(um->storage, id);
        rem_segment->mapped = 0;
        UArray_free(&rem_segment->segment);
        Umsegment_Id *id_ptr = NEW(id_ptr);
        *id_ptr = id;
        Seq_addhi(um->avail_id, id_ptr);

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
                /* Replaced
                replace_program(um->segments, um->registers[b]);
                */
                //Paramaters
                uint32_t id = um->registers[b];

                // UArray_T uarray_program = get_segment(segments, id);
                //check_access(segments, id);
                if (id == program_id || Seq_length(um->storage) <= (int)id) {
                        printf("poor\n");
                        exit(1);
                }
                seg get_seg = Seq_get(um->storage, id);
                if (get_seg->mapped == 0) {
                        printf("poor\n");
                        exit(1);
                }

                UArray_T uarray_program = get_seg->segment;

                int uarray_length = UArray_length(uarray_program);
                Seq_T seq_program = Seq_new(uarray_length);
                for (int i = 0; i < uarray_length; i++) {
                        uint32_t *curr_elem = UArray_at(uarray_program, i);
                        uint32_t *new_elem = NEW(new_elem);
                        *new_elem = *curr_elem;
                        Seq_addhi(seq_program, new_elem);
                }

                //free_seq_elems(um->program);
                int pro_length = Seq_length(um->program);
                uint32_t *pro_elem;
                for (int i = 0; i < pro_length; i++) {
                        pro_elem = Seq_remhi(um->program);
                        FREE(pro_elem);
                }
                Seq_free(&um->program);

                Seq_put(um->storage, program_id, seq_program);
                um->program = seq_program;
        }
        um->program_counter = um->registers[c];
}

/*
* Loadv: Takes in the um and 3 register indexes. Sets the value of a equal to
* the value stored in the rightmost 25 bits of the current word.
*/
void lv(T um, int a)
{
        /* Replaced 
        get_program_elem(um->segments, um->program_counter);
        */
        // Paramater bail , um->program_counter is index
        uint64_t *word = Seq_get(um->program, um->program_counter);
        
        uint32_t value = (*word << 39) >> 39;
        um->registers[a] = value;
        um->program_counter++;
}

#undef T

