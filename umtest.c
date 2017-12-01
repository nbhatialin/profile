#include <stdio.h>
#include "assert.h"

#include "um.h"
#include "um_load.h"

int main(int argc, char *argv[]) 
{
	(void)argc;
	
	int a = 0;
	int b = 1;
	int c = 2;
	int d = 3;
	int e = 4;
	
	FILE *filename = fopen(argv[1], "r");
    assert(filename != NULL);
    Um_T test_um = new_um(load_um(filename));
    fclose(filename);

/*
	//TEST IN/OUT/CMOV
	in(test_um, a, b, c);
	printf("First IN\n");
	in(test_um, a, b, c);
	printf("Second IN\n");
	in(test_um, a, b, c);
	printf("Third IN\n");
	cmov(test_um, a, b, c);
	printf("C:\n");
	out(test_um, a, b, c);
	printf("A:\n");
	out(test_um, a, b, c);
*/


	//TEST MAP/UNMAP/SSTORE/SLOAD
    in(test_um, c, b, a); //always 2
	in(test_um, c, a, b); // second in, is the length //
	map(test_um, a, c, b);
	for (int i = 0; i < 55; i++) {
		map(test_um, a, c, b);
	}
	out(test_um, 0, 0, c); // should print id (56)//
	in(test_um, c, b, d); // third in, is the id to unmap//
	unmap(test_um, 0, 0, d);
	printf("here\n");

	map(test_um, a, c, b);
	printf("Mapped after unmap\n");
	out(test_um, 0, 0, c); // should print id (49) //
	printf("Index?\n");
	in(test_um, 0, 0, c); //fourth in, thing to put in a, b //
	sstore(test_um, b, a, c);
	sload(test_um, e, b, a);
	printf("Loaded A\n");
	out(test_um, 0, 0, e);// should be loaded from segment //



	// Testing add, mult, and divide, nand
	halt(test_um, a, b, c);
	printf("ah");
	lv(test_um, a); // always zero
	out(test_um, a, b, c);
	/*
	lv(test_um, c);
	lv(test_um, d);

	add(test_um, a, b, c);

	out(test_um, a);
	printf("here\n");
	mul(test_um, a, b, c);
	out(test_um, a);
	divide(test_um, a, b, c);
	out(test_um, a);
	//nand(test_um, a, b, c); //bail
	out(test_um, a);
	
	*/
	free_um(test_um);

	return 0;
}