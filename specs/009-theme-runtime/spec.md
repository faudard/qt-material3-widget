# Feature Specification: ThemeRuntime

**Status**: Ready for application/convergence  
**Milestone**: 0.5

Requirements:
- TR-FR-001 explicit -> inherited -> default context resolution.
- TR-FR-002 Core has no direct ThemeManager dependency.
- TR-FR-003 ThemeContextDefaults owns process-default fallback access.
- TR-FR-004 ParentChange and propagation recompute effective context.
- TR-FR-005 destroyed explicit context resumes inheritance/default.
- TR-FR-006 ThemeContext is the only revision authority.
- TR-FR-007 equal snapshots are no-ops.
- TR-FR-008 successful context update increments once.
- TR-FR-009 ThemeManager revision delegates to context.
- TR-FR-010 no ThemeManager fingerprint/duplicate revision.
- TR-FR-011 owner-thread mutation for context/manager.
- TR-FR-012 ThemeObserver observes context, not manager.
- TR-FR-013 ThemeRuntime does not link ThemeIO.
- TR-FR-014 SystemTheme has no textual ThemePreference conversion API.
- TR-FR-015 SystemTheme auto-apply remains opt-in.
- TR-FR-016 runtime checker/tests are repository-health gates.
- TR-FR-017 Qt5.14.2/Qt6 shared/static remain mandatory.
