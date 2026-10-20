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
#include <cjson/cJSON.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static inline int file_exists(const char *path)
{
	return access(path, F_OK) == 0;
}

bool get_config_path(char **output)
{
	// use user-defined config path is set with the "OPSEC_CONFIG_FILE"
	// environment variable
	char *environment_opsec_config = getenv("OPSEC_CONFIG_FILE");
	if (environment_opsec_config != NULL)
	{
		*output = environment_opsec_config;
		return false;
	}

	if (file_exists("./opsec.jsonc"))
	{
		*output = "./opsec.jsonc";
		return false;
	}

	// $XDG_CONFIG_HOME/opsec.json
	char *environment_config_home = getenv("XDG_CONFIG_HOME");
	if (environment_config_home != NULL)
	{
		size_t ret_len = strlen(environment_config_home) +
		                 strlen("/opsec.jsonc") + 1;
		char *ret = malloc(ret_len);

		strcpy(ret, environment_config_home);
		strcpy(&ret[strlen(environment_config_home)], "/opsec.jsonc");

		if (!file_exists(ret))
		{
			free(ret);
		}
		else
		{
			*output = ret;
			return true;
		}
	}

	// $HOME/.config/opsec.json
	char *environment_home = getenv("HOME");
	if (environment_home != NULL)
	{
		size_t ret_len = strlen(environment_home) +
		                 strlen("/.config/opsec.jsonc") + 1;
		char *ret = malloc(ret_len);

		strcpy(ret, environment_home);
		strcpy(&ret[strlen(environment_home)], "/.config/opsec.jsonc");

		if (!file_exists(ret))
		{
			free(ret);
		}
		else
		{
			*output = ret;
			return true;
		}
	}

	if (file_exists("/etc/opsec.jsonc"))
	{
		*output = "/etc/opsec.jsonc";
		return false;
	}

	return false; // no file found
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

	// Logic to remove comments
	// NOTE: AI used here a little
	char *src = buffer;
	char *dst = buffer;

	while (*src != '\0')
	{
		char *line = src;

		// find the end of the current line
		char *newline = strchr(src, '\n');
		size_t line_len =
		    newline ? (size_t)(newline - src) : strlen(src);

		// skip leading whitespace
		char *p = line;
		while (p < line + line_len && isspace((unsigned char)*p))
			p++;

		// ignore the line if it starts with '//'
		if (!(p + 1 < line + line_len && p[0] == '/' && p[1] == '/'))
		{
			memmove(dst, line, line_len);
			dst += line_len;

			if (newline)
				*dst++ = '\n';
		}

		src = newline ? newline + 1 : src + line_len;
	}

	*dst = '\0';

	cJSON *config = cJSON_Parse(buffer);
	free(buffer); // free the buffer

	if (config == NULL)
	{
		fprintf(stderr, "Config is invalid!\n");
		abort();
	}

	return config;
}

#endif // ifndef _WIN32