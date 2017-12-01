#include "segments.h"
#include "uarray.h"
#include "assert.h"
#include "seq.h"
#include "um_load.h"

#include <stdio.h>

int main(int argc, char *argv[]) {

	(void)argc;
	FILE *filename = fopen(argv[1], "r");
    assert(filename != NULL);
    Seq_T init_program = load_um(filename);
    fclose(filename);

    printf("not poor\n");
	Segments_T test_segments = new_segments(init_program);
	for (int i = 0; i < 2000; i++) {
			add_segment(test_segments, 69);
	}
printf("not poor\n");
	for (int i = 1; i < 20; i++) {
		remove_segment(test_segments, i);
	}
printf("not poor\n");
	for (int i = 0; i < 2000; i++) {
			add_segment(test_segments, 69);
	}
	printf("not poor\n");
	//add_length_zero();
	//add_many_segments(test_segments, 123456);
/*
	remove_segment(test_segments, id_one);
	remove_segment(test_segments, id_two);
	remove_segment(test_segments, id_three);
	remove_segment(test_segments, id_four);
	remove_segment(test_segments, id_five);

	id_one = add_segment(test_segments, 69);
	id_two = add_segment(test_segments, 69);
 	id_three = add_segment(test_segments, 69);
	id_four = add_segment(test_segments, 69);
 	id_five = add_segment(test_segments, 69);
 	//uint32_t nee = 6;


 	remove_segment(test_segments, id_one);
	remove_segment(test_segments, id_two);
	remove_segment(test_segments, id_three);
	remove_segment(test_segments, id_four);
	//remove_segment(test_segments, nee);

	add_segment(test_segments, 420);


	//get_bad_index();
	//put_bad_index();
	test_put();
	//access_bad_seg();
	//invalid_index();
*/
	for (int i = 0; i < 69; i++) {
			put_segment_elem(test_segments, 1, i, 356);

	}
	for (int i = 0; i < 69; i++) {
			put_segment_elem(test_segments, 2, i, 356);

	}
	printf("bad poor\n");
	uint32_t zeroth = put_segment_elem(test_segments, 1, 18, 356);
	uint32_t first_elem = get_segment_elem(test_segments, 1, 18);
	uint32_t second = put_segment_elem(test_segments, 1, 22, first_elem);
	uint32_t same = get_segment_elem(test_segments, 1, 22);

	printf("All four: %u %u %u %u\n", zeroth, first_elem, second, same);

	replace_program(test_segments, 1);
	replace_program(test_segments, 2);

	uint32_t dee = get_program_elem(test_segments, 18);
	uint32_t deek = get_program_elem(test_segments, 22);
	printf("All two: %u %u\n", dee, deek);


	free_segments(test_segments);
	return 0;
}

