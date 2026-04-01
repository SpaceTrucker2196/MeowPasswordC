/*
 * meowpass.h - MeowPassword Header
 * Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#ifndef MEOWPASS_H
#define MEOWPASS_H

#include <stddef.h>
#include <stdbool.h>

/* Version info */
#define MEOWPASS_VERSION "1.0.0"

/* Configuration limits */
#define MIN_NUMBERS 1
#define MAX_NUMBERS 10
#define DEFAULT_NUM_NUMBERS 3
#define MIN_SYMBOLS 1
#define MAX_SYMBOLS 10
#define DEFAULT_NUM_SYMBOLS 2
#define MIN_LENGTH 15
#define MAX_LENGTH 50
#define DEFAULT_MAX_LENGTH 25
#define NUM_CANDIDATES 5

/* Maximum password buffer size */
#define MAX_PASSWORD_LENGTH 128

/* GitHub repository for update checks */
#define MEOWPASS_GITHUB_OWNER "SpaceTrucker2196"
#define MEOWPASS_GITHUB_REPO  "MeowPasswordC"

/* Configuration structure */
typedef struct {
    int num_numbers;
    int num_symbols;
    int max_length;
    bool show_tests;
    bool copy_to_clipboard;
    bool psssst;
    bool show_help;
    bool check_update;
    char *analyze_string;
} PasswordConfig;

/* Complexity analysis result */
typedef struct {
    double score;
    double entropy;
    double compression_ratio;
    double pattern_complexity;
    double character_diversity;
    int length;
} ComplexityResult;

/* Password candidate */
typedef struct {
    char password[MAX_PASSWORD_LENGTH];
    ComplexityResult complexity;
} PasswordCandidate;

/* ============ Config Functions (config.c) ============ */

/**
 * Initialize configuration from command line arguments
 * @param config Pointer to config structure to initialize
 * @param argc Argument count
 * @param argv Argument vector
 */
void config_init(PasswordConfig *config, int argc, char *argv[]);

/**
 * Clamp integer value between min and max
 * @param value Value to clamp
 * @param min Minimum value
 * @param max Maximum value
 * @return Clamped value
 */
int clamp_int(int value, int min, int max);

/* ============ Cat Names Functions (catnames.c) ============ */

/**
 * Get the array of embedded cat names
 * @return Pointer to the array of cat name strings
 */
const char **get_cat_names(void);

/**
 * Get the number of embedded cat names
 * @return Number of cat names
 */
size_t get_cat_names_count(void);

/* ============ Password Functions (password.c) ============ */

/**
 * Generate a secure password from cat names
 * @param config Password configuration
 * @param output Buffer to store generated password
 * @param output_size Size of output buffer
 */
void generate_password(const PasswordConfig *config, char *output, size_t output_size);

/**
 * Randomly capitalize letters in password
 * @param password Password buffer to modify
 * @param count Number of letters to capitalize
 */
void randomly_capitalize(char *password, int count);

/**
 * Insert random numbers into password
 * @param password Password buffer to modify
 * @param password_size Size of password buffer
 * @param count Number of numbers to insert
 */
void insert_random_numbers(char *password, size_t password_size, int count);

/**
 * Replace random letters with symbols
 * @param password Password buffer to modify
 * @param count Number of replacements
 */
void replace_with_symbols(char *password, int count);

/* ============ Complexity Functions (complexity.c) ============ */

/**
 * Calculate Shannon entropy of a string
 * @param str Input string
 * @return Entropy in bits per character
 */
double calculate_shannon_entropy(const char *str);

/**
 * Calculate compression ratio approximation
 * @param str Input string
 * @return Compression ratio (0.0 to 1.0)
 */
double calculate_compression_ratio(const char *str);

/**
 * Calculate pattern complexity
 * @param str Input string
 * @return Pattern complexity score (0.0 to 1.0)
 */
double calculate_pattern_complexity(const char *str);

/**
 * Calculate character diversity score
 * @param str Input string
 * @return Diversity score (0.0 to 1.0)
 */
double calculate_character_diversity(const char *str);

/**
 * Analyze overall complexity of password
 * @param password Password to analyze
 * @param result Pointer to store analysis result
 */
void analyze_complexity(const char *password, ComplexityResult *result);

/* ============ Display Functions (display.c) ============ */

/**
 * Display ASCII art header
 */
void display_header(void);

/**
 * Display help message
 */
void display_help(void);

/**
 * Display complexity analysis
 * @param result Complexity analysis result
 */
void display_analysis(const ComplexityResult *result, const char *password);

/**
 * Display password candidate
 * @param index Candidate number (1-based)
 * @param candidate Password candidate
 */
void display_candidate(int index, const PasswordCandidate *candidate);

/**
 * Display final selected password
 * @param candidate Best password candidate
 */
void display_final_selection(const PasswordCandidate *candidate);

/* ============ Update Functions (update.c) ============ */

/**
 * Compare two semantic version strings (e.g. "1.0.0" vs "1.0.1")
 * @param current Current version string
 * @param latest Latest version string
 * @return 1 if latest > current, 0 if equal, -1 if latest < current
 */
int compare_versions(const char *current, const char *latest);

/**
 * Check GitHub for a newer release, prompt user, and install if desired.
 * Requires curl to be installed.
 * @return 0 on success or no update, non-zero on error
 */
int check_for_update(void);

/* ============ Test Functions (for --test mode) ============ */

/**
 * Run all basic tests
 * @return 0 on success, non-zero on failure
 */
int run_tests(void);

#endif /* MEOWPASS_H */
