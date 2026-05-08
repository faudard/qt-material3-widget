# Public widget header hygiene

This check is the post-PIMPL guardrail for public widget headers.

After a widget owns a `std::unique_ptr<QtMaterialFooPrivate> d_ptr`, the public
header should expose only the user-facing contract: constructors, properties,
signals, slots, virtual overrides and deliberately public value types. Rendering
caches, transition controllers, effects, child widgets, paths, timers, layout
objects and resolved specs belong in the private implementation.

## Rules

Public widget headers under `include/qtmaterial/widgets` should not include
implementation-heavy Qt headers such as:

- `QPainter`, `QPainterPath`, `QStyleOption`, `QPixmap`, `QImage`
- `QTimer`, `QPropertyAnimation`, animation groups
- child widgets such as `QLabel`, `QLineEdit`, `QCalendarWidget`, `QToolButton`
- layout objects such as `QVBoxLayout`, `QHBoxLayout`, `QGridLayout`
- internal implementation headers under `qtmaterial/effects`, `qtmaterial/specs`,
  `qtmaterial/core/internal` or `qtmaterial/widgets/private`

Acceptable public API value types include common Qt value classes such as
`QString`, `QColor`, `QIcon`, `QDate`, `QSize`, `QRect`, `QFont` and `QVariant`.

## Local commands

```bash
python3 tools/check_public_header_hygiene.py --root . --strict
python3 tools/check_public_header_hygiene.py --root . --dry-run
python3 tools/check_public_header_hygiene.py --root . --fix
```

`--fix` is intentionally conservative. It only removes obviously unused heavy
includes or replaces pointer/reference-only usages with forward declarations. It
will not rewrite public value APIs or rendering/detail exposure that requires a
human decision.

## CTest integration

The tranche adds two kinds of tests:

1. `public_widget_header_hygiene` — static audit of implementation-heavy includes.
2. `public_widget_header_self_contained/<header>` — one generated translation
   unit per public widget header, proving that each header is self-contained when
   included alone.

Run them with:

```bash
ctest --test-dir build -R public_widget_header --output-on-failure
```

## Temporary exception

If a public header intentionally keeps a normally banned include, add a narrow
comment next to the include:

```cpp
#include <QCalendarWidget> // QTMATERIAL3_PUBLIC_HEADER_HYGIENE_ALLOW(QCalendarWidget)
```

Use this only for real public API requirements, not for private state.
