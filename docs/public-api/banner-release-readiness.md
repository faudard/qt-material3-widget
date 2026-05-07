# Banner release readiness

`QtMaterialBanner` is release-ready when it provides a predictable transient surface contract:

- title and body text are exposed through `titleText()` and `bodyText()`;
- optional primary and secondary actions are keyboard-focusable `QToolButton` children;
- the dismiss action has a stable accessible name and object name;
- `Escape` dismisses only when both `isDismissible()` and `dismissOnEscape()` are true;
- `accessibilitySummary()` includes title, body, actions and dismiss affordance;
- `sizeHint()` and `minimumSizeHint()` never return empty sizes.

Recommended usage:

```cpp
banner->setTitleText(tr("Network offline"));
banner->setBodyText(tr("Changes will sync when connectivity returns."));
banner->setPrimaryActionText(tr("Retry"));
banner->setSecondaryActionText(tr("Settings"));
banner->setDismissAccessibleName(tr("Dismiss network banner"));
```
