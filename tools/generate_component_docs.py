#!/usr/bin/env python3
"""Generate consumer component pages from the canonical registry."""
from __future__ import annotations
import json, re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/"docs/widgets/components"
entries=[e for e in json.loads((ROOT/"docs/components/component-registry.json").read_text(encoding="utf-8")) if e.get("releaseScope") is True]
def slug(e): return re.sub(r"[^a-z0-9]+","-",e["id"].lower()).strip("-")
def bullets(v,f): return "\n".join("- "+str(x) for x in (v or [f]))
OUT.mkdir(parents=True,exist_ok=True)
pages=[]
for i,e in enumerate(entries):
    a=e.get("maturityAxes",{}); ev=a.get("evidence",{})
    guide="../../"+Path(e["docsPath"]).as_posix().removeprefix("docs/")
    lines=["# "+e["name"],"",
      "**Public type:** "+e["widgetType"],"**Header:** "+e["publicHeader"],"**Family:** "+e["family"],
      "**Maturity:** "+str(e.get("maturity","unknown")),"**Gallery route:** "+e["galleryRoute"],"",
      "## Screenshot / visual reference","",
      "; ".join(ev.get("rendering",[])) or "Use the Gallery route as the maintained visual preview.","",
      "The Gallery is the interactive source for light/dark, state and direction inspection. Reviewed deterministic goldens remain the release source of truth where available.","",
      "## When to use","",
      "Use this component for the "+e["name"]+" interaction in the "+e["family"]+" family. See the [canonical family guide]("+guide+") for behavioral detail and related components.","",
      "## API","","Installed header: "+e["publicHeader"],"",
      "Public type: `"+e["widgetType"]+"`. See the [generated C++ API reference](../../api/index.md) for Doxygen-rendered declarations that are part of the central API map.","",
      "## States","",bullets(ev.get("states"),"State behavior follows the family contract and native Qt semantics."),"",
      "## Keyboard","",bullets(ev.get("keyboard"),"Native Qt focus/navigation remains authoritative."),"",
      "## Accessibility","",bullets(ev.get("accessibility"),"Provide a visible label or accessible name and preserve meaningful state semantics."),"",
      "## RTL","",bullets(ev.get("rtl"),"Layout direction follows QWidget::layoutDirection() where directional geometry applies."),"",
      "## Example","","Open Gallery route "+e["galleryRoute"]+". Focused verification target: "+e["testTarget"]+". See the [examples guide](../../examples/index.md).","",
      "## Maturity evidence","",
      "- API: "+str(a.get("api","N/A")),"- Rendering: "+str(a.get("rendering","N/A")),"- States: "+str(a.get("states","N/A")),
      "- Accessibility: "+str(a.get("accessibility","N/A")),"- Keyboard: "+str(a.get("keyboard","N/A")),"- HiDPI: "+str(a.get("hidpi","N/A")),
      "- RTL: "+str(a.get("rtl","N/A")),"- Tests: "+str(a.get("tests","N/A")),"- Example: "+str(a.get("example","N/A")),"- Docs: "+str(a.get("docs","N/A")),""]
    if a.get("gaps"): lines += ["Known maturity gaps:","",bullets(a["gaps"],"None"),""]
    nav=[]
    if i: nav.append("[← "+entries[i-1]["name"]+"]("+slug(entries[i-1])+".md)")
    nav.append("[All components](index.md)")
    if i+1<len(entries): nav.append("["+entries[i+1]["name"]+" →]("+slug(entries[i+1])+".md)")
    lines += ["---","", " · ".join(nav),""]
    fn=slug(e)+".md"; (OUT/fn).write_text("\n".join(lines),encoding="utf-8"); pages.append((e,fn))
idx=["# Components","","Generated consumer pages for every release-scoped widget. Do not hand-edit generated pages.",""]
for fam in dict.fromkeys(e["family"] for e,_ in pages):
    idx += ["## "+fam,""]
    idx += ["- ["+e["name"]+"]("+fn+") — "+e["widgetType"] for e,fn in pages if e["family"]==fam]
    idx.append("")
(OUT/"index.md").write_text("\n".join(idx)+"\n",encoding="utf-8")
print("Generated "+str(len(pages))+" component pages")
