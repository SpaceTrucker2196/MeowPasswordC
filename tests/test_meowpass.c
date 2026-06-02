/*
 * test_meowpass.c - MeowPassword Test Suite
 * Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../src/meowpass.h"

static int tests_passed = 0;
static int tests_failed = 0;

/**
 * Assert helper function
 */
static void assert_true(int condition, const char *message) {
    if (condition) {
        printf("PASS: %s\n", message);
        tests_passed++;
    } else {
        printf("FAIL: %s\n", message);
        tests_failed++;
    }
}

/**
 * Assert equality helper
 */
static void assert_equal_int(int actual, int expected, const char *message) {
    if (actual == expected) {
        printf("PASS: %s\n", message);
        tests_passed++;
    } else {
        printf("FAIL: %s - Expected: %d, Got: %d\n", message, expected, actual);
        tests_failed++;
    }
}

static double meow_abs_d(double x) { return x < 0 ? -x : x; }

static void assert_double_eq(double actual, double expected, const char *message) {
    if (meow_abs_d(actual - expected) < 1e-9) {
        printf("PASS: %s\n", message);
        tests_passed++;
    } else {
        printf("FAIL: %s - Expected: %.9f, Got: %.9f\n", message, expected, actual);
        tests_failed++;
    }
}

static void assert_double_eq_eps(double actual, double expected, double eps, const char *message) {
    if (meow_abs_d(actual - expected) < eps) {
        printf("PASS: %s\n", message);
        tests_passed++;
    } else {
        printf("FAIL: %s - Expected: %.6f +- %.6f, Got: %.6f\n", message, expected, eps, actual);
        tests_failed++;
    }
}

/**
 * Test cat name loading
 */
static void test_load_cat_names(void) {
    printf("\nTesting Cat Name Loading...\n");

    const char **names = get_cat_names();
    size_t count = get_cat_names_count();

    assert_true(names != NULL, "Should Meow load cat names from embedded data");
    assert_true(count > 100, "Should load a substantial Meow number of cat names");

    /* Check that names are non-empty */
    int non_empty = 1;
    for (size_t i = 0; i < count && non_empty; i++) {
        if (names[i] == NULL || names[i][0] == '\0') {
            non_empty = 0;
        }
    }
    assert_true(non_empty, "All Meow Meow loaded names should be non-empty");

    printf("Cat names loaded meow: %zu\n", count);
    if (count >= 5) {
        printf("First few names: %s, %s, %s, %s, %s\n",
               names[0], names[1], names[2], names[3], names[4]);
    }
}

/**
 * Test password generation
 */
static void test_complete_password_generation(void) {
    printf("\nTesting MeowMeow Complete Password Generation...\n");

    PasswordConfig config;
    config.num_numbers = 3;
    config.num_symbols = 2;
    config.max_length = 25;
    config.show_tests = false;
    config.copy_to_clipboard = false;
    config.show_help = false;

    for (int i = 1; i <= 3; i++) {
        char password[MAX_PASSWORD_LENGTH];
        generate_password(&config, password, MAX_PASSWORD_LENGTH);

        printf("Generated password %d: %s\n", i, password);

        size_t len = strlen(password);
        assert_true(len >= 10, "MeowPassword should be at least 10 characters");
        assert_true(len <= (size_t)(config.max_length + 10),
                    "Password should not greatly exceed Meow max length");

        /* Check for numbers */
        int has_numbers = 0;
        int has_letters = 0;
        int has_symbols = 0;

        for (size_t j = 0; j < len; j++) {
            unsigned char c = (unsigned char)password[j];
            if (isdigit(c)) has_numbers = 1;
            else if (isalpha(c)) has_letters = 1;
            else has_symbols = 1;
        }

        assert_true(has_numbers, "Password should contain meow numbers");
        assert_true(has_letters, "Password meow should contain letters");
        assert_true(has_symbols, "Password should contain meow symbols");

        /* Analyze complexity */
        ComplexityResult result;
        analyze_complexity(password, &result);

        assert_true(result.score >= 0.0 && result.score <= 10.0,
                    "Complexity score should be meow valid");
    }
}

/**
 * Test Shannon entropy calculation
 */
