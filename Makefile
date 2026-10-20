# SPDX-FileCopyrightText: NONE
# SPDX-License-Identifier: Unlicense

CC := cc
CFLAGS := -Wall -Wextra -std=c99
LDFLAGS := -lcjson

SRC_DIR := src
OUT := out

.PHONY: all
all:
	mkdir -p out
	$(CC) -O2 $(CFLAGS) -o $(OUT)/opsec $(SRC_DIR)/*.c $(LDFLAGS)

.PHONY: debug
debug:
	mkdir -p out
	$(CC) -O0 -g $(CFLAGS) -o $(OUT)/opsec $(SRC_DIR)/*.c -fsanitize=address,undefined -fno-omit-frame-pointer $(LDFLAGS)

.PHONY: profile
profile:
	mkdir -p out
	$(CC) -O0 -pg -g $(CFLAGS) -o $(OUT)/opsec $(SRC_DIR)/*.c $(LDFLAGS)
	# cd $(OUT)
	# ./opsec
	# gprof ./opsec gmon.out | less
