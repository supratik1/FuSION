# Bugs fixed in pathz3 mode 0 Pareto enumeration

This document summarizes two related bugs in the in-process Pareto-front
enumeration used by `pathz3` mode 0, and the fixes applied in this commit.
All file references are to `Linux/src/GraphManagerNew.cc` after the patch.

## Background

The function `GraphManagerNew::get_PO_points_within_limits` performs a binary
search over a (node-relax, edge-relax) rectangle, calling `z3::solver::check`
at each probe point. The result is one of `z3::sat`, `z3::unsat`, or
`z3::unknown`. The cache (`sat_state_matrix`) supports four states defined in
`Linux/include/macros.h`: `UNDEFINED`, `SAT`, `UNSAT`, `TIMEOUT`.

The function `GraphManagerNew::is_sat_at_existing_PO_points` is reached only
from `pathz3` mode 1 (see `Linux/src/main.cc`) and iterates over the running
Pareto list to check whether any existing point is still satisfiable under
the current constraints.

## Bug 1: z3::unknown was conflated with z3::unsat

### Symptom

Two runs of the same problem with different constraint sets (for example
OFF vs ON for the `$NEW_CONSTRAINTS_ENABLED` flag) produced Pareto fronts
that appeared to violate dominance. The per-call solver timeout was too
small relative to the actual solve-time distribution, so z3 returned
`unknown` (timeout) on a non-trivial fraction of probes. Those probes were
silently treated as `unsat`, biasing the binary search outward and recording
upper-bound-only Pareto corners in `PO.dat` with no indication of
incompleteness.

Experiment 7 (per-call timeout widened to 600s / 3000s) confirmed the
mechanism: with a timeout above the longest actual solve, no probe returned
`unknown` and both OFF and ON produced the identical front
`{(0,1), (1,0)}`.

### Where the bug lived

Three `s.check()` sites inside `get_PO_points_within_limits`, each followed
by two coupled blocks:

- Diagonal loop: cache write and steering after the check.
- Smallest-n loop: cache write and steering after the check.
- Smallest-e loop: cache write and steering after the check.

In every cache write, the structure was

```cpp
if (is_sat == z3::sat)        { cache = SAT; }
else if (is_sat == z3::unsat) { cache = UNSAT; }
// no else: z3::unknown leaves the cell as UNDEFINED, never as TIMEOUT
```

In every steering block, the structure was

```cpp
if (is_sat == z3::sat) { /* lower the upper bound */ }
else                   { /* raise the lower bound */ }
// the bare else catches z3::unknown and treats it as unsat
```

A fourth instance of the same bare-else pattern appeared in
`is_sat_at_existing_PO_points`, where a non-sat verdict (whether unsat or
unknown) was bucketed together.

### Fix

At every one of the seven sites, an explicit `else if (is_sat == z3::unknown)`
branch was added. Inside that branch, the z3 reason string from
`s.reason_unknown()` is inspected:

- If the reason contains "timeout", the branch writes
  `sat_state_t::TIMEOUT` into the cache (where applicable) and logs to
  both `fout` (per-run log file) and `cout` (terminal) the offending
  (e, n) point together with a prompt to raise the solver timeout via the
  batch-file macros `$CONSTR_SOLVER_TIMEOUT1` and `$CONSTR_SOLVER_TIMEOUT2`.
- Otherwise, the branch writes `sat_state_t::UNDEFINED` into the cache
  (where applicable) and logs the (e, n) point together with the z3 reason
  string so the user can diagnose the non-timeout cause of the unknown
  verdict.

For the three steering blocks, the new branches preserve the existing
"raise the lower bound" action after logging, so the binary-search loop
still makes forward progress. This intentionally does not change the
search semantics: it only makes the timeout and undef cases visible. A
follow-up change can decide to abort the window on timeout, retry with a
larger budget, or fall back to a per-window grid sweep.

The bare `else` after the new `else if (is_sat == z3::unknown)` branch now
catches only `z3::unsat`, which is the intended semantics.

## Bug 2: i++ was unreachable in is_sat_at_existing_PO_points

### Symptom

`is_sat_at_existing_PO_points` would spin forever on the first non-sat
point in `PO_value_pairs`, never returning. The function is only reached
from `pathz3` mode 1, so the bug did not fire during normal mode 0 runs,
but any mode 1 invocation with a non-sat existing point would hang.

### Where the bug lived

The original loop was

```cpp
vector<pair<int, int>>::iterator i = PO_value_pairs.begin();
while (i != PO_value_pairs.end())
{
    ...
    if (is_sat == z3::sat)        { ...; return true; }
    else if (is_sat == z3::unsat) { ...; continue; }
    else                          { continue; }

    i++;  // unreachable: every arm above returns or continues
}
```

Every arm of the if/else chain either returns or hits `continue`, so the
trailing `i++` could never execute. The `continue` then re-entered the
`while` with the same iterator value, producing an infinite loop.

### Fix

The `while` loop was converted to a `for` loop with the increment in the
loop header:

```cpp
for (auto i = PO_value_pairs.begin(); i != PO_value_pairs.end(); ++i)
{
    ...
    if (is_sat == z3::sat)        { ...; return true; }
    else if (is_sat == z3::unsat) { ... }
    else if (is_sat == z3::unknown) { ... }
}
return false;
```

With the increment in the header, the iterator advances at every loop
re-entry regardless of which arm ran. The redundant `continue` statements
were removed, and the unreachable trailing `i++` was deleted. The bare
`else` was also removed since `z3::check_result` has only three values and
all are now handled explicitly.

This pattern also makes the same class of bug harder to reintroduce: any
future arm added inside the loop body will inherit the automatic advance
from the for-header.

## Sites touched

All edits are in `Linux/src/GraphManagerNew.cc`.

| Site | Function | Change |
| --- | --- | --- |
| Diagonal cache write | `get_PO_points_within_limits` | added undef/timeout split |
| Diagonal steering | `get_PO_points_within_limits` | added undef/timeout split |
| Smallest-n cache write | `get_PO_points_within_limits` | added undef/timeout split |
| Smallest-n steering | `get_PO_points_within_limits` | added undef/timeout split |
| Smallest-e cache write | `get_PO_points_within_limits` | added undef/timeout split |
| Smallest-e steering | `get_PO_points_within_limits` | added undef/timeout split |
| Loop header | `is_sat_at_existing_PO_points` | while converted to for |
| Cache/return arms | `is_sat_at_existing_PO_points` | added undef/timeout split |

## Build status

`make fusion` from `Linux/` builds cleanly with all edits applied.

## Out of scope for this commit

- The dead helper `binary_search_for_exact_bound_with_z3` (only reachable
  from a commented-out call site in `main.cc`) already records `TIMEOUT`
  on every unknown verdict, so it was not touched. If it is ever revived,
  the undef vs timeout split should be applied there too.
- Tier-3 improvements such as retry-with-doubled-budget, per-window grid
  sweep on persistent unknown, and emitting timeout markers into `PO.dat`
  for downstream plots are not included.