static void test_shannon_entropy(void) {
    printf("\nTesting Shannon Entropy Calculation...\n");

    /* Test known entropy values */
    double entropy_aa = calculate_shannon_entropy("aaaa");
    assert_true(entropy_aa < 0.1, "Entropy of 'aaaa' should be near 0");

    double entropy_ab = calculate_shannon_entropy("abab");
    assert_true(entropy_ab > 0.9 && entropy_ab < 1.1,
                "Entropy of 'abab' should be near 1 bit");

    double entropy_abc = calculate_shannon_entropy("abcabc");
    assert_true(entropy_abc > 1.5, "Entropy of 'abcabc' should be > 1.5 bits");

    printf("Entropy tests passed!\n");
}

/**
 * Test character diversity
 */
static void test_character_diversity(void) {
    printf("\nTesting Character Diversity...\n");

    double div1 = calculate_character_diversity("abc");
    assert_equal_int((int)(div1 * 4), 1, "Lowercase only should be 0.25");

    double div2 = calculate_character_diversity("aA1!");
    assert_equal_int((int)(div2 * 4), 4, "All categories should be 1.0");

    printf("Diversity tests passed!\n");
}

/**
 * Test configuration parsing
 */
static void test_config_parsing(void) {
    printf("\nTesting Configuration Parsing...\n");

    char *argv1[] = {"meowpass", "--numbers", "5", "--symbols", "3", "--max-length", "30"};
    PasswordConfig config1;
    config_init(&config1, 7, argv1);

    assert_equal_int(config1.num_numbers, 5, "Numbers should be 5");
    assert_equal_int(config1.num_symbols, 3, "Symbols should be 3");
    assert_equal_int(config1.max_length, 30, "Max length should be 30");

    /* Test clamping */
    char *argv2[] = {"meowpass", "--numbers", "100", "--symbols", "-5"};
    PasswordConfig config2;
    config_init(&config2, 5, argv2);

    assert_equal_int(config2.num_numbers, MAX_NUMBERS, "Numbers should be clamped to max");
    assert_equal_int(config2.num_symbols, MIN_SYMBOLS, "Symbols should be clamped to min");

    /* Test --psssst flag */
    char *argv3[] = {"meowpass", "--psssst"};
    PasswordConfig config3;
    config_init(&config3, 2, argv3);

    assert_true(config3.psssst, "Psssst should be enabled with --psssst");
    assert_true(config3.copy_to_clipboard, "Copy to clipboard should be enabled with --psssst");

    /* Test -p flag */
    char *argv4[] = {"meowpass", "-p"};
    PasswordConfig config4;
    config_init(&config4, 2, argv4);

    assert_true(config4.psssst, "Psssst should be enabled with -p");
    assert_true(config4.copy_to_clipboard, "Copy to clipboard should be enabled with -p");

    printf("Config parsing tests passed!\n");
}

/**
 * Test relevancy score explanation in display output
 */
static void test_relevancy_score_explanation(void) {
    printf("\nTesting Relevancy Score Explanation...\n");

    /* Create a test complexity result */
    ComplexityResult result;
    result.length = 20;
    result.entropy = 3.5;
    result.compression_ratio = 0.8;
    result.pattern_complexity = 0.9;
    result.character_diversity = 1.0;
    result.score = 2.45;

    /* Capture display_analysis output by redirecting stdout */
    char buffer[2048];
    FILE *memstream = fmemopen(buffer, sizeof(buffer), "w");
    assert_true(memstream != NULL, "Should open memory stream for output capture");

    FILE *old_stdout = stdout;
    stdout = memstream;
    display_analysis(&result, "testMeow123!");
    fflush(stdout);
    stdout = old_stdout;
    fclose(memstream);

    /* Verify the explanation is present */
    assert_true(strstr(buffer, "Lower relevancy is better") != NULL,
                "Display should explain that lower relevancy is better");
    assert_true(strstr(buffer, "easy for cats to crack") != NULL,
                "Display should warn about high relevance passwords being easy for cats to crack");
    assert_true(strstr(buffer, "Overall Relavency") != NULL,
                "Display should show Overall Relavency score");

    printf("Relevancy explanation tests passed!\n");
}

/**
 * Test update checker version comparison
 */
