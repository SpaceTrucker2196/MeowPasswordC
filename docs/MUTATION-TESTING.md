# Mutation testing

`make mutate` introduces small, well-defined faults into `src/*.c`,
rebuilds the test binary, and reruns `make test`. Every mutant should
be killed (at least one assertion flips red). Surviving mutants are
concrete gaps in the test suite — either a missing assertion (write
the test that would have caught it) or genuinely dead / equivalent
code (document in `.github/scripts/mutate-equivalents.txt`).

The product source under `src/` is **never modified**. The harness
copies the repo into a tmpdir and mutates there.

## Why

`make test` is the oracle. Coverage tools (gcov, llvm-cov) measure
*execution* — which lines ran. Mutation testing measures *fault
detection* — whether the assertions would notice if those lines
broke. The two are weakly correlated: 100% line coverage can pair
with 0% mutation kill rate.

The point is not a green score. The point is: every surviving mutant
is either a real test gap (write the assertion) or a known-equivalent
mutant (document and move on). Either way the suite gets stronger.

## Running

```sh
# Default: mutate src/complexity.c with all operator families.
make mutate

# Pass-through flags.
make mutate MUTATE_FLAGS="--files src/password.c"
make mutate MUTATE_FLAGS="--files src/complexity.c --operators rel const"
make mutate MUTATE_FLAGS="--files src/random.c --limit 30 --seed 7"
make mutate MUTATE_FLAGS="--report /tmp/mutate.txt"
make mutate MUTATE_FLAGS="--keep-sandbox"
```

Direct invocation:

```sh
python3 .github/scripts/mutate.py --files src/complexity.c
```

## Operators

Five text-level operator families. A small state machine masks string
literals, character literals, comments, and preprocessor lines so
mutations only land on executable code.

| op | What changes | Example | Why it matters here |
|---|---|---|---|
| `rel` | Relational / equality swaps: `>` ↔ `>=`, `<` ↔ `<=`, `==` ↔ `!=`. | `if (count > 0)` → `if (count >= 0)` | Off-by-one in `clamp_int`, `meow_random_uniform` rejection bound, complexity loop bounds. |
| `arith` | `+` ↔ `-` on binary infix integer arithmetic (skips `++`, `--`, `+=`, unary, `->`). | `i + 1` → `i - 1` | Index math in `password.c` insertion routines; weight sums in `complexity.c`. |
| `bool` | `&&` ↔ `\|\|`. | `a && b` → `a \|\| b` | Predicate shape in validators / multi-clause guards. |
| `const` | Integer literals ±1. Skips `0`, values ≥ 1024 (buffer sizes), hex/binary. | `NUM_CANDIDATES = 5` → `4` or `6` | Threshold drift — clamp ranges, candidate count, complexity weight literals. |
| `return` | `return N;` (N ≠ 0) → `return 0;`. | `return 1;` → `return 0;` | Boolean / error-code return-path regressions in `update.c`, helpers. |

**Not** mutated (yet):

- Statement deletion (high uncompilable-mutant rate).
- Pointer arithmetic (too many equivalent or segfault mutants).
- Floating-point operators (small surface here; revisit if needed).
- String literals (format strings, CLI flag names aren't where bugs hide).

## File priority

Highest scoring-yield first; mutate in this order.

1. `src/complexity.c` — the scoring math. If `arith`/`const` mutants survive, candidate selection silently degrades and "best of five" stops meaning anything.
2. `src/password.c` — character transformations on generated passwords. Direct security regression surface.
3. `src/random.c` — RNG wrapper. Mutated rejection bound = modulo bias back.
4. `src/config.c` — `clamp_int` + arg parser. Classic relational territory.
5. `src/update.c` — `compare_versions` is pure math, easy kills.
6. `src/main.c` — mostly CLI dispatch; lower yield.

Skip `src/catnames.c` (data) and `src/display.c` (printf formatting).

## Equivalence file

`.github/scripts/mutate-equivalents.txt` lists mutations that are
provably equivalent (no observable behaviour change). Adding an entry
is a deliberate act — it permanently suppresses that mutant from the
survivor count. Always cite the class (`OPT`, `WGT`, `LZT`, …) in the
trailing comment.

Start empty. Earn each entry by triaging a real survivor.

## Reading the report

```
== meowpass mutation testing ==
files:     src/complexity.c
operators: rel, arith, bool, const, return
sandbox:   /tmp/meowpass-mutate-XXXX

baseline:  green (3.2s)

== Mutation summary ==
Mutants:   188
Killed:    62 (33.0%)
Survived:  126
Ignored:   0
```

Each surviving mutant prints as `file:line:operator:original:mutated`.
Triage: either add an assertion that kills it, or add an
equivalents-file entry citing the class.

## Performance note

Baseline build + test takes ~2s. Per-mutant `make -B test` cycle
runs at ~0.6s on Apple Silicon, so a 200-mutant round on
`complexity.c` lands in ~2 min. `src/catnames.c` is 16,936 lines of
static data and recompiles every cycle, but gcc handles a single TU
fast enough that the cost is negligible here. If a CI runner is
slower and this starts to matter, the optimization is to pre-build
`catnames.o` in the sandbox once and patch the sandbox `Makefile` to
skip rebuilding it.
