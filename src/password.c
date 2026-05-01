/*
 * password.c - Password Generation Functions
 * MeowPassword - Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "meowpass.h"

/* Symbols for replacement */
static const char SYMBOLS[] = "!@#$%^&*()-_=+[]{;:.<>?";
static const char NUMBERS[] = "0123456789";

/**
 * Fisher-Yates shuffle using the secure RNG.
 */
static void shuffle_indices(size_t *arr, size_t n) {
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)meow_random_uniform((uint32_t)(i + 1));
        size_t tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

/**
 * Pick `k` distinct random indices in [0, n) without shuffling the full
 * range. With k <= ~16 and n in the thousands, collision probability is
 * tiny; rejection sampling stays O(k^2) and avoids the n-sized malloc.
 */
static void pick_random_indices(size_t *out, int k, size_t n) {
    int chosen = 0;
    while (chosen < k) {
        size_t idx = (size_t)meow_random_uniform((uint32_t)n);
        int dup = 0;
        for (int j = 0; j < chosen; j++) {
            if (out[j] == idx) { dup = 1; break; }
        }
        if (!dup) out[chosen++] = idx;
    }
}

/**
 * Append a name (lowercase, spaces stripped) to output, respecting max_length.
 */
static void append_name(const char *name, char *output, size_t *out_len, int max_length) {
    for (const char *p = name; *p && *out_len < (size_t)(max_length - 1); p++) {
        if (*p != ' ') {
            output[(*out_len)++] = (char)tolower((unsigned char)*p);
        }
    }
    output[*out_len] = '\0';
}

/**
 * Select random cat names and join them
 */
static void select_and_join_names(int count, char *output, size_t output_size, int max_length) {
    (void)output_size; /* Bounds enforced via max_length */
    const char **names = get_cat_names();
    size_t names_count = get_cat_names_count();

    if (names_count == 0 || count <= 0) {
        output[0] = '\0';
        return;
    }

    int actual_count = (count > (int)names_count) ? (int)names_count : count;

    /* Pick up to actual_count + 5 distinct indices: the extras are used
       only if the joined string is shorter than MIN_LENGTH. */
    int extras = (actual_count + 5 > (int)names_count) ? (int)names_count - actual_count : 5;
    int total_picks = actual_count + extras;
    size_t indices[16]; /* actual_count <= 6 (caller) + 5 extras = 11 */
    if (total_picks > (int)(sizeof(indices) / sizeof(indices[0]))) {
        total_picks = (int)(sizeof(indices) / sizeof(indices[0]));
        if (actual_count > total_picks) actual_count = total_picks;
        extras = total_picks - actual_count;
    }
    pick_random_indices(indices, total_picks, names_count);

    output[0] = '\0';
    size_t out_len = 0;

    for (int i = 0; i < actual_count && out_len < (size_t)(max_length - 1); i++) {
        append_name(names[indices[i]], output, &out_len, max_length);
    }

    /* If too short, fold in the pre-picked extras. */
    for (int i = 0; i < extras && out_len < (size_t)max_length && out_len < MIN_LENGTH; i++) {
        append_name(names[indices[actual_count + i]], output, &out_len, max_length);
    }

    if (strlen(output) > (size_t)max_length) {
        output[max_length] = '\0';
    }
}

void randomly_capitalize(char *password, int count) {
    size_t len = strlen(password);
    if (len == 0 || count <= 0) return;

    /* Find letter indices */
    size_t *letter_indices = malloc(len * sizeof(size_t));
    if (!letter_indices) return;

    size_t letter_count = 0;
    for (size_t i = 0; i < len; i++) {
        if (isalpha((unsigned char)password[i])) {
            letter_indices[letter_count++] = i;
        }
    }

    if (letter_count == 0) {
        free(letter_indices);
        return;
    }

    /* Shuffle and capitalize */
    shuffle_indices(letter_indices, letter_count);
    int to_cap = (count > (int)letter_count) ? (int)letter_count : count;

    for (int i = 0; i < to_cap; i++) {
        size_t idx = letter_indices[i];
        password[idx] = (char)toupper((unsigned char)password[idx]);
    }

    free(letter_indices);
}

void insert_random_numbers(char *password, size_t password_size, int count) {
    size_t len = strlen(password);
    size_t num_chars = strlen(NUMBERS);

    for (int i = 0; i < count; i++) {
        if (len >= password_size - 1) break;

        char num = NUMBERS[meow_random_uniform((uint32_t)num_chars)];
        size_t pos = (size_t)meow_random_uniform((uint32_t)(len + 1));

        memmove(&password[pos + 1], &password[pos], len - pos + 1);
        password[pos] = num;
        len++;
    }
}

void replace_with_symbols(char *password, int count) {
    size_t len = strlen(password);
    if (len == 0 || count <= 0) return;

    size_t sym_chars = strlen(SYMBOLS);

    /* Find letter indices */
    size_t *letter_indices = malloc(len * sizeof(size_t));
    if (!letter_indices) return;

    size_t letter_count = 0;
    for (size_t i = 0; i < len; i++) {
        if (isalpha((unsigned char)password[i])) {
            letter_indices[letter_count++] = i;
        }
    }

    if (letter_count == 0) {
        free(letter_indices);
        return;
    }

    /* Shuffle and replace */
    shuffle_indices(letter_indices, letter_count);
    int to_replace = (count > (int)letter_count) ? (int)letter_count : count;

    for (int i = 0; i < to_replace; i++) {
        size_t idx = letter_indices[i];
        password[idx] = SYMBOLS[meow_random_uniform((uint32_t)sym_chars)];
    }

    free(letter_indices);
}

void generate_password(const PasswordConfig *config, char *output, size_t output_size) {
    /* Step 1: Select 2-6 random cat names */
    int name_count = (int)meow_random_uniform(5) + 2;  /* 2 to 6 names */

    /* Step 2: Create base phrase */
    select_and_join_names(name_count, output, output_size, config->max_length);

    /* Step 3: Apply security transformations */
    randomly_capitalize(output, 3);
    insert_random_numbers(output, output_size, config->num_numbers);

    /* Truncate before symbol replacement so symbols aren't placed past max_length */
    if (strlen(output) > (size_t)config->max_length) {
        output[config->max_length] = '\0';
    }
    replace_with_symbols(output, config->num_symbols);
}
