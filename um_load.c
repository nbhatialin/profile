/*
 *     um_load.c
 *     by Nikhil Bhatia-Lin, Charlie Caron, 11/27/17
 *     HW6: UM
 *
 *     Exports just one function which takes in a file and parses its 
 *     informaion into a sequence, returning a void pointer to this seq
 *     
 */ 
#include <stdlib.h>
#include <stdio.h>

#include <stdint.h>
#include "mem.h"
#include "bitpack.h"
#include "sequence.h"

/*
* Takes in a file pointer and parses through, reading a byte at a time. 4 bytes
* make one word, this word is added to the sequence representation of the 
* initial program. This program is returned as a void pointer.
*/
void *load_um(FILE* filename) {
        Sequence_T program_segment = new_seq(8); 

        uint32_t word = 0;
        int64_t c = getc(filename);
        int count = 3;
        while (c != EOF) {
                word = Bitpack_newu(word, 8, count*8, c);
                count--;
                if (count == -1) {
                        count = 3;
                        uint32_t *codeword = NEW(codeword);
                        *codeword = word;
                        seq_addhi(program_segment, codeword);
                        word = 0;
                }
                c = getc(filename);
        }
        return program_segment;
}

