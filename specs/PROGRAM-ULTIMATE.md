# QtMaterial3 - Ultimate Spec Kit Vision

**Status:** canonical long-term vision backlog  
**Baseline:** 2026-08-15, repository 0.4.0  
**Purpose:** preserve every roadmap direction discussed while keeping the executable 1.0 program finite.

> This file is a vision/program source, not an instruction to create 1,500 empty Spec Kit folders. A Vision Spec becomes an executable Spec Kit feature only when it enters a committed milestone; then it receives `spec.md`, `plan.md`, `tasks.md`, checklists and evidence.

## 1. Canonical planning hierarchy

```text
Ultimate Vision / Capability Epics
        |
        v
Release Program (0.5 -> 1.0 -> 1.x -> 2.x ...)
        |
        v
Executable Spec Kit features
        |
        v
spec -> clarify -> plan -> checklist -> tasks -> analyze -> implement -> converge
        |
        v
commits / PRs / tests / evidence
```

## 2. Rings - keep the vision ambitious without making 1.0 impossible

| Ring | Horizon | Meaning |
|---|---|---|
| R0 MUST | 0.5-1.0 | Architecture, M3 Classic scope, components, a11y, tests, performance, API/ABI, packaging |
| R1 PRODUCT | 1.x | Python, QStyle, Designer, Adaptive, Expressive, Theme Studio, WASM showcase, integrations |
| R2 PLATFORM | 2.x | DesignSystem IR/compiler, DTCG, plugins, semantic models, Render IR, tooling |
| R3 ADVANCED | 3.x | Generated components/tests, formal state models, time travel, remote inspection, multi-language |
| R4 INDUSTRIAL/RESEARCH | 4.x+ | HMI, embedded, distributed UI, formal verification, hardware-in-loop, spatial, safety-oriented subsets |

## 3. Release spine already adopted

- **0.5 Architecture Foundation:** CI/registry, SpecFactory removal, Theme split, typed tokens, architecture debt = 0.
- **0.6 Core Components:** buttons, selection, text fields, core surfaces, accessibility/keyboard/RTL/DPI/visual.
- **0.7 Navigation & Inputs:** navigation, menu, autocomplete, date/time, search, chips.
- **0.8 Desktop/Data:** table/list/grid/carousel/sliders and large-model desktop behavior.
- **0.9 API Freeze / RC:** sanitizers, performance gates, API/ABI, packaging, complete docs/conformance evidence.
- **1.0 Stable:** zero known P0/P1, advertised compatibility tested, component DoD satisfied, ABI baseline, release evidence.
- **1.x:** Python/PySide6, Designer, QStyle, adaptive, Expressive, external integrations, Theme Studio.
- **2.x:** design-system platform/compiler/plugins/IR.
- **3.x+:** verifiable/generative/industrial UI engine research.

## 4. Executable program - Specs 001-063

# QtMaterial3 Spec Kit Program

**Baseline:** 2026-08-15 — repository 0.4.0

## Dependency spine

```text
001 -> 002 -> 003 -> 004
              \-> 005 -> 006 -> 007 -> 008/009 -> 010 -> 011
011 -> 013..019 (Material conformance) and 020..030 (runtime foundations)
then component families -> API/ABI/packaging -> 1.0 -> ecosystem/Expressive
```

## Catalog

| # | Spec | Milestone | Priority |
|---:|---|---|---|
| 001 | `001-repository-health-and-ci` | 0.5 | P0 |
| 002 | `002-component-registry-source-of-truth` | 0.5 | P0 |
| 003 | `003-build-and-consumer-matrix` | 0.5 | P0 |
| 004 | `004-qt5-qt6-compatibility-contract` | 0.5 | P0 |
| 005 | `005-remove-legacy-specfactory` | 0.5 | P0 |
| 006 | `006-theme-target-decomposition` | 0.5 | P0 |
| 007 | `007-theme-model` | 0.5 | P0 |
| 008 | `008-theme-io` | 0.5 | P0 |
| 009 | `009-theme-runtime` | 0.5 | P0 |
| 010 | `010-typed-token-system` | 0.5 | P0 |
| 011 | `011-architecture-zero-debt` | 0.5 | P0 |
| 012 | `012-public-private-header-boundaries` | 0.5/0.9 | P1 |
| 013 | `013-material-reference-model` | 0.5/0.6 | P1 |
| 014 | `014-material-structural-conformance` | 0.6 | P1 |
| 015 | `015-material-render-reference-generator` | 0.6 | P1 |
| 016 | `016-material-visual-comparator` | 0.6 | P1 |
| 017 | `017-material-conformance-report` | 0.6 | P1 |
| 018 | `018-material-motion-conformance` | 0.7+ | P2 |
| 019 | `019-material-conformance-ci-gates` | 0.6+ | P1 |
| 020 | `020-resolved-spec-contract` | 0.6 | P0 |
| 021 | `021-spec-invalidation-system` | 0.6 | P1 |
| 022 | `022-spec-cache-and-revisions` | 0.6 | P1 |
| 023 | `023-renderer-purity` | 0.6 | P0 |
| 024 | `024-effects-engine` | 0.6 | P1 |
| 025 | `025-motion-engine` | 0.6/1.x | P1 |
| 026 | `026-core-runtime-cleanup` | 0.6 | P0 |
| 027 | `027-theme-context-propagation` | 0.6 | P0 |
| 028 | `028-focus-interaction-framework` | 0.6 | P1 |
| 029 | `029-accessibility-foundation` | 0.6 | P0 |
| 030 | `030-high-dpi-platform-foundation` | 0.6 | P1 |
| 031 | `031-button-family-conformance` | 0.6 | P0 |
| 032 | `032-selection-family` | 0.6 | P0 |
| 033 | `033-chip-family` | 0.7 | P1 |
| 034 | `034-text-field-family` | 0.6 | P0 |
| 035 | `035-autocomplete` | 0.7 | P1 |
| 036 | `036-date-time-inputs` | 0.7 | P1 |
| 037 | `037-search-and-combobox` | 0.7 | P1 |
| 038 | `038-surface-family` | 0.6 | P0 |
| 039 | `039-navigation-model` | 0.7 | P1 |
| 040 | `040-navigation-components` | 0.7 | P1 |
| 041 | `041-menu-system` | 0.7 | P1 |
| 042 | `042-data-table` | 0.8 | P1 |
| 043 | `043-list-grid` | 0.8 | P1 |
| 044 | `044-carousel` | 0.8 | P1 |
| 045 | `045-slider-family` | 0.8 | P1 |
| 046 | `046-visual-regression-infrastructure` | 0.6 | P1 |
| 047 | `047-performance-budgets` | 0.8/0.9 | P1 |
| 048 | `048-robustness-sanitizers-fuzzing` | 0.9 | P1 |
| 049 | `049-public-api-freeze` | 0.9 | P0 |
| 050 | `050-abi-contract` | 0.9/1.0 | P0 |
| 051 | `051-packaging-release-system` | 0.9/1.0 | P0 |
| 052 | `052-documentation-system` | 0.6/1.0 | P1 |
| 053 | `053-gallery-reference-app` | 0.6/1.0 | P1 |
| 054 | `054-theme-studio` | 1.x | P2 |
| 055 | `055-developer-tooling` | 0.8/1.x | P2 |
| 056 | `056-security-supply-chain` | 0.9/1.0 | P1 |
| 057 | `057-governance-release-process` | 0.9/1.0 | P1 |
| 058 | `058-material3-coverage-completion` | 0.8/1.0 | P1 |
| 059 | `059-adaptive-layout-navigation` | 1.x | P2 |
| 060 | `060-material3-expressive-profile` | 1.x/2.x | P2 |
| 061 | `061-qt-designer-integration` | 1.x | P3 |
| 062 | `062-pyside-bindings` | 1.x | P3 |
| 063 | `063-qml-theme-bridge` | 1.x | P3 |


## 5. Vision Specs 064-100 - future-readiness and ecosystem foundation
- **064 `qt7-readiness`** - Track deprecations, isolate compatibility shims, prepare Qt 7 without dropping current contracts prematurely.
- **065 `platform-backend-certification`** - Explicit Windows/macOS/Linux X11/Wayland certification for focus, popup, z-order, clipboard, drag and rendering.
- **066 `multiscreen-dpi-runtime`** - Dynamic screen/DPR transitions, cache invalidation, hotplug and mixed-scale desktops.
- **067 `design-token-standard-interchange`** - DTCG Design Tokens import/export as an interoperable boundary alongside native QtMaterial3 formats.
- **068 `multibrand-theme-system`** - Brand/profile inheritance and context-aware themes without duplicated theme files.
- **069 `wide-gamut-color-management`** - Prepare Color/Theme models for sRGB/P3/modern color spaces rather than assuming simple QRgb forever.
- **070 `material-symbols-system`** - Semantic Material Symbol IDs, variable axes, filled state, optical size, RTL and caching.
- **071 `icon-asset-pipeline`** - Icon identity/backend/cache/DPR/colorization/animation/licensing pipeline.
- **072 `material-upstream-watch`** - Govern upstream Material/AndroidX reference updates; never silently move conformance baselines.
- **073 `material-reference-archive`** - Archive classic/expressive reference generations and keep releases bound to explicit references.
- **074 `material-adaptive-layouts`** - Canonical adaptive/list-detail/supporting-pane patterns and multi-pane composition.
- **075 `adaptive-strategies`** - Reflow/levitate/collapse and content-priority strategies, not merely width thresholds.
- **076 `accessibility-platform-hints`** - Map platform accessibility preferences to AccessibilityTokens with Qt-version fallbacks.
- **077 `accessible-virtual-children`** - Accessibility objects for complex components that do not map one-to-one to QWidget children.
- **078 `accessible-live-announcements`** - Snackbar/validation/search/loading announcements through capability-aware platform adapters.
- **079 `input-methods-and-ime`** - IME, compose, dead keys, CJK composition, cursor/selection and input-method regression suite.
- **080 `desktop-interaction-policy`** - Document and test double-click/context menu/wheel/drag/hover/shortcuts/selection desktop adaptations.
- **081 `drag-drop-clipboard`** - Coherent DnD, MIME, reordering and clipboard contracts across inputs and data widgets.
- **082 `model-view-interoperability`** - QAbstractItemModel/proxy/selection/delegate/fetchMore is the native data contract.
- **083 `material-delegate-library`** - Material delegates for QListView/QTreeView/QTableView so existing apps can adopt styling incrementally.
- **084 `qstyle-complete-integration`** - Progressive styling of standard Qt widgets using QStyle/QProxyStyle.
- **085 `style-plugin`** - Loadable QStyle plugin distribution.
- **086 `qt-designer-deep-integration`** - Designer properties, previews and editing, not only widget palette registration.
- **087 `ui-file-runtime-compatibility`** - Designer .ui/promoted widgets/QFormBuilder/pyside6-uic compatibility.
- **088 `figma-design-token-import`** - Figma/DTCG -> ThemeModel workflow; prefer structured token import to blind UI code generation.
- **089 `design-roundtrip-validation`** - Design import -> Qt render -> conformance report -> unsupported/approximate mapping diagnostics.
- **090 `migration-codemods`** - qtm3-migrate for CMake targets, headers, classes, properties and token migrations.
- **091 `downstream-compatibility-lab`** - Real representative applications beyond toy consumers.
- **092 `diagnostics-bundle`** - Version/Qt/OS/QPA/DPR/screens/theme hash/profile/backend/build-options report for bug reports.
- **093 `reproducible-render-environment`** - Versioned hermetic visual test environment.
- **094 `theme-parser-hardening`** - Limits, cycle detection, malformed input handling and fuzzing for public theme/token parsers.
- **095 `plugin-and-extension-api`** - Stable extension points for custom tokens/resolvers/components/theme adapters without private headers.
- **096 `reference-applications`** - Official settings, data-heavy and adaptive example applications.
- **097 `documentation-versioning`** - Versioned docs, latest/stable and migration-aware documentation.
- **098 `lts-and-support-policy`** - Support windows, deprecation duration, maintained branches and minimum Qt policy.
- **099 `agentic-development-integration`** - Spec-Kit-governed AI-assisted development; agents cannot bypass constitution/tests/evidence.
- **100 `upstream-material-diff-bot`** - Periodic upstream reference change detector producing reviewable issues/reports, never automatic visual changes.

