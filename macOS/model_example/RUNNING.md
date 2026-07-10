# Running the model example

Step-by-step instructions for reproducing the EGFR → CXCL8 (FSC = JUN) analysis,
both from the command line and from the graphical frontend.

For what the example *means* — the biology, the reduced master graph, why the two
frontiers separate — see [README.md](README.md).

---

## Before you start

1. Build the executable. From the `macOS` directory: `make`. This produces
   `macOS/fusion`. See [../README.md](../README.md).
2. Make sure `gnuplot` is installed (`brew install gnuplot`). Without it the
   solve still runs but no plot is written.

---

## Step 1 — Set the working directory (required)

Near the top of `run_model_example.batch` is a placeholder `let $WORK_DIR` line:

```
let $WORK_DIR /ABSOLUTE/PATH/TO/FuSION/macOS/model_example
```

Replace it with the **absolute path** to a working directory of your choice. That
directory must contain the `inputs/` folder from this example. Pointing it at this
directory is the simplest option:

```
let $WORK_DIR /Users/<you>/projects/FuSION/macOS/model_example
```

To use a different location, copy `inputs/` there first and point `$WORK_DIR` at it.

Two things to know:

- **The path must be absolute.** Several steps `cd` into the output directory and
  then use paths built from `$WORK_DIR`. With a relative path those resolve twice,
  and the run exits 0 while quietly producing no plot — an easy failure to miss.
- **If you forget to edit it**, the run aborts immediately with
  `mkdir: /ABSOLUTE: Read-only file system`. Nothing is created or overwritten.

---

## Step 2a — Run from the command line

From the `macOS` directory:

```bash
./fusion -b model_example/run_model_example.batch
```

Results are written to `$WORK_DIR/run/`.

---

## Step 2b — Run from the frontend

Launch the GUI from the `macOS` directory (not from `macOS/frontend` — it resolves
`./fusion` relative to the working directory):

```bash
javac -d frontend/out -cp "frontend/lib/json-20250517.jar" frontend/src/*.java
java -cp "frontend/out:frontend/resources:frontend/lib/json-20250517.jar" LoginPage
```

Then enter the values below. Paths shown as `inputs/...` are relative to this
example directory; select them with the file pickers.

### Working directory

| Field | Value |
|---|---|
| Working directory | your absolute `model_example` path (same as `$WORK_DIR`) |

### HSA Node IDs

| Field | Value | Gene |
|---|---|---|
| Source HSA ID | `hsa1956` | EGFR |
| Target HSA ID | `hsa3576` | CXCL8 / IL8 |
| Candidate Implicated Node HSA ID | `hsa3725` | JUN |
| HSA IDs not to be merged | `inputs/hsa_ids_not_to_be_merged.txt` | |

### Mapping files

| Field | Value |
|---|---|
| HSA to Gene Symbol Map | `inputs/hsa_to_gene_symbol_map.txt` |
| HSA path to Path Name Map | `inputs/hsa_path_to_path_name_map.txt` |
| Cross Database Map | `inputs/inter_db_mapping_file` (empty file; select it anyway) |

### Pathway XML Files

| Field | Value |
|---|---|
| Select input type | **XML File of Merged KEGG Pathways** |
| XML File of Merged KEGG Pathways | `inputs/dummy_master_graph.xml` |
| Node Split Threshold | `1` |

The graph is already merged, so choosing this option skips the KEGG merge step.
Do **not** pick "List of KEGG XML Files to Merge".

### Regulatory thresholds

| Field | Value |
|---|---|
| Log fold changes file | `inputs/egf_hela_t120_lfc.tsv` |
| Up Threshold | `1.01` |
| Down Threshold | `-1.01` |

After entering the thresholds, press **Filter** on this panel. The thresholds are
only read when you do; clicking `Next` alone leaves the previous values in place.

### Reach path bound

| Field | Value |
|---|---|
| Maximum Signalling Path Length | `10` |
| Final Edges in pathways restricted to | **No restriction** |

A smaller bound is not a milder version of this run. It prunes the reachable graph,
which in turn clamps the relaxation bound, and the frontier collapses to a single
point. Use 10.

### Relaxation bounds

| Field | Value |
|---|---|
| Node relaxation | lower `0`, upper `7` |
| Edge relaxation | lower `0`, upper `7` |

### Edge files

| Field | Value |
|---|---|
| Additional Edges File | leave empty |
| Essential Edge File | leave empty |
| Avoid Edge File | leave empty |
| Relaxed Edges File | leave empty |
| Non Relaxed Edges File | leave empty |
| Coexpression CSV File | `inputs/coexpression_links.csv` |
| Coexpression Threshold (COEXP_THRESH) | `700` |
| Frozen Threshold (FROZEN_THRESH) | `800` |

The coexpression CSV is optional: leaving it empty simply loads no coexpression
pairs, and the frozen-edge constraints still apply. This example needs it set, so
that both constraint families are exercised.

### Backend solver configuration

| Field | Value |
|---|---|
| Increment Solver Timeout | `600000` |
| Overall Solver Timeout | `3000000` |
| Solutions to Count | `1` |
| Solutions to Explore | `1` |

### Not exposed in the GUI

`NEW_CONSTRAINTS_ENABLED` (1) and `Z3_RANDOM_SEED` (42) are fixed defaults in
`advtempscript.txt`. They match the reference run.

> Before running, open the generated `macOS/output_script.txt` and confirm your
> values landed. It is a plain substitution of the table above into the template,
> so it is the definitive record of what the backend will receive.

The frontend writes results to a directory named after the run, e.g.
`$WORK_DIR/hsa1956_to_hsa3576_fsc_hsa3725_egf_hela_t120_lfc.tsv_b10/`, rather than
to `run/` as the batch script does.

---

## Expected result

Both routes produce the same two Pareto frontiers (node relaxations, edge
relaxations):

```
with JUN     : (2,0) (1,1) (0,2)          frontier cost 2
without JUN  : (3,0) (2,1) (1,2) (0,3)    frontier cost 3
```

plus an overlaid plot, `PO_hsa1956_to_hsa3576_fsc_hsa3725_..._b10.png`.

Excluding JUN makes the explanation uniformly one relaxation worse. That gap is
the signature of JUN's functional significance.

The log should show the new constraints engaging, twice (once per solve):

```
Loaded 7 coexpression pairs from .../coexpression_links.csv at threshold > 700.00
[coexpression] emitted 7 clauses
[frozen-edge]  emitted 35 clauses
[new-constraints] enabled=1 z3_random_seed=42
```

The solve takes roughly a second.

---

## Troubleshooting

**`mkdir: /ABSOLUTE: Read-only file system`**
You did not edit `$WORK_DIR`. See Step 1.

**Exit 0, `.dat` files written, but no `.png`**
`$WORK_DIR` is a relative path, or `gnuplot` is missing.

**The frontier collapses to a single point, and the without-JUN file is empty**
`Maximum Signalling Path Length` is not 10. Check `output_script.txt` for the
value of `$REACH_PATH_BOUND`.

**`[new-constraints] disabled`**
`$NEW_CONSTRAINTS_ENABLED` is 0, or the script predates the coexpression support
and does not pass the trailing `pathz3` arguments at all. An empty coexpression
CSV does *not* cause this.

**`[coexpression] emitted 0 clauses`**
No coexpression CSV was given, or none of its pairs scored above
`$COEXP_THRESH`. The frozen-edge constraints are unaffected.
