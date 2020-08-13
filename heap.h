/* * * * * * *
 * Module for creating and manipulating binary min-heaps on integer indices
 *
 * created for COMP20007 Design of Algorithms 2013
 * by Andrew Turpin
 * modified by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 * modified more by Rowan Warneke
 */

typedef struct heap Heap;

// returns a pointer to a new, empty heap with a capacity for max_size items
Heap *new_heap(int max_size);

// inserts data into h with priotiry key
void heap_insert(Heap *h, float key, void *data);

// remove and return the item with the smallest key in h
void *heap_remove_min(Heap *h);
 
// return, but not remove, the item with the smallest key in h
void *heap_peek_min(Heap *h);

// return the key of the item with the smallest key in h
float heap_peek_key(Heap *h);

// de-allocate all memory used by h
void free_heap(Heap *h);

// return the size of a heap
int heap_size(Heap *h);


// FOR TESTING

// print the contents of a heap to stdout
void print_heap(Heap *h);