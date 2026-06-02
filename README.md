# meowpass

[![CI](https://github.com/SpaceTrucker2196/MeowPasswordC/actions/workflows/ci.yml/badge.svg)](https://github.com/SpaceTrucker2196/MeowPasswordC/actions/workflows/ci.yml)
[![C/C++ CI](https://github.com/SpaceTrucker2196/MeowPasswordC/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/SpaceTrucker2196/MeowPasswordC/actions/workflows/c-cpp.yml)
[![CMake](https://github.com/SpaceTrucker2196/MeowPasswordC/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/SpaceTrucker2196/MeowPasswordC/actions/workflows/cmake-single-platform.yml)

> Cats are more complex than dogs.

A phrase-password generator that draws from 16,913 embedded cat names, sprinkles
in digits and symbols, scores five candidates with Shannon entropy and a stack
of complexity heuristics, and hands you the best one. Pure C11. Single binary.
Linux + macOS. White-hat.

Marketing site: **https://spacetrucker2196.github.io/MeowPasswordC/**

## Features

- 16,913 cat names embedded directly in the binary — no data files, no network
- OS-grade randomness (`getrandom()` / `/dev/urandom`) with rejection sampling
- Best-of-five candidate selection scored across four metrics
- Catified analysis labels in the CLI (Tail Size, Ball of Yarn Entropy,
  Mashing Resistance, Shiny Foil Ball Uniqueness, Organic NonGMO Catnip)
- `--analyze` mode to score any existing string
- `--psssst` silent mode: copy the winner without ever displaying it
- `--update` self-updater that checks GitHub Releases
- Clipboard via `pbcopy` (macOS), `xclip`, or `wl-copy` (Linux)
- Pure C11. Zero runtime dependencies.

## Build

### Make
```bash
make
./meowpass
```

### CMake
```bash
cmake -B build
cmake --build build
./build/meowpass
```

### Install system-wide
```bash
sudo make install
```

## Usage

```bash
# default — generate, score, show, hint to copy
meowpass

# tune the recipe
meowpass --numbers 4 --symbols 3 --max-length 30

# silent mode — copy winner to clipboard, print nothing
meowpass --psssst

# audit a string you already have
meowpass --analyze "MyP@ssw0rd!"

# run the test suite
meowpass --test

# self-update
meowpass --update
```

### Flags

| Flag | What it does | Range / default |
|---|---|---|
| `--numbers N` | Random digits inserted into the phrase | 1–10 · default 1–4 |
| `--symbols N` | Letter→symbol substitutions | 1–10 · default 2 |
| `--max-length N` | Cap on final password length | 15–50 · default 25 |
| `--copy` | Copy winner to clipboard after display | pbcopy / xclip / wl-copy |
| `--psssst`, `-p` | Silent mode — copy only, never print | — |
| `--analyze`, `-a S` | Score an existing string instead of generating | — |
| `--update` | Self-update from GitHub releases | requires `curl` |
| `--test` | Run bundled tests | — |
| `--help`, `-h` | Print help | — |

## How it works

1. Draw a handful of cat names from the embedded dictionary
2. Join them lowercase into a phrase
3. Randomly capitalize a few characters
4. Insert digits at random positions
5. Substitute symbols for select letters
6. Repeat steps 1–5 five times to produce candidates
7. Score each candidate; emit the one with the lowest (best) composite score

## Complexity metrics

Internally these are the standard names; in CLI output they wear catified labels.

| Internal | CLI label | What it measures |
|---|---|---|
| Shannon entropy | Ball of Yarn Entropy | Bits of information per character |
| Compression ratio | Mashing Resistance | How repetitive the string is (lower = better) |
| Pattern complexity | Shiny Foil Ball Uniqueness | Substring uniqueness vs length |
| Character diversity | Organic NonGMO Catnip | Coverage of lower/upper/digits/symbols |
| Composite score | Overall Relavency | Weighted blend — **lower is better** |

## Requirements

- C11 compiler (gcc or clang)
- Linux or macOS
- Optional: `xclip` or `wl-copy` on Linux, or `pbcopy` on macOS, for `--copy`
- Optional: `curl` for `--update`

## Project layout

```
src/         core C — main, config, password, complexity, random, display, update
src/catnames.c     embedded cat-name dictionary (16,913 entries)
tests/       unit tests run by --test
features/    BDD/behave feature files
docs/        marketing site (GitHub Pages)
```

## License

MIT — see [LICENSE](LICENSE).

## Author

Jeffrey Kunzelman

## Credits

Original Swift implementation: [MeowPassword](https://github.com/SpaceTrucker2196/MeowPassword).
