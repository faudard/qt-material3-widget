# Qt Material 3 Widgets — proposed roadmap 1.22.0 to 2.0.0

> **Status: proposal, not a release announcement.** This document extends [ROADMAP.md](../ROADMAP.md); the existing 0.x–1.21 commitments remain unchanged.
>
> **Authoritative maturity data:** [component registry](components/component-registry.json). On 2026-10-22, `STATUS.md` reported 60 tracked components, 43 `complete` and 17 `usable`. These are a dated baseline, not guaranteed current figures.
>
> **Versioning principle:** the 1.x line retains its documented installed public-header source-compatibility policy. Breaking public changes, if justified, are reserved for 2.0. Release numbers describe intended milestones, not dates or completion claims.

## Product vision

Make Qt Material 3 a production-ready **Qt Widgets desktop SDK** with five complementary entry points:

1. **C++ Widgets:** a coherent Material 3 component catalogue on top of native Qt ownership and Model/View contracts.
2. **Existing Qt Widgets applications:** opt-in visual adaptation without replacing standard widget classes or signals.
3. **Qt Designer:** persistence-safe authoring of Material widgets and supported native adaptation policies in `.ui` files.
4. **Visual UI Studio:** visual composition, versioned interchange and audited export to `.ui` or C++.
5. **Python / PySide6:** supported Qt 6 bindings with explicit ABI, ownership and packaging contracts.

Screenshot-based UI reconstruction is a separately gated **experimental** authoring input. It must not become a prerequisite for a stable runtime or 2.0 release.

## Current baseline and non-negotiable constraints

- Published README currently labels **1.0.0 stable**, while `main` also carries merged work from later roadmap initiatives; merge state alone does not imply a tagged release or completed certification.
- Native C++ minimums for 1.x remain **Qt 5.14.2**, **Qt 6.4+**, **C++17** and the existing CMake floor. The exact test/compiler expectations come from `docs/compatibility/qt-support.json` and the compatibility policy. Do not silently drop the Windows MSVC v142/MSVC 14.28 reference contract.
- PySide6/Shiboken6 is Qt 6-only and has its own qualified Python/Qt/wheel matrix.
- `docs/components/component-registry.json`, not prose, decides maturity and release scope.
- Installed package consumers, public-header audits, sanitizer builds, reduced motion, keyboard behavior, RTL, HiDPI and accessibility remain release gates.
- Application code keeps ownership of models, delegates, proxies, validators, menu actions and business commands. Visual tools must not silently take ownership of them.
- Designers and editors are optional tooling; normal runtime consumers must not depend on the Qt Designer development module or UI Studio.
- Use the existing theme/spec/interaction infrastructure rather than introducing competing renderers or parallel token systems.

## Phase plan and dependencies

| Phase | Versions | Outcome | Relationship to 2.0 |
| --- | --- | --- | --- |
| Quality and foundations | 1.22–1.24 | Certified catalogue, consistent visuals, reliable native adaptation | Mandatory |
| Visual authoring | 1.25–1.27 | Designer 4.0, UI Studio and experimental screenshot import | Stable authoring subset mandatory only if advertised in 2.0; screenshot import optional |
| Desktop ecosystem | 1.28–1.29 | Python SDK and complex desktop workflows | Qualified advertised surfaces mandatory |
| Release readiness | 1.30 | Packages, API audit, performance, migration and RC | Mandatory |
| Major release | 2.0.0 | Versioned stable API and supported toolchain matrix | Final release gate |

**Critical path:** 1.22 -> 1.23 -> 1.24 -> 1.30 -> 2.0.

**Parallel paths:** 1.24 -> 1.25 -> 1.26 -> 1.27; 1.24 -> 1.28; 1.22 -> 1.29. Experimental 1.27 must not block 2.0. Higher-numbered versions do not imply that parallel development starts only after all earlier features are complete.

## 1.22.0 — Quality & Certification 2.0 [P0]

**Goal:** close the 17 `usable` component maturity gaps with real platform and rendering evidence, instead of declaring implemented controls complete on the strength of unit tests alone.

### 1.22.A — Native accessibility certification

