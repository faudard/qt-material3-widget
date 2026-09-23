# Public widget PIMPL guidance

Public widget headers should expose stable user-facing API only. Rendering caches,
resolved specs, child widgets, controllers, paths, timers, animation state and
layout state belong in a private implementation object defined in the matching
`.cpp` file.

Typical shape:

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

The destructor is declared in the header and defined out-of-line after the
private type is complete.

## Validation

The historical standalone `check_pimpl_contract.py` integration was removed
because the backing checker no longer existed. Public-header publication and
include boundaries are enforced by the unified API gate:

```bash
python3 tools/check_api_surface.py --root . --scope source
```

PIMPL remains a design rule reviewed with API changes; the API gate does not
claim to be a binary ABI compatibility analyzer.
