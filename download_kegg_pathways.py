#!/usr/bin/env python3
"""
Download 163 KEGG pathway KGML XML files into hsa_pathways/ using the KEGG REST API.

KEGG REST API endpoint: https://rest.kegg.jp/get/{pathway_id}/kgml
Rate limit: KEGG allows ~10 requests per second for academic use; we use a small
delay to be polite.

Usage:
    python download_kegg_pathways.py
"""

import os
import time
import urllib.request
import urllib.error

PATHWAY_IDS = [
    "hsa03320", "hsa03460", "hsa04010", "hsa04012", "hsa04014", "hsa04015",
    "hsa04020", "hsa04022", "hsa04024", "hsa04060", "hsa04062", "hsa04064",
    "hsa04066", "hsa04068", "hsa04080", "hsa04110", "hsa04114", "hsa04115",
    "hsa04122", "hsa04130", "hsa04140", "hsa04141", "hsa04150", "hsa04151",
    "hsa04152", "hsa04210", "hsa04261", "hsa04270", "hsa04310", "hsa04330",
    "hsa04340", "hsa04350", "hsa04360", "hsa04370", "hsa04380", "hsa04390",
    "hsa04510", "hsa04512", "hsa04514", "hsa04520", "hsa04530", "hsa04540",
    "hsa04550", "hsa04610", "hsa04611", "hsa04612", "hsa04620", "hsa04621",
    "hsa04622", "hsa04623", "hsa04630", "hsa04650", "hsa04660", "hsa04662",
    "hsa04664", "hsa04666", "hsa04668", "hsa04670", "hsa04672", "hsa04710",
    "hsa04713", "hsa04720", "hsa04722", "hsa04723", "hsa04724", "hsa04725",
    "hsa04726", "hsa04727", "hsa04728", "hsa04730", "hsa04740", "hsa04742",
    "hsa04744", "hsa04750", "hsa04810", "hsa04910", "hsa04911", "hsa04912",
    "hsa04913", "hsa04914", "hsa04915", "hsa04916", "hsa04917", "hsa04918",
    "hsa04919", "hsa04920", "hsa04921", "hsa04930", "hsa04932", "hsa04940",
    "hsa04950", "hsa04960", "hsa04961", "hsa04962", "hsa04970", "hsa04971",
    "hsa04972", "hsa04973", "hsa04974", "hsa04976", "hsa04978", "hsa05010",
    "hsa05012", "hsa05014", "hsa05016", "hsa05020", "hsa05030", "hsa05031",
    "hsa05032", "hsa05033", "hsa05034", "hsa05100", "hsa05110", "hsa05120",
    "hsa05130", "hsa05131", "hsa05132", "hsa05133", "hsa05134", "hsa05140",
    "hsa05142", "hsa05143", "hsa05144", "hsa05145", "hsa05146", "hsa05150",
    "hsa05152", "hsa05160", "hsa05161", "hsa05162", "hsa05164", "hsa05166",
    "hsa05168", "hsa05169", "hsa05200", "hsa05202", "hsa05203", "hsa05204",
    "hsa05205", "hsa05206", "hsa05210", "hsa05211", "hsa05212", "hsa05213",
    "hsa05214", "hsa05215", "hsa05216", "hsa05217", "hsa05218", "hsa05219",
    "hsa05220", "hsa05221", "hsa05222", "hsa05223", "hsa05310", "hsa05320",
    "hsa05321", "hsa05322", "hsa05330", "hsa05332", "hsa05412", "hsa05414",
    "hsa05416",
]

KEGG_API_URL = "https://rest.kegg.jp/get/{pathway_id}/kgml"
OUTPUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hsa_pathways")
DELAY_SECONDS = 0.4  # ~2.5 req/s, conservative to avoid rate limiting
MAX_RETRIES = 3


def download_one(pathway_id: str, output_dir: str) -> bool:
    """Download a single KGML file. Returns True on success."""
    url = KEGG_API_URL.format(pathway_id=pathway_id)
    out_path = os.path.join(output_dir, f"{pathway_id}.xml")

    if os.path.exists(out_path) and os.path.getsize(out_path) > 100:
        return True  # already downloaded

    for attempt in range(1, MAX_RETRIES + 1):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "Python/KEGG-downloader"})
            with urllib.request.urlopen(req, timeout=30) as resp:
                data = resp.read()
            with open(out_path, "wb") as f:
                f.write(data)
            return True
        except (urllib.error.HTTPError, urllib.error.URLError, TimeoutError) as e:
            print(f"  Attempt {attempt}/{MAX_RETRIES} failed for {pathway_id}: {e}")
            if attempt < MAX_RETRIES:
                time.sleep(2 * attempt)  # back off
    return False


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    total = len(PATHWAY_IDS)
    print(f"Downloading {total} KEGG pathway KGML files to {OUTPUT_DIR}/")

    # Count already-downloaded files to skip
    already = sum(
        1 for pid in PATHWAY_IDS
        if os.path.exists(os.path.join(OUTPUT_DIR, f"{pid}.xml"))
        and os.path.getsize(os.path.join(OUTPUT_DIR, f"{pid}.xml")) > 100
    )
    if already:
        print(f"  {already} already downloaded, skipping those.")

    succeeded = 0
    failed = []
    for i, pid in enumerate(PATHWAY_IDS, start=1):
        out_path = os.path.join(OUTPUT_DIR, f"{pid}.xml")
        if os.path.exists(out_path) and os.path.getsize(out_path) > 100:
            succeeded += 1
            continue

        print(f"[{i}/{total}] Downloading {pid}...", end=" ", flush=True)
        if download_one(pid, OUTPUT_DIR):
            print("OK")
            succeeded += 1
        else:
            print("FAILED")
            failed.append(pid)

        time.sleep(DELAY_SECONDS)

    print(f"\nDone: {succeeded}/{total} succeeded.")
    if failed:
        print(f"Failed ({len(failed)}): {', '.join(failed)}")


if __name__ == "__main__":
    main()