- Record actual **NVDA / Windows**, **Orca / Linux**, and **VoiceOver / macOS** traversal, state announcements, activation and focus evidence for Navigation Rail, Tabs, Menu, Breadcrumb, Command Palette and Split View.
- Finish native AT verification for Navigation Bar, Side Sheet, Tooltip, Badge, Adaptive Shell and Navigation Suite, including destination focus, modal containment/restoration, announcements and responsive navigation transitions.
- Certify Split Button, Button Group, Floating Toolbar, Loading Indicator and Segmented List with group semantics, busy-state announcements, keyboard-only operation and appropriate reduced-motion behavior.
- Keep assistive-technology evidence separate from `QAccessible` contract tests: the latter do not prove native spoken output.
- Record known failures and OS-specific limitations explicitly, with owners and reproductions; do not auto-promote maturity.

### 1.22.B — Approved deterministic visual references

- Review the existing `missing_material3_matrix` and `adaptive_desktop_*` candidates using pinned Qt 6.4/Fusion, repeatable independent renders and renderer provenance.
- Approve the documented adaptive 30-reference matrix and missing-M3 light/dark/high-contrast evidence; cover relevant interaction states, RTL, and DPR 1x/2x.
- Extend reviewed Expressive references to the Split Button, Button Group, Floating Toolbar, Loading Indicator and Segmented List.
- Distinguish candidate capture, reviewer approval and checked-in goldens in the certification logs.

### 1.22.C — Release health dashboard

- Add a derived status overview showing per-component remaining gates and provenance, never a manually maintained maturity count.
- Re-run registry-to-doc generation, installed-header/API audits, consumer builds, CTest, sanitizers and packaging on the supported matrix.
- Update `STATUS.md` and generated maturity docs only through their canonical generators.

**Release gate:** 60/60 `complete` **only after** all required visual and native AT evidence exists; no critical matrix failures or forged certification. If native AT equipment is unavailable, ship a truthful partial-certification status instead of claiming completion.

**Dependencies:** existing 1.5/1.7/1.9/1.10/1.11 certification fixtures and ledgers. **Deliverable:** audit-ready component certification.

## 1.23.0 — Material 3 Visual Consistency [P0]

**Goal:** unify appearance and state behavior without changing the established default look of existing 1.x applications.

### 1.23.A — Tokens/specs audit

- Audit shared color roles, typography, shape, elevation, density, focus and motion tokens across the complete catalogue.
- Remove duplicated or local constants when a canonical spec/token resolver already exists; keep component-specific behavior explicit.
- Ensure `ThemeContext` propagation is deterministic, avoids redundant revisions and does not depend on a process-global singleton where per-window contexts are required.

### 1.23.B — State and geometry review

- Validate Filled/Outlined/Tonal/Elevated/Expressive variants and semantic size/shape selections where supported.
- Fix actual truncation, minimum-size, clipping, popup-placement and keyboard focus visibility defects found in the Gallery.
- Exercise hot theme switching, Font/DPI changes, RTL, hover/press/selected/disabled/error and reduced-motion final states.
- Verify adapted native controls and first-class Material widgets use the same tokens while retaining their distinct native interaction contracts.

### 1.23.C — Visual QA tooling

- Make each reviewed Gallery route reproducible in visual tests; include state capture and diagnostics rather than screenshots with unverifiable parameters.
- Run deterministic regression checks for 1x/2x and theme variants; keep new visual references review-gated.

**Release gate:** all previously approved goldens remain valid or receive explicit reviewed updates; no unresolved high-severity visual or input regressions. **Dependency:** 1.22 evidence baseline.

## 1.24.0 — Native Runtime 3.0 [P0]

**Goal:** certify progressive adoption of Material styling in existing Qt Widgets applications.

### 1.24.A — Native adapter consolidation

- Consolidate the already supported `QPushButton`, `QToolButton`, `QCheckBox`, `QRadioButton`, `QSlider`, `QComboBox`, `QLineEdit` and `QProgressBar` adaptation paths.
- Preserve original Qt type, signals, models, validators, delegates, popup behavior, action ownership and normal keyboard/pointer semantics.
- Support idempotent apply/remove, per-widget opt-out, density/variant overrides and restoration of the pre-existing style.
- Do not implement a fake custom combo popup or silently promise feature parity with dedicated `QtMaterial*` controls.

### 1.24.B — Live theme updates and object lifetime

- Qualify the opt-in live form watcher for dynamic insertion/reparenting/deletion and multiple independent `ThemeContext` instances.
- Avoid recursive adaptation of internal implementation children and first-class Material widgets.
- Exercise rapid theme changes and repeated lifetime churn under ASan/UBSan and the existing leak/stress harnesses.

### 1.24.C — Adoption reference application

