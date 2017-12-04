/*
 *     sequence.c
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
#include "except.h"

#include "sequence.h"

#define T Sequence_T

struct T {
        elem_ptr *sequence;
	int sequenceCapacity;
	int sequenceSize;
};

T new_seq(int hint) 
{
        T seq;
        NEW(seq);
        seq->sequenceCapacity = hint;
        seq->sequence = malloc(sizeof(elem_ptr)*(seq->sequenceCapacity));
        seq->sequenceSize = 0;
        return seq;
}

int seq_length(T seq)
{
        return seq->sequenceSize;
}

void seq_addhi(T seq, elem_ptr to_add)
{
        int size = seq->sequenceSize;
        int cap = seq->sequenceCapacity;
        if (size == cap) {
                ensureCapacity(seq);
        }
        
        seq->sequence[size] = to_add;
        seq->sequenceSize = size + 1;
}

elem_ptr seq_remhi(T seq)
{
        assert(seq->sequenceSize != 0);
        seq->sequenceSize--;
        return seq->sequence[sequenceSize];
}

elem_ptr seq_get(T seq, int index)
{
        assert(index < seq->length);
        assert(index >= 0);
        return seq->sequence[index];
}

void seq_put(T seq, int index, elem_ptr insert)
{
        assert(index < seq->length);
        assert(index >= 0);
        seq->sequence[index] = insert;
}

void free_seq(T seq) 
{
        free(seq->sequence);
        FREE(seq);
}
void ensureCapacity(T seq) 
{
        int desiredCap = (seq->sequenceCapacity) * 2 + 2;
        elem_ptr *tempSequence = malloc(sizeof(elem_ptr)*desiredCap);
        
        for (int i = 0; i < seq->sequenceCapacity; i++){
                tempSequence[i] = seq->sequence[i];
        }

        free(seq->sequence);
        seq->sequence = tempSequence;

        seq->sequenceCapacity = desiredCap;
}
