// utils.c

#include "utils.h"
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description) \
    do { \
        if (assertion) { \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
            perror(call_description); \
            exit(errno); \
        } \
    } while (0)

g_node_t *create_node(void) {
    g_node_t *node = (g_node_t *)malloc(sizeof(g_node_t));
    DIE(!node, "Malloc failed");

    node->is_finished = 0;
    node->count = 0;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->sons[i] = NULL;
    return node;
}

g_trie_t *create_trie(void) {
    g_trie_t *trie = (g_trie_t *)malloc(sizeof(g_trie_t));
    DIE(!trie, "Malloc failed");

    trie->root = create_node();
    trie->word_count = 0;
    return trie;
}

void insert(g_trie_t *trie, const char *word) {
    g_node_t *curr = trie->root;
    int n = strlen(word);
    for (int i = 0; i < n; i++) {
        int index = word[i] - 'a';
        if (!curr->sons[index]) {
            curr->count++;
            curr->sons[index] = create_node();
        }
        curr = curr->sons[index];
    }
    curr->is_finished++;
    trie->word_count++;
}

void load(g_trie_t *trie, const char *filename) {
    if (!trie || !filename) return;
    FILE *file = fopen(filename, "r");
    DIE(!file, "Open file failed");

    char word[100];
    while (fscanf(file, "%s", word) != EOF)
        insert(trie, word);
    fclose(file);
}

void remove_word(g_trie_t *trie, const char *word)
{
	if (!trie || !word)
		return;
	g_node_t *current = trie->root;
	g_node_t *path[100];

	int i;
	for (i = 0; word[i] != '\0'; i++) {
		int index = word[i] - 'a';
		if (!current->sons[index])
			return;
		path[i] = current;
		current = current->sons[index];
	}

	if (current->is_finished) {
		current->is_finished = 0;
		current->count--;

		if (current->count > 1) {
			for (int j = i - 1; j >= 0; j--) {
				int index = word[j] - 'a';
				g_node_t *parent = path[j];
				g_node_t *node = parent->sons[index];
				if (node->count == 0) {
					parent->sons[index] = NULL;
					free(node);
				} else {
					break;
				}
			}
		}
	}
}

int compare_strings(const void *a, const void *b)
{
	return strcmp(*(const char **)a, *(const char **)b);
}

void get_words_autocorrect(g_node_t *node, char *word, char *current_word,
						   int k, char **words, int *count)
{
	if (k < 0)
		return;
	if (*word == 0) {
		if (node->is_finished) {
			words[*count] = (char *)malloc((strlen(current_word) + 1) *
									  sizeof(char));
		DIE(!words[*count], "Malloc failed\n");

		strcpy(words[*count], current_word);
		(*count)++;
		}
		return;
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->sons[i]) {
			current_word[strlen(current_word)] = i + 'a';
			current_word[strlen(current_word)] = '\0';

			int changes = k;
		if (word[0] != current_word[strlen(current_word) - 1])
			changes--;
		get_words_autocorrect(node->sons[i], word + 1, current_word,
							  changes, words, count);
		current_word[strlen(current_word) - 1] = '\0';
		}
	}
}

void autocorrect(g_trie_t *trie, char *word, int k)
{
	int len = strlen(word);
	char *current_word = (char *)calloc((len + 2), sizeof(char));
	DIE(!current_word, "Calloc failed\n");

	char **words = (char **)malloc(trie->word_count * sizeof(char *));
	DIE(!words, "Malloc failed\n");

	int count = 0;
	get_words_autocorrect(trie->root, word, current_word, k, words, &count);
	if (count == 0) {
		printf("No words found\n");
	} else {
		qsort(words, count, sizeof(char *), compare_strings);
		for (int i = 0; i < count; i++)
			printf("%s\n", words[i]);
	}

	free(current_word);
	for (int i = 0; i < count; i++)
		free(words[i]);
	free(words);
}

void get_prefix(g_node_t *node, char *prefix, char **words, int *count)
{
	if (!node)
		return;

	if (node->is_finished) {
		words[*count] = (char *)malloc((strlen(prefix) + 1) * sizeof(char));
		DIE(!words[*count], "Malloc failed\n");

		strcpy(words[*count], prefix);
		(*count)++;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		char *new_prefix = (char *)malloc((strlen(prefix) + 2) * sizeof(char));
		DIE(!new_prefix, "Malloc failed\n");

		strcpy(new_prefix, prefix);
		new_prefix[strlen(prefix)] = 'a' + i;
		new_prefix[strlen(prefix) + 1] = '\0';
		get_prefix(node->sons[i], new_prefix, words, count);
		free(new_prefix);
	}
}

void autocomplete(g_trie_t *trie, char *prefix, int nr_criteriu)
{
	g_node_t *curr = trie->root;
	int n = strlen(prefix);
	char current_word[100] = "";

	for (int i = 0; i < n; i++) {
		int index = prefix[i] - 'a';
		if (!curr->sons[index]) {
			printf("No words found\n");
			return;
		}
		current_word[i] = prefix[i];
		curr = curr->sons[index];
	}

	char **words = (char **)malloc(trie->word_count * sizeof(char *));
	int count = 0;
	get_prefix(curr, current_word, words, &count);
	if (count == 0) {
		printf("No words found\n");
		if (nr_criteriu == 0) {
			printf("No words found\n");
			printf("No words found\n");
		}
		free(words);
		return;
	}

	if (nr_criteriu == 0 || nr_criteriu == 1) {
		char *smallest_lex = words[0];
		for (int i = 1; i < count; i++)
			if (strcmp(words[i], smallest_lex) < 0)
				smallest_lex = words[i];
		if (smallest_lex)
			printf("%s\n", smallest_lex);
		else
			printf("No words found\n");
	}

	if (nr_criteriu == 0 || nr_criteriu == 2) {
		char *shortest_word = NULL;
		int shortest_len = -1;
		for (int i = 0; i < count; i++) {
			int current_len = strlen(words[i]);
			if (!shortest_word || current_len < shortest_len) {
				shortest_word = words[i];
				shortest_len = current_len;
			}
		}
		if (shortest_word)
			printf("%s\n", shortest_word);
		else
			printf("No words found\n");
	}

	if (nr_criteriu == 0 || nr_criteriu == 3) {
		int max_freq = -1;
		char *most_frequent = NULL;
		for (int i = 0; i < count; i++) {
			curr = trie->root;
			for (int j = 0; j < strlen(words[i]); j++)
				curr = curr->sons[words[i][j] - 'a'];

			if (curr->is_finished > max_freq) {
				max_freq = curr->is_finished;
				most_frequent = words[i];
			} else if (curr->is_finished == max_freq) {
				if (strcmp(words[i], most_frequent) < 0)
					most_frequent = words[i];
			}
		}
		if (most_frequent)
			printf("%s\n", most_frequent);
		else
			printf("No words found\n");
	}

	for (int i = 0; i < count; i++)
		free(words[i]);
	free(words);
}

void free_trie_nodes(g_node_t *node)
{
	if (!node)
		return;
	for (int i = 0; i < ALPHABET_SIZE; i++)
		free_trie_nodes(node->sons[i]);
	free(node);
}

// Function to free the trie
void free_trie(g_trie_t *trie)
{
	if (!trie)
		return;
	free_trie_nodes(trie->root);
	free(trie);
}