## 6. Vision Specs 101-200 - Design-system compiler, Render IR, generated metadata and zero drift
- **101 `design-system-ir`** - Frontend-independent typed intermediate representation for tokens, aliases, contexts, components and assets.
- **102 `theme-compiler`** - `qtm3c` compiles native/DTCG design sources to validated model/packages.
- **103 `compile-time-theme-generation`** - Generate constexpr/resource tables for runtime-free parsing profiles.
- **104 `token-expression-engine`** - Typed derived token expressions with cycle/error handling.
- **105 `token-dependency-graph`** - Know exactly which tokens/components/tests are affected by a token change.
- **106 `incremental-theme-resolution`** - Recompute only impacted branches after changes.
- **107 `semantic-theme-diff`** - Semantic theme differences and affected component explanation.
- **108 `theme-impact-analysis`** - Preview downstream component/state impact before applying an edit.
- **109 `theme-hot-reload`** - Development hot reload for theme/token sources.
- **110 `design-system-server`** - Optional development service for live Theme Studio/application exchange.
- **111 `render-ir`** - Backend-independent logical shapes/fills/strokes/text/icons/elevation/clipping/state layers.
- **112 `headless-renderer`** - Render to image without live windows for tests/tooling.
- **113 `reference-renderer`** - Deterministic conformance renderer separated from interactive optimized renderer.
- **114 `render-command-recorder`** - Record logical draw operations.
- **115 `render-command-diff`** - Explain render differences at semantic command level.
- **116 `gpu-rendering-research`** - Evaluate GPU/QRhi only where benchmark evidence justifies it and keep it private/experimental.
- **117 `hybrid-raster-gpu-effects`** - Optional GPU acceleration only for proven expensive effects.
- **118 `render-backend-capabilities`** - Explicit blur/wide-gamut/etc capability negotiation.
- **119 `deterministic-font-metrics-layer`** - Reduce cross-platform text-metric noise for reference tests.
- **120 `golden-render-farm`** - Multiple preserved render reference environments.
- **121 `event-recorder`** - Record deterministic user event scenarios.
- **122 `event-replay`** - Replay exact interaction sequences.
- **123 `ui-state-machine-models`** - Explicit state machines for complex widgets instead of boolean proliferation.
- **124 `state-machine-invariant-checker`** - Detect impossible or illegal component states.
- **125 `model-based-testing`** - Generate interaction paths from behavior models.
- **126 `ui-fuzzing`** - Fuzz show/hide/resize/reparent/theme/focus/delete sequences.
- **127 `interaction-trace-diff`** - Behavior regression diff between versions.
- **128 `motion-trace-format`** - Position/opacity/shape/elevation/velocity timeline format.
- **129 `motion-debugger`** - Inspect real animation curves/timelines.
- **130 `frame-time-profiler`** - Resolve/layout/paint frame cost attribution.
- **131 `repaint-profiler`** - Repaint region heatmaps and unnecessary update detection.
- **132 `allocation-profiler-contract`** - Allocation budgets on hot paths.
- **133 `energy-efficiency`** - Timers/wakeups/idle CPU/animation power behavior.
- **134 `mass-widget-stress-lab`** - 1k/10k/100k scale scenarios with Model/View where appropriate.
- **135 `long-running-stability`** - Thousands of lifecycle/theme/popup/navigation cycles.
- **136 `reference-application-corpus`** - Downstream-realistic application corpus.
- **137 `downstream-api-usage-scanner`** - Measure real public API usage before breaking changes.
- **138 `migration-assistant`** - Analyze consumer project and propose/apply structured migrations.
- **139 `api-semantic-diff`** - Diff properties/signals/defaults/behavior in addition to ABI.
- **140 `behavioral-compatibility-contract`** - Treat selected behavior as post-1.0 compatibility contract.
- **141 `feature-capability-system`** - Capabilities/profiles instead of uncontrolled conditional compilation.
- **142 `plugin-api`** - Stable third-party extension surface.
- **143 `plugin-version-contract`** - Plugin compatibility/version negotiation.
- **144 `component-schema`** - Data description of properties/states/variants/tokens/a11y/slots.
- **145 `code-generated-component-metadata`** - Generate docs/gallery/inspectors/base tests/binding metadata.
- **146 `automatic-test-matrix-generation`** - Generate variant x state x theme x direction x density x DPI test cases.
- **147 `automatic-gallery-generation`** - Generate much of Gallery from ComponentDefinition.
- **148 `automatic-accessibility-contract-generation`** - Generate a11y checks from semantic component schema.
- **149 `design-system-linter`** - Contrast/magic values/unused tokens/cycles/inconsistent role linting.
- **150 `design-system-doctor`** - `qtm3 doctor` aggregates schema/token/conformance/unused/missing mapping diagnostics.
- **151 `wasm-gallery`** - Zero-install browser Gallery.
- **152 `wasm-conformance-demo`** - Browser conformance demonstration.
- **153 `online-theme-playground`** - Browser Theme Studio/playground.
- **154 `theme-share-links`** - Share seed/profile/density/component/state by URL.
- **155 `qstyle-token-bridge`** - Map design tokens to QStyle semantics.
- **156-164 `qstyle-control-mapping-family`** - Standard buttons/checks/combobox/line edits/menus/scrollbars and plugin distribution.
- **165-174 `designer-material-environment`** - Designer palette/properties/theme/state/density/responsive/token preview and runtime UI-file metadata.
- **175 `design-system-project-format`** - Canonical design-system project directory/manifest.
- **176 `qtm3-compiler-cli`** - CLI compile/validate/test/render/doctor/migrate/inspect/report.
- **177 `code-generation-backend`** - Generate C++/metadata from compiled design system.
- **178 `qresource-generation`** - Generate deterministic Qt resources.
- **179 `schema-generation`** - Generate/expose schemas for tooling.
- **180 `documentation-generation`** - Generated design-system documentation.
- **181 `component-metadata-generation`** - Generated component metadata package.
- **182 `conformance-generation`** - Compiler-driven conformance inputs/reports.
- **183 `cmake-integration`** - `qtmaterial3_add_design_system(...)` integration.
- **184 `incremental-generation`** - Incremental compiler.
- **185 `build-cache`** - Content-addressed generation cache.
- **186 `component-definition-language`** - Declarative formal component definition.
- **187 `interaction-state-schema`** - Explicit interaction/state schema.
- **188 `state-machine-codegen`** - Generate state-machine implementation.
- **189 `state-machine-tests`** - Generated state behavior tests.
- **190 `state-transition-coverage`** - Coverage over transitions.
- **191 `event-fuzzer`** - Generate event sequences.
- **192 `accessibility-schema`** - Declarative a11y contract.
- **193 `keyboard-contract-schema`** - Declarative keyboard contract.
- **194 `generated-component-documentation`** - Schema-driven docs.
- **195 `generated-component-test-matrix`** - Schema-driven test matrix.
- **200 `zero-manual-drift`** - All structured project truth has one canonical source and generated/validated derivatives.

## 7. Vision Specs 201-318 - formal behavior, compiler, reproducibility, embedded, distributed and impact-aware CI
### 201-220 Formal component behavior

- `201 component-behavior-model`
- `202 component-statecharts`
- `203 scxml-experimental-backend`
- `204 statechart-code-generation`
- `205 transition-invariant-engine`
- `206 state-coverage`
- `207 state-path-generation`
- `208 invalid-state-detection`
- `209 interaction-model-diff`
- `210 material-behavior-reference`
- `211 input-event-normalization`
- `212 pointer-capture-framework`
- `213 gesture-arbitration`
- `214 double-click-policy`
- `215 long-press-policy`
- `216 hover-capability-policy`
- `217 modality-tracking`
- `218 modality-adaptive-focus`
- `219 interaction-replay-format`
- `220 behavior-certification-report`

### 221-240 Component compiler/code generation

- `221 component-definition-schema`
- `222 component-schema-validator`
- `223 component-cpp-generator`
- `224 resolved-spec-generator`
- `225 token-accessor-generator`
- `226 property-generator`
- `227 qproperty-generator`
- `228 signal-generator`
- `229 accessibility-code-generator`
- `230 keyboard-code-generator`
- `231 state-machine-generator`
- `232 gallery-page-generator`
- `233 documentation-generator`
- `234 unit-test-generator`
- `235 conformance-test-generator`
- `236 visual-test-generator`
- `237 benchmark-skeleton-generator`
- `238 public-api-manifest-generator`
- `239 python-binding-metadata-generator`
- `240 designer-metadata-generator`

### 241-250 Static design-system analysis

- `241 design-system-type-checker`
- `242 token-unit-system`
- `243 token-expression-type-check`
- `244 token-cycle-proof`
- `245 unused-token-analysis`
- `246 unreachable-component-state-analysis`
- `247 unused-component-variant-analysis`
- `248 theme-completeness-analysis`
- `249 material-contract-linter`
- `250 design-system-static-analysis-report`

### 251-260 Hermetic/reproducible evidence

- `251 render-environment-manifest`
- `252 hermetic-visual-container`
- `253 font-fingerprint`
- `254 rasterizer-fingerprint`
- `255 reference-render-seed`
- `256 deterministic-animation-clock`
- `257 deterministic-event-clock`
- `258 reproducible-random-source`
- `259 build-reproducibility`
- `260 release-evidence-bundle`

