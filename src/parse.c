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

#include "h/parse.h"
#include "h/colors.h"
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static inline bool executable_exists(const char *name)
{
	// might consider changing this to search PATH manually...
	char cmd[512];
	snprintf(cmd, sizeof(cmd), "which \"%s\" > /dev/null 2>&1", name);

	return system(cmd) == 0;
}

static inline bool flatpak_exists(const char *name)
{
	// same applies as above
	char cmd[512];
	snprintf(cmd, sizeof(cmd), "flatpak info \"%s\" > /dev/null 2>&1",
	         name);

	return system(cmd) == 0;
}

int parse_config_and_do_logic_and_stuff(cJSON *config)
{
	int mistakes = 0;
	cJSON *item = NULL;

	cJSON_ArrayForEach(item, config)
	{
		char *consider_removing = NULL;

		if (cJSON_IsObject(item))
		{
			cJSON *executables = cJSON_GetObjectItemCaseSensitive(
			    item, "executables");
			cJSON *flatpak_executables =
			    cJSON_GetObjectItemCaseSensitive(
			        item, "flatpak_executables");

			cJSON *notes =
			    cJSON_GetObjectItemCaseSensitive(item, "notes");
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
						{
							consider_removing =
							    executable
							        ->valuestring;
							goto executable_found;
						}
					}
					else
					{
						fprintf(stderr,
						        "Invalid json!\n");
						abort();
					}
				}
			} // NOTE: no 'else' here as executables is optional

			if (cJSON_IsArray(flatpak_executables))
			{
				cJSON *executable = NULL;

				cJSON_ArrayForEach(executable,
				                   flatpak_executables)
				{
					if (cJSON_IsString(executable))
					{
						if (flatpak_exists(
						        executable
						            ->valuestring))
						{
							consider_removing =
							    executable
							        ->valuestring;
							goto executable_found;
						}
					}
					else
					{
						fprintf(stderr,
						        "Invalid json!\n");
						abort();
					}
				}
			} // NOTE: no 'else' here as flatpak is optional

			// executable was not found:
			continue;

		executable_found: // executable was found if we jump
		                  // here

			mistakes++; // increment 'mistakes'
			printf(RED "OPSEC MISTAKE FOUND" RESET ": " YELLOW
			           "%s" RESET "\n",
			       item->string);

			if (cJSON_IsString(notes))
			{
				printf(CYAN "NOTES" RESET ": " BLUE "%s" RESET
				            "\n",
				       notes->valuestring);
			} // NOTE: no 'else' here as notes is optional

			if (cJSON_IsString(alternatives))
			{
				printf(GREEN "Alternative" GRAY "(" GREEN
				             "s" GRAY ")" RESET ": " MAGENTA
				             "%s" RESET "\n",
				       alternatives->valuestring);
			} // NOTE: no 'else' here as alternatives is optional

			if (consider_removing != NULL)
				printf(GRAY "Consider removing: '%s'" RESET
				            "\n",
				       consider_removing);

			// add space between mistakes
			putchar('\n');
		}
	}

	return mistakes;
}

#endif // ifndef _WIN32