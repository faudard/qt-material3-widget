#!/usr/bin/env python3
"""Expose reviewed visual goldens as documentation assets without duplicating source images."""
from __future__ import annotations
import json, shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
rules=json.loads((ROOT/"tools/release_rules.json").read_text(encoding="utf-8"))
goldens=rules["base"]["stable_release"]["visual_goldens"]
dest=ROOT/"docs/_static/visuals"
dest.mkdir(parents=True,exist_ok=True)
for rel in goldens:
    src=ROOT/rel
    if not src.is_file():
        raise SystemExit("missing reviewed golden: "+rel)
    shutil.copy2(src,dest/src.name)
print("Prepared "+str(len(goldens))+" reviewed documentation visuals")
