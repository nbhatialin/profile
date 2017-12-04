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
#include "uarray.h"
#include "except.h"

#include "um_load.h"
#include "execute_um.h"

#define T Um_T
#define S Sequence_T
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
        Sequence_T program;
        Sequence_T storage;
        Sequence_T avail_id;
        register_val registers[num_registers];
        counter_type program_counter; 
};

struct S {
        elem_ptr *sequence;
        int sequenceCapacity;
        int length;
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
                //uint32_t *elem = seq_get(um->program, um->program_counter);
                // Paramaters
                int index = um->program_counter;
                assert(index < um->program->length);
                assert(index >= 0);
                uint32_t *elem = um->program->sequence[index];

                uint64_t curr_word = *elem;


                opcode = (curr_word << 32) >> 60; 
                assert(opcode <= max_ops);
                if (opcode == LV) {
                        uint32_t a = (curr_word << 36) >> 61; 
                        /* Replaced
                        lv(um, a);
                        */
                        //uint64_t *word = seq_get(um->program, um->program_counter);
                        uint32_t value = (curr_word << 39) >> 39;
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
                                int id = um->registers[b];
                                int index = um->registers[c];
                                //check_unmap(segments, id);
                                if ((um->storage->length) <= (int)id) {
                                        printf("poor\n");
                                        exit(1);
                                }

                                if (id == program_id) {
                                        //uint32_t *index_val = seq_get(um->program, index);
                                        assert(index < um->program->length);
                                        assert(index >= 0);
                                        uint32_t *index_val = um->program->sequence[index];
                                        um->registers[a] = *index_val;
                                } else {
                                        //seg check_seg = seq_get(um->storage, id);
                                        assert(id < um->storage->length);
                                        assert(id >= 0);
                                        seg check_seg = um->storage->sequence[id];
                                        if (check_seg->mapped == 0) {
                                                printf("poor\n");
                                                exit(1);
                                        }
                                        //seg get_seg = seq_get(um->storage, id);
                                        UArray_T get_segment = check_seg->segment;
                                        uint32_t *index_val = UArray_at(get_segment, index);
                                        um->registers[a] = *index_val;
                                }

                                um->program_counter++;
                        }
                        else if (opcode == SSTORE) {
                                int id = um->registers[a];
                                int index = um->registers[b];
                                uint32_t add = um->registers[c];

                                // check_unmap(segments, id);
                                if (um->storage->length <= (int)id) {
                                        printf("poor\n");
                                        exit(1);
                                }

                                if (id == program_id) {
                                        //uint32_t *index_val = seq_get(um->program, index);

                                        assert(index < um->program->length);
                                        assert(index >= 0);
                                        uint32_t *index_val = um->program->sequence[index];
                                        *index_val = add; 
                                }
                                else {
                                        //seg check_seg = seq_get(um->storage, id);
                                        assert(id < um->storage->length);
                                        assert(id >= 0);
                                        seg check_seg = um->storage->sequence[id];
                                        if (check_seg->mapped == 0) {
                                                printf("poor\n");
                                                exit(1);
                                        }
                                        //seg get_seg = seq_get(um->storage, id);
                                        UArray_T get_segment = check_seg->segment;
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
                                if (um->avail_id->length == 0) {
                                        seg add_seg;
                                        NEW(add_seg);
                                        add_seg->mapped = 1;
                                        add_seg->segment = new_segment;
                                        //seq_addhi(um->storage, add_seg);

                                        int size = um->storage->length;
                                        int cap = um->storage->sequenceCapacity;
                                        if (size == cap) {
                                                //ensureCapacity(um->storage);
                                                int desiredCap = (um->storage->sequenceCapacity) * 2 + 2;
                                                elem_ptr *tempSequence = malloc(sizeof(elem_ptr)*desiredCap);
                                                
                                                for (int i = 0; i < um->storage->sequenceCapacity; i++){
                                                        tempSequence[i] = um->storage->sequence[i];
                                                }

                                                free(um->storage->sequence);
                                                um->storage->sequence = tempSequence;

                                                um->storage->sequenceCapacity = desiredCap;
                                        }
                                        
                                        um->storage->sequence[size] = add_seg;
                                        um->storage->length = size + 1;

                                        um->registers[b] = (um->storage->length - 1);

                                }
                                else {
                                        //Umsegment_Id *id_ptr = seq_remhi(um->avail_id);

                                        assert(um->avail_id->length != 0);
                                        um->avail_id->length--;
                                        Umsegment_Id *id_ptr = um->avail_id->sequence[um->avail_id->length];

                                        int id = *id_ptr;

                                        //seg get_seg = seq_get(um->storage, id);

                                        assert(id < um->storage->length);
                                        assert(id >= 0);
                                        seg get_seg = um->storage->sequence[id];

                                        get_seg->segment = new_segment;
                                        get_seg->mapped = 1;

                                        FREE(id_ptr);
                                        um->registers[b] = id;
                                }

                                um->program_counter++;
                        }
                        else if (opcode == INACTIVATE) {
                                int id = um->registers[c];

                                //check_access(segments, id);
                                if (id == program_id || um->storage->length <= (int)id) {
                                        printf("poor\n");
                                        exit(1);
                                }
                                //seg check_seg = seq_get(um->storage, id);
                                assert(id < um->storage->length);
                                assert(id >= 0);
                                seg check_seg = um->storage->sequence[id];

                                if (check_seg->mapped == 0) {
                                        printf("poor\n");
                                        exit(1);
                                }

                                /* Freeing the UArray when we need to add something to its spot */
                                //seg rem_segment = seq_get(um->storage, id);

                                check_seg->mapped = 0;
                                UArray_free(&check_seg->segment);
                                Umsegment_Id *id_ptr = NEW(id_ptr);
                                *id_ptr = id;
                                //seq_addhi(um->avail_id, id_ptr);

                                int size = um->avail_id->length;
                                int cap = um->avail_id->sequenceCapacity;
                                if (size == cap) {
                                        //ensureCapacity(um->storage);
                                        int desiredCap = (um->avail_id->sequenceCapacity) * 2 + 2;
                                        elem_ptr *tempSequence = malloc(sizeof(elem_ptr)*desiredCap);
                                        
                                        for (int i = 0; i < um->avail_id->sequenceCapacity; i++){
                                                tempSequence[i] = um->avail_id->sequence[i];
                                        }

                                        free(um->avail_id->sequence);
                                        um->avail_id->sequence = tempSequence;

                                        um->avail_id->sequenceCapacity = desiredCap;
                                }
                                
                                um->avail_id->sequence[size] = id_ptr;
                                um->avail_id->length = size + 1;

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
                                        int id = um->registers[b];

                                        // UArray_T uarray_program = get_segment(segments, id);
                                        //check_access(segments, id);
                                        if (id == program_id || um->storage->length <= (int)id) {
                                                printf("poor\n");
                                                exit(1);
                                        }
                                        //seg get_seg = seq_get(um->storage, id);
                                        assert(id < um->storage->length);
                                        assert(id >= 0);
                                        seg get_seg = um->storage->sequence[id];

                                        if (get_seg->mapped == 0) {
                                                printf("poor\n");
                                                exit(1);
                                        }

                                        UArray_T uarray_program = get_seg->segment;

                                        int uarray_length = UArray_length(uarray_program);
                                        //Sequence_T seq_program = new_seq(uarray_length);

                                        S seq;
                                        NEW(seq);
                                        seq->sequenceCapacity = uarray_length;
                                        seq->sequence = malloc(sizeof(elem_ptr)*(seq->sequenceCapacity));
                                        seq->length = 0;
                                        Sequence_T seq_program = seq;

                                        for (int i = 0; i < uarray_length; i++) {
                                                uint32_t *curr_elem = UArray_at(uarray_program, i);
                                                uint32_t *new_elem = NEW(new_elem);
                                                *new_elem = *curr_elem;
                                                // seq_addhi(seq_program, new_elem);
                                                int size = seq_program->length;
                                                int cap = seq_program->sequenceCapacity;
                                                if (size == cap) {
                                                        // ensureCapacity(seq_program);
                                                        int desiredCap = (seq_program->sequenceCapacity) * 2 + 2;
                                                        elem_ptr *tempSequence = malloc(sizeof(elem_ptr)*desiredCap);
                                                        
                                                        for (int i = 0; i < seq_program->sequenceCapacity; i++){
                                                                tempSequence[i] = seq_program->sequence[i];
                                                        }

                                                        free(seq_program->sequence);
                                                        seq_program->sequence = tempSequence;

                                                        seq_program->sequenceCapacity = desiredCap;
                                                }
                                                
                                                seq_program->sequence[size] = new_elem;
                                                seq_program->length = size + 1;
                                        }

                                        //free_seq_elems(um->program);
                                        int pro_length = um->program->length;
                                        uint32_t *pro_elem;
                                        for (int i = 0; i < pro_length; i++) {
                                                //pro_elem = seq_remhi(um->program);
                                                um->program->length--;
                                                pro_elem = um->program->sequence[um->program->length];
                                                FREE(pro_elem);
                                        }
                                        //free_seq(&um->program);
                                        free((um->program)->sequence);
                                        FREE(um->program);

                                        //seq_put(um->storage, program_id, seq_program);
                                        assert(program_id < um->storage->length);
                                        assert(program_id >= 0);
                                        um->storage->sequence[program_id] = seq_program;

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

        //um_new->storage = new_seq(8);
        S stor_seq;
        NEW(stor_seq);
        stor_seq->sequenceCapacity = 8;
        stor_seq->sequence = malloc(sizeof(elem_ptr)*(stor_seq->sequenceCapacity));
        stor_seq->length = 0;
        um_new->storage = stor_seq;

        //um_new->avail_id = new_seq(10);
        S id_seq;
        NEW(id_seq);
        id_seq->sequenceCapacity = 10;
        id_seq->sequence = malloc(sizeof(elem_ptr)*(id_seq->sequenceCapacity));
        id_seq->length = 0;
        um_new->avail_id = id_seq;

        um_new->program = program;

        //seq_addhi(um_new->storage, um_new->program);
        int size = um_new->storage->length;
        int cap = um_new->storage->sequenceCapacity;
        if (size == cap) {
                int desiredCap = (um_new->storage->sequenceCapacity) * 2 + 2;
                elem_ptr *tempSequence = malloc(sizeof(elem_ptr)*desiredCap);
                
                for (int i = 0; i < um_new->storage->sequenceCapacity; i++){
                        tempSequence[i] = um_new->storage->sequence[i];
                }

                free(um_new->storage->sequence);
                um_new->storage->sequence = tempSequence;

                um_new->storage->sequenceCapacity = desiredCap;
        }
        
        um_new->storage->sequence[size] = um_new->program;
        um_new->storage->length = size + 1;
        
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
        int pro_length = um->program->length;
        uint32_t *pro_elem;
        for (int i = 0; i < pro_length; i++) {
                //pro_elem = seq_remhi(um->program);
                assert(um->program->length != 0);
                um->program->length--;
                pro_elem = um->program->sequence[um->program->length];
                FREE(pro_elem);
        }


        //free_seq(&um->program); //Stays
        free((um->program)->sequence);
        FREE(um->program);

        //free_mapped(um->storage);
        int stor_length = um->storage->length;
        for (int i = 1; i < stor_length; i++) {
                //seg stor_seg = seq_get(um->storage, i);
                seg stor_seg = um->storage->sequence[i];
                if (stor_seg->mapped == 1) {
                        UArray_free(&(stor_seg->segment));
                }
                FREE(stor_seg);
        }

        //free_seq(&um->storage); // Stays
        free((um->storage)->sequence);
        FREE(um->storage);

        //free_seq_elems(um->avail_id);
        int id_length = um->avail_id->length;
        uint32_t *id_elem;
        for (int i = 0; i < id_length; i++) {
                //id_elem = seq_remhi(um->avail_id);
                um->avail_id->length--;
                id_elem = um->avail_id->sequence[um->avail_id->length];
                FREE(id_elem);
        }

        //free_seq(&um->avail_id); // Same
        free((um->avail_id)->sequence);
        FREE(um->avail_id);
      
        FREE(um);
}

