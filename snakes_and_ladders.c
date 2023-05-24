#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define VALID_AMOUNT 3
#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define BASE 10
#define SEED 1
#define PAMO 2
/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder
    // in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in
    // case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
  {
      free_database(database);
  }
  return EXIT_FAILURE;
}

static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc (sizeof (Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++)
      {
        free (cells[j]);
      }
      handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
        add_node_to_frequencies_list(from_node, to_node, markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell *) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
          add_node_to_frequencies_list(from_node,
                                   to_node, markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

static void cellprt (void *cell)
{
  Cell *c_cell = (Cell *) cell;
  if (c_cell->number==BOARD_SIZE)
  {
    printf ("[100]");
    return;
  }
  if (c_cell->ladder_to!=-1)
  {
    printf ("[%d]-ladder to %d -> ", c_cell->number, c_cell->ladder_to);
  }
  else if (c_cell->snake_to!=-1)
  {
    printf ("[%d]-snake to %d -> ", c_cell->number, c_cell->snake_to);
  }
  else
  {
    printf ("[%d] -> ", c_cell->number);
  }
}

static int cellcmp (void *d1, void *d2)
{
  Cell *c_d1 = (Cell *) d1;
  Cell *c_d2 = (Cell *) d2;
  return c_d1->number - c_d2->number;
}

static void *cellcpy (void *original)
{
  Cell *original1 = (Cell *) original;
  Cell *replica = malloc (sizeof (Cell));
  if (!replica)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  replica->number = original1->number;
  replica->ladder_to = original1->ladder_to;
  replica->snake_to = original1->snake_to;
  return replica;
}

static bool celllst (void *cell)
{
  Cell *c_cell = (Cell *) cell;
  if (c_cell->number ==BOARD_SIZE)
  {
    return true;
  }
  return false;
}

static void free_cell (void *cell)
{
  Cell *cell1 = (Cell *) cell;
  cell1->snake_to=0;
  cell1->ladder_to=0;
  cell1->number=0;
  free (cell1);
  cell1=NULL;
}
static void print_path (int paths_amount, MarkovChain *mc, \
                                    MarkovNode *first_node, int i)
{
  printf ("Random Walk %d: ", i + 1);
    generate_tweet(mc, first_node, MAX_GENERATION_LENGTH);
  if (i != paths_amount)
  {
    printf ("\n");
  }
}

static MarkovChain *create_markov_chain ()
{
  MarkovChain *mc = malloc (sizeof (MarkovChain));
  if (!mc) //Couldn't allocate the MarkovChain
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  mc->database = create_link_list ();
  if (!mc->database) //Couldn't allocate the linkedList
  {
    free (mc);
    mc = NULL;
    return NULL;
  }

  mc->comp_func = cellcmp;
  mc->free_data = free_cell;
  mc->print_func = cellprt;
  mc->copy_func = cellcpy;
  mc->is_last = celllst;
  return mc;
}

static int define_seed (char *const *argv)
{
  int seed = (int) strtol (argv[SEED], NULL, BASE);
  int paths_amount = (int) strtol (argv[PAMO], NULL, BASE);
  srand (seed);
  return paths_amount;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  if (argc != VALID_AMOUNT)
  {
    return EXIT_FAILURE;
  }
  int paths_amount = define_seed (argv);
//  int cubes_to_throw = MAX_GENERATION_LENGTH;
  MarkovChain *mc = create_markov_chain ();
  if (!mc)
  {
    return EXIT_FAILURE;
  }
// All allocations succeed -> Fill the DB
  if (fill_database (mc))
  {
    return EXIT_FAILURE;
  }
  // Generating paths base on the filled DB
  MarkovNode *first_node = mc->database->first->data;
  for (int i = 0; i < paths_amount; i++)
  {
    print_path (paths_amount, mc, first_node, i);
  }
    free_database(&mc);
  return EXIT_SUCCESS;
}
