# ADR-001: Layering and Target Direction
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 uses an enforceable layered architecture so theme, spec, runtime,
effects, and widget responsibilities remain explicit.

## Decision
Foundation -> ThemeModel -> Specs -> Core/Effects -> Widgets. ThemeIO and
ThemeRuntime depend on ThemeModel, never reverse. Integration remains
peripheral.

## Consequences
The decision is binding on new specs and architecture changes. Breaking cleanup
remains acceptable before 1.0 when required to preserve the target architecture.

## Verification
CMake targets and architecture checks enforce the graph.
