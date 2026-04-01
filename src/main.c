/*
 * main.c - MeowPassword Main Entry Point
 * Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "meowpass.h"

/**
 * Copy password to clipboard using xclip on Linux
 */
static void copy_to_clipboard(const char *password, bool silent) {
#ifdef __linux__
    /* Check if xclip is available */
    FILE *test = popen("which xclip 2>/dev/null", "r");
    if (test) {
        char buf[256];
        if (fgets(buf, sizeof(buf), test) != NULL) {
            pclose(test);

            /* Use xclip to copy to clipboard */
            FILE *clip = popen("xclip -selection clipboard", "w");
            if (clip) {
                fprintf(clip, "%s", password);
                pclose(clip);
                if (silent) {
                    printf("----> copied!\n");
                } else {
                    printf("\nPassword copied to clipboard!\n");
                }
                return;
            }
        } else {
            pclose(test);
        }
    }
    printf("\nClipboard functionality requires xclip (apt install xclip)\n");
#else
    (void)silent;
    printf("\nClipboard functionality only available on Linux with xclip\n");
#endif
}

/**
 * Find the best candidate (highest score)
 */
static int find_best_candidate(PasswordCandidate *candidates, int count) {
    int best_idx = 0;
    double best_score = candidates[0].complexity.score;

    for (int i = 1; i < count; i++) {
        if (candidates[i].complexity.score > best_score) {
            best_score = candidates[i].complexity.score;
            best_idx = i;
        }
    }

    return best_idx;
}

int main(int argc, char *argv[]) {
    /* Seed random number generator */
    srand((unsigned int)time(NULL));

    /* Parse configuration */
    PasswordConfig config;
    config_init(&config, argc, argv);

    /* Handle help */
    if (config.show_help) {
        display_help();
        return 0;
    }

    /* Handle test mode */
    if (config.show_tests) {
        return run_tests();
    }

    /* Handle update check */
    if (config.check_update) {
        return check_for_update();
    }

    /* Handle analyze mode */
    if (config.analyze_string) {
        display_header();
        printf("Meow Analyzing your string for cat-cracking resistance...\n\n");

        ComplexityResult result;
        analyze_complexity(config.analyze_string, &result);

        display_analysis(&result, config.analyze_string);

        printf("\n");
        if (result.score < 2.0) {
            printf("🐱 Purrfect! This string is fur-midably complex. Even the cleverest cats can't crack it!\n");
        } else if (result.score < 4.0) {
            printf("😺 Not bad, hooman! This string has decent whisker-resistance.\n");
        } else if (result.score < 6.0) {
            printf("😿 Meow... this string is a bit too easy for a clever cat to guess.\n");
        } else {
            printf("🙀 Hiss! A kitten could paw this one open! Try a meowpass-generated password instead!\n");
        }

        return 0;
    }

    /* Load cat names */
    size_t names_count = get_cat_names_count();
    if (names_count == 0) {
        fprintf(stderr, "ERROR: No cat names loaded from embedded data.\n");
        return 1;
    }

    /* Generate 5 password candidates */
    PasswordCandidate candidates[NUM_CANDIDATES];

    if (config.psssst) {
        /* Silent mode: generate passwords, copy best to clipboard, no display */
        for (int i = 0; i < NUM_CANDIDATES; i++) {
            generate_password(&config, candidates[i].password, MAX_PASSWORD_LENGTH);
            analyze_complexity(candidates[i].password, &candidates[i].complexity);
        }
        int best_idx = find_best_candidate(candidates, NUM_CANDIDATES);
        copy_to_clipboard(candidates[best_idx].password, true);
    } else {
        /* Normal mode: show everything */
        display_header();

        printf("Loaded %zu meow cat names\n", names_count);
        printf("Generating 5 secure password meow candidates...\n");
        printf("Config: %d numbers, %d symbols, max meow length %d\n\n",
               config.num_numbers, config.num_symbols, config.max_length);

        for (int i = 0; i < NUM_CANDIDATES; i++) {
            generate_password(&config, candidates[i].password, MAX_PASSWORD_LENGTH);
            analyze_complexity(candidates[i].password, &candidates[i].complexity);
            display_candidate(i + 1, &candidates[i]);
        }

        /* Select the best password */
        int best_idx = find_best_candidate(candidates, NUM_CANDIDATES);

        display_final_selection(&candidates[best_idx]);

        /* Copy to clipboard if requested */
        if (config.copy_to_clipboard) {
            copy_to_clipboard(candidates[best_idx].password, false);
        } else {
            printf("\nUse 'meowpass --copy' to copy password to clipboard\n");
        }
    }

    return 0;
}
