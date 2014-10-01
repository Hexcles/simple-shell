/*
 * myshell.c
 * A simple Linux shell for CMPT 300 HW1
 *
 * By Robert Hongpu Ma, September 2014
 *
 * Using `lex.c` from http://users.soe.ucsc.edu/~sbrandt/111/assignment1.html
 * Thanks for their work!
 */

/* Since we are compiling in std=c11. */
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

/*
 * ISO 8601 date + 24-hour time
 * %Y-%m-%d %H:%M:%S
 */
#define PROMPT_FORMAT "%F %T# "
#define PROMPT_MAX_LENGTH 30

/* In parse_line.o */
extern char **parse_line();
/* Explicit declare the environ. */
extern char **environ;
/* Signal long jump buffer. */
sigjmp_buf sigint_buf;

char *prompt() {
	static char* _prompt = NULL;
	time_t now;
	struct tm* tmp;
	size_t size;
	if (_prompt == NULL) {
		_prompt = malloc(PROMPT_MAX_LENGTH * sizeof(char));
		if (_prompt == NULL) {
			perror("Unable to locate memory");
			exit(EXIT_FAILURE);
		}
	}
	now = time(NULL);
	if (now == -1) {
		fprintf(stderr, "Cannot get current timestamp");
		exit(EXIT_FAILURE);
	}
	tmp = localtime(&now);
	if (tmp == NULL) {
		fprintf(stderr, "Cannot identify timestamp");
		exit(EXIT_FAILURE);
	}
	/* The max size for strftime includes trailing 0. */
	size = strftime(_prompt, PROMPT_MAX_LENGTH, PROMPT_FORMAT, tmp);
	if (size == 0) {
		fprintf(stderr, "Cannot convert time to string");
		exit(EXIT_FAILURE);
	}
	return _prompt;
}

void sigint_handler() {
	/*
	 * According to signal(2), its behaviour is not unique.
	 * In System V (or compiled with --std=), signals are reset everytime.
	 * Otherwise they are not (e.g. BSD or Linux default).
	 * So set them again here to avoid glitches.
	 */
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("Failed to set up signal handler");
		exit(EXIT_FAILURE);
	}
	puts("");
	siglongjmp(sigint_buf, 1);
}

void dispatch(char **args) {
	int argc;
	int status;
	pid_t child;
	for (argc = 0; args[argc] != NULL; argc++);
	if (argc == 0) return;
	if (strcmp(args[0], "exit") == 0) {
		exit(EXIT_SUCCESS);
	}
	if (strcmp(args[0], "cd") == 0) {
		if (argc == 1) {
			argc = 2;
			args[1] = "~";
		}
		if (argc != 2) fprintf(stderr, "Wrong command format");
		if (strcmp(args[1], "~") == 0) args[1] = getenv("HOME");
		if (chdir(args[1]) == -1) perror("Error changind directory");
		return;
	}

	/* Run external commands. */
	child = fork();
	if (child == -1) {
		perror("Error when forking");
		exit(EXIT_FAILURE);
	}
	if (child == 0) {
		/* child process */
		execvp(args[0], args);
		/* Should not run the following code except error happens. */
		perror("Error when executing");
		exit(EXIT_FAILURE);
	} else {
		/* parent process */
		if (waitpid(child, &status, 0) == -1) {
			perror("Error when waiting");
			exit(EXIT_FAILURE);
		}
	}
}

int main() {
	char *line;
	char **args;

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("Failed to set up signal handler");
		exit(EXIT_FAILURE);
	}

	/*
	 * Two situations:
	 * 1. Normally called: setting up long jump buffer and return 0;
	 * 2. Jumping back: return 1, needs to set the jump buffer again.
	 */
	while (sigsetjmp(sigint_buf, 1) != 0);

	while (1) {
		line = readline(prompt());
		/* Happens when press CTRL-D (Bash will exit in this case.) */
		if (line == NULL) {
			exit(EXIT_SUCCESS);
		}
		add_history(line);
		args = parse_line(line);
		dispatch(args);
	}
	return 0;
}
