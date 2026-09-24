# ADR-001: Layering and Target Direction
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
Foundation -> ThemeModel -> Specs -> Core/Effects -> Widgets. ThemeIO and ThemeRuntime depend on ThemeModel, never reverse. Integration remains peripheral.

## Consequences
The decision is binding on new specs and architecture changes. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
CMake targets and architecture checks enforce the graph.
