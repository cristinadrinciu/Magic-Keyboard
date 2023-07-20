#include "trie.h"

trie_node_t *trie_create_node(trie_t *trie)
{
	// allocate the memory or the node
	trie_node_t *node = malloc(sizeof(trie_node_t));
	DIE(!node, "Error. Failed to alloc.\n");

	// initialize the end_of_word with 0
	// it's not yet th end of an word
	node->end_of_word = 0;

	// initialize the children array all with 0
	node->children = calloc(trie->alphabet_size, sizeof(trie_node_t *));
	DIE(!node->children, "Error. Failed to alloc.\n");

	// initialize the number of children with 0
	node->n_children = 0;

	// set the value 0/NULL
	node->value = NULL;

	return node;
}

trie_t *trie_create(int data_size, int alphabet_size, char *alphabet,
					void (*free_value_cb)(void *))
{
	// allocate memory for the trie as a structure
	trie_t *trie = malloc(sizeof(trie_t));
	DIE(!trie, "Error. Failed to alloc.\n");

	trie->size = 0;

	// set data size
	trie->data_size = data_size;

	trie->alphabet_size = alphabet_size;

	// set alphabet
	trie->alphabet = alphabet;

	// create the root
	trie->root = trie_create_node(trie);

	// alloc the value
	trie->root->value = malloc(trie->data_size);
	int value = -1;  // the value given for ""
	memcpy(trie->root->value, &value, trie->data_size);

	// set the free_function
	trie->free_value_cb = free_value_cb;

	// set the number of nodes with 1(has the root)
	trie->n_nodes = 1;
	return trie;
}

void trie_insert(trie_t *trie, char *key, void *value)
{
	if (strlen(key) == 0) {
		trie->root->value = malloc(trie->data_size);
		DIE(!trie->root->value, "Error. Failed to alloc.\n");
		memcpy(trie->root->value, &value, trie->data_size);
		// mark the end of the word
		trie->root->end_of_word = 1;
		return;
	}

	trie_node_t *node = trie->root;
	for (int i = 0; i < (int)strlen(key); i++) {
		int index = key[i] - 'a';
		// if the node does not have a child for that letter
		if (!node->children[index]) {
			// create the node to insert the letter
			node->children[index] = trie_create_node(trie);
			// increase the number of children
			node->n_children++;
			// increse the number of nodes in the tree
			trie->n_nodes++;
		}
		// work with the child
		node = node->children[index];
	}

	// for the final node atribute th value

	if (!node->value) {
		node->value = malloc(trie->data_size);
		DIE(!node->value, "Error. Failed to alloc.\n");
	}
	memcpy(node->value, value, trie->data_size);
	// mark the end of the word
	node->end_of_word = 1;
	// increase the size of the trie
	trie->size++;
}

void *trie_search(trie_t *trie, char *key)
{
	if (strlen(key) == 0)
		return trie->root->value;

	// get a node for the way to the key
	trie_node_t *node = trie->root;

	for (int i = 0; i < (int)strlen(key); i++) {
		int index = key[i] - 'a';
		if (!node->children[index])
			return NULL;
		node = node->children[index];
	}
	return node->value;
}

int remove_helper(trie_t *trie, trie_node_t *node, char *key)
{
	// cannot delete a non existent node
	if (!node)
		return 0;
	// for the last node
	if (!strlen(key)) {
		// free the value
		trie->free_value_cb(node->value);
		node->value = NULL;
		// the number of words from the trie decreases
		trie->size--;
		node->end_of_word = 0;
		// free the children array if there are no sons
		if (node->n_children == 0) {
			free(node->children);
			free(node);
			trie->n_nodes--;
			return 1;
		}
		return 0;
	}

	// take the first letter of the new key
	int index = key[0] - 'a';
	trie_node_t *next_node = node->children[index];
	// if it can be deleted the next node
	if (remove_helper(trie, next_node, key + 1)) {
		// remove it
		node->n_children--;
		node->children[index] = NULL;
		if (node->n_children == 0 && node->end_of_word == 0 &&
			node != trie->root) {
			free(node->children);
			free(node);
			trie->n_nodes--;
			// can be deleted the parent
			return 1;
		}
		return 0;
	}
	return 0;
}

void trie_remove(trie_t *trie, char *key)
{
	// call the function from the root
	remove_helper(trie, trie->root, key);
}

void free_helper(trie_t *trie, trie_node_t *node)
{
	// cannot delete a NULL node
	if (!node)
		return;

	for (int i = 0; i < trie->alphabet_size && node->n_children; ++i) {
		// get to each child and free the subtrie
		if (!node->children[i])
			continue;  // go to the next step

		// call recusivly the free_helper
		free_helper(trie, node->children[i]);
		// decrease the number of children
		node->n_children--;
		node->children[i] = NULL;
	}

	if (node->value) {
		// free the value
		trie->free_value_cb(node->value);
	}
	// free the children array
	free(node->children);

	// free the node itself
	free(node);

	// decrease the number of nodes of the trie
	trie->n_nodes--;
}

void trie_free(trie_t **p_trie)
{
	free_helper(*p_trie, (*p_trie)->root);

	// free the structure of the trie
	free(*p_trie);
}
