/*
 *  Need: 		Capture the heap allocation need of a process 
 *	  		at a given instance of time.
 *
 *  Idea:		Capture the summary of total allocation at the instance of state change in
 *			the heap. 
 *			State change is defined as any operation that alters the heap.   
 *
 *  Reference:  	Benjamin Zorn and Dirk Grunwald. 1994. Evaluating models of memory allocation. 
 *	   		ACM Trans. Model. Comput. Simul. 4, 1 (January 1994), 107-131. 
 *			DOI=http://dx.doi.org/10.1145/174619.174624.
 *
 *  Initial Author:  	Gokul Vasan [gokulvas@gmail.com]
 *
 * TODO: 		[*] Pool level accounting
 *			[*] Algorithm sensitive size rounding. 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _chronicle {
	/* 
	 * max_seen = MAX(max_seen, current_seen_sum)
	 */
	unsigned long long max_seen;
	unsigned long long prev_sum;
	unsigned long long total_sum;
	unsigned int cnt;
	/*  Avg is the average of the sum
	 *  of total on change in the state.
	 */
	unsigned long long avg;
} chronicle;

typedef enum _alloc_type {
	type_malloc = 0,
	type_free,
	type_realloc,
	type_calloc
} alloc_type;


static chronicle book_keep;

/* 
 * call this in constructor
 */
void stat_init(void) 
{
	memset(&book_keep, 0x00, sizeof(book_keep));
	return;
}

/*
 * call this when you want to print the account
 */
void stat_print(void) 
{
	printf("Total Allocation request: %u, Max Needed: %llu, Average Need: %llu\n", 
		book_keep.cnt, book_keep.max_seen, book_keep.avg);
	return;
}

/*
 * Variable if(!realloc) old_size = 0
 */
#define MAX(A,B) (((A) > (B))?(A):(B))
void stat_account(alloc_type type, size_t new_size, size_t old_size)
{

	switch (type) {
	case type_malloc:
	case type_calloc:
		book_keep.prev_sum += new_size;
		book_keep.cnt++;
	break;
	case type_free:
		book_keep.prev_sum -= new_size;
		book_keep.cnt++;
	break;
	case type_realloc:
		book_keep.prev_sum -= old_size;
		book_keep.prev_sum += new_size;
		book_keep.cnt++;
	break;
	default:
		printf("Something Wrong! Wrong type\n");
	break;
	}

	book_keep.total_sum = book_keep.total_sum + book_keep.prev_sum;
	book_keep.avg = book_keep.total_sum/book_keep.cnt;
	book_keep.max_seen = MAX(book_keep.max_seen, book_keep.prev_sum);
	return;
}
