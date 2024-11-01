
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26

typedef struct g_node_t g_node_t;
typedef struct g_trie_t g_trie_t;

struct g_node_t {
    g_node_t *sons[ALPHABET_SIZE];
    int is_finished;
    int count;
};

struct g_trie_t {
    g_node_t *root;
    int word_count;
};

// Function declarations
g_node_t *create_node(void);
g_trie_t *create_trie(void);
void insert(g_trie_t *trie, const char *word);
void load(g_trie_t *trie, const char *filename);
void remove_word(g_trie_t *trie, const char *word);
void autocorrect(g_trie_t *trie, char *word, int k);
void autocomplete(g_trie_t *trie, char *prefix, int nr_criteriu);
void free_trie(g_trie_t *trie);

#endif
