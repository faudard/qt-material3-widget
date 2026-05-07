# Menu release readiness

`QtMaterialMenu` is a lightweight Material-style menu widget for action lists.

The usable contract is intentionally small:

- action items and separators;
- disabled items skipped by keyboard navigation;
- optional checkable items;
- mouse activation;
- keyboard navigation with Up, Down, Left, Right, Home and End;
- activation with Space, Return and Enter;
- Escape dismissal;
- accessible name and description summary;
- RTL-aware icon/check/text placement.

The widget is not intended to replace every advanced `QMenu` feature yet. Submenus,
section headers, shortcuts and embedded custom widgets should remain future work.
