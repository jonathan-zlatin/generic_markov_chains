//
// Created by יהונתן זלטין on 06/05/2023.
//

#include "markov_chain.h"
#include "string.h"
#define LEGAL_INPUT_AMOUNT1 4
#define LEGAL_INPUT_AMOUNT2 5
#define LINE 1000
#define BASE 10
#define SEED 1
#define TWEA 2
#define PATH 3
#define WO2R 4
#define MAXL 20
#define INFY -1

//Error messages:
#define ERROR_PATH "Error: The path failed to open\n"

//Usage messages:
#define ERROR_INPUT_AMOUNT "Usage: The args amount should be 3 or 4\n"

static int define_seed (char *const *argv);

static void promote_word2 (Node *node2, int *words_to_read, \
                   Node **node1, char **word2_s);

static int
fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char line[LINE];
  Node *node1 = NULL;
  Node *node2 = NULL;
  while (words_to_read != 0)
  {
    if (NULL == fgets (line, LINE, fp))
    {
      break;
    }
    char *word2_s = strtok (line, " \n\t\r");
    while ((word2_s != NULL) && words_to_read != 0)
    {// Check if word2 in the DB
      node2 = get_node_from_database (markov_chain, word2_s);
      if (!node2)
      {
        node2 = add_to_database (markov_chain, word2_s);
        if (!node2)
        {
          return 1;
        }

      }// Add word2_d to word1_d FrequentList (if node1 finishes sentence skip)
      if (node1 != NULL
          &&
          ((char *) (node1->data->data))[strlen (((char *) node1->data->data))
                                         - 1] != '.')
      {
        if (!add_node_to_frequencies_list (node1->data,
                                           node2->data, markov_chain))
        {
          return 1;
        }
      }
      promote_word2 (node2, &words_to_read, &node1,
                     &word2_s);
    }
  }
  return 0;
}

static void promote_word2 (Node *node2, int *words_to_read, \
                   Node **node1, char **word2_s)
{
  (*node1) = node2;
  (*word2_s) = strtok (NULL, " \n\t\r");
  (*words_to_read)--;
}

static int test_valid_args (int argc)
{
  if (argc != LEGAL_INPUT_AMOUNT1 && argc != LEGAL_INPUT_AMOUNT2)
  {
    printf (ERROR_INPUT_AMOUNT);
    return 1;
  }
  return 0;
}
static int define_seed (char *const *argv)
{
  int seed = (int) strtol (argv[SEED], NULL, BASE);
  int tweets_amount = (int) strtol (argv[TWEA], NULL, BASE);
  srand (seed);
  return tweets_amount;
}

static void print_tweet (int tweets_amount, MarkovChain *mc, \
                                    MarkovNode *first_node, int i)
{
  printf ("Tweet %d: ", i + 1);
  first_node = get_first_random_node (mc);
    generate_tweet(mc, first_node, MAXL);
  if (i != tweets_amount)
  {
    printf ("\n");
  }
}

static void charprt (void *word)
{
  char *c_word = (char *) word;
  if (c_word[strlen(c_word)-1]=='.')
  {
      printf ("%s", c_word);
  }
  else
    {
      printf ("%s ", c_word);
    }
}

static int charcmp (void *d1, void *d2)
{
  return strcmp ((char *) d1, (char *) d2);
}

static void *charcpy (void *original)
{
  original = (char *) original;
  char *replica = malloc ((strlen (original) + 1));
  if (!replica)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  if (!strcpy(replica, original))
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    free (original);
    return NULL;
  }
  return replica;
}

static bool charlst (void *word)
{
  char *word1 = (char *) word;
  if (word1[strlen (word1) - 1] == '.')
  {
    return true;
  }
  return false;
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

  mc->comp_func = charcmp;
  mc->free_data = free;
  mc->print_func = charprt;
  mc->copy_func = charcpy;
  mc->is_last = charlst;
  return mc;
}

int main (int argc, char *argv[])
{
  if (test_valid_args (argc))
  {
    return EXIT_FAILURE;
  }
  int tweets_amount = define_seed (argv);
  FILE *file = fopen (argv[PATH], "r");
  if (file == NULL)
  {
    printf (ERROR_PATH);
    return EXIT_FAILURE;
  }
  int words_to_read = INFY;
  if (argc == LEGAL_INPUT_AMOUNT2)
  {
    words_to_read = (int) strtol (argv[WO2R], \
                          NULL, BASE);
  }
  if (!words_to_read) // No need to read at all.
  {
    fclose (file);
    return EXIT_SUCCESS;
  }// Create a new markov chain.
  MarkovChain *mc = create_markov_chain ();
  if (!mc)
  {
    return EXIT_FAILURE;
  }

// All allocations succeed -> Fill the DB
  if (fill_database (file, words_to_read, mc))
  {
    fclose (file);
    return EXIT_FAILURE;
  }
  fclose (file);// Generating tweets base on the filled DB
  MarkovNode *first_node = NULL;
  for (int i = 0; i < tweets_amount; i++)
  {
    print_tweet (tweets_amount, mc, first_node, i);
  }
    free_database(&mc);
  return EXIT_SUCCESS;
}
