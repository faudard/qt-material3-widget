# Requirements Checklist

- [x] Public API surface is explicit, not inferred from directory installation.
- [x] Every source header is classified exactly once.
- [x] Private naming/path convention cannot enter public manifest.
- [x] Public headers cannot depend on private headers.
- [x] Public headers cannot reach into src/.
- [x] Install package contains only public manifest headers.
- [x] Consumer matrix validates package header contents.
- [x] Self-contained compilation uses explicit public manifest.
- [x] Manifest drift fails CI.
- [x] Public/private distinction is not confused with ABI freeze.
- [ ] Actual repository manifest reviewed.
- [ ] No real public→private include leak remains.
- [ ] Qt5/Qt6 installed consumers are green.
