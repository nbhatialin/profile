/*
 *     segments.c
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     An implementation allowing the user to work with and store segment 
 *     representations in a UM
 *     
 */

#include "segments.h"
#include <stdio.h>
#include "seq.h"
#include "table.h"
#include "uarray.h"
#include "mem.h"
#include "except.h"

#define T Segments_T
#define program_id 0
#define word_size 4

Except_T Too_large = {"Too many segments added"};

/*
* Our Segments struct
*/
struct T {
        Seq_T program;
        Seq_T storage;
        Seq_T avail_id;
        /* Potential sequences for the 2^31 to 2^32 segments */
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
* Performs a check to make sure the id is not already unmapped/out of bounds
* This check is also called for functions that should not access the program
* (0th) segment. prints poor and exits if this check fails (we really went wild)
*/
static void check_access(T segments, uint32_t length);
/*
* Performs a check to make sure the id is not already unmapped/out of bounds
* prints poor and exits if this check fails (we really went wild)
*/
static void check_unmap(T segments, Umsegment_Id id);
/* Frees all of the Uarrays (segment representations) in the storage sequence */
static void free_mapped(Seq_T storage);
/* Frees all of the dynamically allocated memory in a given sequence */
static void free_seq_elems(Seq_T sequence);
/* Returns a full segment as a UArray_T given its id */
static UArray_T get_segment(T segments, Umsegment_Id id);

int count = 0;
/*
* Creates a new segments variable. Take in a void * (information hiding wow!) 
* of the program to init with and returns a Segments_T
*/
T new_segments(void *init_program)
{
        T new_segments;
        NEW(new_segments);
        new_segments->storage = Seq_new(8);
        new_segments->avail_id = Seq_new(10);

        new_segments->program = init_program;

        Seq_addhi(new_segments->storage, new_segments->program);
        return new_segments;
}

/*
* Adds a segment (represented as a UArray) to the storage/segments sequence. 
* Checks to see if there is an avaiable id that has been unmapped
* Takes the Segments struct & the length of the segment to add, returns the id
*/
Umsegment_Id add_segment(T segments, uint32_t length)
{
        UArray_T new_segment = UArray_new(length, word_size);
        if (Seq_length(segments->avail_id) == 0) {
                seg add_seg;
                NEW(add_seg);
                add_seg->mapped = 1;
                add_seg->segment = new_segment;
                Seq_addhi(segments->storage, add_seg);
                return (Seq_length(segments->storage) - 1);

        }
        else {
                Umsegment_Id *id_ptr = Seq_remhi(segments->avail_id);
                Umsegment_Id id = *id_ptr;

                seg get_seg = Seq_get(segments->storage, id);
                get_seg->segment = new_segment;
                get_seg->mapped = 1;

                FREE(id_ptr);
		/*
                UArray_T to_free = get_seg->segment;
                UArray_free(&to_free);
		FREE(get_seg);
                */
		return id;
        }
}

/*
* Replaces the program (stored at segment zero) given a pointer to the new 
* program and the Segments struct. Calls get segment (static). Turns a UArray
* into a sequence and sets it as the program.
*/
void replace_program(T segments, Umsegment_Id id)
{

        UArray_T uarray_program = get_segment(segments, id);
        Seq_T seq_program = Seq_new(UArray_length(uarray_program));
        for (int i = 0; i < UArray_length(uarray_program); i++) {
                uint32_t *curr_elem = UArray_at(uarray_program, i);
                uint32_t *new_elem = NEW(new_elem);
                *new_elem = *curr_elem;
                Seq_addhi(seq_program, new_elem);
        }

        free_seq_elems(segments->program);
        Seq_free(&segments->program);

        Seq_put(segments->storage, program_id, seq_program);
        segments->program = seq_program;
}

/*
* Gets an individual word from the program stored in segments given the index 
* (program counter probably). Returns this word.
*/
uint32_t get_program_elem(T segments, uint32_t index)
{
        uint32_t *elem = Seq_get(segments->program, index);
        return *elem;
}
/*
* "removes" the segment by adding the segment id to the unmapped ids sequence
* Will actually deallocate the UArray if the id is reused (this makes freeing
* memory very efficient). Performs a check to make sure this id is not out
* of bounds/already unmapped
*/
void remove_segment(T segments, Umsegment_Id id)
{
        check_access(segments, id);
        /* Freeing the UArray when we need to add something to its spot */
        seg rem_segment = Seq_get(segments->storage, id);
	rem_segment->mapped = 0;
	UArray_free(&rem_segment->segment);
        Umsegment_Id *id_ptr = NEW(id_ptr);
        *id_ptr = id;
        Seq_addhi(segments->avail_id, id_ptr);
}

/*
* Gets an individual word from the segment stored in segments given the id 
* and index. Returns this word. Performs a check to make sure the id is not
* already unmapped/out of bounds
*/
uint32_t get_segment_elem(T segments, Umsegment_Id id, uint32_t index)
{
        check_unmap(segments, id);
        if (id == program_id) {
                uint32_t *index_val = Seq_get(segments->program, index);
                return *index_val;
        } else {
		seg get_seg = Seq_get(segments->storage, id);
                UArray_T get_segment = get_seg->segment;
                uint32_t *index_val = UArray_at(get_segment, index);
                return *index_val;
        }
}

/*
* Puts an individual word into the segment stored in segments given the id 
* and index. Returns the old word. Performs a check to make sure the id is not
* already unmapped/out of bounds
*/
uint32_t put_segment_elem(T segments, Umsegment_Id id, uint32_t index, 
							uint32_t add)
{
        check_unmap(segments, id);
        if (id == program_id) {
                uint32_t *to_free = Seq_get(segments->program, index);
                //FREE(to_free);
                //uint32_t *codeword = NEW(codeword);
		uint32_t return_val = *to_free;
                *to_free = add;
                return return_val; 
                
        }
        else {
                seg get_seg = Seq_get(segments->storage, id);
		UArray_T get_segment = get_seg->segment;
                uint32_t *old_val = UArray_at(get_segment, index);
                uint32_t return_val = *old_val;
                *old_val = add;
                return return_val;
        }
}

/*
* Frees all allocated memory from segments. Checks to see if the program was
* ever replaced becuase freeing a UArray and Seq is different
*/
void free_segments(T segments)
{
        free_seq_elems(segments->program);
        Seq_free(&segments->program);
        free_mapped(segments->storage);
        Seq_free(&segments->storage);
        free_seq_elems(segments->avail_id);
        Seq_free(&segments->avail_id);
      
        FREE(segments);
}

/*
* Performs a check to make sure the id is not already unmapped/out of bounds
* This check is also called for functions that should not access the program
* (0th) segment. prints poor and exits if this check fails (we really went wild)
*/
static void check_access(T segments, Umsegment_Id id)
{
        if (id == program_id || Seq_length(segments->storage) <= (int)id) {
                printf("poor\n");
                exit(1);
        }
	seg check_seg = Seq_get(segments->storage, id);
        if (check_seg->mapped == 0) {
                printf("poor\n");
                exit(1);
        }
}

/*
* Performs a check to make sure the id is not already unmapped/out of bounds
* prints poor and exits if this check fails (we really went wild)
*/
static void check_unmap(T segments, Umsegment_Id id)
{
        if (Seq_length(segments->storage) <= (int)id) {
                printf("poor\n");
                exit(1);
        }
        if (id != program_id) {
                seg check_seg = Seq_get(segments->storage, id);
                if (check_seg->mapped == 0) {
                        printf("poor\n");
                        exit(1);
                }
        }
}

/*
* Frees all of the Uarrays (segment representations) in the storage sequence 
*/
static void free_mapped(Seq_T storage)
{
        int length = Seq_length(storage);
        for (int i = 1; i < length; i++) {
                seg to_free = Seq_get(storage, i);
		if (to_free->mapped == 1) {
			UArray_free(&(to_free->segment));
		}
		FREE(to_free);
        }
}

/*
* Frees all of the dynamically allocated memory in a given sequence
*/
static void free_seq_elems(Seq_T sequence)
{
        int length = Seq_length(sequence);
        uint32_t *to_free;
        for (int i = 0; i < length; i++) {
                to_free = Seq_remhi(sequence);
                FREE(to_free);
        }
}

/*
* Returns a full segment as a UArray_T given its id
*/
static UArray_T get_segment(T segments, Umsegment_Id id)
{
        check_access(segments, id);
        seg get_seg = Seq_get(segments->storage, id);
        UArray_T id_seg = get_seg->segment;
        return id_seg;
}

#undef T
