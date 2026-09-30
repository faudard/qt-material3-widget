# Dashboard showcase

`qtmaterial3_dashboard_demo` is the application-level showcase for the widget library. It complements the component gallery by showing the same controls inside a realistic responsive product shell.

## Page map

| Page | Purpose | Main Material surfaces |
| --- | --- | --- |
| Dashboard | KPIs, revenue, charts, tasks and recent orders | Card, ComboBox, Table, Checkbox, FilledTonalButton |
| Analytics | acquisition and audience metrics | Card, LinearProgressIndicator, custom theme-aware charts |
| Orders | searchable/filterable paginated order data | Chip, Table, Pagination, Dialog |
| Customers | CRM metrics and directory | Card, Table |
| Components | compact embedded component examples | SearchBar, Checkbox, Chip, FilledTonalButton |
| Profile | editable account preferences | OutlinedTextField, ComboBox, Switch, RadioButton, Filled/OutlinedButton |
| Pricing | commercial plan selection | SegmentedButton, Card, Chip, Switch, Dialog |
| Application States | loading, empty, error, offline and ready flows | Circular/LinearProgressIndicator, Banner, Switch |
| Showcase Settings | live theme and layout verification | ComboBox, SegmentedButton, Switch |

## Responsive navigation

The example intentionally exercises the navigation family at different widths:

- **Desktop (>= 1200 px):** full application sidebar.
- **Tablet (800–1199 px):** `QtMaterialNavigationRail`.
- **Compact (< 800 px):** top-bar menu opening `QtMaterialNavigationDrawer`.

The active destination remains synchronized across all three navigation surfaces.

## Live theme verification

The **Showcase Settings** page changes the global `ThemeManager` rather than applying example-only colors. It can switch:

- seed color: Indigo, Material Purple, Azure, Amber or Teal;
- mode: Light or Dark;
- contrast: Standard, Medium or High;
- color variant: Tonal or Expressive;
- direction: LTR or RTL.

A live role preview displays Primary, Secondary, Tertiary, Error and Surface colors after every change.

## Deterministic screenshots

The executable exposes a small capture CLI so documentation screenshots can come from the actual Qt application instead of mockups.

```text
--dark
    Start in dark mode.

--rtl
    Start in right-to-left layout.

--page <name>
    dashboard | analytics | orders | customers | components |
    profile | pricing | states | settings

--size <width>x<height>
    Window size used for the showcase. Default: 1440x920.

--screenshot <file>
    Save the rendered window as a PNG and exit.
```

Examples:

```bash
./qtmaterial3_dashboard_demo \
    --page dashboard \
    --size 1440x920 \
    --screenshot dashboard-light.png

./qtmaterial3_dashboard_demo \
    --dark \
    --page dashboard \
    --size 1440x920 \
    --screenshot dashboard-dark.png

./qtmaterial3_dashboard_demo \
    --rtl \
    --page profile \
    --size 1200x800 \
    --screenshot profile-rtl.png
```

When screenshots are added to repository documentation, generate them from a built executable with these options so the images remain traceable to the current implementation.
