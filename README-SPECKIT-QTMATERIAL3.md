# QtMaterial3 + GitHub Spec Kit bootstrap

This overlay converts the QtMaterial3 master plan into an executable Spec-Driven Development program.

## Included

- `.specify/memory/constitution.md`: project engineering constitution.
- `.specify/templates/overrides/`: Qt/C++/Material-aware Spec Kit templates.
- `specs/PROGRAM.md`: complete capability catalog and dependency order.
- `specs/001-*` through `specs/011-*`: seeded 0.5 specs with plans, tasks, research, contracts/checklists.
- `docs/adr/`: durable architecture decisions.
- `apply_qtm3_speckit_overlay.py`: safe installer with backups.

## Initialize Spec Kit in the existing repository

From the `qt-material3-widget` repository root:

```powershell
specify version
specify self check
specify init --here --integration copilot --script ps
```

Use another integration if desired (`specify integration list`).

Then apply this overlay:

```powershell
python apply_qtm3_speckit_overlay.py --repo E:\Project\qt-material3-widget --dry-run
python apply_qtm3_speckit_overlay.py --repo E:\Project\qt-material3-widget --apply
```

## 0.5 execution order

1. 001 repository health and CI
2. 002 component registry source of truth
3. 003 build and consumer matrix
4. 004 Qt 5 / Qt 6 compatibility contract
5. 005 remove legacy SpecFactory
6. 006 theme target decomposition
7. 007 ThemeModel
8. 008 ThemeIO
9. 009 ThemeRuntime
10. 010 typed token system
11. 011 architecture zero debt

For each feature, use the normal Spec Kit loop:

```text
/speckit.clarify
/speckit.plan
/speckit.tasks
/speckit.analyze
/speckit.implement
/speckit.converge
```

The seeded artifacts are a brownfield starting point. If the live repository has changed, update the spec/plan/tasks first, rerun analyze, then implement.

References:
- https://github.com/github/spec-kit
- https://github.com/faudard/qt-material3-widget
- https://m3.material.io/


## Ultimate vision files

- `specs/PROGRAM-ULTIMATE.md`: exhaustive long-term capability/spec vision discussed through the 1500 conceptual gate.
- `specs/epics/python-sdk.md`: Python/PySide6-Shiboken vision.
- `specs/epics/external-integrations/qt-advanced-docking-system.md`: ADS pilot integration epic.
- `.specify/memory/constitution.md`: amended constitution including semantic IDs, explicit contexts, privacy, binding-awareness and third-party-adapter rules.
- `docs/roadmap/qtmaterial3_master_plan_exhaustif.docx`: archived 628-task master plan.

The 001-011 specs remain the first implementation tranche. Do not start the visionary epics before their dependencies/milestones justify them.
