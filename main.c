/* * * * * * * * *
 * Main program:
 * reads command line options, builds index, calls querying code
 * 
 * created for COMP20007 Design of Algorithms - Assignment 1, 2018
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "index.h"
#include "query.h"

/*                         DO NOT CHANGE THIS FILE
 * 
 * DO NOT modify any of the constants, types, functions or anything else
 * defined inside this file.
 *
 * We will test your assignment with an unmodified version of this file. Any
 * changes you make will be lost. This may lead to compile errors.
 */


#define MAXIMUM_NUM_TERMS 256
#define DEFAULT_NUM_DOCUMENTS 131563

#define DATA_PATH "data/"
#define DATA_PATH_FORMAT DATA_PATH "%s.txt"
#define MAXIMUM_DATA_PATH_LEN 256


// enumeration of the tasks in order of their numbers
typedef enum task {
	TASK_ARRAY = 1,
	TASK_MERGE = 2
} Task;

// struct to store the command line options
typedef struct options {
	Task task;
	int n_results, n_documents;
	int n_terms;
	char *terms[MAXIMUM_NUM_TERMS];
} Options;

// helper functions
Options get_options(int argc, char **argv);
bool validate_options(Options options);
Index *read_index_files(char *terms[], int n_terms);
bool validate_index(Index *index, int n_documents);

int main(int argc, char **argv) {
	
	// parse and validate command line options
	Options options = get_options(argc, argv);
	bool valid_options = validate_options(options);
	if (!valid_options) {
		exit(EXIT_FAILURE);
	}

	// build index from data files
	Index *index = read_index_files(options.terms, options.n_terms);
	bool valid_index = validate_index(index, options.n_documents);
	if (!valid_index) {
		exit(EXIT_FAILURE);
	}

	// branch to relevant function depending on execution mode
	switch (options.task) {
		case TASK_ARRAY:
			print_array_results(index, options.n_results, options.n_documents);
			break;
		case TASK_MERGE:
			print_merge_results(index, options.n_results);
			break;
	}

	// clean up
	free_index(index);

	// done!
	exit(EXIT_SUCCESS);
}

// read command line options into Options struct
Options get_options(int argc, char **argv) {
	
	// create the Options structure with defaults
	Options options = {
		.task = 0,
		.n_terms = 0,
		.n_results = -1,
		.n_documents = DEFAULT_NUM_DOCUMENTS,
	};
	for (int i = 0; i < MAXIMUM_NUM_TERMS; i++) {
		options.terms[i] = NULL;
	}

	// use C's built-in getopt function to scan inputs by flag
	char input;
	while ((input = getopt(argc, argv, "t:r:d:")) != EOF){
		switch (input){
			case 't': // set task
				options.task = atoi(optarg);
				break;
			case 'r': // set number of results to find
				options.n_results = atoi(optarg);
				break;
			case 'd': // override number of documents
				options.n_documents = atoi(optarg);
				break;
			default:
				break;
		}
	}
	
	// any remaining inputs are query terms
	for (int i = optind; i < argc; i++) {
		if (options.n_terms < MAXIMUM_NUM_TERMS) {
			options.terms[options.n_terms] = argv[i];
		}
		options.n_terms++;
	}

	return options;
}

// check validity of options provided
// prints usage tips on missing or invalid arguments
bool validate_options(Options options) {
	bool valid = true;

	// check task has been set
	if (options.task != TASK_ARRAY && options.task != TASK_MERGE) {
		fprintf(stderr,
			"please specify which task to run, using the -t flag:\n");
		fprintf(stderr, " -t 1: accumulator array approach (task 1)\n");
		fprintf(stderr, " -t 2: multi-way merge approach (task 2)\n");
		valid = false;
	}

	// check results has been set, and is not less than 0
	if (options.n_results < 0) {
		fprintf(stderr,
			"please provide a valid number of results via the -r flag.\n");
		valid = false;
	}

	// if documents has been set, check it is not less than 0
	if (options.n_documents < 0) {
		fprintf(stderr,
			"please provide a valid number of documents via the -d flag\n"
			"(or omit this option for the default number of documents).\n");
		valid = false;
	}

	// check terms have been provided
	if (options.n_terms == 0) {
		fprintf(stderr,
			"please provide at least one query term (after other options).\n");
		valid = false;

	// but not too many! ;)
	} else if (options.n_terms > MAXIMUM_NUM_TERMS) {
		fprintf(stderr,
			"warning: query too long, truncated to first %d terms.",
			MAXIMUM_NUM_TERMS);
		// NOT ACTUALLY AN ERROR so do not set valid to false
	}

	return valid;
}

// read data files into an index
Index *read_index_files(char *terms[], int n_terms) {
	char filenamebuffer[MAXIMUM_DATA_PATH_LEN];

	Index *index = new_index(n_terms);

	for (int i = 0; i < n_terms; i++) {
		char *term = terms[i];
		snprintf(filenamebuffer, MAXIMUM_DATA_PATH_LEN, DATA_PATH_FORMAT, term);
		int num_docs = index_add_term(index, term, filenamebuffer);
		if (num_docs < 0) {
			// error while reading data files!?
			free_index(index);
			return NULL;
		}
	}
	
	return index;
}

// check validity of index loaded
// prints data loading and format tips on any errors enocuntered
bool validate_index(Index *index, int n_documents) {
	// check that the index was actually created
	if (!index) {
		fprintf(stderr,
			"error creating index.\n"
			"all data files must be accessible through relative path '%s'\n"
			"(relative to wherever you are calling the program from)\n", 
			DATA_PATH);
		return false;
	}
	
	// check that each document in the index has a valid id and a valid score
	for (int i = 0; i < index->num_terms; i++) {
		for (Node *node = index->doclists[i]->head; node; node = node->next) {
			Document *document = node->data;
			if (document->id > n_documents) {
				fprintf(stderr,
					"error: unexpected document id (%d)\n"
					"data contains a document id (%d) at or above the number "
					"of documents specified\n"
					"via the -d flag (or default number documents) (%d).\n",
					document->id, document->id, n_documents);
				return false;
			}
			if (document->score < 0) {
				fprintf(stderr,
					"error: data contains a negative score (%f...)\n",
					document->score);
				return false;
			}
		}
	}
	return true;
}
