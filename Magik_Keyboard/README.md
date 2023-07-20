Magic Keyboard

The Magic Keyboard was implemented using a Trie.

In trie.h and trie.c are the functions for implementing a trie: create, insert, remove, search and free.

The values in the trie are put at the end of each word, not in each node.

In mk.c ar is the implementation for the Magic Keyboard. Using a trie, the value that is put in the final node for each word is its frequency. 

INSERT: calls the function insert in a trie. If the key is already present in the trie, it increases the value, which represents the frequency.

LOAD: Opens the file and reads every word. For every word is called the inset function in the trie.

REMOVE: Calls the function remove from a trie.

AUTOCORRECT: Uses two functions: dfs_autocorrect and autocorrect. Using the DFS, it goes in the trie and checks every words that has a distance(difference) less or equal to k given as a parametre. It constructs the right words we need for the operation. Is it is put a letter that does not exist in the original word, the DFS function is called with the k decreased, means it already has a different letter, otherwise it is called with the same k at that step. The words are put in an array, which will be sorted and then printed.

AUTOCOMPLETE: Uses 6 functions, 2 for each criteria, a helper and the printing function. With DFS it goes in the trie to the final node of the prefix. From there with the helper it goes from the end of the prefix and builds the words needed for this operation: the shortest, the minimum in lexicographic order and the word with the maximum frequency. For the criteria 0, all th functions are called to print all 3 words.

EXIT: It calls the function trie_free for freeing all the resources in the program and it ends it.