### 261-270 Constrained/embedded design core

- `261 embedded-design-core-profile`
- `262 static-theme-profile`
- `263 zero-allocation-resolved-spec`
- `264 memory-budget-manifest`
- `265 no-exceptions-profile`
- `266 no-rtti-profile`
- `267 fixed-capacity-containers`
- `268 mcu-token-export`
- `269 qml-ultralite-theme-export`
- `270 embedded-conformance-profile`

### 271-280 Safety-oriented research subset

- `271 safety-ui-subset`
- `272 no-animation-safety-profile`
- `273 static-layout-safety-profile`
- `274 prevalidated-resource-bundle`
- `275 resource-integrity-hashes`
- `276 safety-theme-lock`
- `277 safety-layout-validation`
- `278 safety-critical-contrast-profile`
- `279 safety-render-isolation-research`
- `280 safe-renderer-export-research`

### 281-290 Multi-process/distributed runtime

- `281 ui-process-isolation`
- `282 remote-navigation-model`
- `283 remote-data-model-adapter`
- `284 remote-theme-control`
- `285 remote-diagnostics`
- `286 ui-crash-recovery`
- `287 state-serialization`
- `288 ui-session-restore`
- `289 remote-object-version-contract`
- `290 distributed-ui-demo`

### 291-300 WebAssembly evaluation

- `291 wasm-gallery`
- `292 online-theme-studio`
- `293 online-component-playground`
- `294 shareable-theme-url`
- `295 online-conformance-viewer`
- `296 online-regression-viewer`
- `297 wasm-demo-release`
- `298 wasm-browser-matrix`
- `299 wasm-accessibility-tracking`
- `300 zero-install-evaluation`

### 301-307 Assisted analysis (never source of truth)

- `301 conformance-difference-classifier`
- `302 regression-root-cause-assistant`
- `303 failing-test-clustering`
- `304 api-migration-assistant`
- `305 theme-repair-assistant`
- `306 accessibility-suggestion-engine`
- `307 performance-regression-assistant`

### 308-312 Intelligent combinatorial testing

- `308 test-space-model`
- `309 pairwise-test-generator`
- `310 boundary-test-generator`
- `311 state-space-risk-model`
- `312 change-impact-test-selection`

### 313-318 Semantic dependency/impact graph

- `313 design-dependency-graph`
- `314 component-impact-graph`
- `315 test-impact-graph`
- `316 doc-impact-graph`
- `317 gallery-impact-graph`
- `318 api-impact-graph`

## 8. Python SDK / bindings - 320-372

- `320 python-binding-architecture: C++ remains source of truth; generated native Python binding.`
- `321 python-qt-contract: C++ may retain Qt5 support while Python targets Qt6/PySide6 only.`
- `322 python-package-layout: single native module first; modularize only with evidence.`
- `323 python-binding-manifest: explicitly choose exposed vs C++-only API.`
- `324 shiboken-typesystem: typesystem XML and controlled signature/ownership adaptations.`
- `325 pythonic-api-policy: snake_case/properties where useful while retaining coherent Qt conventions.`
- `326 python-signals-slots: native QObject signal semantics.`
- `327 python-object-ownership`
- `328 python-qobject-parenting`
- `329 python-wrapper-invalidation`
- `330 python-lifetime-stress-tests`
- `331 python-value-types: value/object distinction for Theme/ColorScheme/ResolvedSpec/etc.`
- `332 python-conversions: restrained ergonomic conversions, e.g. QColor/string where deliberate.`
- `333 python-enums: typed enums and stubs.`
- `334 python-type-stubs`
- `335 python-pytyped-package`
- `336 python-mypy-tests`
- `337 python-pyright-tests`
- `338 cpp-to-python-documentation: one canonical description with language-specific examples.`
- `339 python-component-tests: construction/properties/signals/ownership/destruction/inheritance.`
- `340 cpp-python-parity-matrix`
- `341 binding-completeness-checker`
- `342 python-api-parity-tests`
- `343 python-wheel-packaging`
- `344 python-wheel-platform-matrix`
- `345 pyside-compatibility-matrix`
- `346 python-binding-abi-contract`
- `347 abi3-research`
- `348 python-subclassing`
- `349 no-python-in-critical-paint-path-by-default`
- `350 python-custom-resolver-api`
- `351 python-component-plugins`
- `352 python-theme-tooling`
- `353 python-design-system-compiler-api`
- `354 python-conformance-api`
- `355 python-screenshot-testing`
- `356 pytest-integration`
- `357 python-gallery-examples`
- `358 pyside6-designer-uic-integration`
- `359 python-deployment-validation`
- `360 python-sdk-definition-of-done`
- `361 embedded-python-scripting`
- `362 python-theme-scripts`
- `363 python-ui-automation`
- `364 python-plugin-runtime`
- `365 python-script-sandbox-research`
- `366 python-async-validation`
- `367 python-async-autocomplete`
- `368 python-async-data-provider`
- `369 qtasyncio-integration`
- `370 python-gil-policy`
- `371 release-gil-on-pure-computation`
- `372 python-threading-tests`

**Immediate pre-binding rules:** evaluate every public API for Shiboken bindability; document QObject/pointer ownership; keep public value types simple; add `python.exposed/name/status`-style metadata to the future ComponentDefinition.

## 9. Third-party Qt ecosystem integration - adapter architecture

### Generic integration epics
- `EXT-001 integration-provider-contract`
- `EXT-002 optional-integration-modules`
- `EXT-003 external-theme-adapter`
- `EXT-004 external-icon-provider`
- `EXT-005 external-state-mapping`
- `EXT-006 external-density-mapping`
- `EXT-007 external-accessibility-audit`
- `EXT-008 external-visual-conformance`
- `EXT-009 external-version-compatibility`
- `EXT-010 external-plugin-discovery`

**Invariant:** `QtMaterial3IntegrationX` may depend on QtMaterial3 + X; QtMaterial3 Core/Theme/Specs/Widgets never depend on X.

### Qt Advanced Docking System pilot
- `ADS-001 ads-adapter`
- `ADS-002 ads-disable-internal-stylesheet`
- `ADS-003 ads-material-qss-generator`
- `ADS-004 ads-titlebar-theme`
- `ADS-005 ads-tab-theme`
- `ADS-006 ads-splitter-theme`
- `ADS-007 ads-dock-buttons`
- `ADS-008 ads-auto-hide-tabs`
- `ADS-009 ads-drag-overlay`
- `ADS-010 ads-drag-preview`
- `ADS-011 ads-floating-window`
- `ADS-012 ads-focus-highlighting`
- `ADS-013 ads-central-widget`
- `ADS-014 ads-runtime-theme-switch`
- `ADS-015 ads-high-contrast`
- `ADS-016 ads-density-switch`
- `ADS-017 ads-rtl`
- `ADS-018 ads-high-dpi`
- `ADS-019 ads-platform-matrix`
- `ADS-020 ads-layout-persistence-theme-independence`
- `ADS-021 ads-component-registry`
- `ADS-022 ads-gallery-workbench`
- `ADS-023 ads-conformance-report`
- `ADS-024 ads-example-application`
- `ADS-025 ads-python-integration`

Potential later adapters: Qwt, QCustomPlot, KDDockWidgets/KDAB components, QScintilla, property browsers, OpenGL/3D viewers, other mature Qt desktop libraries.

## 10. Vision 373-625 - portability, generic kernel, plugins, threading, devtools, compatibility
### 373-390 Multi-architecture/toolchains

- `373 multi-architecture-policy`
- `374 windows-arm64`
- `375 linux-arm64`
- `376 macos-arm64`
- `377 cross-compilation-contract`
- `378 toolchain-file-certification`
- `379 cpu-feature-baseline`
- `380 build-reproducibility-across-architectures`
- `381 native-vs-cross-build-validation`
- `382 architecture-specific-performance-baselines`
- `383 endianness-assumption-audit`
- `384 pointer-size-audit`
- `385 alignment-and-packing-audit`
- `386 large-address-space-tests`
- `387 32-bit-research-profile`
- `388 compiler-portability`
- `389 msvc-gcc-clang-diagnostic-parity`
- `390 cpp-standard-migration-policy`

### 391-420 Stable FFI/multi-language

- `391 c-ffi-feasibility`
- `392 stable-opaque-handle-model`
- `393 c-error-model`
- `394 c-string-value-boundary`
- `395 callback-contract`
- `396 ffi-threading-contract`
- `397 ffi-ownership-contract`
- `398 ffi-version-negotiation`
- `399 ffi-symbol-versioning`
- `400 ffi-abi-tests`
- `401 rust-sdk`
- `402 rust-safe-wrapper`
- `403 rust-ownership-model`
- `404 dotnet-sdk`
- `405 csharp-bindings`
- `406 lua-scripting`
- `407 zig-bindings-research`
- `408 java-jni-research`
- `409 language-binding-generation`
- `410 binding-conformance-matrix`

### 421-440 Generic QtDesignSystem kernel

- `421 generic-design-system-kernel`
- `422 design-system-profile-interface`
- `423 material3-profile`
- `424 company-design-system-profile`
- `425 profile-token-registry`
- `426 profile-component-registry`
- `427 profile-reference-registry`
- `428 profile-capabilities`
- `429 profile-inheritance`
- `430 profile-validation`
- `431 design-system-ir`
- `432 design-system-ir-versioning`
- `433 design-system-compiler-api`
- `434 design-system-package-format`
- `435 compiled-design-system-package`
- `436 design-system-dependency-resolution`
- `437 multi-profile-application`
- `438 profile-migration`
- `439 profile-diff`
- `440 profile-conformance`

### 441-480 Plugin/package ecosystem

- `441 plugin-manifest`
- `442 plugin-api-version`
- `443 plugin-abi-version`
- `444 plugin-capability-negotiation`
- `445 plugin-dependency-graph`
- `446 plugin-load-validation`
- `447 plugin-unload-policy`
- `448 plugin-failure-isolation`
- `449 plugin-signature-research`
- `450 plugin-package-format`
- `461 qtm3-package-manifest`
- `462 package-versioning`
- `463 package-lockfile`
- `464 package-dependency-resolution`
- `465 package-local-cache`
- `466 package-integrity-hash`
- `467 offline-package-installation`
- `468 package-license-metadata`
- `469 package-trust-policy`
- `470 package-vulnerability-metadata`
- `471 package-registry-protocol`
- `472 community-theme-packages`
- `473 community-component-packages`
- `474 community-icon-packages`
- `475 registry-client`
- `476 registry-search`
- `477 registry-publishing`
- `478 registry-moderation-model`
- `479 package-signing`
- `480 package-reproducibility`

