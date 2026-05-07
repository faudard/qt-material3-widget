# BottomSheet release readiness

`QtMaterialBottomSheet` is promoted to the release-scope surface set when it supports the minimum behavior expected from a Material 3 bottom sheet:

- modal and non-modal usage;
- expanded and collapsed heights;
- Escape/Back dismissal policy;
- scrim/outside dismissal policy;
- drag-down collapse/dismiss affordance;
- initial focus and focus restoration;
- stable `contentWidget()` container for application content;
- accessible name/description derived from title, supporting text and sheet state.

## Public API

```cpp
QtMaterialBottomSheet sheet(parent);
sheet.setTitleText("Filters");
sheet.setSupportingText("3 options available");
sheet.setExpandedHeight(420);
sheet.setCollapsedHeight(96);
sheet.setDismissOnEscape(true);
sheet.setDismissOnScrim(true);
sheet.open();
```

Use `contentWidget()` as the container for controls shown inside the sheet:

```cpp
auto *layout = new QVBoxLayout(sheet.contentWidget());
layout->addWidget(new QPushButton("Apply", sheet.contentWidget()));
```

## Keyboard contract

- `Escape` / `Back`: closes the sheet when `dismissOnEscape` is enabled.
- `Ctrl+Down`: collapses the sheet, or dismisses it if already collapsed.
- `Ctrl+Up`: expands the sheet.

## Accessibility contract

The accessible description is synchronized with `accessibilitySummary()`, which includes:

- `titleText`, or a default `Bottom sheet` label;
- `supportingText`, when present;
- expanded/collapsed state;
- modal state.
