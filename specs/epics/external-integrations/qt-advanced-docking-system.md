# Epic: Qt Advanced Docking System Integration

**Ring:** R1 PRODUCT / optional integration  
**Rule:** ADS is never a dependency of QtMaterial3 Core/Theme/Specs/Widgets.

## Architecture

```text
QtMaterial3 public design/runtime API
        +
Qt Advanced Docking System public API
        -> QtMaterial3IntegrationADS
             - resolved style adapter
             - palette/QSS adapter
             - icon adapter
             - density/state/focus mapping
             - diagnostics/conformance
```

## Capability set

Palette, generated stylesheet, icons, typography, density, state/focus, runtime theme switching, high contrast, RTL, high DPI, drag overlay/preview, floating windows, auto-hide, diagnostics.

## Vision tasks

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

## Acceptance direction

- ADS can be absent and QtMaterial3 builds unchanged.
- Integration target is optional and independently versioned/tested.
- No hard-coded Material colors in ADS integration; use a resolved integration style.
- Runtime theme/density changes do not corrupt ADS layout persistence.
- Windows/macOS/Linux X11/Wayland behavior has an explicit compatibility matrix.
- Gallery contains a workbench/IDE-style integration example.
- Python compatibility is tested when both QtMaterial3 Python and ADS Python bindings are in the supported matrix.
