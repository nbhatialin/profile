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

#define S Sequence_T

struct S {
        elem_ptr *sequence;
	int sequenceCapacity;
	int length;
};

S new_seq(int hint) 
{
        S seq;
        NEW(seq);
        seq->sequenceCapacity = hint;
        seq->sequence = malloc(sizeof(elem_ptr)*(seq->sequenceCapacity));
        seq->length = 0;
        return seq;
}

int seq_length(S seq)
{
        return seq->length;
}

void seq_addhi(S seq, elem_ptr to_add)
{
        int size = seq->length;
        int cap = seq->sequenceCapacity;
        if (size == cap) {
                ensureCapacity(seq);
        }
        
        seq->sequence[size] = to_add;
        seq->length = size + 1;
}

elem_ptr seq_remhi(S seq)
{
        assert(seq->length != 0);
        seq->length--;
        return seq->sequence[seq->length];
}

elem_ptr seq_get(S seq, int index)
{
        assert(index < seq->length);
        assert(index >= 0);
        return seq->sequence[index];
}

void seq_put(S seq, int index, elem_ptr insert)
{
        assert(index < seq->length);
        assert(index >= 0);
        seq->sequence[index] = insert;
}

void free_seq(S *seq) 
{
        free((*seq)->sequence);
        FREE(*seq);
}
void ensureCapacity(S seq) 
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
