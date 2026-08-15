# QtMaterial3 Widgets Constitution

## Core Principles

### I. Layered Architecture Is Enforced
The dependency direction is `Foundation -> ThemeModel -> Specs -> Core/Effects -> Widgets`.
ThemeIO and ThemeRuntime depend on ThemeModel, never the reverse. Tooling, Testing and Integration are peripheral.
CMake targets and architecture checks MUST enforce these boundaries.

### II. ResolvedSpec-Only Rendering
Rendering MUST follow `ThemeContext + authored state + interaction state + density -> Resolver -> ResolvedSpec -> layout/render`.
Renderers and paint code MUST NOT query Theme, ThemeContext, ThemeManager or textual token keys.

### III. Theme Model / IO / Runtime Separation
ThemeModel owns typed design values and model invariants. ThemeIO owns serialization, migration and textual keys.
ThemeRuntime owns ThemeContext, ThemeManager/global fallback, observation and system propagation.
ThemeManager is optional convenience, not a hidden requirement for widget correctness.

### IV. Runtime Tokens Are Typed
Specs/runtime use typed token identifiers. Textual token names are confined to ThemeIO/tooling boundaries.
Each token has one owner, type/category, persistence mapping and tests.

### V. Material Conformance Is Versioned Evidence
Every conformance claim names a Material profile/reference version.
Qt desktop adaptations are explicitly classified. Structural, visual and motion evidence are separate.
QtMaterial3's own golden PNGs prove non-regression, not upstream Material conformance.

### VI. Compatibility Claims Must Be Executable
While Qt 5.14.2 is advertised, a real Qt 5.14.2/MSVC build/test gate is mandatory.
The Qt 6 minimum is explicit and tested. Compatibility shims are isolated.

### VII. Tests Are Part of the Feature
A regression fix includes a reproducer when possible.
A component marked `complete` has behavior, keyboard, accessibility, RTL, DPI, theme-switch, visual and conformance evidence as applicable.

### VIII. Accessibility and Keyboard Are Product Requirements
All interactive components are keyboard operable. QAccessible semantics, focus behavior, reduced motion and contrast are specified and tested.
Manual screen-reader audits are 1.0 release evidence.

### IX. Performance Is Measured
No theme lookup in hot paint paths. Global caches are bounded. Hidden widgets do not keep unnecessary animation/timers active.
Optimization requires benchmark/profile evidence.

### X. Public API Is Deliberate
Before 1.0, breaking changes are allowed when they improve architecture/API.
Public headers compile independently; visibility, lifetime and PIMPL/ABI impact are reviewed.
After 1.0, SemVer and the published ABI policy govern changes.

### XI. Architecture Debt Only Decreases
The architecture baseline is transitional debt. No change adds violations.
Fixed entries are removed in the same change. Release 0.5 requires baseline = 0, then strict mode becomes permanent.

### XII. One Source of Truth for Maturity
Component status is generated from one canonical registry.
`complete` is allowed only when the project Definition of Done is satisfied.

## Engineering Constraints

- C++17.
- CMake 3.21+ while advertised.
- Qt Widgets public surface.
- Qt 5.14.2 while advertised plus an explicit Qt 6 minimum.
- LGPL-3.0-only with third-party licence notices.

### Component Definition of Done
A public component marked complete has: reviewed API; header hygiene; authored spec/resolver/ResolvedSpec path;
no prohibited theme access; all applicable states; keyboard/a11y; RTL/DPI; light/dark/runtime theme switching;
visual regression; Material conformance evidence or explicit Qt-extension classification; lifecycle tests; docs/gallery/registry metadata.

### Material profiles
Material 3 Classic and future Material 3 Expressive are versioned separately. Expressive support MUST NOT silently change classic behavior.

## Development Workflow

For every Spec Kit feature:
1. specification defines observable outcome and scope;
2. clarification resolves material ambiguity;
3. plan records architecture, compatibility, API/ABI, Material, a11y and performance impacts;
4. tasks are atomic, path-specific and test-bearing;
5. `/speckit.analyze` must have no critical inconsistency before implementation;
6. implementation proceeds in dependency order with build/tests after coherent changes;
7. `/speckit.converge` checks the implementation against artifacts;
8. DONE requires success criteria and checklists.

## Governance
This constitution supersedes stale roadmap prose and informal habits. Amendments require rationale, impact/migration analysis, version increment and date.
Source code MUST NOT silently become the specification.

### XIII. Semantic Identity Survives Implementation Changes

Important tokens, components, variants, states, references and capabilities MUST have stable semantic identities independent of C++ class names, file names and accidental enum ordinals. Example: `material.button.filled` may remain stable even if an implementation class is renamed.

### XIV. Resolution Inputs Are Explicit

Resolvers MUST converge toward explicit input-only resolution:

`AuthoredSpec + ComponentState + DesignContext + EnvironmentContext -> ResolvedSpec`

Hidden singleton/global state is prohibited as a source of rendering semantics. Business/component state (checked, selected, expanded, error) MUST be distinguishable from interaction state (hovered, focused, pressed, dragged).

### XV. Structured Formats Version Independently

Theme schema, component registry, token registry, conformance reports, reference manifests, design-system IR, plugin manifests, inspection protocols and future component schemas MUST each carry an explicit format/protocol version. Their lifecycle MUST NOT be coupled blindly to the library version.

### XVI. Tooling Is Privacy-Aware by Default

Inspectors, traces, snapshots, diagnostics, screenshots and remote tooling MUST support sensitive/secret value classification and redaction. Production telemetry/network reporting is OFF by default. Secret content MUST never be exported merely because debug tooling is enabled.

