# FuSION model example: EGFR -> CXCL8, FSC = JUN

A small, self-contained FuSION run that demonstrates functional-significance
testing of a candidate node (FSC). It asks: how significant is JUN (hsa3725)
for explaining the EGFR (hsa1956) -> CXCL8/IL8 (hsa3576) signalling response in
the HeLa EGF-induction dataset?

Two solves are run and their Pareto-optimal (PO) relaxation frontiers are
overlaid:

- with FSC: JUN forced active/essential
- without FSC: JUN forbidden (avoided)

If excluding JUN makes the explanation strictly harder (its PO curve sits
above the with-JUN curve), JUN is significant.

## Layout

```
model_example/
  run_model_example.batch     filled-out template script (all $fields set)
  inputs/
    dummy_master_graph.xml     reduced KEGG master graph (see below)
    egf_hela_t120_lfc.tsv      per-gene log2 fold changes (HeLa EGF, t=120)
    coexpression_links.csv     StringDB coexpression links among these genes
    hsa_to_gene_symbol_map.txt hsa id -> gene symbol (subset)
    hsa_path_to_path_name_map.txt  pathway id -> name (subset)
    hsa_ids_not_to_be_merged.txt
    inter_db_mapping_file      (empty, kept for the pipeline interface)
  run/                         generated on each run (not version controlled)
```

## Running

```
cd macOS/model_example
mkdir -p run && cd run
../../fusion -b ../run_model_example.batch    # or Linux/fusion for this box
```

Outputs land in `run/`:

- `hsa1956_to_hsa3576_w_hsa3725_..._b10.dat`  PO points, with JUN
- `hsa1956_to_hsa3576_wo_hsa3725_..._b10.dat` PO points, without JUN
- `PO_hsa1956_to_hsa3576_fsc_hsa3725_..._b10.png` overlaid Pareto plot
- `temp_w_solutions.txt` / `temp_wo_solutions.txt` explanation graphs
- `temp_w_relaxations.txt` / `temp_wo_relaxations.txt` relaxation witnesses

## Expected result

Two clearly separated frontiers (node-relaxns, edge-relaxns):

```
with JUN     : (2,0) (1,1) (0,2)          frontier cost 2
without JUN  : (3,0) (2,1) (1,2) (0,3)    frontier cost 3
```

Without JUN the frontier is uniformly one relaxation worse: the only cheap
route to CXCL8 runs through JUN, so forbidding it forces the FOS branch, which
drags in the extra up-regulated gene FOSB (hsa2354) as an unsatisfied node
(+1 relaxation). That gap is the signature of JUN's significance.

## The reduced ("dummy") master graph

`inputs/dummy_master_graph.xml` is NOT the full KEGG master. It is the induced
subgraph on the union of nodes and edges appearing in a completed full-scale
run's explanation witnesses (both the with-JUN and without-JUN solution graphs
AND their relaxation witnesses), taken from
`EGF_HeLa/experiment/egfr_to_il8_fsc_jun_b10_t700_f800_nr7_er7_on`.

- 125 gene entries / 479 relations (versus ~3600 / ~7500 in the full master).
- The original `experiment_score` on every relation is preserved, so the
  frozen-edge constraints (score >= 800) behave exactly as in the full run.
- Because the reduced graph contains every PO point's witness for both arms,
  the solve reproduces the full run's asymmetry (without-JUN one relaxation
  worse) while finishing in about a second.

Parameters mirror the reference run: reachability bound 10, up to 7 node and 7
edge relaxations, coexpression threshold 700, frozen threshold 800, new
constraints ON, up/down fold-change cutoffs +/-1.01.
