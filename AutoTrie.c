#include "utils.h"

int main(void)
{
	g_trie_t *trie = create_trie();
	char command[500];
	char argument[500];

	while (1) {
		scanf("%s", command);
		if (strcmp(command, "INSERT") == 0) {
			scanf("%s", argument);
			insert(trie, argument);
		} else if (strcmp(command, "LOAD") == 0) {
			scanf("%s", argument);
			load(trie, argument);
		} else if (strcmp(command, "REMOVE") == 0) {
			scanf("%s", argument);
			remove_word(trie, argument);
			trie->word_count--;
		} else if (strcmp(command, "AUTOCORRECT") == 0) {
			scanf("%s", argument);
			int k;
			scanf("%d", &k);
			autocorrect(trie, argument, k);
		} else if (strcmp(command, "AUTOCOMPLETE") == 0) {
			scanf("%s", argument);
			int k;
			scanf("%d", &k);
			autocomplete(trie, argument, k);
		} else if (strcmp(command, "EXIT") == 0) {
			free_trie(trie);
			break;
		}
	}
	return 0;
}