### XVII. Capabilities Are Data, Not Scattered Preprocessor Policy

Platform, renderer, input, accessibility and backend capabilities SHOULD be represented as explicit data/capability sets used by resolution and integration layers. Platform `#ifdef` branches may exist at the lowest necessary implementation boundary, but must not become the design-system model.

### XVIII. Third-Party Integrations Are Adapters, Never Structural Dependencies

QtMaterial3 Core, Theme, Specs and Widgets MUST NOT depend structurally on third-party widget frameworks. Optional integration modules consume public/stable QtMaterial3 APIs and the third-party API. Each integration owns its compatibility matrix, diagnostics and styling adapter. Qt Advanced Docking System is the reference pilot for this architecture.

### XIX. Public APIs Are Binding-Aware

Every new public C++ API MUST be reviewed for Shiboken/PySide bindability. QObject/pointer ownership MUST be explicit (owned, borrowed, parent-owned, transferred). Public value types SHOULD have clear copy/move/equality/lifetime semantics. Complex templates SHOULD remain internal unless there is a compelling public reason.

### XX. Theme and Resolved Models Are Snapshot-Friendly

ThemeModel and other pure design data SHOULD be immutable-friendly and suitable for read-only worker-thread computation. QWidget/QPainter/QWindow pointers MUST NOT be embedded in ThemeModel or ResolvedSpec. GUI object mutation remains on the GUI thread.

### XXI. Resolution and Presentation Remain Separatable

The architecture MUST preserve a path toward:

`AuthoredSpec -> ResolvedSpec -> LayoutSpec -> RenderSpec`

without requiring all four layers to exist for every component immediately. ResolvedSpec MUST describe resolved semantics rather than carrying painter/widget implementation state.

### XXII. ComponentRegistry May Evolve into ComponentDefinition

The canonical component registry MUST remain extensible enough to describe identity, variants, properties, states, tokens, resolver, accessibility, tests, references, bindings, integrations and maturity. Generated inventories are preferred to duplicated manual lists.

### XXIII. Stable IDs Are Explicit

Persisted IDs, token IDs and semantic component IDs MUST have explicit stable values/mappings. Their persistence identity MUST NOT depend on declaration order or compiler-specific enum representation.

### XXIV. Time Is Injectable in Testable Systems

Motion, timeouts, debounce, tooltip/menu delays, snackbar lifetime and other temporal behavior SHOULD converge toward injectable clocks/schedulers so deterministic testing and frame seeking are possible.

### XXV. Zero Manual Drift Is a Long-Term Invariant

Structured truth has one canonical owner. Component existence, maturity, tokens, Material references, dependency contracts, public API manifests, documentation tables, gallery navigation and test matrices SHOULD be generated or validated from their canonical sources rather than independently edited.

### XXVI. Frontend Neutrality Is Preserved Below Presentation

Qt Widgets is the primary product frontend, but Foundation, ThemeModel, DesignSystem IR, semantic IDs and Material reference data MUST NOT acquire Qt Quick/QML dependencies merely to enable a future QML frontend. Frontends consume shared design semantics; they do not define them.

### XXVII. Native Platform Access Is Isolated

Native window/shell APIs and version-sensitive native interfaces MUST remain behind platform adapters. ThemeModel, Specs and generic component semantics MUST NOT depend directly on native platform handles or unstable native API contracts.

### XXVIII. Localization Has an Asset Lifecycle

Library-owned user-visible strings, accessibility labels and tooling text MUST have explicit translation ownership, extraction/build/package rules and completeness diagnostics when localization support is enabled. Locale behavior and translated-resource delivery are separate concerns and both require tests.

### XXIX. Host Styling Must Be Reversible and Non-Destructive

QtMaterial3 MUST define precedence/coexistence rules for QPalette, QStyle, QProxyStyle, Qt Style Sheets and third-party styling. Optional global application styling MUST be reversible; partial adoption MUST NOT require uncontrolled global side effects.

### XXX. Test Quantity Is Not Test Effectiveness

Coverage, requirement traceability, flakiness and—where practical—mutation/fault effectiveness SHOULD be measured. Automatic retries MUST NOT conceal deterministic regressions. A large green suite is not evidence of quality when it cannot demonstrate defect-detection capability.

### XXXI. Temporary Exceptions Are Explicit and Expiring

Architecture, compatibility, security and quality-rule exceptions MUST have a stable identifier, owner, reason, scope and expiry/review point. CI SHOULD detect expired exceptions. An exception MUST NOT silently become permanent baseline debt.

### XXXII. Research Does Not Imply Product Commitment

Experimental technologies and research branches MUST be classified through the project technology radar. Prototype code MUST NOT create a mandatory runtime/build dependency unless the capability is deliberately promoted through specification, constitution checks and milestone planning.

### XXXIII. Vision-to-Release Traceability Is Preserved

When a long-term capability enters active development, it MUST be traceable from Vision/Epic to milestone, feature specification, requirement, task, implementation evidence and release evidence. A feature is not considered delivered solely because matching code exists.

### XXXIV. Roadmap Identifiers Are Stable

Vision IDs and other persistent roadmap identifiers MUST NOT be renumbered for cosmetic reorganization. Superseded or abandoned IDs remain historically resolvable. New capabilities receive new identifiers or are nested beneath an existing capability family.


**Version**: 2.2.0 | **Ratified**: 2026-08-15 | **Last Amended**: 2026-08-15