### 481-500 Threading/pure computation

- `481 threading-contract`
- `482 immutable-theme-snapshot`
- `483 background-theme-compilation`
- `484 background-theme-validation`
- `485 background-design-system-compilation`
- `486 async-theme-load`
- `487 async-theme-switch`
- `488 cancellation-token`
- `489 stale-result-rejection`
- `490 job-generation-id`
- `491 thread-safe-token-graph`
- `492 thread-safe-reference-database`
- `493 background-conformance-analysis`
- `494 worker-pool-policy`
- `495 no-gui-object-in-worker-contract`
- `496 cross-thread-diagnostics`
- `497 threading-stress-tests`
- `498 race-detection`
- `499 threading-performance`
- `500 threading-documentation`

### 501-525 Runtime DevTools protocol

- `501 inspection-protocol`
- `502 debug-agent`
- `503 widget-tree-inspection`
- `504 theme-context-tree`
- `505 resolved-spec-inspection`
- `506 token-provenance-inspection`
- `507 layout-inspection`
- `508 hit-test-inspection`
- `509 focus-inspection`
- `510 accessibility-inspection`
- `511 animation-inspection`
- `512 repaint-inspection`
- `513 performance-inspection`
- `514 cache-inspection`
- `515 memory-inspection`
- `516 live-theme-edit`
- `517 live-token-edit`
- `518 live-state-forcing`
- `519 live-density-switch`
- `520 live-rtl-switch`
- `521 debug-agent-disabled-by-default`
- `522 localhost-only-default`
- `523 authentication-if-remote`
- `524 debug-build-policy`
- `525 no-production-secret-exposure`

### 526-531 Provenance

- `526 value-provenance-model`
- `527 token-provenance`
- `528 resolver-provenance`
- `529 override-provenance`
- `530 layout-provenance`
- `531 render-provenance`

### 532-550 Adaptive performance/quality

- `532 render-quality-profile`
- `533 low-power-profile`
- `534 animation-budget`
- `535 shadow-quality-level`
- `536 effect-quality-level`
- `537 frame-budget-monitor`
- `538 adaptive-effect-degradation`
- `539 battery-awareness-research`
- `540 background-window-throttling`
- `541 hidden-window-throttling`
- `542 low-power-conformance-contract`

### 551-570 Industrial typography/font system

- `551 font-family-registry`
- `552 font-fallback-chain`
- `553 script-specific-fonts`
- `554 variable-font-support`
- `555 variable-font-weight`
- `556 variable-font-width`
- `557 variable-font-optical-size`
- `558 font-feature-settings`
- `559 ligature-policy`
- `560 number-feature-policy`
- `561 cjk-font-validation`
- `562 arabic-font-validation`
- `563 hebrew-font-validation`
- `564 indic-script-validation`
- `565 emoji-fallback`
- `566 symbol-fallback`
- `567 missing-glyph-detection`
- `568 font-licensing-metadata`
- `569 font-package-validation`
- `570 font-conformance-report`

### 571-590 Privacy-preserving observability

- `571 observability-api`
- `572 performance-counters`
- `573 cache-counters`
- `574 theme-resolution-counters`
- `575 animation-counters`
- `576 no-telemetry-default`
- `577 no-network-runtime-dependency`
- `578 opt-in-export`
- `579 user-data-free-metrics`
- `580 privacy-contract`
- `581 opentelemetry-adapter`
- `582 custom-metrics-sink`
- `583 json-diagnostic-export`

### 591-610 Compatibility lab

- `591 compatibility-lab`
- `592 old-header-new-library-test`
- `593 old-binary-new-library-test`
- `594 theme-schema-backward-compat`
- `595 theme-schema-forward-diagnostic`
- `596 plugin-compatibility`
- `597 python-api-compatibility`
- `598 behavioral-golden-compatibility`
- `599 migration-fixture-apps`
- `600 real-world-consumer-suite`

### 611-625 Protocol/schema governance

- `Independent versions for C++ API, ABI, Theme schema, DesignSystem IR, Plugin API, Component schema, Material reference, inspection protocol and Python SDK; compatibility/migration tables and tooling for each.`

## 11. Vision 626-975 - formal verification, semantic UI, time travel, declarative composition and industrial QA
### 626-650 Formal verification of behavior

- `626 formal-component-model`
- `627 formal-state-definition`
- `628 formal-event-definition`
- `629 transition-guard-model`
- `630 component-invariant-language`
- `631 dead-state-detection`
- `632 unreachable-transition-detection`
- `633 transition-cycle-analysis`
- `634 liveness-analysis`
- `635 focus-deadlock-detection`
- `636 modal-focus-proof`
- `637 popup-lifecycle-proof`
- `638 pointer-grab-invariant`
- `639 disabled-interaction-proof`
- `640 selection-invariant-proof`
- `641 model-checking-research`
- `642 bounded-state-exploration`
- `643 state-space-reduction`
- `644 counterexample-generation`
- `645 counterexample-to-test`
- `646 generated-behavior-tests`
- `647 state-machine-coverage-report`
- `648 component-proof-report`
- `649 formal-contract-ci`
- `650 formal-behavior-dashboard`

### 651-675 Semantic UI Model

- `651 semantic-ui-tree`
- `652 semantic-role-model`
- `653 semantic-action-model`
- `654 semantic-state-model`
- `655 semantic-value-model`
- `656 semantic-relation-model`
- `657 widget-to-semantic-mapping`
- `658 semantic-to-accessibility-mapping`
- `659 semantic-to-automation-mapping`
- `660 semantic-navigation-tree`
- `661 semantic-query-language`
- `662 find-component-by-role`
- `663 find-component-by-token`
- `664 find-component-by-state`
- `665 find-component-by-route`
- `666 semantic-tree-diff`
- `667 semantic-tree-snapshot`
- `668 semantic-tree-validation`
- `669 semantic-tree-versioning`
- `670 semantic-tree-inspector`
- `671 semantic-event-stream`
- `672 semantic-test-assertions`
- `673 semantic-accessibility-validator`
- `674 semantic-remote-inspection`
- `675 semantic-ui-contract`

### 676-700 Time-travel UI debugging

- `676 runtime-event-log`
- `677 ui-event-journal`
- `678 state-checkpoint`
- `679 theme-checkpoint`
- `680 resolved-spec-checkpoint`
- `681 focus-checkpoint`
- `682 animation-checkpoint`
- `683 time-travel-replay`
- `684 step-forward`
- `685 step-backward`
- `686 break-on-state-change`
- `687 break-on-token-change`
- `688 break-on-focus-change`
- `689 break-on-repaint`
- `690 trace-compression`
- `691 trace-file-format`
- `692 trace-versioning`
- `693 trace-anonymization`
- `694 trace-export`
- `695 crash-trace-ring-buffer`
- `696 last-events-dump`
- `697 bug-report-trace`
- `698 trace-regression-test`
- `699 trace-comparison`
- `700 time-travel-inspector`

### 701-730 Declarative composition layer

- `701 ui-composition-ir`
- `702 component-instance-definition`
- `703 component-slot-model`
- `704 content-model`
- `705 layout-node-model`
- `706 row-layout`
- `707 column-layout`
- `708 stack-layout`
- `709 grid-layout`
- `710 form-layout`
- `711 responsive-rule-model`
- `712 breakpoint-free-layout-rules`
- `713 content-priority`
- `714 min-preferred-max-sizing`
- `715 layout-intrinsic-sizing`
- `716 layout-validation`
- `717 layout-overflow-analysis`
- `718 layout-conflict-analysis`
- `719 layout-debugger`
- `720 layout-provenance`
- `721 declarative-form-model`
- `722 declarative-settings-model`
- `723 declarative-navigation-model`
- `724 declarative-dialog-model`
- `725 composition-to-widgets`
- `726 composition-to-qml-research`
- `727 composition-code-generation`
- `728 composition-schema`
- `729 composition-versioning`
- `730 composition-inspector`

### 731-750 Layout constraint analysis

- `731 layout-constraint-model`
- `732 size-constraint-model`
- `733 content-constraint-model`
- `734 accessibility-size-constraint`
- `735 touch-target-constraint`
- `736 text-overflow-constraint`
- `737 localization-expansion-constraint`
- `738 screen-boundary-constraint`
- `739 popup-boundary-constraint`
- `740 safe-area-constraint`
- `741 layout-constraint-solver`
- `742 layout-static-analysis`
- `743 layout-scenario-generator`
- `744 layout-extreme-tests`
- `745 auto-layout-report`
- `746 layout-failure-explanation`
- `747 layout-counterexample`
- `748 layout-visualization`
- `749 layout-regression`
- `750 layout-certification`

### 751-770 Metamorphic testing

- `751 metamorphic-test-framework`
- `752 theme-metamorphic-tests`
- `753 rtl-metamorphic-tests`
- `754 dpi-metamorphic-tests`
- `755 font-scale-metamorphic-tests`
- `756 density-metamorphic-tests`
- `757 color-scheme-metamorphic-tests`
- `758 state-metamorphic-tests`
- `759 disabled-invariant-tests`
- `760 readonly-invariant-tests`
- `761 theme-roundtrip-invariant`
- `762 serialization-roundtrip-invariant`
- `763 token-alias-invariant`
- `764 model-view-invariant`
- `765 metamorphic-test-generator`
- `766 component-schema-metamorphic-tests`
- `767 randomized-metamorphic-tests`
- `768 counterexample-shrinker`
- `769 metamorphic-report`
- `770 metamorphic-ci`

### 771-790 Fault injection lab

- `771 ui-fault-injection-framework`
- `772 font-load-failure`
- `773 icon-load-failure`
- `774 corrupt-resource`
- `775 theme-parse-failure`
- `776 parent-destruction`
- `777 model-reset-during-selection`
- `778 model-reset-during-popup`
- `779 screen-disconnect`
- `780 dpi-change-during-animation`
- `781 theme-change-during-paint-cycle`
- `782 theme-change-during-popup`
- `783 late-async-result`
- `784 cancelled-async-result`
- `785 reordered-async-result`
- `786 out-of-memory-research`
- `787 cache-allocation-failure-research`
- `788 fault-scenario-generator`
- `789 fault-report`
- `790 fault-ci`

### 791-815 Accessibility compatibility lab

- `791 a11y-platform-contract`
- `792 windows-uia-lab`
- `793 macos-ax-lab`
- `794 linux-atspi-lab`
- `795 accessibility-event-recorder`
- `796 accessibility-event-replay`
- `797 accessibility-event-order`
- `798 screen-reader-announcement-contract`
- `799 focus-event-contract`
- `800 selection-event-contract`
- `801 expanded-event-contract`
- `802 value-event-contract`
- `803 table-accessibility-model`
- `804 tree-accessibility-model`
- `805 calendar-accessibility-model`
- `806 carousel-accessibility-model`
- `807 slider-accessibility-model`
- `808 a11y-regression-snapshot`
- `809 a11y-tree-diff`
- `810 a11y-event-diff`
- `811 manual-audit-protocol`
- `812 audit-evidence-store`
- `813 platform-a11y-scorecard`
- `814 accessibility-release-report`
- `815 accessibility-certification-gate`

