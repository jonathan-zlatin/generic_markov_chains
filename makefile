tweets: linked_list.c markov_chain.c tweets_generator.c linked_list.h markov_chain.h
	gcc -Wall -Wextra -Wvla -std=c99 linked_list.c markov_chain.c tweets_generator.c -o tweets_generator

snakes: linked_list.c markov_chain.c tweets_generator.c linked_list.h markov_chain.h
	gcc -Wall -Wextra -Wvla -std=c99 linked_list.c markov_chain.c snakes_and_ladders.c -o snakes_and_ladders
