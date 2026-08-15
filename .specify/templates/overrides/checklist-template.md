# [CHECKLIST TYPE] Requirements Checklist: [FEATURE NAME]

**Purpose**: reviewer-owned requirements-quality gate.
`[x]` means the requirement is precise/testable; it does not mean implementation is done.

## Architecture
- [ ] CHK001 Layer ownership is explicit.
- [ ] CHK002 Forbidden dependencies are explicitly excluded.
- [ ] CHK003 Migration does not create indefinite parallel legacy/new paths.

## Compatibility / Verification
- [ ] CHK004 Qt/toolchain impact is explicit.
- [ ] CHK005 Install/export/header impact is explicit.
- [ ] CHK006 Every success criterion is measurable.
- [ ] CHK007 Required tests/evidence are named.
- [ ] CHK008 Architecture-debt movement is quantified when relevant.

## Material / UI
- [ ] CHK009 Reference profile/version is specified when applicable.
- [ ] CHK010 Accessibility/keyboard/RTL/DPI requirements are explicit when applicable.

## API / Performance
- [ ] CHK011 Public API/ABI impact is explicit.
- [ ] CHK012 Performance/cache/allocation impact is explicit.