- Ship a realistic native Qt Widgets app progressively adapted via C++ and typed `.ui` dynamic properties.
- Test `applyDeclaredToDescendants()`, opt-out and reapplication across Windows Qt 5.14.2 and Qt 6 platforms.
- Document where full first-class Material behavior requires replacing a native control.

**Release gate:** no change to native application-owned behavior, no accumulated state/leaks, stable undo of adaptation, passing Qt 5/6 consumer contracts. **Dependencies:** 1.23 visual contracts and existing 1.18–1.19 native declarations.

## 1.25.0 — Qt Designer 4.0 [P1]

**Goal:** move from an extended widget palette to reliable, reversible Material authoring in actual Qt Designer hosts.

### 1.25.A — High-value property editing

- Extend the existing Designer 3.0 QColor/token/enum/collection editors only where the underlying `Q_PROPERTY` persistence contract is proven.
- Improve editing of widget variants, semantic size, density, serialized label collections and defaults.
- Preserve native form cursor undo semantics, Reset versus Remove declarations and reversible temporary theme previews.

### 1.25.B — Templates and containers

- Provide editable form templates for settings, login, dashboard, data explorer and responsive navigation.
- Add AUTOUIC samples that preserve custom container slots and supported dynamic properties.
- Make unsupported/unserializable configurations visible instead of serializing runtime-only state.

### 1.25.C — Real-host qualification

- Test actual Designer save/reopen and Undo/Redo workflows on representative Qt 5 and Qt 6 hosts.
- Automate host integration only with an actual host-runner; do not relabel standalone QWidget tests as native Designer certification.
- Retain the `UiPlugin`-only fallback where Qt Designer development APIs are unavailable.

**Release gate:** supported templates round-trip Designer -> `.ui` -> UIC -> runtime, without hand-editing XML, and real-host manual evidence is recorded. **Dependency:** 1.24 declaration/runtime stability.

## 1.26.0 — Material Visual UI Studio 1.0 [P1]

**Goal:** build an **optional** visual composer for Material 3 desktop screens, complementary to Qt Designer.

### 1.26.A — Composition workspace

- Canvas selection, resize, drag, guides, snap, zoom/pan, widget tree, layout inspector and undo/redo.
- Theme/variant/density switching and Compact/Medium/Expanded responsive preview with RTL and DPR simulations.
- Palette backed by the actual component registry so widgets and properties are not advertised without runtime/export support.

### 1.26.B — Versioned intermediate UI schema

- Define a canonical versioned schema for components, layout hierarchy, properties, token references, stable element IDs and reusable compositions.
- Validate against a whitelist of supported serializable properties; fail closed on malformed or unknown required fields.
- Provide deterministic load/save, migrations and round-trip fixtures with minimal/no silent data loss.
- Clearly separate view description from application-owned models, bindings, signals and business logic.

### 1.26.C — Exporters

- Emit `.ui` **only** where UIC and supported Designer plugins can express the composition faithfully.
- For unsupported-but-valid runtime layouts, emit C++ Qt Widgets with explicit dependencies/CMake integration.
- Add generated-output compile tests, re-import tests for the supported subset and visual parity reports.
- Record unsupported properties with actionable diagnostics, never claim guaranteed pixel equality.

### 1.26.D — Reusable templates

- Support component presets, simple local template library, named styles and theme references.
- Include one example each of an adaptive dashboard, settings screen and form.

**Release gate:** create, save, reopen, edit, export, compile and run a representative project; deterministic serialization and no runtime dependency on Studio. **Dependencies:** 1.24 runtime and 1.25 Designer persistence contracts.

## 1.27.0 — Screenshot to Qt Widgets (experimental) [P2]

**Goal:** turn an imported image into a **reviewable, editable proposal**, not a claimed automatic lossless reverse-engineering tool.

### 1.27.A — Image-to-layout analysis

- Accept PNG/JPEG and identify candidate navigation regions, panels, controls, layout boundaries and text.
- Associate detected regions with a supported Material widget or mark them uncertain.
- Produce a draft of the **1.26 intermediate schema** with confidence and unresolved regions recorded.
- Keep the original image as a comparison asset.

### 1.27.B — Human correction and export

- Allow editing mistaken widget types, layout grouping, spacing and theme token choices before generation.
- Export only through the validated 1.26 pipelines; never bypass schema checks.
- Provide overlays/difference views and a measurable reference dataset for fidelity and editing effort.

