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
#include "assert.h"
#include "um.h"
#include "um_load.h"

#define max_ops 14
typedef enum Um_opcode {
        CMOV = 0, SLOAD = 1, SSTORE = 2, ADD = 3, MUL = 4, DIV = 5,
        NAND = 6, HALT = 7, ACTIVATE = 8, INACTIVATE = 9, OUT = 10, 
        IN = 11, LOADP = 12, LV = 13
} Um_opcode;

typedef void (*func_type)();

void run_um(Um_T um);

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

        Um_opcode opcode = 0;
        while(1) {
                uint64_t curr_word = next_program_elem(um);
                opcode = (curr_word << 32) >> 60; 
                assert(opcode <= max_ops);
                if (opcode == LV) {
                        uint32_t a = (curr_word << 36) >> 61; 
                        lv(um, a);
                }
                else {
                        uint32_t c = (curr_word << 61) >> 61; 
                        uint32_t b = (curr_word << 58) >> 61; 
                        uint32_t a = (curr_word << 55) >> 61; 
                        operations[opcode](um, a, b, c);
                }
        }
}
