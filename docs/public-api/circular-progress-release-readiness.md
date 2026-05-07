# Circular Progress Indicator release readiness

`QtMaterialCircularProgressIndicator` is release-ready when it exposes the same functional contract as the linear progress indicator:

- determinate and indeterminate modes;
- clamped `value` in the `[0, 1]` interval;
- `QtMaterialAsyncState` integration;
- status text synchronization;
- accessible name and accessible value text;
- stable size hints and spec round-trip behavior.

## Accessibility contract

The widget sets a default accessible name of `Progress indicator`.

For determinate progress, `accessibleValueText()` returns the status text plus a rounded percentage when status text is available:

```text
Uploading, 25%
```

Without status text, the accessible value is only the percentage:

```text
25%
```

For indeterminate progress, the accessible value is the status text when available, otherwise `In progress`.

The same text is mirrored into `accessibleDescription()` so platform accessibility bridges have a useful fallback even without a custom interface implementation.
