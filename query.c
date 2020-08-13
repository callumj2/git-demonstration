/* Program to perform a basic search on a list of documents, given an input
	 query.

	 Written by Rowan Warneke 911865, Marc 2018.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "query.h"
#include "list.h"
#include "heap.h"

/****************** function prototypes **************************************/

float *make_scorelist(int len);
void populate_scorelist(float *scores, Index *index);
Heap *get_top_items(float *scores, Index *index, int n_results, int n_docs);
Document *heap_to_array(Heap *top_items, int n_results);
void print_results(Document *top_items_hilo, int len);
Heap *build_doclist_queue(Index *index);
Heap *get_top_items_merge(Heap *queue, int size);

/****************** implementation of task 1 *********************************/

void print_array_results(Index *index, int n_results, int n_documents) {
	// implement Task 1 for the assignment
	float *scores = make_scorelist(n_documents);
	populate_scorelist(scores, index);
	Heap *top_items = get_top_items(scores, index, n_results, n_documents);
	int num_outs = heap_size(top_items);
	Document *top_items_hilo = heap_to_array(top_items, num_outs);
	print_results(top_items_hilo, num_outs);
	// clean up
	free(scores);
	free_heap(top_items);
	free(top_items_hilo);
	// all done
}

/****************** implementation of task 2 *********************************/

void print_merge_results(Index *index, int n_results) {
	// implement Task 2 for the assignment
	Heap *p_queue = build_doclist_queue(index);
	Heap *top_items = get_top_items_merge(p_queue, n_results);
	int num_outs = heap_size(top_items);
	Document *top_items_hilo = heap_to_array(top_items, num_outs);
	print_results(top_items_hilo, num_outs);
	// clean up
	free(p_queue);
	free_heap(top_items);
	free(top_items_hilo);
	// finished! imgur.com/TRz1sxm
}

/****************** helper functions *****************************************/

/* build a list of floating point numbers, initialised to 0, to store document
   scores */
float *make_scorelist(int len) {
	float *scores = malloc(len*sizeof(float));
	assert(scores);
	int i;
	for (i=0; i<len; i++) {
		scores[i] = 0.0;
	}
	return scores;
}

/* run through the document list for each query, and add document scores to the
   running totals in the "scores" array */
void populate_scorelist(float *scores, Index *index) {
	int i;
	Node *curr;
	Document *doc;
	for (i=0; i<index->num_terms; i++) {
		// for every query term...
		curr = index->doclists[i]->head;
		while(curr) {
			// move from one document-storing node to the next until none remain
			doc = curr->data;
			scores[doc->id] += doc->score;
			curr = curr->next;
		}
	}
	free(doc);
	free(curr);
}

/* run through a scores array with a min-heap to extract the top n_results
   scores */
Heap *get_top_items(float *scores, Index *index, int n_results, int n_docs) {
	Heap *top_items = new_heap(n_results);
	int i;
	for (i=0; i<n_docs; i++) {
		// for every score...
		if (heap_size(top_items) < n_results || scores[i] > heap_peek_key(top_items)) {
			// score belongs in the heap, so add it!
			int *index = malloc(sizeof(int));
			assert(index);
			*index = i;
			if (i >= n_results) {
				// heap is already full, remove the smallest item to make room
				heap_remove_min(top_items);
			}
			// use the score as the key in the heap. record the index as well
			heap_insert(top_items, scores[i], index);
		}
	}
	return top_items;
}

/* converts a min heap to a sorted max-to-min array */
Document *heap_to_array(Heap *top_items, int len) {
	Document *top_items_hilo = malloc(len*sizeof(Document));
	assert(top_items_hilo);
	int i=len-1;
	while (heap_size(top_items)) {
		// since the heap and array have exactly the same length, every array
		// position will be filled by exactly one heap item
		Document item;
		item.score = heap_peek_key(top_items);
		int *id = heap_remove_min(top_items);
		item.id = *id;
		// start at the end of the array and work backwards (so that max is first)
		top_items_hilo[i] = item;
		i--;
	}
	return top_items_hilo;
}

/* print the search results from a sorted array of documents */
void print_results(Document *top_items_hilo, int n_results) {
	int i;
	for (i=0; i<n_results; i++) {
		// only print documents with non-zero scores
		if (top_items_hilo[i].score > 0) {
			printf("%6d %.6f\n", top_items_hilo[i].id, top_items_hilo[i].score);
		}
	}
}

/* build a queue of nodes containing the first node in each document list,
	 which is to be sorted by the id of the document inside the latter node */
Heap *build_doclist_queue(Index *index) {
	int num_terms = index->num_terms;
	// the queue has room for as many nodes as there are terms in the query
	Heap *p_queue = new_heap(num_terms);
	int i;
	for (i=0; i<num_terms; i++) {
		// for ever term in the query, take its document list. insert into the 
		// queue the first node in the document list, with the key being the id
		// of the document contained within that node.
		List *doclist = index->doclists[i];
		Node *first_item = doclist->head;
		Document *first_doc = first_item->data;
		heap_insert(p_queue, (float)(first_doc->id), first_item);
	}
	return p_queue;
}

/* perform an iterative search over the priority queue, and store the top
   results in a heap */
Heap *get_top_items_merge(Heap *p_queue, int n_results) {
	Heap *top_items = new_heap(n_results);
	float curr_id;
	float running_score;
	// while the queue isn't empty, look at the next minimum item
	while (heap_size(p_queue)) {
		running_score = 0.0;
		curr_id = heap_peek_key(p_queue);
		// while the id of the document in the minimum node remains the same, 
		// increment the running score
		while (heap_size(p_queue) && heap_peek_key(p_queue) == curr_id) {
			Node *min_item = heap_remove_min(p_queue);
			Document *min_doc = min_item->data;
			float increment = min_doc->score;
			running_score += increment;
			// replace the node with the next node in the appropriate list (the node
			// which the current node points to)
			Node *replacement = min_item->next;
			if (replacement) {
				Document *next_min_doc = replacement->data;
				float new_key = (float)(next_min_doc->id);
				heap_insert(p_queue, new_key, replacement);
			}
		}
		// if the score is large enough, or if the top items heap is not yet full,
		// insert the document score and index into the top items heap
		if (heap_size(top_items) < n_results || 
			  running_score > heap_peek_key(top_items)) {
			int *doc_index = malloc(sizeof(int));
			assert(doc_index);
			*doc_index = (int)curr_id;
			if (heap_size(top_items) == n_results) {
				// make space 
				heap_remove_min(top_items);
			}
			heap_insert(top_items, running_score, doc_index);
		}
	}

	return top_items;
}