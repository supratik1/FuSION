# FuSION C++ Backend — Debloating Changelog

**Date:** 2026-04-08  
**Scope:** `Linux/` subdirectory  
**Objective:** Remove dead code from the C++ backend while preserving the core `pathz3` workflow (KEGG XML parsing, Z3 constraint generation, SMT solving, Pareto optimality analysis).

---

## Files Modified

### 1. `Linux/src/main.cc`

Original: 7,772 lines | Post-debloat: ~7,577 lines (net reduction after commenting)

#### Round 1 — Dead Local Functions (5 functions)

Commented out functions defined in `main.cc` that are never called anywhere in the repository.

| Function | Former Lines | Reason |
|---|---|---|
| `get_min_cut` | 52–62 | Superseded by GH-tree mincut approach; never invoked. |
| `getMinCutMatrix_dummy` | 64–83 | Stub for dummy mincut matrix; never invoked. |
| `nonempty_intersection_files` | 104–132 | File intersection utility; never invoked. |
| `compare_pair_on_second_descending_int_float` | 1051–1056 | Comparator referenced only in commented-out code. |
| `compare_pair_on_second_descending_string_to_int` | 1065–1071 | Has an extern declaration in `GraphManagerNew.cc` but is never actually called. |

#### Round 2 — Dead Command Handlers (18 commands, ~1,700 lines)

Commented out interactive command handlers that are not part of the active `pathz3` workflow. Each block was replaced with a `// DEAD CODE: <label>` header followed by the original code commented out.

| Command(s) | Former Lines | Description |
|---|---|---|
| `name` | 1381–1387 | Print node name by index. |
| `solve` | 2684–2723 | Legacy DIMACS CNF + z3 CLI solver pipeline. |
| `get_all_solutions_from_cnf` | 2867–3082 | Enumerate all CNF solutions via z3 CLI. |
| `call_ApproxMC` | 3084–3106 | Approximate model counting. |
| `gerstein` | 5305–5354 | Gerstein scoring heuristic. |
| `bcc` | 5628–5685 | Biconnected components analysis. |
| `mincut1` / `mc1` | 6036–6224 | Stoer-Wagner mincut variant. |
| `mcd` | 6226–6554 | Mincut-based disconnect analysis. |
| `matrix` / `mat` | 6732–6815 | Adjacency matrix display. |
| `partition` / `part` | 6817–6869 | Graph partitioning. |
| `disconnect` / `dc` | 6871–7002 | Node disconnect analysis (variant 1). |
| `disconnect2` / `dc2` | 7004–7131 | Node disconnect analysis (variant 2). |
| `domination` / `dom` | 7239–7276 | Domination analysis. |
| `cbc` | 7277–7307 | Betweenness centrality calculation. |
| `ford` | 7308–7354 | Ford-Fulkerson max flow. |
| `cbp` | 7355–7577 | Critical bridge/path analysis. |
| `unroll` | 7579–7653 | Graph unrolling. |
| `frank` | 7655–7761 | Frank-Wolfe optimization variant. |

#### Round 3 — CNF Infrastructure Cleanup

Commented out remnants of the legacy DIMACS/CNF solver pipeline that are no longer needed by the active `pathz3` workflow.

| Item | Former Lines | Action |
|---|---|---|
| `read_z3_output` definition | 79–118 | Commented out. Parsed z3 CLI text output; unused by Z3 C++ API path. |
| `build_OR_constraints(string,...)` overload | 705–732 | Commented out. Used `getExpressionMap` (CNF path only). |
| `build_AND_constraints(string,...)` overload | 739–766 | Commented out. Used `getExpressionMap` (CNF path only). |
| Forward declarations for above + `add_cnf` | 71–72 | Commented out. |
| `clear_expression_maps` / `clxm` handler | 3079–3087 | Commented out. Cleared CNF-only maps. |
| `get_assignments_z3` / `get_assg_z3` handler | 5568–5574 | Commented out. Read z3 CLI output file. |
| CNF globals (`idx`, `clauses`, `getExpressionMap`, `getCNFIndexMap`) | — | **Retained.** Required for `GraphManager.cc` link-time references. |
| `add_cnf` function definition | — | **Retained.** Called by `GraphManager.cc:5309`. |

---

### 2. `Linux/Makefile`

Single-line change to fix idempotent builds.

**Change:** Added `cp $(INCLUDE_DIR)/rules_parser.tab.h $(SOURCE_DIR)/rules_parser.tab.h;` before the compile command in the `rules_parser.tab.o` build recipe.

**Reason:** The final link step deletes `src/rules_parser.tab.h`, so subsequent `make` invocations fail because `src/rules_parser.tab.c` includes it. The fix copies the header from `include/` before compilation, making incremental builds reliable.

**Before:**
```makefile
$(BUILD_DIR)/rules_parser.tab.o : $(SOURCE_DIR)/rules_parser.tab.c $(INCLUDE_DIR)/rules_parser.tab.h
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/rules_parser.tab.c -o $(BUILD_DIR)/rules_parser.tab.o
```

**After:**
```makefile
$(BUILD_DIR)/rules_parser.tab.o : $(SOURCE_DIR)/rules_parser.tab.c $(INCLUDE_DIR)/rules_parser.tab.h
	cp $(INCLUDE_DIR)/rules_parser.tab.h $(SOURCE_DIR)/rules_parser.tab.h; $(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/rules_parser.tab.c -o $(BUILD_DIR)/rules_parser.tab.o
```

---

## New Files Created

None. All changes were made to existing files.

---

## Build Verification

- Full `make clean && make` completed successfully after each round of changes.
- Binary size reduced from 21 MB to 20 MB.
- `./fusion -h` confirmed the executable runs correctly.

---

## Identified but Deferred

The following additional debloating opportunities were identified but not yet acted on:

- **`GraphManager.cc`** (~6,000 lines) — entirely legacy, superseded by `GraphManagerNew.cc`. Removing it would also allow removing the CNF globals retained above.
- **SBML parser in `XMLParser.cc`** — never called in the current workflow.
- **Redundant mincut variants in `GraphManagerNew.cc`** — multiple algorithm implementations where only one is used.
- **~300 lines of dead inline methods** in `GraphManagerNew.cc` (lines 655–945) — already commented out in source.