static void test_update_version_compare(void) {
    printf("\nTesting Update Version Comparison...\n");

    assert_equal_int(compare_versions("1.0.0", "1.0.0"), 0,
                     "Same version should return 0");
    assert_equal_int(compare_versions("1.0.0", "1.0.1"), 1,
                     "Newer patch version should return 1");
    assert_equal_int(compare_versions("1.0.0", "1.1.0"), 1,
                     "Newer minor version should return 1");
    assert_equal_int(compare_versions("1.0.0", "2.0.0"), 1,
                     "Newer major version should return 1");
    assert_equal_int(compare_versions("1.0.1", "1.0.0"), -1,
                     "Older patch version should return -1");
    assert_equal_int(compare_versions("2.0.0", "1.9.9"), -1,
                     "Older version should return -1");
    assert_equal_int(compare_versions("0.0.1", "0.0.2"), 1,
                     "Pre-release newer patch should return 1");

    printf("Update version comparison tests passed!\n");
}

/**
 * Test --analyze config parsing
 */
static void test_analyze_config(void) {
    printf("\nTesting Meow Analyze Config Parsing...\n");

    /* Test --analyze flag */
    char *argv1[] = {"meowpass", "--analyze", "TestMeow123!"};
    PasswordConfig config1;
    config_init(&config1, 3, argv1);
    assert_true(config1.analyze_string != NULL, "Analyze string should be set with --analyze");
    assert_true(strcmp(config1.analyze_string, "TestMeow123!") == 0,
                "Analyze string should match meow input");

    /* Test -a short flag */
    char *argv2[] = {"meowpass", "-a", "CatPaws42"};
    PasswordConfig config2;
    config_init(&config2, 3, argv2);
    assert_true(config2.analyze_string != NULL, "Analyze string should be set with -a");
    assert_true(strcmp(config2.analyze_string, "CatPaws42") == 0,
                "Analyze string should match meow input with -a");

    /* Test missing argument */
    char *argv3[] = {"meowpass", "--analyze"};
    PasswordConfig config3;
    config_init(&config3, 2, argv3);
    assert_true(config3.analyze_string == NULL,
                "Analyze string should be NULL when no meow argument given");

    printf("Meow analyze config tests passed!\n");
}

/**
 * Test analyze complexity on user input strings
 */
static void test_analyze_input_string(void) {
    printf("\nTesting Meow Analyze Input String...\n");

    /* Test a strong password */
    ComplexityResult strong_result;
    analyze_complexity("C@tP4ws!Str0ng#Meow", &strong_result);
    assert_true(strong_result.score > 0.0, "Strong password should have a meow positive score");
    assert_true(strong_result.entropy > 2.0, "Strong password should have high ball of yarn entropy");
    assert_true(strong_result.character_diversity > 0.5,
                "Strong password should have high catnip diversity");

    /* Test a weak password */
    ComplexityResult weak_result;
    analyze_complexity("aaaa", &weak_result);
    assert_true(weak_result.entropy < 0.1, "Weak password should have low meow entropy");
    assert_true(weak_result.character_diversity < 0.5,
                "Weak password should have low catnip diversity");

    /* Test empty string */
    ComplexityResult empty_result;
    memset(&empty_result, 0xFF, sizeof(empty_result));
    analyze_complexity("", &empty_result);
    assert_true(empty_result.length == 0, "Empty string should have zero meow tail size");

    printf("Meow analyze input string tests passed!\n");
}

/**
 * Round-1 mutation gap closers for src/complexity.c.
 *
 * Each block pins behavior that was unobservable to the old assertion
 * set. Hand-derived expected values; see docs/MUTATION-TESTING.md for
 * the equivalence-class notes on what is intentionally NOT pinned.
 */
