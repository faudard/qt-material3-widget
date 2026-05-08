# Public widget PIMPL contract

Public widget headers should expose the stable user-facing API only. Rendering caches,
resolved specs, child widgets, controllers, paths, timers, animation state and layout
state belong in a private implementation object defined in the matching `.cpp` file.

Required shape:

```cpp
#include <memory>

class QtMaterialFooPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFoo : public QWidget
{
    Q_OBJECT

public:
    explicit QtMaterialFoo(QWidget *parent = nullptr);
    ~QtMaterialFoo() override;

private:
    std::unique_ptr<QtMaterialFooPrivate> d_ptr;
};
```

The destructor must be declared in the header but defined out-of-line in the `.cpp`:

```cpp
struct QtMaterialFooPrivate
{
    // specs, caches, paths, controllers, child widgets, state flags...
};

QtMaterialFoo::~QtMaterialFoo() = default;
```

Do not write `~QtMaterialFoo() = default;` in the public header when the class owns a
`std::unique_ptr<QtMaterialFooPrivate>`. Defaulting the destructor in the header can
instantiate `std::default_delete<QtMaterialFooPrivate>` while the private type is still
incomplete.

Run the audit manually with:

```bash
python3 tools/check_pimpl_contract.py --root .
```

Apply conservative fixes with:

```bash
python3 tools/check_pimpl_contract.py --root . --fix
```

The audit is intentionally narrow: it only verifies classes that already use
`std::unique_ptr<...Private>`. It is a guardrail for the PIMPL contract, not a full C++
ABI compatibility checker.
