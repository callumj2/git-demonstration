/* * * * * * *
 * Module for creating and manipulating binary min-heaps on integer indices
 *
 * created for COMP20007 Design of Algorithms 2013
 * by Andrew Turpin
 * modified by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 * modified more by Rowan Warneke: added heap_size function, made data in 
 * heapItem a void pointer.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "heap.h"

typedef struct item {
	float key;  // the key for deciding position in heap
	void  *data;  // pointer to the associated with this key
} HeapItem;

struct heap {
	HeapItem *items;  // the underlying array
	int cur_size; // the number of items currently in the heap
	int max_size; // the maximum number of items allowed in the heap
};

// HELPER FUNCTIONS

// swap two heap items (the items at index i and index j)
// assumes h != NULL
void swap(Heap *h, int i, int j) {
	HeapItem t  = h->items[i];
	h->items[i] = h->items[j];
	h->items[j] = t;
}

// move an item up the heap structure to re-establish heap order
// bonus challenge: improve this function by avoiding some copies/swaps
// assumes h != NULL
void siftup(Heap *h, int i) {
	int parent = (i - 1) / 2;
	while (h->items[i].key < h->items[parent].key) {
		swap(h, parent, i);
		i = parent;
		parent = (i - 1) / 2;
	}
}

// find the index of the smallest child among the children of an item
// if the item has no children, return 0
// assumes h != NULL
int min_child(Heap *h, int i) {
	int child = i * 2 + 1;
	if (child >= h->cur_size) {
		// no children
		return 0;
	} else if (child+1 >= h->cur_size || h->items[child].key < h->items[child+1].key) {
		// only child or first child is smallest child
		return child;
	} else {
		// second child exists and is smallest child
		return child+1;
	}
}

// move an item down the heap structure to re-establish heap order
// bonus challenge: improve this function by avoiding some copies/swaps
// assumes h != NULL
void siftdown(Heap *h, int index) {
	int child = min_child(h, index);
	while (child && h->items[child].key < h->items[index].key) {
		swap(h, child, index);
		index = child;
		child = min_child(h, index);
	}	
}

// returns a pointer to a new, empty heap with a capacity for maximum_size items
Heap *new_heap(int max_size) {
	Heap *h = malloc(sizeof *h);
	assert(h);
	
	h->items = malloc(max_size * (sizeof *h->items));
	assert(h->items);

	h->cur_size = 0;
	h->max_size = max_size;

	return h;
}

// inserts data into heap with priority key
void heap_insert(Heap *h, float key, void *data) {
	assert(h != NULL);
	assert(h->cur_size < h->max_size);
	
	h->items[h->cur_size].key  = key;
	h->items[h->cur_size].data = data;
	h->cur_size++;

	siftup(h, h->cur_size-1);
}

// remove and return the item with the smallest key in h
void *heap_remove_min(Heap *h) {
	assert(h != NULL);
	assert(h->cur_size > 0);

	void *min = h->items[0].data;

	h->items[0] = h->items[h->cur_size-1];
	h->cur_size -= 1;
	siftdown(h, 0);

	return min;
}
 
// return, but not remove, the item with the smallest key in h
void *heap_peek_min(Heap *h) {
	assert(h != NULL);
	assert(h->cur_size > 0);

	return h->items[0].data;
}

// return the key of the item with the smallest key in h
float heap_peek_key(Heap *h) {
	assert(h != NULL);
	assert(h->cur_size > 0);

	return h->items[0].key;
}

// de-allocate all memory used by h
void free_heap(Heap *h) {
	assert(h != NULL);

	free(h->items);
	free(h);
}

// return the size of a heap
int heap_size(Heap *h) {
	return h->cur_size;
}


// TESTING FUNCTIONS

// print the contents of a heap to stdout
void print_heap(Heap *h) {
	assert(h != NULL);

	int i;

	printf("printing heap\n-------------\n");
	printf("heap array:\n");
	printf("   i: ");
	for (i = 0; i < h->cur_size; i++) {
		printf("%5d ", i);
	}
	printf("\n");
	printf("data: ");
	for (i = 0; i < h->cur_size; i++) {
		printf("%5d ", h->items[i].data);
	}
	printf("\n");
	printf("keys: ");
	for (i = 0; i < h->cur_size; i++) {
		printf("%5.1f ", h->items[i].key);
	}

	printf("\n");
}