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

#include <stdint.h>
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
#elif !defined(__unix__) && !defined(_POSIX_VERSION) && !defined(ASSUME_POSIX)
#error                                                                         \
    "This program is intended for a POSIX-complaint system, define 'ASSUME_POSIX' to bypass this check!"
#else

#include "h/colors.h"
#include "h/config.h"
#include "h/parse.h"
#include <cjson/cJSON.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static inline void print_level_name(double opsec_mistakes_percent)
{
	switch ((uint_fast8_t)opsec_mistakes_percent / 10)
	{
	case 0:
		puts(GREEN "OPSEC GOD!" RESET);
		break;
	case 1:
		puts(CYAN "OPSEC PRO!" RESET);
		break;
	case 2:
		puts(CYAN "OPSEC AMATEUR!" RESET);
		break;
	case 3:
		puts(MAGENTA "OPSEC BEGINNER!" RESET);
		break;
	// anything 60% and above is considered bad - considering many of the
	// applications that are mistakes are stuff like webbrowsers where you
	// would only reasonably have 1 or 2
	default:
		puts(RED "OPSEC NOOB" RESET);
		break;
	}
}

static inline void print_opsec_level(unsigned int mistakes,
                                     unsigned int maximum_mistakes)
{
	if (mistakes != 0)
		// print some lines to separate the final opsec level
		printf("--------------------------\n");

	double opsec_mistakes_percent;
	if (maximum_mistakes == 0)
	{
		// to prevent a divide-by-zero error
		opsec_mistakes_percent = 100;
	}
	else
	{
		opsec_mistakes_percent =
		    ((double)mistakes / (double)maximum_mistakes) * 100.0;
	}

	printf(GRAY "OPSEC LEVEL: " RESET);

	// handle super-special cases here
	if (mistakes == 0)
	{
		puts(GREEN "TRUE GOD OF OPSEC, MASTER OF LARPERS!" RESET);
		puts(GRAY "(" GREEN "No OPSEC mistakes detected!" GRAY
		          ")" RESET);
		return;
	}
	else if (mistakes == maximum_mistakes)
	{
		puts(RED
		     "HOW DID YOU EVEN GET A NUMBER THIS HIGH LIKE WHAT THE "
		     "FUCK" RESET);
		return;
	}
	else
	{
		print_level_name(opsec_mistakes_percent);
	}
}

int main(void)
{
	int bad_distro = 0; // I did the soyjack and went "bad apple reference"
	                    // after writing this - Uniolo1

	// omarchy is bad opsec for many, many, many reasons
	if (access("/usr/share/omarchy/version", F_OK) == 0)
	{
		puts(RED "CRITICAL OPSEC MISTAKE FOUND" RESET
		         ": You are using Omarchy!");
		puts("Alternatives: " CYAN "CachyOS, Fedora Linux" RESET);
		puts(GRAY "This program will continue..." RESET);

		putchar('\n'); // add space for other mistakes
		bad_distro++;
	}
	possible_mistakes++; // increment it here to avoid forgetting to
	                     // increment it elsewhere when adding a new one of
	                     // these in the future

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
	unsigned int mistakes =
	    parse_config_and_do_logic_and_stuff(config) + bad_distro;

	cJSON_Delete(config);

	print_opsec_level((unsigned int)mistakes, possible_mistakes);
	return mistakes;
}

#endif // if defined(_WIN32)