/**
 * Operating Systems 2013 - Assignment 2
 *
 */

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
#include "debug_help.h"

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	return chdir(dir->string);
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	exit(SHELL_EXIT);
}

/**
 * Concatenate parts of the word to obtain the command
 */
static char *get_word(word_t *s)
{
    TRACE("static", "%p", s);

	int string_length = 0;
	int substring_length = 0;

	char *string = NULL;
	char *substring = NULL;

	while (s != NULL) {
		substring = strdup(s->string);

		if (substring == NULL)
			return NULL;

		if (s->expand == true) {
			char *aux = substring;

			substring = getenv(substring);

			/* prevents strlen from failing */
			if (substring == NULL)
				substring = "";

			free(aux);
		}

		substring_length = strlen(substring);

		string = realloc(string, string_length + substring_length + 1);
		if (string == NULL) {
			if (s->expand == false)
				free(substring);
			return NULL;
		}

		memset(string + string_length, 0, substring_length + 1);

		strcat(string, substring);
		string_length += substring_length;

		if (s->expand == false)
			free(substring);

		s = s->next_part;
	}

    RETRACE();
	return string;
}

/**
 * Concatenate command arguments in a NULL terminated list in order to pass
 * them directly to execv.
 */
static char **get_argv(simple_command_t *command, int *size)
{
    TRACE("static", "%p", command);
	char **argv;
	word_t *param;

	int argc = 0;

	argv = calloc(argc + 1, sizeof(char *));
	assert(argv != NULL);

	argv[argc] = get_word(command->verb);
	assert(argv[argc] != NULL);

	argc++;

	param = command->params;
	while (param != NULL) {
		argv = realloc(argv, (argc + 1) * sizeof(char *));
		assert(argv != NULL);

		argv[argc] = get_word(param);
		assert(argv[argc] != NULL);

		param = param->next_word;
		argc++;
	}

	argv = realloc(argv, (argc + 1) * sizeof(char *));
	assert(argv != NULL);

	argv[argc] = NULL;
	*size = argc;

    RETRACE();
	return argv;
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
    UNUSED(father);
    if (NULL == s || 0 > level)
        return 1;

    TRACE("s", "%p, %d, %p", s, level, father);
    if (0 == strcmp("quit", s->verb->string)
    	|| 0 == strcmp("exit", s->verb->string))
    		shell_exit();
    if (0 == strcmp("cd", s->verb->string))
        shell_cd(s->params);

    if (NULL != s->verb->next_part
        && 0 == strcmp(s->verb->next_part->string, "=")) {
        const char *var_name  = s->verb->string;
        const char *var_value = s->verb->next_part->next_part->string;

        return setenv(var_name, var_value, 1);
    }

    pid_t pid;
    int fd_in = -1;
    char *in_file_name = get_word(s->in);
    int fd_out = -1;
    char *out_file_name = get_word(s->out);
    int fd_err = -1;
    char *err_file_name = get_word(s->err);
    int size = 0;
    char *cmd = NULL;
    char **args = NULL;

    pid = fork();
    switch(pid) {
    case -1:
        REPORT ("Fork failed.");
        break;
    case 0:
        cmd = get_word(s->verb);
        args = get_argv(s, &size);
        /*the code for child process*/
        if (NULL != s->in) {
            fd_in = open(in_file_name, O_RDONLY, 0644);
            redirect (STDIN_FILENO) to (fd_in);
            close(fd_in);
            if (0 > fd_in)
                REPORT ("Open failed at fd_in.");
        }
        if (NULL != s->out) {
            int flags = 0;

            if (IO_OUT_APPEND == s->io_flags) {
                flags = O_CREAT | O_APPEND | O_WRONLY;
                fd_out = open(out_file_name, flags, 0644);
            } else {
                flags = O_CREAT | O_TRUNC | O_WRONLY;
                fd_out = open(out_file_name, flags, 0644);
            }
            if (0 > fd_out)
                REPORT ("Open failed at fd_out.");
        }
        if (NULL != s->err) {
            int flags = 0;

            if (IO_ERR_APPEND == s->io_flags) {
                flags = O_CREAT | O_APPEND | O_WRONLY;
                fd_err = open(err_file_name, flags, 0644);
            } else {
                flags = O_CREAT | O_TRUNC | O_WRONLY;
                fd_err = open(err_file_name, flags, 0644);
            }

            if (0 > fd_err)
                REPORT ("Open failed at fd_out.");
        }
        if (NULL != s->out && NULL != s->err) {
            if (0 == strcmp(out_file_name, err_file_name)) {
                redirect (STDERR_FILENO) to (fd_err);
                close(fd_err);
                close(fd_out);
                fd_out = open(out_file_name,
                            O_CREAT | O_APPEND | O_WRONLY,
                            0644);
                redirect (STDOUT_FILENO) to (fd_out);
                close(fd_out);
            } else {
                if (0 < fd_err) {
                    redirect (STDERR_FILENO) to (fd_err);
                    close(fd_err);
                }
                if (0 < fd_out) {
                    redirect (STDOUT_FILENO) to (fd_out);
                    close(fd_out);
                }
            }
        } else {
            if (0 < fd_err) {
                redirect (STDERR_FILENO) to (fd_err);
                close(fd_err);
            }
            if (0 < fd_out) {
                redirect (STDOUT_FILENO) to (fd_out);
                close(fd_out);
            }
        }

        int rex = execvp (cmd, (char *const *) args);

        if (-1 == rex)
            fprintf(stderr, "Execution failed for '%s'\n", cmd);
        if (NULL != cmd)
            free(cmd);
        int i = 0;

        for (; i < size; ++i)
            free (args[i]);
        free(args);

        if (NULL != in_file_name)
            free (in_file_name);
        if (NULL != out_file_name)
            free (out_file_name);
        if (NULL != err_file_name)
            free (err_file_name);

        exit (EXIT_SUCCESS);
        break;
    }

    int status = -1;
    waitpid(pid, &status, 0);

    if (! WIFEXITED (status)) {
        REPORT("Child terminated abnormally.");
        REPORT(WEXITSTATUS(status));
    }
    RETRACE();
	return status;
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool do_in_parallel(command_t *cmd1,
                            command_t *cmd2,
                            int level,
                            command_t *father)
{
    UNUSED(father);
    if (NULL == cmd1 || NULL == cmd2 || 0 > level)
        return false;

    pid_t pid1, pid2;
    bool ret = false;
    int status = 0;

    pid1 = fork();
    if (pid1 == 0) {
        ret = parse_command(cmd1, level + 1, father);
        exit(ret);
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            ret |= parse_command(cmd2, level + 1, father);
            exit(ret);
        }
    }

    waitpid(pid1, &status, 0);
    if (! WIFEXITED (status)) {
        REPORT("Child1 terminated abnormally.");
        REPORT(WEXITSTATUS(status));
    }

    waitpid(pid2, &status, 0);
    if (! WIFEXITED (status)) {
        REPORT("Child2 terminated abnormally.");
        REPORT(WEXITSTATUS(status));
    }

	return true;
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2)
 */
