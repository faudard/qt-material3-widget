# DateField release readiness

`QtMaterialDateField` is a Material text-field derivative for date input.

## Release contract

The widget is considered usable when it supports:

- date round-trip via `date()` / `setDate()`;
- display formatting via `displayFormat`;
- optional clear affordance;
- calendar request affordance through `calendarRequested()`;
- minimum and maximum date range validation;
- parse error reporting through `parseErrorChanged(bool)`;
- acceptable-state reporting through `dateAcceptableChanged(bool)`;
- accessible name and description synchronized with label, value, range and error.

## Validation behavior

Empty input is allowed unless the inherited TextField required validation is enabled.
A non-empty value must parse with the current `displayFormat`. If `minimumDate` or
`maximumDate` is set, a parsed value outside the range is rejected and exposed as
an error state.

## Accessibility

`accessibilitySummary()` combines the label, selected date, optional range and
current parse/range error. The same text is used as the accessible description so
screen-reader output matches the visual field state.