### 816-840 Deep internationalization

- `816 locale-capability-model`
- `817 numbering-system`
- `818 decimal-format`
- `819 currency-format`
- `820 percent-format`
- `821 date-calendar-system`
- `822 gregorian-calendar`
- `823 alternate-calendar-research`
- `824 first-day-week`
- `825 week-number-policy`
- `826 timezone-aware-components`
- `827 12h-24h-policy`
- `828 bidi-mixed-text`
- `829 bidi-icons`
- `830 bidi-cursor-navigation`
- `831 grapheme-cluster-navigation`
- `832 unicode-normalization`
- `833 combining-character-tests`
- `834 surrogate-nonbmp-tests`
- `835 emoji-sequence-tests`
- `836 locale-hot-switch`
- `837 font-hot-switch`
- `838 pseudo-localization-engine`
- `839 extreme-localization-corpus`
- `840 i18n-certification-report`

### 841-865 Asset compiler

- `841 asset-ir`
- `842 asset-manifest`
- `843 svg-validator`
- `844 svg-normalizer`
- `845 svg-optimizer`
- `846 svg-color-tokenization`
- `847 icon-registry`
- `848 icon-id-system`
- `849 icon-code-generation`
- `850 icon-subsetting`
- `851 material-symbol-subsetting`
- `852 variable-symbol-generation`
- `853 raster-density-generation`
- `854 resource-deduplication`
- `855 resource-hashing`
- `856 resource-cache`
- `857 license-metadata`
- `858 resource-license-validation`
- `859 unused-resource-analysis`
- `860 resource-qrc-generator`
- `861 resource-package`
- `862 resource-hot-reload`
- `863 resource-debugger`
- `864 resource-report`
- `865 resource-compiler`

### 866-890 Remote/distributed semantic UI

- `866 remote-ui-contract`
- `867 remote-semantic-tree`
- `868 remote-action-model`
- `869 remote-state-model`
- `870 protobuf-semantic-schema`
- `871 semantic-protocol-versioning`
- `872 ui-session-protocol`
- `873 grpc-ui-service-research`
- `874 remote-object-adapter`
- `875 network-latency-model`
- `876 optimistic-interaction`
- `877 state-reconciliation`
- `878 disconnected-mode`
- `879 reconnect-policy`
- `880 remote-theme-package`
- `881 remote-asset-package`
- `882 remote-ui-auth`
- `883 remote-ui-encryption`
- `884 remote-permission-model`
- `885 protocol-fuzzing`
- `886 protocol-compatibility`
- `887 remote-debug`
- `888 remote-conformance`
- `889 remote-demo`
- `890 distributed-ui-profile`

### 891-910 Feature slicing/minimal builds

- `891 component-build-features`
- `892 family-build-features`
- `893 minimal-core-profile`
- `894 no-effects-profile`
- `895 no-animation-profile`
- `896 no-io-profile`
- `897 static-component-selection`
- `898 link-time-dead-code-analysis`
- `899 resource-feature-pruning`
- `900 package-componentization`
- `901 cmake-component-find-package`
- `902 minimal-binary-benchmark`
- `903 minimal-memory-benchmark`
- `904 minimal-startup-benchmark`
- `905 build-feature-manifest`
- `906 build-feature-compatibility`
- `907 feature-dependency-solver`
- `908 invalid-feature-combination-check`
- `909 feature-matrix-ci`
- `910 size-optimized-profile`

### 911-935 Design-system LSP/IDE

- `911 design-system-language-server`
- `912 token-autocomplete`
- `913 component-autocomplete`
- `914 schema-completion`
- `915 reference-completion`
- `916 token-hover`
- `917 token-definition`
- `918 token-find-references`
- `919 token-rename`
- `920 token-diagnostics`
- `921 theme-diagnostics`
- `922 schema-diagnostics`
- `923 conformance-diagnostics`
- `924 design-preview-hover`
- `925 color-preview`
- `926 shape-preview`
- `927 typography-preview`
- `928 qtcreator-extension`
- `929 vscode-extension`
- `930 clion-integration-research`
- `931 code-actions`
- `932 generate-token`
- `933 migrate-token`
- `934 fix-invalid-reference`
- `935 design-system-lsp`

### 936-955 Git-aware design review

- `936 design-git-diff`
- `937 token-aware-diff`
- `938 component-aware-diff`
- `939 reference-aware-diff`
- `940 design-change-impact`
- `941 visual-impact-summary`
- `942 accessibility-impact`
- `943 performance-impact`
- `944 pr-conformance-comment`
- `945 pr-visual-report`
- `946 pr-theme-preview`
- `947 semantic-merge-driver`
- `948 token-merge-driver`
- `949 theme-conflict-explanation`
- `950 design-history`
- `951 token-blame`
- `952 component-blame`
- `953 reference-blame`
- `954 design-changelog`
- `955 design-review-workflow`

### 956-975 Design intent preservation

- `956 semantic-value-origin`
- `957 semantic-dimension-token`
- `958 semantic-spacing-token`
- `959 semantic-shape-token`
- `960 semantic-motion-token`
- `961 avoid-magic-value-linter`
- `962 component-magic-value-audit`
- `963 render-magic-value-audit`
- `964 authored-vs-resolved-separation`
- `965 intent-preservation`
- `966 design-intent-diff`
- `967 design-intent-migration`
- `968 design-intent-documentation`
- `969 material-intent-mapping`
- `970 profile-intent-mapping`
- `971 intent-provenance`
- `972 intent-inspector`
- `973 intent-conformance`
- `974 intent-regression`
- `975 semantic-design-contract`

## 12. Conceptual Spec 1000 - Executable Design System

**1000 `executable-design-system`**: the entire design system can be described, validated, compiled, executed, inspected and verified through versioned data/contracts. Material is a profile of this engine, not an accidental dependency smeared across runtime code.

```text
Design Intent -> DesignSystem IR -> Material/Company Profile -> Theme
        -> ComponentDefinition -> InteractionModel + ResolvedSpec
        -> Presentation IR -> QWidget / QStyle / Headless / future adapters

Parallel evidence: Architecture + Material + A11y + Behavior + Performance + Compatibility
```

## 13. Vision 1001-1500 - industrial profiles, governance and verifiable UI platform
### 1001-1025 User/accessibility adaptation profiles

- `1001 user-preference-profile`
- `1002 large-text-profile`
- `1003 high-readability-profile`
- `1004 low-motion-profile`
- `1005 high-contrast-profile`
- `1006 low-vision-profile`
- `1007 motor-accessibility-profile`
- `1008 touch-optimized-profile`
- `1009 keyboard-optimized-profile`
- `1010 dense-professional-profile`
- `1011 runtime-profile-composition`
- `1012 profile-conflict-resolution`
- `1013 user-preference-persistence`
- `1014 os-preference-import`
- `1015 enterprise-policy-overrides`
- `1016 per-window-profile`
- `1017 per-component-adaptation`
- `1018 adaptive-touch-targets`
- `1019 adaptive-font-scale`
- `1020 adaptive-motion`

### 1026-1050 Policy engine

- `1026 ui-policy-model`
- `1027 policy-schema`
- `1028 policy-loader`
- `1029 policy-priority`
- `1030 policy-validation`
- `1031 security-policy`
- `1032 accessibility-policy`
- `1033 brand-policy`
- `1034 performance-policy`
- `1035 plugin-policy`
- `1036 theme-policy`
- `1037 policy-enforcement`
- `1038 policy-diagnostics`
- `1039 policy-provenance`
- `1040 policy-lock`

### 1051-1080 Automotive/HMI profile

- `1051 hmi-profile`
- `1052 large-touch-target-profile`
- `1053 glanceable-information-rules`
- `1054 driver-distraction-policy`
- `1055 driving-state-capabilities`
- `1056 parked-state-capabilities`
- `1057 day-theme`
- `1058 night-theme`
- `1059 tunnel-transition`
- `1060 ambient-light-adaptation`
- `1061 steering-wheel-input`
- `1062 rotary-controller-input`
- `1063 hardware-button-input`
- `1064 touch-input`
- `1065 voice-action-model`
- `1066 frame-budget-hmi`
- `1067 startup-budget-hmi`
- `1068 memory-budget-hmi`
- `1069 animation-budget-hmi`
- `1070 deterministic-hmi-profile`
- `1071 instrument-cluster-profile`
- `1072 center-display-profile`
- `1073 passenger-display-profile`
- `1074 multi-display-theme-sync`
- `1075 display-role-model`

### 1081-1100 Medical/high-reliability profile

- `1081 high-reliability-profile`
- `1082 critical-action-model`
- `1083 critical-action-confirmation`
- `1084 destructive-action-policy`
- `1085 warning-priority-model`
- `1086 alarm-color-policy`
- `1087 alarm-shape-policy`
- `1088 alarm-audio-hook`
- `1089 alarm-acknowledgement-state`
- `1090 alarm-history`
- `1091 data-freshness-indicator`
- `1092 stale-data-state`
- `1093 unknown-data-state`
- `1094 sensor-disconnected-state`
- `1095 data-quality-indicator`
- `1096 critical-readability-test`
- `1097 critical-contrast-test`
- `1098 critical-layout-lock`
- `1099 critical-action-audit-log`
- `1100 high-reliability-evidence-report`

### 1101-1125 Multi-display/cockpit/control-room

- `1101 display-role`
- `1102 display-capabilities`
- `1103 display-density-profile`
- `1104 display-distance-profile`
- `1105 display-input-capabilities`
- `1106 theme-sync`
- `1107 theme-independent-display`
- `1108 cross-display-navigation`
- `1109 cross-display-focus-policy`
- `1110 cross-display-drag`
- `1111 display-loss-recovery`
- `1112 display-hotplug`
- `1113 display-reassignment`
- `1114 display-layout-persistence`
- `1115 display-session-restore`
- `1116 control-room-profile`
- `1117 wallboard-profile`
- `1118 operator-console-profile`
- `1119 kiosk-profile`
- `1120 public-display-profile`

### 1126-1150 Collaborative Theme Studio