### 1.27.C — Scope isolation

- Do not treat screenshot-to-QML output as natively equivalent to `QWidget`/`.ui`.
- Do not make external AI services, API keys or screenshot processing mandatory for the core SDK.
- Keep this feature behind an explicit experimental switch until reproducible quality benchmarks justify a stable claim.

**Release gate:** reference demos can import -> edit -> export -> compile; limitations and metrics are published. **Optional for 2.0.** **Dependency:** 1.26 schema/export.

## 1.28.0 — Python / PySide6 SDK 2.0 [P1]

**Goal:** provide an explicitly qualified Python application-development surface backed by native C++ behavior.

### 1.28.A — Coverage and API contracts

- Publish a C++/Python parity matrix; prioritize complete workflows, not a misleading 1:1 count of every C++ class.
- Extend bindings for common controls, navigation, themes, modal surfaces and desktop data as required by the chosen examples.
- Preserve `QObject` ownership, signal and non-owning model lifetimes; avoid a second Python implementation of the C++ behavior.

### 1.28.B — Developer experience

- Supply accurate Python typing/docs and end-to-end samples: navigation, settings, tables, dialogs and theme switching.
- Explain Qt 6-only expectations and ownership caveats.
- Stabilize public module layout and clear diagnostics for unsupported versions/configurations.

### 1.28.C — Wheel/ABI qualification

- Cross-OS wheel builds, ABI preflight and import tests across the explicitly certified Python/PySide6/Shiboken6 matrix.
- Stress QObject parenting, destruction, models, async navigation cancellation and stale-result handling.
- Do not add a Python version to the support claim without passing those gates.

**Release gate:** reference Python desktop app runs from the packaged wheel on every advertised OS/runtime; ownership and ABI tests remain green. **Dependency:** 1.24 native C++ stability.

## 1.29.0 — Desktop Productivity 3.0 [P1]

**Goal:** extend demanding desktop workflows while respecting the existing Qt Model/View boundaries.

### 1.29.A — Tables and trees

- Advanced multi-column sorting/filtering through application-owned proxy chains; do not replace external models.
- Restore versioned selection/header/expanded-node state safely after compatible model setup.
- Verify inline edits, delegates, context hooks, keyboard modifiers, drag/drop and lazy `fetchMore()` behavior.
- Preserve existing large-data performance commitments under realistic proxy chains.

### 1.29.B — Commands and workspaces

- Enrich Command Palette command grouping, favorites, recents, shortcuts and asynchronous cancellation.
- Keep provider/source-model lifetimes and application business commands external.
- Provide persistent dashboard, settings, explorer and multi-pane reference workspaces.

### 1.29.C — Accessibility and scale

- Certify keyboard focus restoration and accessible selection across dynamic content and layout changes.
- Benchmark with the checked-in 1.14/1.21 scale contracts as a non-regression baseline.

**Release gate:** examples and tests prove model-ownership preservation, restore safety, responsive large-data navigation and predictable async cancellation. **Dependency:** 1.22 qualification; can progress independently of Studio.

## 1.30.0 — Production SDK and 2.0 Readiness [P0]

**Goal:** close distribution, performance, migration and API quality gates ahead of a major release.

### 1.30.A — Packaging and consumers

- Verify installed CMake targets for `find_package`, `add_subdirectory`, FetchContent and shared/static consumption.
- Test relocatable release packages and optional Designer/Python/Studio boundaries.
- Promote Conan/vcpkg claims only after executable, compatible recipes exist and pass relocation/consumer tests.
- Ship example applications that build from installed packages, not only the source checkout.

### 1.30.B — Performance regression budgets

- Re-run the 1.21 million-row Table/Tree and lazy/proxy workloads, 100k Command Palette, theme storms and lifecycle churn.
- Track p95/p99, CPU, RSS trends and cache/memory findings against reproducible runner budgets.
- Make enforcement blocking on controlled scheduled/release lanes; keep noisy uncalibrated PR signals report-only.

### 1.30.C — Public API/migration review

- Inventory exported headers, symbols, schemas and Python APIs as stable, deprecated, experimental or internal.
- Identify proposed 2.0 breaking changes with rationale and migration replacements.
- Audit PIMPL/privacy boundaries, dependencies and QObject lifetime responsibilities.
- Document compatibility choices for Qt 6 baseline and Qt 5 1.x maintenance; do not raise compiler/Qt floors by accident.

### 1.30.D — RC validation

