#ifndef SEGMENT_TESTS_H
#define SEGMENT_TESTS_H

#include <stdlib.h>
#include <stdint.h>

void add_one(Segments_T table, uint32_t length);
void add_length_zero();
void add_max_length();
void add_neg_length();
void add_two_segments();
void add_many_segments(Segments_T table, int num_segs);
void get_bad_index();
void put_bad_index();
void test_put();
void access_bad_seg();
void invalid_index();

#endif
