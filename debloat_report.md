# Debloat Report: main.cc

Based on a thorough diff between the newly modified `Linux/src/main.cc` and the original `fusion_old/Linux/src/main.cc`, a major debloat was performed to excise components that lie completely outside the core execution workflow mapping in `advtempscript.txt` (which primarily revolves around generating and solving the Z3 constraints using `pathz3`).

In total, **29 specific code blocks** (comprising 8 standalone global functions, 19 interactive terminal/command handlers inside `main`, and 2 minor hardcoded structural definitions) were commented out and formally marked with `// DEAD CODE:`.

Here is the comprehensive report detailing exactly what algorithms and components were disabled during the trimming exercise:

## 1. Global Helper Functions Disabled (8 Functions)
These were old helper utilities that operated on outdated memory mappings (like `getExpressionMap` which tracks CNFs globally) or were superseded by the much more optimized `GraphManagerNew` modules:

*   **`get_min_cut(...)`** — An old mincut approach superseded by the newer Gomory-Hu (GH) tree implementations.
*   **`getMinCutMatrix_dummy(...)`** — A placeholder matrix implementation replaced natively by GH-tree algorithms.
*   **`nonempty_intersection_files(...)`** — A heavy system `sort -u` / `uniq` wrapper function that was never actively invoked by the core engine.
*   **`read_z3_output(...)`** — A function mapped to reading raw textual Z3 CLI output (bypassed because `pathz3` now handles Z3 directly through C++ bindings in `Z3Solver.cc`).
*   **`build_OR_constraints(string, ...)`** — A deprecated string-overload method mapping to a defunct `getExpressionMap`. 
*   **`build_AND_constraints(string, ...)`** — Deprecated string-based expression builder matching the above.
*   **`compare_pair_on_second_descending_int_float(...)`** — An unused floating-point scaling map sorting utility.
*   **`compare_pair_on_second_descending_string_to_int(...)`** — Defined as an `extern` inside GraphManagerNew but was never invoked.

## 2. Main Method Command Handlers Disabled (19 Handlers)
The interactive prompt sequence inside `main()` was aggressively stripped. Originally, the executable shipped with dozens of miscellaneous network analysis commands (like evaluating graphs for components or calculating disjoint distributions) that `advtempscript.txt` doesn't execute at all. 

These handler routines were wrapped securely under `// DEAD CODE: <label>` headers and bypassed inside the command loop:

*   **The Old CNF Logic Routines:**
    *   `solve` — The old DIMACS / z3-CLI approach, completely superseded by native `pathz3` calls.
    *   `get_all_solutions_from_cnf` — An archaic CNF string-enumeration solver.
    *   `call_ApproxMC` — An approximate model counter invocation.
    *   `get_assignments_z3` — Command strictly for reading output formats produced by the dead `solve` component.
    *   `clear_expression_maps` — Maps the clearing sequence for the defunct CNF string variables.

*   **Alternative/Redundant Mincut Variants:**
    *   `mincut1` (`mc1`) — Redundant legacy mincut pipeline module.
    *   `mcd` — Dinic's algorithm mincut variant (the active workflow uses a consolidated `mc` command).

*   **Graph Topological Analyzers (Outside constraint scope):**
    *   `bcc` — Biconnected components network analysis.
    *   `gerstein` / `cbc` / `cbp` — Three different command suites revolving strictly around "betweenness centrality" graph math and scaling logic.
    *   `domination` (`dom`) — Domination analysis engine.
    *   `ford` — Old Ford-Fulkerson network flow analysis.
    *   `partition` (`part`) — Graph partitioning/layout component.
    *   `disconnect` (`dc`) and `disconnect2` (`dc2`) — Alternative disconnection mapping protocols.
    *   `matrix` (`mat`) — A command to output raw adjacency matrices.
    *   `unroll` — Mathematical DAG structure unrolling component.
    *   `frank` — Fraction-of-paths statistical ranker.

## Summary
By quarantining all these unused graph components, legacy solver endpoints, and archaic math features behind structural `// DEAD CODE:` blocks, the file ensures no unused execution branches are wasting parser cycles or contributing to compilation latency. The binary is now strictly focused on reading the base graph and pushing constraints securely through `fb_rch` and the `pathz3` SMT core.