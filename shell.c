#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "simple_shell.h"

/**
 * sh_read_line - read one line from stdin
 *
 * Return: heap-allocated line without trailing newline, or NULL on EOF/error
 */
char *sh_read_line(void)
{
	char *line;
	size_t cap;
	ssize_t nread;

	line = NULL;
	cap = 0;
	nread = getline(&line, &cap, stdin);
	if (nread == -1)
	{
		/* EOF (Ctrl+D) or read error */
		free(line);
		return (NULL);
	}

	/* TODO(you): decide whether to strip trailing '\n' here or in execute */
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0';

	return (line);
}

/**
 * sh_execute - execute one-word command using execve
 * @prog_name: argv[0] from main (used by perror)
 * @line: input line buffer
 *
 * Return: child status on success path, 1 on shell-side failure
 */
int sh_execute(char *prog_name, char *line)
{
	pid_t pid;
	int wstatus;
	char *argv[2];
	char *cmd;

	/* TODO(you): split command line (0.1 expects one command word only) */
	cmd = strtok(line, " \t");
	if (cmd == NULL)
		return (0);

	/* TODO(you): for later tasks, build argv dynamically for arguments */
	argv[0] = cmd;
	argv[1] = NULL;

	/* TODO(you): create child process */
	pid = fork();
	if (pid == -1)
	{
		perror(prog_name);
		return (1);
	}

	if (pid == 0)
	{
		/* TODO(you): child replaces itself with requested program */
		execve(cmd, argv, environ);

		/* execve only returns on failure */
		perror(prog_name);
		_exit(127);
	}

	/* TODO(you): parent waits for child before prompting again */
	if (waitpid(pid, &wstatus, 0) == -1)
	{
		perror(prog_name);
		return (1);
	}

	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));

	return (1);
}

/**
 * sh_run - interactive/non-interactive shell loop
 * @prog_name: argv[0] from main
 *
 * Return: last command status
 */
int sh_run(char *prog_name)
{
	char *line;
	int interactive;
	int status;

	interactive = isatty(STDIN_FILENO);
	status = 0;

	while (1)
	{
		/* TODO(you): match exact prompt required by your checker */
		if (interactive)
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		line = sh_read_line();
		if (line == NULL)
		{
			if (interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		status = sh_execute(prog_name, line);
		free(line);
	}

	return (status);
}