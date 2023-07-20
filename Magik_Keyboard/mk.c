#include "trie.h"
#define MAX_LENGTH 50

void insert(trie_t *keyboard, char word[])
{
	int value = 1;
	if (trie_search(keyboard, word)) {
		value = *(int *)trie_search(keyboard, word);
		value++;
	}
	// printf("%d\n", value);
	trie_insert(keyboard, word, &value);
}

void load(trie_t *keyboard, FILE *in)
{
	char word[MAX_LENGTH];
	while (fscanf(in, "%s", word) == 1)
		insert(keyboard, word);
	fclose(in);
}

void exit_keyboard(trie_t *keyboard)
{
	trie_free(&keyboard);
}

void remove_word(trie_t *keyboard, char word[])
{
	if (!trie_search(keyboard, word))
		return;
	trie_remove(keyboard, word);
}

void dfs_autocorrect(trie_t *keyboard, trie_node_t *node, char word[],
					 char new_word[], int k, int letter, char ***replace,
					 int *number_words)
{
	if (!node || !word || !new_word)
		return;
	int length = strlen(word);
	// if the new word is fully built
	if (letter == length) {
		if (node->end_of_word && k >= 0 && strcmp(new_word, word) != 0) {
			// insert in the array of replace words
			(*number_words)++;
			(*replace)[(*number_words) - 1] =
				malloc((strlen(new_word) + 1) * sizeof(char));
			strcpy((*replace)[(*number_words) - 1], new_word);
		}
		return;
	}

	if (letter >= length)
		return;

	for (int i = 0; i < keyboard->alphabet_size; i++) {
		if (node->children[i]) {
			new_word[letter] = i + 'a';
			if (word[letter] != i + 'a')  // a change made
				dfs_autocorrect(keyboard, node->children[i], word, new_word,
								k - 1, letter + 1, replace, number_words);
			else  // no changes
				dfs_autocorrect(keyboard, node->children[i], word, new_word, k,
								letter + 1, replace, number_words);
		}
	}
}

void autocorrect(trie_t *keyboard, char word[], int k)
{
	if (!keyboard->root || !word)
		return;
	int number_words = 0;
	char **replace = malloc(sizeof(char *) * keyboard->size);

	char new_word[strlen(word) + 1];
	strcpy(new_word, word);
	new_word[strlen(word)] = '\0';
	if (trie_search(keyboard, word)) {
		// insert in the array of words
		number_words++;
		replace[number_words - 1] = malloc((strlen(word) + 1) * sizeof(char));
		strcpy(replace[number_words - 1], word);
	}

	dfs_autocorrect(keyboard, keyboard->root, word, new_word, k, 0, &replace,
					&number_words);

	if (!number_words) {
		printf("No words found\n");
		return;
	}

	// sort the array
	for (int i = 0; i < number_words - 1; i++)
		for (int j = i + 1; j < number_words; j++)
			if (strcmp(replace[i], replace[j]) > 0) {
				char aux[MAX_LENGTH];
				strcpy(aux, replace[i]);
				strcpy(replace[i], replace[j]);
				strcpy(replace[j], aux);
			}
	for (int i = 0; i < number_words; i++)
		printf("%s\n", replace[i]);
	for (int i = 0; i < number_words; i++)
		free(replace[i]);
	free(replace);
}

void get_shortest_helper(trie_t *keyboard, trie_node_t *node, char **shortest,
						 char word[])
{
	if (!keyboard)
		return;

	if (node->end_of_word && !strlen(*shortest))
		strcpy(*shortest, word);
	// update the shortest word
	else if (node->end_of_word && strlen(word) < strlen(*shortest)) {
		free(*shortest);
		*shortest = calloc(MAX_STRING_SIZE, sizeof(char));
		strcpy(*shortest, word);
		(*shortest)[strlen(word)] = '\0';
	}

	// check each child and go to the word
	for (int i = 0; i < keyboard->alphabet_size; i++) {
		if (node->children[i]) {
			char letter = i + 'a';
			char next_word[strlen(word) + 2];
			// build with sprintf the word needed, letter by letter
			sprintf(next_word, "%s%c", word, letter);
			get_shortest_helper(keyboard, node->children[i], shortest,
								next_word);
		}
	}
}

void get_shortest(trie_t *keyboard, char prefix[])
{
	if (!keyboard || !prefix)
		return;
	// parse the trie to the end of the prefix
	trie_node_t *node = keyboard->root;
	for (int i = 0; i < (int)strlen(prefix); i++) {
		int index = prefix[i] - 'a';
		if (!node->children[index]) {
			printf("No words found\n");
			return;
		}
		node = node->children[index];
	}

	char *shortest = calloc(MAX_STRING_SIZE, sizeof(char));

	char word[strlen(prefix) + 1];
	strcpy(word, prefix);
	word[strlen(word)] = '\0';

	get_shortest_helper(keyboard, node, &shortest, word);
	if (!strlen(shortest)) {
		printf("No words found\n");
		free(shortest);
		return;
	}
	printf("%s\n", shortest);
	free(shortest);
}

