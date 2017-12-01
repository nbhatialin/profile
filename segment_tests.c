#include "segments.h"
#include "segment_tests.h"

#include <stdio.h>


void add_one(Segments_T table, uint32_t length) 
{
	uint32_t added_id = add_segment(table, length);
	printf("%u\n", added_id);
}

void add_length_zero() 
{
	Segments_T table = new_segments();
	add_segment(table, 0);
	free_segments(table);
}

void add_max_length() 
{
	Segments_T table = new_segments();
	uint32_t added_id = add_segment(table, 4294967295U);
	printf("%d\n", added_id);
	free_segments(table);
}

void add_neg_length() 
{
	Segments_T table = new_segments();
	uint32_t added_id = add_segment(table, -1);
	printf("%d\n", added_id);
	free_segments(table);
}

void add_two_segments() 
{
	Segments_T table = new_segments();
	uint32_t first_id = add_segment(table, 498);
	uint32_t second_id = add_segment(table, 333);
	uint32_t first_word = get_segment_elem(table, first_id, 16);
	uint32_t second_word = get_segment_elem(table, second_id, 12);

	printf("%d, %d\n", first_word, second_word);

	free_segments(table);
}

void add_many_segments(Segments_T table, int num_segs) 
{
	uint32_t id = 0;
	for(int64_t i = 0; i < num_segs; i++) {
		id = add_segment(table, 2);
		printf("%u\n", id);
		/* Should get a CRE */
	}

}

void get_bad_index()
{
	Segments_T table = new_segments();
	uint32_t first_id = add_segment(table, 419);
	uint32_t first_word = get_segment_elem(table, first_id, 421);
	printf("%d\n", first_word);
	free_segments(table);
}

void put_bad_index()
{
	Segments_T table = new_segments();
	uint32_t first_id = add_segment(table, 419);
	uint32_t first_word = put_segment_elem(table, first_id, 421, 1234567);
	printf("%d\n", first_word);
	free_segments(table);
}

void test_put()
{
	Segments_T table = new_segments();
	uint32_t first_id = add_segment(table, 419);
	uint32_t first_word = put_segment_elem(table, first_id, 14, 1234567);
	uint32_t second_word = put_segment_elem(table, first_id, 99, 98765);
	first_word = get_segment_elem(table, first_id, 14);
	second_word = get_segment_elem(table, first_id, 99);
	printf("%d, %d\n", first_word, second_word);

	free_segments(table);
}

void access_bad_seg()
{
	Segments_T table = new_segments();
	uint32_t first_id = add_segment(table, 49);
	uint32_t first_word = get_segment_elem(table, (first_id+1), 14);
	// should throw CRE
	printf("%d\n", first_word);

	free_segments(table);
}

void invalid_index()
{
	Segments_T table = new_segments();
	uint32_t first_id = add_segment(table, 943543);
	uint32_t first_word = get_segment_elem(table, first_id, -13);
	printf("%d\n", first_word);

	free_segments(table);
}


