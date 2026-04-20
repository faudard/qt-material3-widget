
# QtMaterialDateField Shell Checklist

Use this checklist when integrating the date-field shell into the main branch.

## Core shell
- [ ] Build `QtMaterialDateField` on the shared text-field shell path
- [ ] Keep parsing/formatting logic outside paint code
- [ ] Support empty date state cleanly
- [ ] Support parse-error state cleanly

## Prefix / suffix / affordances
- [ ] Route calendar affordance through shared suffix slot handling
- [ ] Route optional clear affordance through the same path
- [ ] Keep icon tint spec-driven

## Behavior
- [ ] Enter / Return commits parsing
- [ ] Esc clears transient popup focus if present
- [ ] `calendarRequested()` is emitted without requiring a picker implementation
- [ ] Focus forwards to the hosted `QLineEdit`

## Accessibility
- [ ] Accessible name follows label when unset
- [ ] Accessible description reflects error or current date text
- [ ] Date shell remains keyboard-correct

## Rendering
- [ ] No raw theme lookups in paint code
- [ ] Use shared text-field shell helper
- [ ] Cache shell geometry/layout as needed
- [ ] Add default + error render baselines

## Future extension
- [ ] Add locale-aware format strategy
- [ ] Add min/max date
- [ ] Add picker attachment helper
