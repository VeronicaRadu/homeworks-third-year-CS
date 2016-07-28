#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/*
 * Parses every line from the input and sets the command enum to the specifc
 * command type read from input. If there are any arguments with the command
 * argumen1 and argument 2 may be set(depends on the command format). If
 * nothing out of order happened the function should return ERROR_SUCCESS
 * and set at least command parameter to something not null.
 * @arg line = line of from the input given as char*
 * @ret command = a pointer to an enum that represents the command
 * @ret argument1 and argument2 are set if the command has parameters
 * return error code
 */
int parse_input(char *line, command *c, char **argument1, char **argument2)
{
	int ret = ERROR_SUCCESS;
	char *token = strtok(line, " ");

	TRACE("", "%s", line);
	if (NULL == line
		|| 0 == strlen(line)
		|| NULL == c
		|| NULL == argument1
		|| NULL == argument2)
		return ERROR_SUCCESS;
	*argument1 = NULL;
	*argument2 = NULL;
	*c = unknown;

	if (0 == strcmp(ADD, token)) {
		*c = add;
		token = strtok(NULL, " ");
		if (NULL == token)
			ret = ERROR_MISSING_PARAMETER;
		*argument1 = token;
	} else if (0 == strcmp(REMOVE, token)) {
		*c = remov;
		token = strtok(NULL, " ");
		if (NULL == token)
			ret = ERROR_MISSING_PARAMETER;
		*argument1 = token;
	} else if (0 == strcmp(FIND, token)) {
		*c = find;
		token = strtok(NULL, " ");
		if (NULL == token)
			ret = ERROR_MISSING_PARAMETER;
		*argument1 = token;
		token = strtok(NULL, " ");
		if (NULL != token)
			*argument2 = token;
	} else if (0 == strcmp(CLEAR, token)) {
		*c = clear;
	} else if (0 == strcmp(PRINT_BUCKET, token)) {
		*c = print_bucket;
		token = strtok(NULL, " ");
		if (NULL == token)
			ret = ERROR_MISSING_PARAMETER;
		*argument1 = token;
		token = strtok(NULL, " ");
		if (NULL != token)
			*argument2 = token;
	} else if (0 == strcmp(PRINT, token)) {
		*c = print;
		token = strtok(NULL, " ");
		if (NULL != token)
			*argument1 = token;
	} else if (0 == strcmp(RESIZE, token)) {
		*c = resize;
		token = strtok(NULL, " ");
		if (NULL == token)
			ret = ERROR_MISSING_PARAMETER;
		*argument1 = token;
	}
	if (unknown == *c)
		ret = ERROR_UNKNOWN_COMMAND;
	return ret;
}

int execute_command(command comm, char *argument1, char *argument2, void *hh)
{
	pHashTable pht = (pHashTable) hh;
	int ret = ERROR_SUCCESS;
	int r = 0;
	FILE *fil = stdout;
	size_t idx = 0;
	size_t ns = 0;

	if (NULL == hh)
		return ERROR_FAILURE;
	TRACE("***", "%d, %s, %s", comm, argument1, argument2);

	switch (comm) {
	case add:
		ret = ret | addhash(pht, argument1, strlen(argument1));
		break;
	case remov:
		ret = ret | removeh(pht, argument1, strlen(argument1));
		break;
	case find:
		if (NULL != argument2)
			fil = fopen(argument2, "a");
		r = findhas(pht, argument1, strlen(argument1));
		if (TRUE == r)
			fprintf(fil, "True\n");
		else
			fprintf(fil, "False\n");
		if (NULL != argument2) {
			fclose(fil);
			fil = stdout;
		}
		break;
	case clear:
		ret = ret | clearhs(pht);
		break;
	case print_bucket:
		idx = atoi(argument1);
		if (NULL != argument2)
			fil = fopen(argument2, "a");
		print_bucketh(pht, idx, fil);
		if (NULL != argument2) {
			fclose(fil);
			fil = stdout;
		}
		break;
	case print:
		if (NULL != argument1)
			fil = fopen(argument1, "a");

		for (; idx < pht->size; ++idx)
			print_bucketh(pht, idx, fil);

		if (NULL != argument1) {
			fclose(fil);
			fil = stdout;
		}
		break;
	case resize:
		if (0 == strcmp("double", argument1))
			ns = pht->size * 2;
		else if (0 == strcmp("halve", argument1))
			ns = pht->size / 2;

		resizeh(pht, ns);
		break;
	default:
		break;
	}
	return ret;
}

int execute_commands_from_file(FILE *stream, void *hh)
{
	int ret = ERROR_SUCCESS;
	command comm = unknown;
	char *cparam1 = NULL;
	char *cparam2 = NULL;
	char input_buffer[LINE_MAX_LENGTH];

	if (NULL == stream || NULL == hh)
		return ERROR_FAILURE;
	TRACE("", " ");

	while (NULL != fgets(input_buffer, LINE_MAX_LENGTH, stream)) {
		if (NULL != strstr(input_buffer, "\n"))
			correct_buffer(input_buffer);

		ret = parse_input(input_buffer, &comm, &cparam1, &cparam2);
		ret = ret | execute_command(comm, cparam1, cparam2, hh);
	}
	return ret;
}

int main(int argc, char **argv)
{
	pHashTable ptr = NULL;
	FILE *instream = NULL;
	int r = 0;
	int file_name_idx = 0;

	DIE(argc < 2, "Invalid usage! ./executabe size_of_table [<files>]\n");

	ptr = new_hash_table(atoi(argv[1]), &hash, &cmprer);

	if (argc == 2) {
		instream = stdin;
		r = execute_commands_from_file(instream, ptr);
		if (ERROR_SUCCESS != r) {
			instream = NULL;
			goto free_memory;
		}

	} else {
		file_name_idx = 2;
		for (; file_name_idx < argc; ++file_name_idx) {
			instream = fopen(argv[file_name_idx], "r");
			DIE(NULL == instream,
				"Couldn't open a file given as parameter.\n");
			r = execute_commands_from_file(instream, ptr);
			if (ERROR_SUCCESS != r)
				goto free_memory;

			fclose(instream);
			instream = NULL;
		}
	}

free_memory:
	if (NULL != instream)
		fclose(instream);
	if (NULL != ptr) {
		destroy(ptr);
		free(ptr);
		ptr = NULL;
	}
	return EXIT_SUCCESS;
}
