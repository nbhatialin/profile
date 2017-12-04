/*
 *     execute_um.c
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     Is the main function of this UM. Calls load_um to read in the file, and
 *     run_um (defined below) to call instructions until either a halt is 
 *     called or the counter exceeds the length (contained restriction)
 *     
 */ 
#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "seq.h"
#include "uarray.h"
#include "except.h"

#include "um_load.h"
#include "execute_um.h"

#define T Um_T
#define max_bound 4294967296
#define eof 26
#define num_registers 8
#define program_id 0
#define word_size 4
#define max_ops 14

Except_T Too_large = {"Too many segments added"};

typedef enum Um_opcode {
        CMOV = 0, SLOAD = 1, SSTORE = 2, ADD = 3, MUL = 4, DIV = 5,
        NAND = 6, HALT = 7, ACTIVATE = 8, INACTIVATE = 9, OUT = 10, 
        IN = 11, LOADP = 12, LV = 13
} Um_opcode;

typedef void (*func_type)();

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


void run_um(Um_T um);
static T new_um(void *program);
static void free_um(T um);

int main(int argc, char *argv[]) 
{
        (void) argc;
        FILE *filename = fopen(argv[1], "r");
        assert(filename != NULL);
        Um_T um = new_um(load_um(filename));
        fclose(filename);
        run_um(um);
        free_um(um);

        return 0;
}

/*
* Takes in a UM. Initializes an array of functions, and for each instruction
* given by the program we call that operation with the given register info.
* contains an "infinite" loop, which is quit via halts or by going out
* of bounds
*/
void run_um(Um_T um) 
{
        /* Replaced :'(
        func_type operations[max_ops - 1];
        operations[CMOV] = cmov;
        operations[SLOAD] = sload;
        operations[SSTORE] = sstore;
        operations[ADD] = add;
        operations[MUL] = mul;
        operations[DIV] = divide;
        operations[NAND] = nand;
        operations[HALT] = halt;
        operations[ACTIVATE] = map;
        operations[INACTIVATE] = unmap;
        operations[OUT] = out;
        operations[IN] = in;
        operations[LOADP] = loadp;
        */
        Um_opcode opcode = 0;
        while(1) {
                /* Replaced
                uint64_t curr_word = next_program_elem(um);
                */
                uint32_t *elem = Seq_get(um->program, um->program_counter);
                uint64_t curr_word = *elem;


                opcode = (curr_word << 32) >> 60; 
                assert(opcode <= max_ops);
                if (opcode == LV) {
                        uint32_t a = (curr_word << 36) >> 61; 
                        /* Replaced
                        lv(um, a);
                        */
                        uint64_t *word = Seq_get(um->program, um->program_counter);
                        uint32_t value = (*word << 39) >> 39;
                        um->registers[a] = value;
                        um->program_counter++;
                }
                else {
                // Replaced: operations[opcode](um, a, b, c);

                        uint32_t c = (curr_word << 61) >> 61; 
                        uint32_t b = (curr_word << 58) >> 61; 
                        uint32_t a = (curr_word << 55) >> 61;
                        if (opcode == CMOV) {
                                if (um->registers[c] != 0) {
                                        um->registers[a] = um->registers[b];
                                }
                                um->program_counter++;
                        }
                        else if (opcode == SLOAD) {
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
                        else if (opcode == SSTORE) {
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
                        else if (opcode == ADD) {
                                um->registers[a] = (um->registers[b] + um->registers[c]) % max_bound;
                                um->program_counter++;
                        }
                        else if (opcode == MUL) {
                                um->registers[a] = (um->registers[b] * um->registers[c]) % max_bound;
                                um->program_counter++;
                        }
                        else if (opcode == DIV) {
                                if (um->registers[c] == 0) {
                                        printf("poor\n");
                                        exit(1);
                                }
                                um->registers[a] = (um->registers[b] / um->registers[c]);
                                um->program_counter++;
                        }
                        else if (opcode == NAND) {
                                um->registers[a] = ~(um->registers[b] & um->registers[c]);
                                um->program_counter++;
                        }
                        else if (opcode == HALT) {
                                free_um(um);
                                exit(0);        
                        }
                        else if (opcode == ACTIVATE) {
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
                        else if (opcode == INACTIVATE) {
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
                        else if (opcode == OUT) {
                                assert(um->registers[c] < 256);
                                putc(um->registers[c], stdout);
                                um->program_counter++;
                        }
                        else if (opcode == IN) {
                                char input = getc(stdin);
                                if (input == eof) {
                                        um->registers[c] = (max_bound - 1);
                                }
                                else {
                                        um->registers[c] = input;
                                }
                                um->program_counter++;
                        }
                        else if (opcode == LOADP) {
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
                }
        }
}

/*
* Creates a new um variable. Take in a void * (information hiding wow!) 
* of the program to init with and returns a Um_T
*/
static T new_um(void *program)
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
static void free_um(T um)
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