- `1126 theme-project`
- `1127 theme-user`
- `1128 theme-role`
- `1129 theme-permission`
- `1130 live-theme-session`
- `1131 live-token-edit`
- `1132 presence`
- `1133 comments`
- `1134 review-thread`
- `1135 design-change-request`
- `1136 theme-branch`
- `1137 theme-merge`
- `1138 semantic-conflict`
- `1139 theme-review`
- `1140 design-approval`
- `1141 design-release`
- `1142 theme-release-channel`
- `1143 development-channel`
- `1144 production-channel`
- `1145 design-audit-log`
- `1146 design-history`
- `1147 design-rollback`
- `1148 design-release-signature`
- `1149 design-environment-promotion`
- `1150 collaborative-theme-studio`

### 1151-1175 Secure design hot patching

- `1151 runtime-design-package`
- `1152 package-signature`
- `1153 package-compatibility-check`
- `1154 package-schema-check`
- `1155 package-rollback`
- `1156 staged-theme-update`
- `1157 canary-theme-update`
- `1158 design-package-channel`
- `1159 offline-update`
- `1160 enterprise-update`
- `1161 hot-theme-reload-production`
- `1162 hot-asset-update`
- `1163 hot-token-update`
- `1164 no-code-component-config-update`

### 1176-1200 Capability negotiation

- `1176 capability-id`
- `1177 capability-set`
- `1178 capability-query`
- `1179 capability-version`
- `1180 platform-capabilities`
- `1181 render-capabilities`
- `1182 input-capabilities`
- `1183 accessibility-capabilities`
- `1184 color-capabilities`
- `1185 font-capabilities`
- `1186 component-capability-requirements`
- `1187 feature-fallback`
- `1188 degraded-presentation`
- `1189 unsupported-feature-diagnostic`
- `1190 capability-negotiation`
- `1191 plugin-capability-negotiation`
- `1192 remote-capability-negotiation`
- `1193 design-package-capability-check`

### 1201-1225 Spatial/XR research

- `1201 spatial-ui-profile`
- `1202 spatial-density`
- `1203 viewing-distance`
- `1204 angular-size`
- `1205 depth-layer`
- `1206 spatial-focus`
- `1207 ray-input`
- `1208 gaze-input-research`
- `1209 controller-input`
- `1210 spatial-pointer`
- `1211 spatial-surface`
- `1212 spatial-dialog`
- `1213 spatial-tooltip`
- `1214 spatial-navigation`
- `1215 spatial-accessibility-research`

### 1226-1250 Static/print/export rendering

- `1226 static-render-backend`
- `1227 pdf-render-backend`
- `1228 printer-render-backend`
- `1229 vector-export`
- `1230 svg-export-research`
- `1231 print-color-profile`
- `1232 print-typography`
- `1233 print-shadows`
- `1234 print-layout`
- `1235 page-break-policy`
- `1236 high-resolution-export`
- `1237 transparent-export`
- `1238 component-export-api`
- `1239 gallery-export`
- `1240 theme-specimen-export`

### 1251-1275 Deterministic scheduler

- `1251 clock-interface`
- `1252 real-clock`
- `1253 test-clock`
- `1254 manual-clock`
- `1255 timer-abstraction`
- `1256 animation-scheduler`
- `1257 debounce-scheduler`
- `1258 timeout-scheduler`
- `1259 deterministic-tooltip`
- `1260 deterministic-snackbar`
- `1261 deterministic-menu-delay`
- `1262 deterministic-longpress`
- `1263 time-freeze`
- `1264 time-step`
- `1265 time-warp`
- `1266 animation-seek`
- `1267 temporal-test-runner`
- `1268 temporal-invariant`
- `1269 temporal-regression`
- `1270 temporal-trace`

### 1276-1300 Digital UI Twin

- `1276 ui-snapshot-model`
- `1277 ui-state-fixture`
- `1278 fixture-data-model`
- `1279 fixture-navigation`
- `1280 fixture-theme`
- `1281 ui-twin-runtime`
- `1282 ui-twin-headless`
- `1283 ui-twin-gallery`
- `1284 ui-twin-screenshot`
- `1285 production-state-export`
- `1286 state-sanitization`
- `1287 anonymized-state-fixture`
- `1288 ui-twin-regression`
- `1289 ui-twin-accessibility`
- `1290 ui-twin-performance`

### 1301-1325 Hardware-in-loop research

- `1301 hardware-test-agent`
- `1302 device-manifest`
- `1303 device-capabilities`
- `1304 device-farm`
- `1305 hardware-deploy`
- `1306 hardware-launch`
- `1307 hardware-screenshot`
- `1308 hardware-input-injection`
- `1309 touch-injection`
- `1310 rotary-injection`
- `1311 hardware-button-injection`
- `1312 hardware-frame-timing`
- `1313 hardware-memory`
- `1314 hardware-startup-time`
- `1315 hardware-visual-conformance`
- `1316 hardware-color-calibration`
- `1317 hardware-dpi-calibration`
- `1318 hardware-font-calibration`

### 1326-1350 Advanced color science

- `1326 color-management-engine`
- `1327 display-color-profile`
- `1328 color-space-conversion`
- `1329 srgb-profile`
- `1330 display-p3-profile`
- `1331 bt2020-profile`
- `1332 hdr-theme-profile`
- `1333 sdr-fallback`
- `1334 hdr-luminance-token`
- `1335 tone-mapping`
- `1336 perceptual-color-difference`
- `1337 color-blindness-simulation`
- `1338 protanopia-simulation`
- `1339 deuteranopia-simulation`
- `1340 tritanopia-simulation`
- `1341 color-accessibility-report`
- `1342 gamut-clipping-report`
- `1343 color-space-conformance`

### 1351-1375 Content-aware components

- `1351 content-type`
- `1352 content-semantics`
- `1353 input-content-type`
- `1354 sensitive-content`
- `1355 secret-content`
- `1356 copy-policy`
- `1357 paste-policy`
- `1358 autofill-policy`
- `1359 password-policy`
- `1360 redaction-policy`

### 1376-1400 Privacy-aware diagnostics

- `1376 diagnostic-data-classification`
- `1377 sensitive-property`
- `1378 secret-property`
- `1379 redacted-value`
- `1380 inspector-redaction`
- `1381 trace-redaction`
- `1382 bug-report-redaction`
- `1383 ui-snapshot-redaction`
- `1384 remote-inspection-redaction`
- `1385 diagnostic-policy`
- `1386 export-policy`
- `1387 screenshot-redaction`
- `1388 text-redaction`

### 1401-1425 Legacy/live migration

- `1401 legacy-widget-inventory`
- `1402 legacy-style-inventory`
- `1403 qstylesheet-inventory`
- `1404 custom-widget-inventory`
- `1405 materialization-score`
- `1406 migration-readiness-report`
- `1407 component-mapping`
- `1408 qpushbutton-migration`
- `1409 qlineedit-migration`
- `1410 qcombobox-migration`
- `1411 qmenu-migration`
- `1412 stylesheet-to-token-analysis`
- `1413 stylesheet-conflict-detection`
- `1414 qpalette-migration`
- `1415 qstyle-migration`
- `1416 partial-migration-mode`
- `1417 legacy-material-coexistence`
- `1418 migration-visual-diff`
- `1419 migration-behavior-diff`
- `1420 migration-accessibility-diff`

### 1426-1450 Automated design QA

- `1426 design-rule-engine`
- `1427 spacing-consistency-rule`
- `1428 shape-consistency-rule`
- `1429 color-role-consistency`
- `1430 typography-role-consistency`
- `1431 component-usage-rule`
- `1432 action-hierarchy-rule`
- `1433 form-structure-rule`
- `1434 dialog-action-rule`
- `1435 navigation-consistency-rule`
- `1436 accessibility-rule-pack`
- `1437 material-rule-pack`
- `1438 desktop-rule-pack`
- `1439 company-rule-pack`
- `1440 design-rule-report`
- `1441 design-rule-suppression`
- `1442 design-rule-baseline`

### 1451-1475 Organizational design governance

- `1451 design-system-consumer-manifest`
- `1452 consumer-version`
- `1453 consumer-capability`
- `1454 component-usage-report`
- `1455 token-usage-report`
- `1456 deprecated-token-usage`
- `1457 deprecated-component-usage`
- `1458 migration-progress`
- `1459 fleet-conformance`
- `1460 fleet-theme-version`
- `1461 fleet-component-version`
- `1462 organization-dashboard`
- `1463 design-system-adoption-score`
- `1464 consumer-health-score`

### 1476-1493 UI supply chain

- `1476 design-bom`
- `1477 component-bom`
- `1478 token-bom`
- `1479 asset-bom`
- `1480 design-provenance`
- `1481 reference-provenance`
- `1482 theme-provenance`
- `1483 component-provenance`
- `1484 design-package-hash`
- `1485 component-hash`
- `1486 theme-hash`
- `1487 reference-hash`
- `1488 ui-release-attestation`
- `1489 design-attestation`
- `1490 conformance-attestation`
- `1491 accessibility-attestation`
- `1492 ui-sbom-extension`
- `1493 ui-provenance-report`

### 1500 Verifiable UI Platform

**1500 `verifiable-ui-platform`**: from design intent and component definitions, construct a Qt UI, explain every resolution decision, verify constraints/behavior/accessibility/performance/security/compatibility, and reproduce the evidence.

## 14. Context architecture implied by the ultimate vision

Long term, do not make `Theme` the only resolver input. Preserve a path toward:

```text
ComponentDefinition
  + AuthoredSpec
  + ComponentState / InteractionState
  + DesignContext
      - Material/Profile
      - Theme/Brand
      - Density
      - Accessibility/User preference
      - Policies
  + EnvironmentContext
      - DPI/screen
      - locale/layout direction
      - platform/input/render capabilities
      - power/render quality
  -> ResolvedSpec -> LayoutSpec -> RenderSpec
```

This is a direction, not a requirement to introduce giant context objects in 0.5. Interfaces should expose only the facets required by a resolver and remain deterministic.

## 15. Architecture properties to protect NOW

- ThemeModel remains frontend-independent and QWidget-free.
- ResolvedSpec never stores QWidget/QPainter/QWindow implementation pointers.
- Typed semantic IDs are stable and persistence mappings explicit.
- Semantic token identity is preserved after value resolution (e.g. Shape.Full -> 20 retains provenance when diagnostics are enabled).
- Component business state is separated from interaction state.
- Resolver inputs become explicit and hidden global state disappears.
- ComponentRegistry remains extensible toward ComponentDefinition.
- QObject/pointer ownership is explicit for future Python/FFI bindings.
- Model snapshots are immutable-friendly; pure compilation/validation can later run off GUI thread.
- External libraries (ADS etc.) integrate only through optional adapters.
- Every structured format carries its own schema/protocol version.
- Temporal behavior can move toward injectable clocks/schedulers.
- Capabilities are modeled explicitly rather than scattered throughout high-level code.
- Diagnostics/provenance are possible without exposing secrets by default.
- One canonical source of truth drives generated docs/gallery/test matrices/maturity where possible.

