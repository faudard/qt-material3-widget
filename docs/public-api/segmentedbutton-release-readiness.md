# Segmented Button release readiness

`QtMaterialSegmentedButton` is now part of the public release scope.

## Contract

The component supports:

- single selection through `currentIndex`;
- optional multi-selection through `setMultiSelection(true)`;
- per-segment enabled state;
- keyboard navigation with Left/Right/Home/End;
- Space/Return/Enter activation;
- disabled segment skipping;
- accessible summary text for assistive technologies.

## Accessibility

Use meaningful segment labels. Icon-only segments should still have text or an explicit accessible label in a future icon-only extension.

The widget exposes:

```cpp
QString accessibilitySummary() const;
QString segmentAccessibleText(int index) const;
QString currentSegmentAccessibleText() const;
```

## Release checklist

Before marking the component complete rather than usable, add render baselines for:

- enabled / disabled segments;
- selected / unselected segments;
- focus ring;
- RTL navigation;
- high-density and compact-density layouts.
