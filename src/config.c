/*
 * config.c - Configuration Handling
 * MeowPassword - Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#include <stdlib.h>
#include <string.h>
#include "meowpass.h"

int clamp_int(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void config_init(PasswordConfig *config, int argc, char *argv[]) {
    /* Set defaults */
    config->num_numbers = (rand() % 4) + 1;  /* Random 1-4 as per Swift spec */
    config->num_symbols = DEFAULT_NUM_SYMBOLS;
    config->max_length = DEFAULT_MAX_LENGTH;
    config->show_tests = false;
    config->copy_to_clipboard = false;
    config->psssst = false;
    config->show_help = false;
    config->check_update = false;
    config->analyze_string = NULL;

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numbers") == 0) {
            if (i + 1 < argc) {
                int val = atoi(argv[i + 1]);
                config->num_numbers = clamp_int(val, MIN_NUMBERS, MAX_NUMBERS);
                i++;
            }
        } else if (strcmp(argv[i], "--symbols") == 0) {
            if (i + 1 < argc) {
                int val = atoi(argv[i + 1]);
                config->num_symbols = clamp_int(val, MIN_SYMBOLS, MAX_SYMBOLS);
                i++;
            }
        } else if (strcmp(argv[i], "--max-length") == 0) {
            if (i + 1 < argc) {
                int val = atoi(argv[i + 1]);
                config->max_length = clamp_int(val, MIN_LENGTH, MAX_LENGTH);
                i++;
            }
        } else if (strcmp(argv[i], "--test") == 0) {
            config->show_tests = true;
        } else if (strcmp(argv[i], "--copy") == 0) {
            config->copy_to_clipboard = true;
        } else if (strcmp(argv[i], "--psssst") == 0 || strcmp(argv[i], "-p") == 0) {
            config->psssst = true;
            config->copy_to_clipboard = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            config->show_help = true;
        } else if (strcmp(argv[i], "--update") == 0) {
            config->check_update = true;
        } else if (strcmp(argv[i], "--analyze") == 0 || strcmp(argv[i], "-a") == 0) {
            if (i + 1 < argc) {
                config->analyze_string = argv[i + 1];
                i++;
            }
        }
    }
}