## 16. Spec Kit decomposition policy for the vision backlog

When a Vision Spec enters a milestone:
1. Allocate/confirm a concrete feature number and slug; do not create parallel duplicate specs.
2. Link it back to this Vision Master and the 628-task master plan when relevant.
3. Create `spec.md` with outcomes, scope and acceptance scenarios.
4. Run clarification before architecture is frozen.
5. Create `plan.md` with constitution, compatibility, API/ABI, Material, a11y, performance and migration gates.
6. Create contracts/schemas/data model only where they are real deliverables.
7. Create atomic `tasks.md` with file paths, dependencies, tests and acceptance evidence.
8. Run `/speckit.analyze`; no critical inconsistency before implementation.
9. Implement in short coherent steps; build/test after each migration step.
10. Run `/speckit.converge`; only then mark DONE.

## 17. Coverage checklist - directions explicitly preserved

- [x] Material 3 Classic and Material 3 Expressive
- [x] structural/visual/motion conformance comparator
- [x] ThemeModel/ThemeIO/ThemeRuntime
- [x] typed tokens/DTCG/multibrand
- [x] ResolvedSpec/LayoutSpec/RenderSpec direction
- [x] accessibility/keyboard/RTL/IME/i18n
- [x] high-DPI/multi-screen/color spaces/fonts
- [x] desktop Model/View/data components
- [x] QStyle/delegates/Designer/.ui
- [x] PySide6/Shiboken/Python async/testing/deployment
- [x] FFI/Rust/.NET/Lua research
- [x] Qt Advanced Docking System and general third-party adapters
- [x] plugin/package ecosystem
- [x] Theme Studio/Figma/design roundtrip
- [x] WASM online gallery
- [x] design-system compiler/IR/component schema
- [x] generated docs/tests/gallery/bindings
- [x] state machines/formal behavior/model testing
- [x] semantic UI tree
- [x] time-travel debugger/runtime inspector/provenance
- [x] performance/memory/energy/render quality
- [x] threading/immutable snapshots
- [x] embedded/MCU/safety-oriented research
- [x] multi-process/Remote Objects/gRPC/Protobuf research
- [x] HMI/automotive/medical/high-reliability/multi-display profiles
- [x] spatial/XR research
- [x] print/PDF/static rendering
- [x] digital UI twin/hardware-in-loop
- [x] privacy/redaction/observability opt-in
- [x] legacy migration/codemods
- [x] organizational design governance/UI supply chain
- [x] schema/protocol governance
- [x] zero manual drift and release evidence bundles
- [x] full Qt Quick/QML frontend strategy and Qt Quick Controls styling
- [x] Qt Linguist translation source/build/package lifecycle
- [x] native shell/window/platform adapters
- [x] crash/hang/post-mortem diagnostics and debug symbol lifecycle
- [x] CMake/package-manager distribution ecosystem
- [x] host QPalette/QStyle/QProxyStyle/QSS coexistence
- [x] advanced input devices: touch/pen/trackpad/gamepad/rotary/haptics
- [x] extension trust/sandbox boundaries
- [x] test effectiveness/coverage/mutation/flakiness governance
- [x] standards/compliance evidence mapping without unearned certification claims
- [x] explicit third-party adapter portfolio beyond ADS
- [x] maintainer continuity and bus-factor governance
- [x] dependency/toolchain EOL watch
- [x] deprecation lifecycle and expiring exception register
- [x] release-signing key lifecycle and disaster recovery
- [x] technology radar and research-to-product promotion
- [x] Vision -> Requirement -> Task -> Test -> Release evidence traceability
- [x] stable Vision ID / taxonomy freeze policy

## 18. Coverage Audit Delta v2.1 - explicit blind spots closed

This section was added after a mechanical audit against the original 628-task master plan and the full roadmap discussion.
These are not requirements for 1.0 by default. They are Vision Specs that close areas which were only implicit or under-specified.

### 1501-1525 Full Qt Quick / QML frontend strategy

The design-system core must not assume that QWidget is the only future presentation frontend. Qt Widgets remains the primary product surface for 1.0, while the architecture preserves an optional Qt Quick frontend.

- `1501 qml-frontend-contract`
- `1502 qml-qt6-only-policy`
- `1503 qml-module-packaging`
- `1504 qml-type-registration`
- `1505 qml-theme-bridge-v2`
- `1506 qml-design-context`
- `1507 qml-token-access`
- `1508 qml-component-state-bridge`
- `1509 qml-resolved-spec-bridge`
- `1510 qtquick-controls-style`
- `1511 qtquick-template-mapping`
- `1512 qml-component-parity-matrix`
- `1513 qml-accessibility-parity`
- `1514 qml-keyboard-parity`
- `1515 qml-rtl-parity`
- `1516 qml-motion-parity`
- `1517 qml-visual-conformance`
- `1518 qml-structural-conformance`
- `1519 qml-performance-baselines`
- `1520 qml-hot-reload-development`
- `1521 qml-python-interoperability`
- `1522 qml-deployment-validation`
- `1523 qml-gallery`
- `1524 qt-labs-stylekit-adapter-research`
- `1525 widgets-quick-shared-reference-suite`

**Invariant:** ThemeModel/DesignSystem IR/Material reference data do not acquire a Qt Quick dependency merely because a QML frontend exists.

### 1526-1545 Translation delivery / Qt Linguist pipeline

Deep i18n behavior is not enough; the project also needs an explicit translation asset lifecycle for library-owned user-visible strings, accessibility labels and examples/tooling.

- `1526 translatable-string-ownership`
- `1527 translation-id-policy`
- `1528 qobject-tr-policy`
- `1529 translator-comment-policy`
- `1530 plural-form-policy`
- `1531 qt-linguist-cmake-pipeline`
- `1532 lupdate-ci`
- `1533 lrelease-packaging`
- `1534 ts-source-layout`
- `1535 qm-resource-layout`
- `1536 locale-fallback-chain`
- `1537 runtime-language-switch`
- `1538 translated-accessibility-labels`
- `1539 translated-tooltips-status-text`
- `1540 translation-completeness-report`
- `1541 missing-translation-diagnostics`
- `1542 pseudo-translation-pipeline`
- `1543 integration-module-translations`
- `1544 gallery-documentation-localization-research`
- `1545 translation-release-gate`

### 1546-1570 Native shell and platform integration

Platform-specific shell integration is a boundary adapter concern and must never leak into ThemeModel/Specs.

- `1546 native-shell-adapter-contract`
- `1547 native-window-capability-model`
- `1548 native-titlebar-policy`
- `1549 frameless-window-integration`
- `1550 window-decoration-theme-policy`
- `1551 system-accent-color-adapter`
- `1552 system-high-contrast-adapter`
- `1553 native-menu-bar-policy`
- `1554 native-system-menu-policy`
- `1555 native-dialog-policy`
- `1556 native-file-dialog-policy`
- `1557 native-color-font-dialog-policy`
- `1558 system-tray-integration`
- `1559 desktop-notification-integration`
- `1560 taskbar-dock-integration-research`
- `1561 native-window-state-restore`
- `1562 platform-shortcut-policy`
- `1563 window-activation-focus-policy`
- `1564 native-drag-window-policy`
- `1565 native-screen-workarea-policy`
- `1566 windows-shell-certification`
- `1567 macos-shell-certification`
- `1568 x11-shell-certification`
- `1569 wayland-shell-certification`
- `1570 native-interface-version-isolation`

**Invariant:** unstable/version-sensitive native API use is contained behind platform adapters and compatibility gates.

### 1571-1590 Crash, hang and post-mortem diagnostics

QtMaterial3 must integrate with application crash tooling without owning or transmitting crash data by default.

- `1571 crash-diagnostics-hook`
- `1572 build-id-manifest`
- `1573 debug-symbol-packaging`
- `1574 debug-symbol-publication-policy`
- `1575 source-indexing-research`
- `1576 stacktrace-integration-hook`
- `1577 crash-safe-log-ring`
- `1578 ui-trace-ring-buffer`
- `1579 last-theme-revision-dump`
- `1580 last-focus-state-dump`
- `1581 last-component-state-dump`
- `1582 gui-event-loop-latency-monitor`
- `1583 ui-freeze-watchdog`
- `1584 hang-dump-integration-hook`
- `1585 crash-reproducer-bundle`
- `1586 postmortem-redaction`
- `1587 minidump-attachment-policy`
- `1588 crash-fixture-testing`
- `1589 hang-fixture-testing`
- `1590 diagnostics-no-network-default`

### 1591-1610 Distribution and package-manager ecosystem

The CMake install package remains canonical. Ecosystem recipes are downstream distribution surfaces validated against it.

- `1591 distribution-contract`
- `1592 source-archive`
- `1593 reproducible-source-archive`
- `1594 cmake-config-package-certification`
- `1595 pkg-config-research`
- `1596 cpack-package-matrix`
- `1597 package-manager-recipe-contract`
- `1598 conan-recipe`
- `1599 vcpkg-port`
- `1600 linux-package-research`
- `1601 homebrew-formula-research`
- `1602 windows-package-research`
- `1603 package-version-mapping`
- `1604 package-feature-mapping`
- `1605 package-license-metadata`
- `1606 package-sbom-linkage`
- `1607 package-signature-policy`
- `1608 package-install-consumer-ci`
- `1609 package-upgrade-downgrade-tests`
- `1610 distribution-dashboard`

### 1611-1630 Host application coexistence / QStyle / QSS policy

Gradual adoption requires deterministic rules when QtMaterial3 coexists with an application's existing palette, QStyle, QProxyStyle, Qt Style Sheets and custom widgets.

- `1611 host-style-coexistence-contract`
- `1612 qpalette-precedence`
- `1613 qstyle-precedence`
- `1614 qproxystyle-coexistence`
- `1615 qstylesheet-precedence`
- `1616 qss-conflict-detector`
- `1617 qss-material-token-bridge-research`
- `1618 global-style-side-effect-policy`
- `1619 attach-detach-theme-adapter`
- `1620 restore-host-style`
- `1621 mixed-native-material-widget-tree`
- `1622 multiple-design-contexts-one-process`
- `1623 per-window-design-context`
- `1624 embedded-third-party-widget-zone`
- `1625 partial-adoption-mode`
- `1626 legacy-qpalette-migration`
- `1627 legacy-qstyle-migration`
- `1628 legacy-qss-migration`
- `1629 coexistence-diagnostics`
- `1630 coexistence-reference-application`

### 1631-1650 Advanced input-device ecosystem

The interaction model must not collapse every pointer into a mouse.

