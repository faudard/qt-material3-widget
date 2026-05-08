# Public widget ABI audit

The public widget headers under `include/qtmaterial/widgets` should expose API, not implementation state.

Allowed pattern:

```cpp
class QtMaterialFooPrivate;

class QTMATERIAL_WIDGETS_EXPORT QtMaterialFoo : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialFoo(QWidget *parent = nullptr);
    ~QtMaterialFoo() override;

private:
    std::unique_ptr<QtMaterialFooPrivate> d_ptr;
};
```

Implementation details that should remain in the `.cpp` or in a private header:

- resolved component specs such as `ButtonSpec`, `TextFieldSpec`, `NavigationRailSpec`;
- painting/layout caches such as `QPainterPath`, cached `QRectF`, elided text, dirty flags;
- child widgets and popup internals such as `QLineEdit`, `QCalendarWidget`, `QLabel`, `QToolButton`;
- effects and controllers such as ripple, state-layer, transition or shadow controllers;
- internal queues, descriptors, models and navigation state.

Run the audit manually:

```bash
python3 tools/check_public_widget_abi.py --root . --strict
```

Add a temporary baseline only when migrating incrementally:

```bash
python3 tools/check_public_widget_abi.py --root . \
  --baseline tools/public_widget_abi_audit_baseline.json \
  --update-baseline
```

Prefer fixing findings over extending the baseline. The baseline exists to make large migrations reviewable, not to normalize ABI leaks.