void get_min_string_helper(trie_t *keyboard, trie_node_t *node, char **min_str,
						   char word[])
{
	if (!keyboard)
		return;
	if (node->end_of_word && !strlen(*min_str))
		strcpy(*min_str, word);
	else if (node->end_of_word && strcmp(word, *min_str) < 0)
		strcpy(*min_str, word);

	// check each child and go to the word
	for (int i = 0; i < keyboard->alphabet_size; i++) {
		if (node->children[i]) {
			char letter = i + 'a';
			char next_word[strlen(word) + 2];
			// build with sprintf the word needed, letter by letter
			sprintf(next_word, "%s%c", word, letter);
			get_min_string_helper(keyboard, node->children[i], min_str,
								  next_word);
		}
	}
}

void get_min_string(trie_t *keyboard, char prefix[])
{
	if (!keyboard || !prefix) {
		printf("No words found\n");
		return;
	}
	// parse the trie to the end of the prefix
	trie_node_t *node = keyboard->root;
	for (int i = 0; i < (int)strlen(prefix); i++) {
		int index = prefix[i] - 'a';
		if (!node->children[index]) {
			printf("No words found\n");
			return;
		}
		node = node->children[index];
	}

	char *min_str = calloc(MAX_STRING_SIZE, sizeof(char));

	char word[strlen(prefix) + 1];
	strcpy(word, prefix);
	word[strlen(word)] = '\0';

	get_min_string_helper(keyboard, node, &min_str, word);

	if (!strlen(min_str)) {
		printf("No words found\n");
		free(min_str);
		return;
	}
	printf("%s\n", min_str);
	free(min_str);
}

void get_max_freq_helper(trie_t *keyboard, trie_node_t *node, char **max_freq,
						 char word[])
{
	if (!keyboard)
		return;

	if (node->end_of_word && !strlen(*max_freq))
		strcpy(*max_freq, word);
	// update the shortest word
	else if (node->end_of_word &&
			 *(int *)trie_search(keyboard, word) >
			 *(int *)trie_search(keyboard, *max_freq)) {
		free(*max_freq);
		*max_freq = calloc(MAX_STRING_SIZE, sizeof(char));
		strcpy(*max_freq, word);
		(*max_freq)[strlen(word)] = '\0';
	}

	// check each child and go to the word
	for (int i = 0; i < keyboard->alphabet_size; i++) {
		if (node->children[i]) {
			char letter = i + 'a';
			char next_word[strlen(word) + 2];
			// build with sprintf the word needed, letter by letter
			sprintf(next_word, "%s%c", word, letter);
			get_max_freq_helper(keyboard, node->children[i], max_freq,
								next_word);
		}
	}
}

void get_max_freq(trie_t *keyboard, char prefix[])
{
	if (!keyboard || !prefix)
		return;
	// parse the trie to the end of the prefix
	trie_node_t *node = keyboard->root;
	for (int i = 0; i < (int)strlen(prefix); i++) {
		int index = prefix[i] - 'a';
		if (!node->children[index]) {
			printf("No words found\n");
			return;
		}
		node = node->children[index];
	}

	char *max_freq = calloc(MAX_STRING_SIZE, sizeof(char));

	char word[strlen(prefix) + 1];
	strcpy(word, prefix);
	word[strlen(word)] = '\0';

	get_max_freq_helper(keyboard, node, &max_freq, word);
	if (!strlen(max_freq)) {
		printf("No words found\n");
		free(max_freq);
		return;
	}
	printf("%s\n", max_freq);
	free(max_freq);
}

void autocomplete(trie_t *keyboard, char prefix[], int nr_crit)
{
	if (nr_crit == 1) {
		get_min_string(keyboard, prefix);
	} else if (nr_crit == 2) {
		get_shortest(keyboard, prefix);
	} else if (nr_crit == 3) {
		get_max_freq(keyboard, prefix);
	} else if (nr_crit == 0) {
		get_min_string(keyboard, prefix);
		get_shortest(keyboard, prefix);
		get_max_freq(keyboard, prefix);
	}
}

int main(void)
{
	char alphabet[] = ALPHABET;
	trie_t *keyboard = trie_create(sizeof(int), ALPHABET_SIZE, alphabet, free);
	char command[MAX_LENGTH];
	while (1) {
		scanf("%s", command);
		if (strcmp(command, "INSERT") == 0) {
			char word[MAX_LENGTH];
			scanf("%s", word);
			insert(keyboard, word);
		} else if (strcmp(command, "LOAD") == 0) {
			char filename[MAX_LENGTH];
			scanf("%s", filename);
			FILE *in = fopen(filename, "rt");
			DIE(!in, "Failed to open file.\n");
			load(keyboard, in);
		} else if (strcmp(command, "REMOVE") == 0) {
			char word[MAX_LENGTH];
			scanf("%s", word);
			remove_word(keyboard, word);
		} else if (strcmp(command, "AUTOCORRECT") == 0) {
			char word[MAX_LENGTH];
			int k;
			scanf("%s%d", word, &k);
			autocorrect(keyboard, word, k);
		} else if (strcmp(command, "AUTOCOMPLETE") == 0) {
			char prefix[MAX_LENGTH];
			int nr_crit;
			scanf("%s%d", prefix, &nr_crit);
			// autocomplete(keyboard, prefix, nr_crit);
			autocomplete(keyboard, prefix, nr_crit);
		} else if (strcmp(command, "EXIT") == 0) {
			exit_keyboard(keyboard);
			break;
		}
	}
	return 0;
}