- `1631 input-device-capability-registry`
- `1632 mouse-capability-profile`
- `1633 touch-capability-profile`
- `1634 pen-tablet-capability-profile`
- `1635 trackpad-capability-profile`
- `1636 high-precision-wheel-input`
- `1637 gesture-source-classification`
- `1638 pointer-pressure-research`
- `1639 pointer-tilt-research`
- `1640 hover-distance-research`
- `1641 stylus-barrel-button-policy`
- `1642 gamepad-navigation-research`
- `1643 rotary-controller-general-profile`
- `1644 hardware-key-input-profile`
- `1645 haptic-feedback-hook`
- `1646 input-device-hotplug`
- `1647 modality-transition-tests`
- `1648 mixed-input-sequence-tests`
- `1649 input-accessibility-interaction`
- `1650 input-device-certification-matrix`

### 1651-1670 Extension trust and sandbox boundaries

Data packages, native plugins and scripts have different trust levels and must not be conflated.

- `1651 extension-trust-model`
- `1652 data-vs-code-package-separation`
- `1653 untrusted-theme-input-policy`
- `1654 untrusted-asset-input-policy`
- `1655 native-plugin-trust-policy`
- `1656 python-script-trust-policy`
- `1657 extension-permission-capabilities`
- `1658 filesystem-access-policy`
- `1659 network-access-policy`
- `1660 plugin-signature-verification`
- `1661 package-path-traversal-hardening`
- `1662 archive-bomb-hardening`
- `1663 decompression-size-limits`
- `1664 resource-count-limits`
- `1665 out-of-process-plugin-research`
- `1666 plugin-sandbox-research`
- `1667 extension-revocation-policy`
- `1668 compromised-extension-diagnostics`
- `1669 extension-security-test-suite`
- `1670 extension-security-release-gate`

### 1671-1685 Test effectiveness and flake control

A large test suite is only valuable if its ability to detect defects is measured.

- `1671 test-coverage-policy`
- `1672 line-coverage-report`
- `1673 branch-coverage-report`
- `1674 public-api-test-coverage`
- `1675 requirement-to-test-traceability`
- `1676 component-definition-test-coverage`
- `1677 mutation-testing-research`
- `1678 mutation-score-baseline`
- `1679 flaky-test-detector`
- `1680 flaky-test-quarantine-policy`
- `1681 retry-policy-no-hidden-failures`
- `1682 deterministic-seed-capture`
- `1683 test-duration-budget`
- `1684 slow-test-classification`
- `1685 test-effectiveness-dashboard`

### 1686-1700 Standards / compliance evidence mapping

QtMaterial3 can produce evidence mapped to applicable requirements, but MUST NOT claim certification that has not actually been performed by the appropriate process/body.

- `1686 compliance-evidence-model`
- `1687 accessibility-standard-mapping`
- `1688 keyboard-standard-mapping`
- `1689 contrast-requirement-mapping`
- `1690 localization-requirement-mapping`
- `1691 platform-accessibility-evidence`
- `1692 security-practice-evidence`
- `1693 open-source-license-compliance`
- `1694 spdx-reuse-evidence`
- `1695 third-party-notice-generation`
- `1696 asset-font-icon-license-evidence`
- `1697 vulnerability-disclosure-policy`
- `1698 security-advisory-process`
- `1699 compliance-report-versioning`
- `1700 no-unearned-certification-claims`

### 1701-1725 Third-party integration reference portfolio

ADS remains the first reference pilot. These entries prevent the generic integration story from remaining theoretical.

- `1701 kddockwidgets-adapter-research`
- `1702 qwt-adapter`
- `1703 qcustomplot-adapter`
- `1704 qscintilla-adapter`
- `1705 property-browser-adapter`
- `1706 qt-graphs-adapter-research`
- `1707 opengl-viewer-integration-contract`
- `1708 custom-render-widget-integration-contract`
- `1709 third-party-qpalette-adapter`
- `1710 third-party-qstyle-adapter`
- `1711 third-party-qss-adapter`
- `1712 third-party-icon-adapter`
- `1713 third-party-font-adapter`
- `1714 third-party-density-adapter`
- `1715 third-party-focus-adapter`
- `1716 third-party-a11y-audit`
- `1717 third-party-runtime-theme-switch`
- `1718 third-party-version-probe`
- `1719 third-party-compatibility-manifest`
- `1720 third-party-integration-ci-template`
- `1721 third-party-gallery-template`
- `1722 third-party-conformance-template`
- `1723 third-party-python-parity-template`
- `1724 third-party-adapter-contribution-guide`
- `1725 third-party-integration-certification-matrix`

## 19. Audit conclusion

After v2.1, the vision master explicitly covers the original product roadmap plus the later architecture/platform/research branches discussed in the conversation. "Exhaustive" means exhaustive with respect to the identified capability taxonomy and current project vision; it is not a promise that no new technology, Material revision, Qt capability or downstream requirement can ever appear.

The canonical review rule is therefore:
- new idea -> classify against sections 1-18;
- if already covered, map it to the existing epic;
- if genuinely new, append a new Vision Spec family without renumbering/reusing stable existing IDs;
- periodically run a roadmap coverage audit before each major milestone.

## 20. Project Sustainability, Operations and Vision Traceability

This section closes the long-term operational/governance gap. These are Vision Specs unless explicitly promoted to a milestone.

### 1726-1750 Project sustainability and operations

- `1726 maintainer-continuity-policy`
- `1727 bus-factor-analysis`
- `1728 maintainer-onboarding`
- `1729 maintainer-offboarding`
- `1730 dependency-eol-watch`
- `1731 qt-eol-watch`
- `1732 compiler-eol-watch`
- `1733 cmake-eol-watch`
- `1734 python-pyside-shiboken-eol-watch`
- `1735 deprecation-lifecycle`
- `1736 deprecation-dashboard`
- `1737 removal-calendar`
- `1738 compatibility-exception-register`
- `1739 release-signing-key-rotation`
- `1740 release-key-revocation`
- `1741 plugin-signing-key-rotation`
- `1742 release-infrastructure-disaster-recovery`
- `1743 reference-baseline-backup`
- `1744 release-artifact-archival`
- `1745 build-environment-archival`
- `1746 security-incident-response`
- `1747 critical-regression-response`
- `1748 support-severity-policy`
- `1749 maintenance-health-dashboard`
- `1750 project-sustainability-gate`

#### Exception Register contract

Temporary exceptions to architecture, compatibility or quality rules MUST be explicit, owned and expiring.

Example:

```yaml
exception: ARCH-0042
rule: renderer-must-not-read-theme
scope: LegacyWidget
reason: migration-blocker
owner: maintainer
introduced: 2.1
expires: 2.2
tracking: issue-1234
```

Required behavior:
- exceptions have stable IDs;
- every exception has owner, reason, scope and expiry/review point;
- CI reports expired exceptions;
- exceptions cannot silently grow the architecture baseline;
- removing an exception is preferred to extending it.

### 1751-1755 Technology radar and promotion lifecycle

- `1751 technology-radar`
- `1752 technology-adoption-criteria`
- `1753 experimental-feature-lifecycle`
- `1754 research-to-product-promotion`
- `1755 abandoned-experiment-cleanup`

Recommended states:

```text
ADOPT   - supported product direction
TRIAL   - controlled implementation/prototype
ASSESS  - research only, architecture should not depend on it
HOLD    - intentionally not pursued
```

Research features MUST NOT become product dependencies merely because an experiment exists.

## 21. Vision -> Product Traceability Contract

The ultimate roadmap is intentionally much larger than the executable roadmap. To keep it actionable, every promoted capability follows this chain:

```text
Vision Capability
      ↓
Capability Epic
      ↓
Milestone
      ↓
Spec Kit Feature
      ↓
Requirement
      ↓
Task
      ↓
Commit / PR
      ↓
Automated or Manual Evidence
      ↓
Release Evidence Bundle
```

Minimum traceability rules:
- a Vision Spec entering implementation MUST be promoted into a concrete Spec Kit feature directory;
- requirements MUST have stable IDs;
- implementation tasks MUST reference requirements;
- tests/evidence MUST reference the requirement or task they prove;
- release gates MUST be able to identify which requirements are satisfied;
- generated reports SHOULD link back to source specifications;
- no research epic is considered implemented merely because prototype code exists.

Example:

```text
Vision: Python SDK (320-372)
  → Milestone: 1.2
    → Feature: python-binding-foundation
      → Requirement: PY-FR-004
        → Task: T018
          → PR/commit
            → tst_python_ownership
              → QtMaterial3 1.2 release evidence
```

## 22. Stable ID and Taxonomy Freeze Policy

From v2.2 onward:
- existing Vision Spec IDs are stable and MUST NOT be renumbered for cosmetic reasons;
- deleted/abandoned concepts keep their historical ID and are marked `HOLD`, `SUPERSEDED` or `REMOVED`;
- new concepts are appended with new IDs or nested under an existing capability epic;
- milestones do not determine ID numbering;
- executable feature directories may use their own feature sequence while retaining links to Vision IDs;
- a roadmap coverage audit is required before every major milestone and before 1.0/2.0 release planning.

This prevents historical links, issues, ADRs and release evidence from becoming invalid when the roadmap evolves.

## 23. Final Coverage Status

After the v2.2 sustainability/operations audit, no major capability family is currently known to be missing from the QtMaterial3 long-term vision.

The roadmap explicitly covers:
- architecture and dependency governance;
- ThemeModel, ThemeIO, ThemeRuntime, tokens and design contexts;
- Material 3 Classic, Expressive and Adaptive;
- component families and desktop/data workloads;
- accessibility, keyboard, RTL, internationalization and translation assets;
- DPI, multi-screen, native shell and advanced input;
- visual/structural/motion/behavior conformance;
- performance, memory, energy and long-running stability;
- robustness, fault injection, fuzzing and formal behavior research;
- API/ABI compatibility and migration;
- CMake/install/package-manager/release distribution;
- docs, Gallery, Theme Studio and Designer;
- Python/PySide/Shiboken and future language/FFI surfaces;
- QML/Qt Quick frontend strategy;
- QStyle/QSS/legacy application coexistence;
- third-party adapters including Qt Advanced Docking System;
- plugins, packages and extension trust;
- DTCG/Figma/design-system compiler/IR;
- Render IR, headless renderers and experimental alternative backends;
- Semantic UI, state machines, model-based/metamorphic tests and time-travel diagnostics;
- WASM, multi-process, remote UI and distributed tooling;
- embedded/HMI/high-reliability/safety-oriented research profiles;
- multi-display, spatial/XR, static/PDF rendering and hardware-in-loop research;
- privacy-aware diagnostics, organizational governance and UI supply-chain evidence;
- project sustainability, EOL tracking, exception governance and technology radar;
- complete Vision-to-Release traceability.

"Complete" here means complete against the current capability taxonomy and known project intent. It does not mean the roadmap is permanently closed to future Qt, Material, platform or user requirements.
