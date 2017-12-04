#include <stdio.h>
#include "assert.h"

#include "sequence.h"

int main() 
{
	Sequence_T test_seq = new_seq(0);

	uint32_t guy = 669;
	seq_addhi(test_seq, &guy);
	free_seq(test_seq);

	return 0;
}