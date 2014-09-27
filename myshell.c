/*
 * myshell.c
 * A simple Linux shell for CMPT 300 HW1
 *
 * By Robert Hongpu Ma, September 2014
 *
 * Using `lex.c` from http://users.soe.ucsc.edu/~sbrandt/111/assignment1.html
 * Thanks for their work!
 */
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

/*
 * ISO 8601 date + 24-hour time
 * %Y-%m-%d %H:%M:%S
 */
#define PROMPT_FORMAT "%F %T#"
#define PROMPT_MAX_LENGTH 30

extern char **parse_line();
extern char **environ;

char *prompt() {
	static char* _prompt = NULL;
	time_t now;
	struct tm* tmp;
	size_t size;
	if (_prompt == NULL) {
		_prompt = malloc(PROMPT_MAX_LENGTH * sizeof(char));
		if (_prompt == NULL) {
			perror("Unable to locate memory.");
			exit(EXIT_FAILURE);
		}
	}
	now = time(NULL);
	if (now == -1) {
		perror("Cannot get current timestamp.");
		exit(EXIT_FAILURE);
	}
	tmp = localtime(&now);
	if (tmp == NULL) {
		perror("Cannot identify timestamp.");
		exit(EXIT_FAILURE);
	}
	/* The max size for strftime includes trailing 0. */
	size = strftime(_prompt, PROMPT_MAX_LENGTH, PROMPT_FORMAT, tmp);
	if (size == 0) {
		perror("Cannot convert time to string.");
		exit(EXIT_FAILURE);
	}
	return _prompt;
}

int main() {
	char *line;
	char **args;
	while (1) {
		int i;
		line = readline(prompt());
		add_history(line);
		args = parse_line(line);
		for (i = 0; args[i] != NULL; i++) {
			printf("Argument %d: %s\n", i, args[i]);
		}
	}
	return 0;
}