- Run full cross-platform consumers, documentation, sanitizers, reproducible packaging and application migration trials.
- Publish release notes and remaining known limits, including the independent experimental screenshot tool.

**Release gate:** a signed-off 2.0 compatibility proposal, complete migration trial, clean shipping matrix and no undocumented production dependencies. **Dependencies:** 1.22–1.24 mandatory; qualified subsets of 1.25/1.26/1.28/1.29 according to the advertised 2.0 scope.

## 2.0.0 — Qt Material 3 Desktop SDK [major]

**Goal:** a coherent, documented and distributable major release; release readiness is about contracts and verified workflows, not merely merging features.

### 2.0.A — Unified runtime surface

- Freeze consistent token/spec/interaction contracts for Core, Widgets and Native Adapter.
- Preserve native Qt models, delegates and commands as application-owned.
- Keep Designer, Studio and Python optional, with explicit dependency boundaries.

### 2.0.B — Public API v2 and Qt support decision

- Publish the exact supported Qt 6 minimum, toolchains, platforms and C++ standard after tests; **Qt 6-first is the strategic direction**, not an unreviewed minimum-version bump.
- If Qt 5 support is removed from 2.0, retain the final 1.x maintenance/support policy and provide a supported migration path.
- Publish header/signature inventory, schema versions, deprecations, documented intentional breaking changes and 1.x -> 2.0 migration recipes.
- Establish distinct guarantees for source compatibility and binary ABI rather than assuming them to be equivalent.

### 2.0.C — Stable end-to-end workflows

- **C++:** create, theme, navigate, package and install a production Widgets app.
- **Native migration:** adapt selected legacy controls and remove adaptation without modifying native ownership/signal semantics.
- **Designer:** author, save, reopen, compile and run supported `.ui` forms.
- **Visual UI Studio:** author and export the officially supported stable subset with validated schema round trips if included in 2.0.
- **Python:** install a qualified wheel and run a packaged PySide6 application.
- Keep screenshot reconstruction experimental and non-blocking unless it meets an explicit separately reviewed stable contract.

### 2.0.D — Mandatory release checklist

1. Every component advertised as production-complete has approved registry, test, golden and applicable native-AT evidence.
2. All installed public API changes, Qt/OS/compiler/Python support matrices and schema migrations are documented and tested.
3. C++ and Python reference apps pass from actual installed packages.
4. Stable visual-authoring workflows (if advertised) round-trip, export, compile and run.
5. Cross-platform CI, release tests, sanitizers and reproducible controlled performance budgets are green.
6. A representative 1.x application follows the migration guide successfully.
7. Known limitations and experimental tooling are prominently distinguished from stable APIs.
8. Release packages, changelog, documentation and maintenance branches are prepared before tagging.

**Deliverable:** the first stable Qt Material 3 **Desktop SDK 2.0** release.

## Architecture boundaries

`Core (ThemeContext / ThemeManager / tokens / specs / interaction / motion / accessibility)`
-> `Widgets (first-class QtMaterial controls)`
-> optional `Native Adapter (standard QWidget adaptation)`
-> optional `Designer (property editors / .ui authoring)`
-> optional `UI Schema + Visual UI Studio (composition / validated export)`
-> optional `Screenshot Import (experimental schema producer)`
-> optional `Python SDK (PySide6 bindings to native runtime)`.

Gallery/visual QA and packaging/CI consume those same public contracts. The Studio/schema should not become a dependency of Core/Widgets; the Python wrapper should not reimplement Widgets. Interface import/export diagnostics must reject unsupported features instead of silently degrading them.

## Pull-request and evidence workflow

- Implement milestones in independently reviewable `A/B/C/D` work packages; split implementation PRs by coherent surface, not by an arbitrary large release batch.
- Every component-affecting PR updates implementation, focused CTest, registry evidence, Gallery route and public docs together.
- Every exported file format has versioned schemas, malformed-input tests, round-trip tests and migration policy.
- Every visual change uses pinned renderer/provenance and reviewed goldens rather than unreviewed generated images.
- Every release checkpoint records platform/Qt/Python matrices, performance budget outcome, unresolved AT checks and package consumer evidence.
- Do not manually edit generated `STATUS.md` or component status tables; run the generator and commit the generated result.
- Use the main `ROADMAP.md` for the existing release history and this companion document for proposed 1.22–2.0 detail. Promote proposals to committed milestones only through reviewed PRs.
