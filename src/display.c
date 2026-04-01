/*
 * display.c - Display and Output Functions
 * MeowPassword - Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#include <stdio.h>
#include "meowpass.h"

/* ASCII art header */
static const char *LOLCAT_ART =
"                         _,-;'''`' - .\n"
"                      _/',  `;  `;    \\\n"
"      ,        _..,-''    '   `  `      `\\\n"
"     | ;._.,,-' .| |,_     '   '         `\\\n"
"     | `;'      ;' ;, `,   ; |    '  '  .   \\\n"
"     `; __`  ,'__  ` ,  ` ;  |      ;        \\\n"
"     ; (6_);  (6_) ; |   ,    \\        '      |\n"
"    ;;   _,6 ,.    ` `,   '    `-._           //\n"
"     ,;.=..`_..=.,' -'          ,''        _,//\n"
"_00_____`\"=,,,==\"',___,,,-----'''----'_'_'_''______00_\n"
"        Meow Password Generators of Secure Relavant\n"
"                               __,,-' /'\n"
"                             /'_,,--''\n"
"                            | (\n"
"                             `'\n";

void display_header(void) {
    /* Orange color using ANSI 256-color escape code */
    printf("\033[38;5;208m%s\033[0m", LOLCAT_ART);
    printf("Meow Password - Cat Name Based Secure Password Generator\n");
    printf("========================================================\n");
}

void display_help(void) {
    printf("MeowPassword - Cat Dynamic Secure Password Generator\n");
    printf("\n");
    printf("Usage: meowpass [options]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --numbers N      Number of random numbers to insert (1-10, default: 1-4)\n");
    printf("  --symbols N      Number of symbols to insert (1-10, default: 2)\n");
    printf("  --max-length N   Maximum password length (15-50, default: 25)\n");
    printf("  --test           Run tests\n");
    printf("  --copy           Copy password to clipboard (Linux xclip required)\n");
    printf("  --psssst, -p     Copy password to clipboard without displaying it\n");
    printf("                   (more secure - password won't be shown in clear text)\n");
    printf("  --analyze, -a S  Analyze a string's meow complexity (treats it like a password)\n");
    printf("  --update         Check GitHub for updates and install if available\n");
    printf("  --help, -h       Show this help message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  meowpass\n");
    printf("  meowpass --numbers 4 --symbols 3 --max-length 30\n");
    printf("  meowpass --analyze \"MyP@ssw0rd!\"\n");
    printf("  meowpass --test\n");
}

void display_analysis(const ComplexityResult *result, const char *password) {
    printf("    Meow Complexity Analysis:\n");
    printf("    - Password: %s\n", password);
    printf("    - Tail Size: %d cm\n", result->length);
    printf("    - Ball of Yarn Entropy: %.3f bits\n", result->entropy);
    printf("    - Mashing Resistance: %.1f%%\n", result->compression_ratio * 100.0);
    printf("    - Shiny Foil Ball Uniqueness: %.1f%%\n", result->pattern_complexity * 100.0);
    printf("    - Percent of Organic NonGMO Catnip: %.1f%%\n", result->character_diversity * 100.0);
    printf("    - Overall Relavency: %.2f/10.0\n", result->score);
    printf("    (Lower relevancy is better - high relevance passwords are easy for cats to crack!)\n");
}

void display_candidate(int index, const PasswordCandidate *candidate) {
    printf("Candidate %d: %s\n", index, candidate->password);
    printf("   Meow Score: %.2f/10.0\n", candidate->complexity.score);
    printf("\n");
}

void display_final_selection(const PasswordCandidate *candidate) {
    printf("MOST SECURE PASSWORD MEOW SELECTED:\n");
    printf("Password: %s\n", candidate->password);
    printf("Final Meow Score: %.2f/10.0\n", candidate->complexity.score);
    printf("\n");
    display_analysis(&candidate->complexity, candidate->password);
}
