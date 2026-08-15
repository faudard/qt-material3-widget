# Automation metadata contract

Every public widget should expose stable Qt properties for UI automation.

The standard properties are:

| Property | Purpose |
| --- | --- |
| `materialComponent` | Stable component family, for example `Button`, `TextField`, `Dialog`. |
| `materialVariant` | Stable component variant, for example `Filled`, `Outlined`, `Text`. |
| `materialRole` | Semantic role inside a composed widget, for example `leadingIcon`, `primaryAction`. |
| `materialTestId` | User/app-provided stable selector for tests. |
| `materialState` | Space-separated interaction state vocabulary. |

Use `QtMaterialAutomation` to set these properties on any `QObject`.

```cpp
QtMaterialAutomation::setComponent(button, QStringLiteral("Button"));
QtMaterialAutomation::setVariant(button, QStringLiteral("Filled"));
QtMaterialAutomation::setTestId(button, QStringLiteral("save-button"));
```

The `materialState` vocabulary is intentionally textual so Robot Framework, QAT, Squish, Appium-like bridges, and diagnostic tools can inspect it without linking against C++ enums.

Common values are:

```text
enabled disabled hovered focused pressed checkable checked selected error readOnly indeterminate dragged busy expanded invalid
```
