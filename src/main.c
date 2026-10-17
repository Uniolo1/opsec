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

#ifdef _WIN32
// if the user is a Windows user, warn them to switch!

#include "h/colors.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	puts(RED "CRITICAL OPSEC MISTAKE FOUND" RESET
	         ": You are using Windows!");
	puts("Alternatives: " CYAN "Fedora Linux, Gentoo, Alpine Linux" RESET);
	puts(GRAY "This program will now exit..." RESET);
	abort();
}
#else

#include "h/colors.h"
#include "h/config.h"
#include "h/parse.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int bad_distro = 0; // I did the soyjack and went "bad apple reference"
	                    // after writing this - Uniolo1

	if (access("/usr/share/omarchy/version", F_OK) == 0)
	{
		puts(RED "CRITICAL OPSEC MISTAKE FOUND" RESET
		         ": You are using Omarchy!");
		puts("Alternatives: " CYAN "Fedora Linux, CachyOS" RESET);
		puts(GRAY "This program will continue..." RESET);

		putchar('\n'); // add space for other mistakes
		bad_distro = 1;
	}

	char *config_path = NULL;
	bool config_path_needs_freeing = get_config_path(&config_path);
	if (config_path == NULL)
	{
		fprintf(stderr, "Unable to find config!\n");
		return 2;
	}

	// NOTE: this function handles errors internally, and abort's on
	// failure:
	cJSON *config = read_config(config_path);
	if (config_path_needs_freeing)
		free(config_path);

	if (!cJSON_IsObject(config))
	{
		fprintf(stderr, "Invalid json!\n");
		abort();
	}

	// this function also abort's on failure:
	int mistakes = parse_config_and_do_logic_and_stuff(config) + bad_distro;

	cJSON_Delete(config);

	if (mistakes == 0)
		puts(GREEN "No OPSEC mistakes detected!" RESET);

	return mistakes;
}

#endif // if defined(_WIN32)