#include "markov_chain.h"

LinkedList *create_link_list ()
{
  LinkedList *link_list = (LinkedList *) malloc (sizeof (LinkedList));
  if (!link_list)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  link_list->first = NULL;
  link_list->last = NULL;
  link_list->size = 0;
  return link_list;
}

void *safe_realloc (void *old_p, size_t new_size)
{
  void *new_p = realloc (old_p, new_size);
  if (!new_p)
  {
    free (old_p);
    return NULL;
  }
  return new_p;
}

//FREE MarkovNode in case of Fail (with safe realloc)
int update_frequencies_list_size (MarkovNode *markov_node)
{
  if (markov_node->frequencies_list_size == 0) // first following word
  {                                            //we don't have freqlist
    markov_node->frequencies_list = calloc (1, \
                                sizeof (MarkovNodeFrequency));
    if (!markov_node->frequencies_list)
    {
      printf (ALLOCATION_ERROR_MASSAGE);
      return 1;
    }
    markov_node->frequencies_list_size = 1;
    return 0;
  }
  markov_node->frequencies_list\
 = safe_realloc (markov_node->frequencies_list, \
        ((markov_node->frequencies_list_size) + 1)\
 * sizeof (MarkovNodeFrequency));
  if (!markov_node->frequencies_list)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return 1;
  }
  markov_node->frequencies_list_size += 1;
  return 0;
}

Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *c_state = markov_chain->database->first;
  while (c_state != NULL)
  {
    if (markov_chain->comp_func (c_state->data->data, data_ptr) == 0)
    {
      return c_state;
    }
    c_state = c_state->next;
  }

  void *n_data_ptr = markov_chain->copy_func (data_ptr);
  if (!n_data_ptr)
  {
    free_database (&markov_chain);
    return NULL;
  }

  MarkovNode *new_mark_node = malloc (sizeof (MarkovNode));
  if (!new_mark_node)
  {
    markov_chain->free_data(n_data_ptr);
    free_database (&markov_chain);
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }

  *new_mark_node = (MarkovNode) {n_data_ptr, NULL, 0};

  int not_succeed_to_add = add (markov_chain->database, \
                             new_mark_node);
  if (not_succeed_to_add)
  {
    markov_chain->free_data(n_data_ptr);
    free (new_mark_node);
    free_database (&markov_chain);
    return NULL;
  }
  return markov_chain->database->last;

}

Node *get_node_from_database (MarkovChain *markov_chain, \
                                     void *data_ptr)
{
  Node *c_state = markov_chain->database->first;
  while (c_state != NULL)
  {
    if (markov_chain->comp_func (c_state->data->data, data_ptr) == 0)
    {
      return c_state;
    }
    c_state = c_state->next;
  }
  return NULL;
}

bool
add_node_to_frequencies_list (MarkovNode *first_node, \
                              MarkovNode *second_node,
                          MarkovChain *markov_chain)
{
  MarkovNodeFrequency *c_state = first_node->frequencies_list;
  for (int i = 0; i < first_node->frequencies_list_size; i++)
  {
    if (markov_chain->comp_func (c_state->Markov_Node->data,
                                 second_node->data)
        == 0)
    {
      c_state->frequency += 1;
      return true;
    }
    c_state++;
  }
  // Adding new Node -> Increasing the frequencies_list_size
  if (update_frequencies_list_size (first_node))
  {
    return false;
  }
  // Adding new MarkovNodeFrequency to the first node FreqList
  c_state = first_node->frequencies_list +(first_node->\
                         frequencies_list_size - 1);
  c_state->Markov_Node = second_node;
  c_state->frequency = 1;
  return true;
}
void free_database (MarkovChain **markov_chain)
//+---------+   +------------+   +------+   +----------+
//|freqList |-->| markovNode |-->| Node |-->| linkList |-->|mc|
//|         |   | + word     |   |      |   |          |
//+---------+   +------------+   +------+   +----------+
{
  if (*markov_chain == NULL)
  {
    return;
  }
  Node *c_node = (*markov_chain)->database->first;
  Node *tmp_node = NULL;
  while (c_node != NULL)
  {
    // free each Node in the linked list;
    tmp_node = c_node;
    c_node = c_node->next;

    // free the frequencies list
    free (tmp_node->data->frequencies_list);
    tmp_node->data->frequencies_list = NULL;
    tmp_node->data->frequencies_list_size = 0;

    // free the MarkovNode and it's relative data
    (*markov_chain)->free_data (tmp_node->data->data); // MarkovNode-data
    tmp_node->data->data = NULL;
    free (tmp_node->data); // MarkovNode
    tmp_node->data = NULL;

    // free the Node
    tmp_node->next = NULL; //delete the Node 'next' pointer
    free (tmp_node); // Node
    tmp_node = NULL;
  }

  // free the linked list
  (*markov_chain)->database->first = NULL;
  (*markov_chain)->database->last = NULL;
  (*markov_chain)->database->size = 0;
  free ((*markov_chain)->database);
  (*markov_chain)->database = NULL;

  // free the MarkovChain
  free (*markov_chain);
  *markov_chain = NULL;
}

int get_random_number (int max_number)
{
  return rand () % max_number;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{ // check more
  Node *tmp_node = NULL;
  int random_number = 0;
  while (true)
  {
    tmp_node = markov_chain->database->first;
    random_number = get_random_number (\
                    markov_chain->database->size);
    for (int i = 0; i < random_number; i++)
    {
      tmp_node = tmp_node->next;
    }
    if (!markov_chain->is_last(tmp_node->data->data))
    {
      break;
    }
  }
  return tmp_node->data;
}

int freq_sum (MarkovNode *state_ptr)
{
  int freq_sum = 0;
  MarkovNodeFrequency *freq_list = state_ptr->\
                                    frequencies_list;
  for (int i = 0; i < state_ptr->frequencies_list_size; i++)
  {
    freq_sum += freq_list->frequency;
    freq_list++;
  }
  return freq_sum;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int random_number = get_random_number (freq_sum\
 (state_struct_ptr));
  MarkovNodeFrequency *freq_list = state_struct_ptr->frequencies_list;
  while (random_number > 0)
  {
    random_number -= freq_list->frequency;
    if (random_number >= 0)
    {
      freq_list++;
    }
  }
  return freq_list->Markov_Node;
}

void generate_tweet (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  MarkovNode *c_node = first_node;
  if (!c_node)
  {
    c_node = get_first_random_node (markov_chain);
  }
  void *c_word = c_node->data;
  while ((!markov_chain->is_last(c_word)) && max_length > 0)
  {
      if(max_length==1 && !markov_chain->is_last(c_word))
      {
          break;
      }
    markov_chain->print_func (c_word);
    c_node = get_next_random_node (c_node);
    c_word = c_node->data;
    max_length--;
  }
  if (max_length > 0)
  {
    markov_chain->print_func (c_word);
  }
}




