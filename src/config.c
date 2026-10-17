// SPDX-FileCopyrightText: NONE
// SPDX-License-Identifier: Unlicense

/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the
 * public domain. We make this dedication for the benefit of the public at large
 * and to the detriment of our heirs and successors. We intend this dedication
 * to be an overt act of relinquishment in perpetuity of all present and future
 * rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _WIN32

#include "h/config.h"
#include "h/colors.h"
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static inline int file_exists(const char *path)
{
	return access(path, F_OK) == 0;
}

char *get_config_path(void)
{
	// use user-defined config path is set with the "OPSEC_CONFIG_FILE"
	// environment variable
	char *environment_opsec_config = getenv("OPSEC_CONFIG_FILE");
	if (environment_opsec_config != NULL)
		return environment_opsec_config;

	if (file_exists("./opsec.json"))
		return "./opsec.json";

	// TODO: add more paths

	return NULL; // no file found
}

cJSON *read_config(const char *config_path)
{
	FILE *fp = fopen(config_path, "r");
	if (fp == NULL)
	{
		perror("fopen");
		abort();
	}

	// detect file size
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	if (size < 0)
	{
		fclose(fp);
		abort();
	}

	// allocate buffer the same size as file contents
	char *buffer = malloc((size_t)size + 1);
	if (buffer == NULL)
	{
		fclose(fp);
		abort();
	}

	size_t bytes_read = fread(buffer, 1, (size_t)size, fp);
	fclose(fp);

	buffer[bytes_read] = '\0';

	cJSON *config = cJSON_Parse(buffer);
	free(buffer); // free the buffer

	if (config == NULL)
	{
		fprintf(stderr, "Config is invalid!\n");
		abort();
	}

	return config;
}

static inline bool executable_exists(const char *name)
{
	printf("TODO: Parse \"%s\"\n", name);
	return false;
}

int parse_config_and_do_logic_and_stuff(cJSON *config)
{
	int mistakes = 0;
	cJSON *item = NULL;

	cJSON_ArrayForEach(item, config)
	{
		if (cJSON_IsObject(item))
		{
			cJSON *executables = cJSON_GetObjectItemCaseSensitive(
			    item, "executables");
			cJSON *alternatives = cJSON_GetObjectItemCaseSensitive(
			    item, "alternatives");

			if (cJSON_IsArray(executables))
			{
				cJSON *executable = NULL;

				cJSON_ArrayForEach(executable, executables)
				{
					if (cJSON_IsString(executable))
					{
						if (executable_exists(
						        executable
						            ->valuestring))
							goto executable_found;
					}
					else
					{
						fprintf(stderr,
						        "Invalid json!\n");
						abort();
					}
				}
			}

			// executable was not found:
			continue;

		executable_found: // executable was found if we jump
		                  // here

			mistakes++; // increment 'mistakes'
			printf(RED "OPSEC MISTAKE FOUND" RESET ": " YELLOW
			           "%s" RESET "\n",
			       item->string);

			if (cJSON_IsString(alternatives))
			{
				printf(GREEN "Alternatives" RESET ": " MAGENTA
				             "%s" RESET "\n",
				       alternatives->valuestring);
			}
			else
			{
				fprintf(stderr, "Invalid json!\n");
				abort();
			}

			// add space between mistakes
			putchar('\n');
		}
	}

	return mistakes;
}

#endif // ifndef _WIN32