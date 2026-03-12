#!/usr/bin/env python3
"""
Extract edges from KEGG KGML pathway XML files into a single CSV.

Usage:
    # Single file:
    python kegg_edges_to_csv.py --input hsa_pathways/hsa03320.xml --output edges.csv

    # Batch (directory of XMLs):
    python kegg_edges_to_csv.py --input hsa_pathways/ --output all_edges.csv
"""

import argparse
import glob
import os
import xml.etree.ElementTree as ET
from collections import OrderedDict

import pandas as pd


def parse_one_pathway(xml_path: str) -> list[dict]:
    """Parse a single KEGG KGML file and return a list of edge row-dicts."""
    tree = ET.parse(xml_path)
    root = tree.getroot()

    # Extract pathway_id from <pathway number="..."> or from filename
    pathway_id = root.get("number", "")
    if pathway_id:
        pathway_id = f"hsa{pathway_id}"
    else:
        pathway_id = os.path.splitext(os.path.basename(xml_path))[0]

    # Build entry lookup: id -> (entry_id, kegg_id, symbol, entry_type)
    # The entry 'id' attribute is the unique identifier within this XML file.
    entries: dict[str, tuple[str, str, str, str]] = {}
    for entry in root.findall("entry"):
        eid = entry.get("id", "")
        kegg_id = entry.get("name", "")
        entry_type = entry.get("type", "")

        graphics = entry.find("graphics")
        symbol = graphics.get("name", "") if graphics is not None else ""

        entries[eid] = (eid, kegg_id, symbol, entry_type)

    # Extract edges from <relation> elements
    rows: list[dict] = []
    for relation in root.findall("relation"):
        e1_id = relation.get("entry1", "")
        e2_id = relation.get("entry2", "")
        rel_type = relation.get("type", "")

        n1_entry_id, n1_kegg_id, n1_symbol, n1_type = entries.get(e1_id, ("", "", "", ""))
        n2_entry_id, n2_kegg_id, n2_symbol, n2_type = entries.get(e2_id, ("", "", "", ""))

        row: dict = OrderedDict()
        row["pathway_id"] = pathway_id
        row["node1_kegg_id"] = n1_kegg_id
        row["node1_symbol"] = n1_symbol
        row["node1_type"] = n1_type
        row["node1_entry_id"] = n1_entry_id
        row["node2_kegg_id"] = n2_kegg_id
        row["node2_symbol"] = n2_symbol
        row["node2_type"] = n2_type
        row["node2_entry_id"] = n2_entry_id
        row["relation_type"] = rel_type

        # Collect subtypes in order
        subtypes = relation.findall("subtype")
        for i, st in enumerate(subtypes, start=1):
            row[f"subtype_{i}_name"] = st.get("name", "")
            row[f"subtype_{i}_value"] = st.get("value", "")

        rows.append(row)

    return rows


def batch_process(xml_paths: list[str], output_csv: str) -> pd.DataFrame:
    """Parse multiple KEGG KGML files and write combined edges to CSV."""
    all_rows: list[dict] = []
    for path in xml_paths:
        all_rows.extend(parse_one_pathway(path))

    if not all_rows:
        print("No edges found in any input file.")
        return pd.DataFrame()

    df = pd.DataFrame(all_rows)

    # Determine ordered columns: fixed columns first, then subtype columns sorted
    fixed_cols = [
        "pathway_id",
        "node1_kegg_id", "node1_symbol", "node1_type", "node1_entry_id",
        "node2_kegg_id", "node2_symbol", "node2_type", "node2_entry_id",
        "relation_type",
    ]
    subtype_cols = sorted(
        [c for c in df.columns if c.startswith("subtype_")],
        key=lambda c: (int(c.split("_")[1]), c.split("_")[2]),  # sort by index, then name<value
    )
    df = df[fixed_cols + subtype_cols]

    df.to_csv(output_csv, index=False)
    print(f"Wrote {len(df)} edges from {len(xml_paths)} file(s) to {output_csv}")
    return df


def main():
    parser = argparse.ArgumentParser(
        description="Extract edges from KEGG KGML pathway XML(s) to CSV."
    )
    parser.add_argument(
        "--input", "-i", required=True,
        help="Path to a single KGML XML file or a directory of XML files.",
    )
    parser.add_argument(
        "--output", "-o", default="kegg_edges.csv",
        help="Output CSV file path (default: kegg_edges.csv).",
    )
    args = parser.parse_args()

    if os.path.isfile(args.input):
        xml_paths = [args.input]
    elif os.path.isdir(args.input):
        xml_paths = sorted(glob.glob(os.path.join(args.input, "*.xml")))
        if not xml_paths:
            print(f"No .xml files found in {args.input}")
            return
    else:
        print(f"Input path does not exist: {args.input}")
        return

    batch_process(xml_paths, args.output)


if __name__ == "__main__":
    main()
