/* * * * * * *
 * Module for creating and manipulating a multi-term inverted file index
 *
 * created for COMP20007 Design of Algorithms - Assignment 1, 2018
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#ifndef INDEX_H
#define INDEX_H

#include "list.h"


/*                         DO NOT CHANGE THIS FILE
 * 
 * DO NOT modify the structs, types or function prototypes defined in this file.
 *
 * We will test your assignment with an unmodified version of this file. Any
 * changes you make will be lost. This may lead to compile errors.
 */


// an index knows its size (number of terms), an array of those terms, and an 
// array of document-score lists
// these values can be used, but should not be *modified* outside of index.c. 
// they are read-only!
typedef struct index Index;
struct index {
	int num_terms;   // the number of terms currently in the index
	int max_terms;   // the maximum number of terms the index has space for
	char **terms;    // array of strings, one string for each term
	List **doclists; // array of Document lists, one for each term
					 // both arrays have undefined valued in cells from index 
					 // num_terms onwards
};

// a document just holds a pair of values: a document id and the correponding
// document score
// these values can be used, but should not be *modified* outside of index.c. 
// they are read-only!
typedef struct document Document;
struct document {
	int id;			// the document's id number
	float score;	// the document's score for a particular term
};

// create a new, empty index, with space for a maximum of max_terms terms
Index *new_index(int max_terms);

// destroy an index, it's associated document-score lists, and their associated
// documents (but not the term strings)
void free_index(Index *index);

// add a new term and its document-scorelist to an index, based on data in 
// datafilename
// return number of documents in list, or -1 on error
int index_add_term(Index *index, char *term, char *datafilename);



// create a new document with a given id and score
Document *new_document(int id, float score);

// destroy a document
void free_document(Document *document);


#endif