static void test_complexity_boundaries(void) {
    printf("\nTesting Meow Complexity Boundary Pinning...\n");

    /* Gap 1 — null & empty guards on every public function. */
    assert_true(calculate_shannon_entropy(NULL)     == 0.0, "shannon NULL is 0");
    assert_true(calculate_shannon_entropy("")       == 0.0, "shannon empty is 0");
    assert_true(calculate_compression_ratio(NULL)   == 0.0, "compress NULL is 0");
    assert_true(calculate_compression_ratio("")     == 0.0, "compress empty is 0");
    assert_true(calculate_pattern_complexity(NULL)  == 0.0, "pattern NULL is 0");
    assert_true(calculate_pattern_complexity("")    == 0.0, "pattern empty is 0");
    assert_true(calculate_character_diversity(NULL) == 0.0, "diversity NULL is 0");
    assert_true(calculate_character_diversity("")   == 0.0, "diversity empty is 0");

    {
        ComplexityResult r;
        analyze_complexity(NULL, &r);   /* must not crash */
        analyze_complexity("x", NULL);  /* must not crash */
        assert_true(1, "analyze_complexity null-safe");
    }

    /* Gap 2a — shannon entropy: exact values on small inputs. */
    assert_double_eq(calculate_shannon_entropy("a"),    0.0, "entropy a is 0");
    assert_double_eq(calculate_shannon_entropy("aa"),   0.0, "entropy aa is 0");
    assert_double_eq(calculate_shannon_entropy("ab"),   1.0, "entropy ab is 1");
    assert_double_eq(calculate_shannon_entropy("abcd"), 2.0, "entropy abcd is log2 4");

    /* Gap 2b — compression ratio: pin the RLE math (including its
     * off-by-one against the implicit '\0' previous). */
    assert_double_eq(calculate_compression_ratio("a"),    -1.0,  "compress a is -1");
    assert_double_eq(calculate_compression_ratio("ab"),   -0.5,  "compress ab is -0.5");
    assert_double_eq(calculate_compression_ratio("aaaa"),  0.25, "compress aaaa is 0.25");
    assert_double_eq(calculate_compression_ratio("aabb"), -0.25, "compress aabb is -0.25");
    assert_double_eq(calculate_compression_ratio("aaab"),  0.0,  "compress aaab is 0");

    /* Gap 3 — pattern complexity: pin substring counts. */
    assert_double_eq(calculate_pattern_complexity("a"),      0.0, "pattern len<2 is 0");
    assert_double_eq(calculate_pattern_complexity("ab"),     1.0, "pattern ab is 1");
    assert_double_eq(calculate_pattern_complexity("aa"),     1.0, "pattern aa is 1");
    assert_double_eq(calculate_pattern_complexity("abcd"),   1.0, "pattern abcd is 1");
    assert_double_eq(calculate_pattern_complexity("aaaa"),   0.5, "pattern aaaa is 0.5");
    /* 6-a string catches mutations that bump max_substr_len up (e.g. 4 -> 5)
     * since slen=5 only fires when len >= 5. */
    assert_double_eq_eps(calculate_pattern_complexity("aaaaaa"), 0.25, 1e-9,
                         "pattern aaaaaa is 0.25");

    /* Gap 4 — analyze_complexity composite at multiple lengths.
     * length=12 catches divisor mutations (25 -> 24/26) because all-a
     * strings make every other component constant, so only length_norm
     * changes. length=50 catches the cap-literal mutation (1 -> 0/2)
     * since fmin(50/25, 1.0) is at the cap. */
    {
        ComplexityResult r;
        analyze_complexity("aaaaaaaaaaaa", &r);                       /* len 12 */
        assert_equal_int(r.length, 12, "analyze len 12");
        assert_double_eq_eps(r.score, 0.293, 0.001, "analyze 12-a score");

        analyze_complexity("aaaaaaaaaaaaaaaaaaaaaaaaa", &r);          /* len 25 */
        assert_equal_int(r.length, 25, "analyze len 25");
        assert_double_eq_eps(r.score, 0.366195, 0.001, "analyze 25-a score");

        analyze_complexity(
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", &r); /* len 50 */
        assert_equal_int(r.length, 50, "analyze len 50");
        assert_double_eq_eps(r.score, 0.376667, 0.001, "analyze 50-a score");
    }

    printf("Meow complexity boundary tests done!\n");
}

/**
 * Run all tests (exported function)
 */
int run_tests(void) {
    printf("Running Basic MeowPassword Tests\n");
    printf("=================================\n");

    test_load_cat_names();
    test_complete_password_generation();
    test_shannon_entropy();
    test_character_diversity();
    test_complexity_boundaries();
    test_config_parsing();
    test_relevancy_score_explanation();
    test_update_version_compare();
    test_analyze_config();
    test_analyze_input_string();

    printf("\nMeow Basic Tests Complete!\n");
    printf("=====================\n");
    printf("Passed: %d, Failed: %d\n", tests_passed, tests_failed);

    return (tests_failed > 0) ? 1 : 0;
}
