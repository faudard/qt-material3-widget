# Navigation and desktop certification

The certification sequence is Tabs → Navigation Rail → Menu → Breadcrumb → Command
Palette → SplitView. Existing component suites remain authoritative for their public
API; two additional CTest targets exercise interactions across the family.

## Runtime contracts

| Component | Evidence prepared in `tst_navigation_desktop_certification` |
| --- | --- |
| Tabs | Tab/Backtab traversal through a host form, disabled-tab skipping, visual arrow direction in LTR/RTL, bounded navigation and accessible PageTab selection/focus/disabled states |
| Navigation Rail | Accessible List/ListItem hierarchy, destination names, hit testing, selected/focused/disabled states, accessible press action and all-disabled keyboard behavior |
| Menu | Accessible PopupMenu/MenuItem/Separator hierarchy, shortcut metadata, checkable/checked/disabled states, accessible press action, Home/End/Space/Escape and empty-menu behavior |
| Breadcrumb | Keyboard access to overflow actions and Ctrl+L editing, native editor accessibility, Enter submission, Escape cancellation and focus retained within the component |
| Command Palette | Native accessible result names include command title, secondary text and shortcut; selection and focus are exposed by QListView |
| SplitView | Focusable accessible divider, exact physical keyboard steps in both orientations and LTR/RTL with normal/thin handles, Shift acceleration, min/max constraints, Enter collapse/restore and corrupt-state rejection |

Tabs, Rail and Menu expose each item through an internal accessibility adapter. Its
item QObjects belong to the widget so Qt can invalidate cached accessible interfaces on
destruction. Disabled entries and separators expose no press actions. Accessible press
uses the same selection and Enter activation path as keyboard interaction. Native Qt
accessibility remains responsible for Tabs' scroll/overflow controls, Breadcrumb
buttons/editors, Palette fields and results, and SplitView handles. Tab children report
selection and disabled state explicitly instead of inheriting the bar's state.

Offscreen fixtures prepare window activation explicitly because there is no desktop
window manager to restore it after popups. Subsequent Tab/Backtab, shortcuts, selection
and focus assertions use actual Qt widget state. Palette dismissal restores its invoker;
reopening while visible retains that target and an invoker destroyed meanwhile is safe.
QtTest keeps text reports beside stdout. Windows CI prints failing reports and uploads
them with CTest's last-run log in the `qttest-<matrix-name>` artifact.

## Command provider stress contracts

`tst_commandpalette_stress` covers:

- a 10,000-command provider, filtering and activation of the final command;
- query changes clear old snapshots; filtering assertions wait for the new request's reply;
- 100 synchronous query changes coalesced into the latest debounced request;
- 30 explicit request generations with queued replies arriving in reverse order;
- stale success/error rejection and cancellation accounting;
- provider removal/destruction, palette destruction, hide and reopen with replies pending;
- completion-order independent deduplication and deterministic equal-score ordering;
- invalid provider entries rejected and external model order preserved, including rows without IDs;
- a host-to-palette-to-host journey using Ctrl+K/P, Tab/Backtab, arrows, Home/End,
  PageUp/PageDown, Ctrl+D, Enter and Escape, without mouse input, in LTR and RTL;
- empty and all-disabled result sets that cannot activate a command.

Within a section, provider rows with equal match scores sort by their stable ID using
case-sensitive QString order. Provider entries without IDs or titles are ignored.
External model rows retain the application's order, including rows without IDs. Section
sorting is case-insensitive QString order with a case-sensitive tie break, independent
of the machine locale. Deduplication still prefers the external source model, then the
first registered provider, regardless of reply order. Applications should supply stable,
globally unique IDs to get deterministic ordering, favorites and recent history.

## Visual evidence

`tst_navigation_visual_goldens` selects 27 cases from the existing visual harness:

- Navigation primary matrix: Tabs, Rail and Menu;
- Navigation desktop matrix: Breadcrumb overflow and Palette sections/favorites/recents;
- dedicated SplitView matrix: horizontal/vertical, expanded/collapsed and RTL;
- separate focused fixtures for all six components, including editable location mode.

Each has light/standard, dark/standard and light/high-contrast variants. Focus fixtures
use actual keyboard focus, disable caret blinking, keep the pointer outside the fixture,
and capture the client widget directly. Themes request reduced motion so snapshots
capture end states and explicitly apply it to motion tokens. Tabs suppress indicator
animation under reduced motion even when authored/component durations are nonzero;
layout, resize, RTL and theme changes cancel obsolete geometry animations. Behavioral
tests sample the selected indicator before/after these changes, independently of PNG
approval. Existing desktop scale-factor suites cover fractional DPI separately.

The pinned Ubuntu 24.04 / Qt 6.4.0 / Fusion lane first compares any checked-in references,
then renders these cases in **two independent processes**. Candidate output is redirected
with `QTMATERIAL3_VISUAL_GOLDENS_DIR`, so generation cannot overwrite reviewed source PNGs.
`check_navigation_visual_repeatability.py` requires all 27 cases, identical PNG bytes and
pixel hashes, matching dimensions, the pinned runtime renderer and the same source commit.
The `navigation-desktop-visual-evidence` artifact includes both passes, strict-comparison
artifacts, and the repeatability report. Its `visualReview` value remains `pending`.

After reviewing borders, radii, icons, text clipping, focus rings and RTL, copy approved
PNGs from the first pass's `goldens` directory to `tests/visual/goldens/` and list the
release-critical paths in `tools/release_rules.json`. Existing candidate cases automatically
compare committed references in strict mode. Repeatability is evidence of determinism;
it does not establish Material visual correctness.

## Platform accessibility review

The new suites query real QAccessible interfaces and execute accessible actions. They
do not certify spoken output or the platform bridge. Before claiming complete maturity,
record a manual session with NVDA on Windows (including the primary Qt 5.14.2 lane),
Orca/AT-SPI on Linux and VoiceOver on macOS. Verify item traversal, selected/checked and
disabled announcements, activation, modal focus containment/return, editable location
submission, and divider resize/collapse announcements. Keep unresolved bridge-specific
issues in the registry instead of inferring completion from widget labels.

These C++ suites and visual captures are registered for CI. Local authoring validation
uses repository/tooling/documentation checks without compiling or installing Qt. New
goldens and OS screen-reader sessions remain review work until their evidence exists;
registry scores are not raised merely because these tests have been added.