static bool do_on_pipe(command_t *cmd1, command_t *cmd2, int level, command_t *father)
{
    UNUSED(father);
    TRACE("static", "%p, %p, %d, %p", cmd1, cmd2, level, father);

    bool rv = true;
    int pipe_fds[2];
    pid_t pid;
    pipe(pipe_fds);

    pid = fork();
    if (0 == pid) {
        redirect (STDOUT_FILENO) to (pipe_fds[1]);
        close(pipe_fds[1]);
        close(pipe_fds[0]);

        rv = parse_command(cmd1, level + 1, father);
        exit(rv);
    } else {
        redirect (STDIN_FILENO) to (pipe_fds[0]);
        close(pipe_fds[1]);
        close(pipe_fds[0]);

        rv = parse_command(cmd2, level + 1, father);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (! WIFEXITED (status)) {
        REPORT("Child1 terminated abnormally.");
        REPORT(WEXITSTATUS(status));
    }

    RETRACE();
	return rv;
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
    UNUSED(father);
    int rv = 1;
    if (NULL == c || 0 > level)
        return rv;

    TRACE("*", "%p, %d, %p", c, level, father);

	if (c->op == OP_NONE) {
        rv = parse_simple(c->scmd, level + 1, c);
        RETRACE();
        return rv;
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
        parse_command(c->cmd1, level + 1, c);
        rv = parse_command(c->cmd2, level + 1, c);
		break;

	case OP_PARALLEL:
		rv = do_in_parallel(c->cmd1, c->cmd2, level + 1, c);
		break;

	case OP_CONDITIONAL_NZERO:
        rv = parse_command(c->cmd1, level + 1, c);
        if (0 != rv)
            rv = parse_command(c->cmd2, level + 1, c);
		break;

	case OP_CONDITIONAL_ZERO:
        rv = parse_command(c->cmd1, level + 1, c);
        if (0 == rv)
            rv = parse_command(c->cmd2, level + 1, c);
		break;

	case OP_PIPE:
        rv = do_on_pipe(c->cmd1, c->cmd2, level + 1, c);
		break;

	default:
        REPORT("Unknow command type.");
		assert(false);
	}

    RETRACE();
	return rv;
}

/**
 * Readline from mini-shell.
 */
char *read_line()
{
	char *instr;
	char *chunk;
	char *ret;

	int instr_length;
	int chunk_length;

	int endline = 0;

	instr = NULL;
	instr_length = 0;

	chunk = calloc(CHUNK_SIZE, sizeof(char));
	if (chunk == NULL) {
		fprintf(stderr, ERR_ALLOCATION);
		return instr;
	}

	while (!endline) {
		ret = fgets(chunk, CHUNK_SIZE, stdin);
		if (ret == NULL)
			break;

		chunk_length = strlen(chunk);
		if (chunk[chunk_length - 1] == '\n') {
			chunk[chunk_length - 1] = 0;
			endline = 1;
		}

		instr = realloc(instr, instr_length + CHUNK_SIZE + 1);
		if (instr == NULL) {
			free(ret);
			return instr;
		}

		memset(instr + instr_length, 0, CHUNK_SIZE);
		strcat(instr, chunk);
		instr_length += chunk_length;
	}

	free(chunk);

	return instr;
}
