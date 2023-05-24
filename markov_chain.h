#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE \
    "Allocation failure: " \
    "Failed to allocate new memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/
typedef struct MarkovNodeFrequency MarkovNodeFrequency;
typedef void (*print_f) (void *data);
typedef int (*comp_f) (void *d1, void *d2);
typedef void (*free_d) (void *ptr);
typedef void *(*copy_f) (void *ptr);
typedef bool (*is_l) (void *ptr);
/***************************/



/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNode
{
    void *data;
    MarkovNodeFrequency *frequencies_list;
    int frequencies_list_size; // Remember to update all the time (starts by 0)
} MarkovNode;

typedef struct MarkovNodeFrequency
{
    MarkovNode *Markov_Node;
    int frequency;
} MarkovNodeFrequency;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain
{
    LinkedList *database;
    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    /* <fill_type> */ print_f print_func;

    // pointer to a func that gets 2 pointers of generic data type
    // (same one) and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    /* <fill_type> */ comp_f comp_func;

    // a pointer to a function that gets a pointer of generic data
    // type and frees it.
    // returns void.
    /*<fill_type>*/ free_d free_data;

    // a pointer to a function that gets a pointer of generic data type and
    // returns a newly allocated copy of it
    // returns a generic pointer.
    /*<fill_type>*/ copy_f copy_func;

    //  a pointer to function that gets a pointer of generic data type and
    //        returns:
    //      - true if it's the last state.
    //      - false otherwise.
    /*<fill_type>*/ is_l is_last;
} MarkovChain;

/**
* Create an empty linked list
 * @return ptr to the new linked list or NULL if it fail.
 */
LinkedList *create_link_list ();

/**
 * Updates markov_node frequent_list by adding its size 1
 * frees the frequent_list in case of fail
 * @param markov_node the chain to look in its database
 * @return 0 if it was able to update the frequent_list.
 *         1 if it wasn't
 */
int update_frequencies_list_size (MarkovNode *markov_node);

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a
 *                                                  random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database (MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the
 * first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful,
*                           false if in case of allocation error.
 */
bool add_node_to_frequencies_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);
/**
* Check if data_ptr is in database. If so,
 * return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state,
 *         NULL if state not in
 * database.
 */
Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node *add_to_database (MarkovChain *markov_chain, void *data_ptr);

#endif /* MARKOV_CHAIN_H */
