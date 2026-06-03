/*
 * update.c - Update Checking Functions
 * MeowPassword - Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meowpass.h"

/* MP_STATIC: file-internal in production, externally linkable in the
 * mutation-testing build so tests can call the JSON / version helpers
 * directly. */
#ifdef MEOW_TEST_RNG
#  define MP_STATIC /* exposed for tests */
#else
#  define MP_STATIC static
#endif

int compare_versions(const char *current, const char *latest) {
    int cur_major = 0, cur_minor = 0, cur_patch = 0;
    int lat_major = 0, lat_minor = 0, lat_patch = 0;

    sscanf(current, "%d.%d.%d", &cur_major, &cur_minor, &cur_patch);
    sscanf(latest,  "%d.%d.%d", &lat_major, &lat_minor, &lat_patch);

    if (lat_major != cur_major) return (lat_major > cur_major) ? 1 : -1;
    if (lat_minor != cur_minor) return (lat_minor > cur_minor) ? 1 : -1;
    if (lat_patch != cur_patch) return (lat_patch > cur_patch) ? 1 : -1;
    return 0;
}

/**
 * Extract version string from a JSON "tag_name" field.
 * Looks for "tag_name":"vX.Y.Z" and writes X.Y.Z into out.
 * Returns 0 on success, -1 on failure.
 */
MP_STATIC int parse_tag_from_json(const char *json, char *out, size_t out_size) {
    const char *key = "\"tag_name\"";
    const char *pos = strstr(json, key);
    if (!pos) return -1;

    pos += strlen(key);
    /* skip optional whitespace and colon */
    while (*pos == ' ' || *pos == ':' || *pos == '\t') pos++;
    if (*pos != '"') return -1;
    pos++; /* skip opening quote */

    /* skip leading 'v' or 'V' if present */
    if (*pos == 'v' || *pos == 'V') pos++;

    size_t i = 0;
    while (*pos && *pos != '"' && i < out_size - 1) {
        out[i++] = *pos++;
    }
    out[i] = '\0';
    return (i > 0) ? 0 : -1;
}

/**
 * Validate that a version string contains only safe characters (digits and dots).
 * Returns 1 if valid, 0 otherwise.
 */
MP_STATIC int is_valid_version(const char *version) {
    if (!version || !*version) return 0;
    for (const char *p = version; *p; p++) {
        if (*p != '.' && (*p < '0' || *p > '9')) return 0;
    }
    return 1;
}

int check_for_update(void) {
    printf("Checking for updates...\n");

    /* Use curl to fetch the latest release info from GitHub */
    const char *cmd =
        "curl -s https://api.github.com/repos/"
        MEOWPASS_GITHUB_OWNER "/" MEOWPASS_GITHUB_REPO
        "/releases/latest";

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not execute update check command.\n");
        return -1;
    }

    /* Read entire response */
    char buf[8192];
    size_t total = 0;
    size_t n;
    while ((n = fread(buf + total, 1, sizeof(buf) - total - 1, fp)) > 0) {
        total += n;
        if (total >= sizeof(buf) - 1) break;
    }
    buf[total] = '\0';
    pclose(fp);

    if (total == 0) {
        fprintf(stderr, "Error: No response from GitHub API.\n");
        return -1;
    }

    /* Parse the tag_name from the JSON */
    char latest_version[64];
    if (parse_tag_from_json(buf, latest_version, sizeof(latest_version)) != 0) {
        fprintf(stderr, "Error: Could not parse version from GitHub response.\n");
        return -1;
    }

    /* Validate version string to prevent shell injection */
    if (!is_valid_version(latest_version)) {
        fprintf(stderr, "Error: Invalid version format received from GitHub.\n");
        return -1;
    }

    printf("Current version: %s\n", MEOWPASS_VERSION);
    printf("Latest  version: %s\n", latest_version);

    int cmp = compare_versions(MEOWPASS_VERSION, latest_version);
    if (cmp <= 0) {
        printf("You are already running the latest version. Meow!\n");
        return 0;
    }

    /* Newer version available — prompt user */
    printf("\nA new version (%s) is available!\n", latest_version);
    printf("Would you like to download, build, and install it? [y/N] ");
    fflush(stdout);

    int ch = getchar();
    /* Consume remaining characters in the input line */
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* discard */ }

    if (ch != 'y' && ch != 'Y') {
        printf("Update skipped.\n");
        return 0;
    }

    /* Download, build, and install */
    char install_cmd[1024];
    snprintf(install_cmd, sizeof(install_cmd),
        "set -e && "
        "TMPDIR=$(mktemp -d) && "
        "echo \"Downloading v%s...\" && "
        "curl -sL https://github.com/%s/%s/archive/refs/tags/v%s.tar.gz "
        "| tar xz -C \"$TMPDIR\" && "
        "echo \"Building...\" && "
        "make -C \"$TMPDIR/%s-%s\" && "
        "echo \"Installing (may require sudo)...\" && "
        "sudo make -C \"$TMPDIR/%s-%s\" install && "
        "rm -rf \"$TMPDIR\" && "
        "echo \"Update complete! Meow!\"",
        latest_version,
        MEOWPASS_GITHUB_OWNER, MEOWPASS_GITHUB_REPO, latest_version,
        MEOWPASS_GITHUB_REPO, latest_version,
        MEOWPASS_GITHUB_REPO, latest_version);

    int ret = system(install_cmd);
    if (ret != 0) {
        fprintf(stderr, "Error: Update failed. Check output above for details.\n");
        return -1;
    }

    return 0;
}
