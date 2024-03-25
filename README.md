# 📝 Tweets Generator

This is a C program for generating tweets using Markov chains. It reads a text file, constructs a Markov chain based on the input text, and generates tweets using the constructed chain.

## 🚀 Overview

The program consists of several components:

- `markov_chain.c` and `markov_chain.h`: Implementations for managing Markov chains, including functions for creating the chain, adding nodes, generating tweets, and freeing memory.
- `linked_list.c` and `linked_list.h`: Implementations for managing linked lists, used in storing Markov nodes.
- `tweets_generator.c`: The main program file, which reads the input text, constructs the Markov chain, and generates tweets.
- `tweets_generator`: The executable file generated after compilation.

## 💻 Usage

To use the program, follow these steps:

1. Compile the program using the provided Makefile:
   
make tweets

2. Run the program with the following command:

- `<seed>`: An integer used as the seed for random number generation.
- `<tweets_amount>`: The number of tweets to generate.
- `<input_file_path>`: The path to the input text file.
- `[<words_to_read>]` (optional): The number of words to read from the input file. If not provided, the program reads the entire file.

## 📄 Example

Suppose you have an input text file named `input.txt`, and you want to generate 10 tweets using seed `12345`:

./tweets_generator 12345 10 input.txt

## 📝 Notes

- The program handles errors such as invalid input arguments, failure to open the input file, and memory allocation errors.
- The generated tweets are based on the Markov chain constructed from the input text, providing a creative way to generate text content.


./tweets_generator <seed> <tweets_amount> <input_file_path> [<words_to_read>]


#c #generic_programing #Markov_chains 
