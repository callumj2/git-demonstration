/* * * * * * *
 * Module for creating and manipulating a multi-term inverted file index
 *
 * created for COMP20007 Design of Algorithms - Assignment 1, 2018
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "index.h"

/*                         DO NOT CHANGE THIS FILE
 * 
 * DO NOT add or modify any definitions or anything else inside this file.
 *
 * We will test your assignment with an unmodified version of this file. Any
 * changes you make will be lost. This may lead to compile errors.
 */


// read a single data file into a document score list
// return number of documents read, or -1 on error
int read_scorelist(char *datafilename, List *doclist);

// create a new, empty index, with space for a maximum of max_terms terms
Index *new_index(int max_terms) {
	Index *index = malloc(sizeof *index);
	assert(index);

	index->num_terms = 0;
	index->max_terms = max_terms;
	index->terms = malloc(max_terms * sizeof *index->terms);
	assert(index->terms);
	index->doclists = malloc(max_terms * sizeof *index->doclists);
	assert(index->doclists);

	return index;
}

// destroy an index, it's associated document-score lists, and their associated
// documents (but not the term strings)
void free_index(Index *index) {
	assert(index != NULL);
	// for each term,
	for (int i = 0; i < index->num_terms; i++) {
		// free each document
		for (Node *n = index->doclists[i]->head; n; n = n->next) {
			free_document(n->data);
		}
		// free the list itself
		free_list(index->doclists[i]);
	}
	// free the arrays and the struct itself
	free(index->doclists);
	free(index->terms);
	free(index);
}

// add a new term and its document-scorelist to an index, based on data in 
// datafilename
// return number of documents read, or -1 on error
int index_add_term(Index *index, char *term, char *datafilename) {
	assert(index != NULL);
	assert(index->num_terms < index->max_terms);

	index->terms[index->num_terms] = term;	
	
	List *doclist = new_list();
	int num_docs = read_scorelist(datafilename, doclist);
	index->doclists[index->num_terms] = doclist;
	
	index->num_terms++;

	return num_docs;
}


// read a single data file into a document score list
// return number of documents read, or -1 on error
int read_scorelist(char *datafilename, List *doclist) {
	int num_docs = 0;

	FILE *file = fopen(datafilename, "r");
	if (!file) {
		perror(datafilename);
		return -1;
	}

	int id;
	float score;
	while (fscanf(file, "%d %f\n", &id, &score) == 2) {
		Document *document = new_document(id, score);
		list_add_end(doclist, document);
		num_docs += 1;
	}

	// all done!
	fclose(file);
	return num_docs;
}


// create a new document with a given id and score
Document *new_document(int id, float score) {
	Document *document = malloc(sizeof *document);
	assert(document);

	document->id = id;
	document->score = score;

	return document;
}

// destroy a document
void free_document(Document *document) {
	assert(document != NULL);
	
	free(document);
}
