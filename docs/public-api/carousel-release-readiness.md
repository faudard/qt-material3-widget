# Carousel release readiness

`QtMaterialCarousel` is promoted to the public release scope as a lightweight Material-style carousel for data/content cards.

## Public contract

The widget provides:

- item management: `addItem`, `insertItem`, `removeItem`, `clear`, `count`;
- item metadata: title, supporting text, icon, enabled state;
- current item state via `currentIndex`;
- wrap-around navigation via `wrapAround`;
- viewport density via `visibleItemCount`;
- activation via mouse, `Space`, `Return`, and `Enter`;
- keyboard navigation via `Left`, `Right`, `Home`, and `End`;
- RTL-aware visual placement and arrow-key semantics;
- accessible summaries for the full carousel and current item.

## Accessibility

`accessibilitySummary()` is kept synchronized with the item count and the current item. Disabled items are exposed through `itemAccessibleText(index)` and skipped by keyboard navigation.

## Release checks

Run:

```bash
cmake --build build --target tst_carousel
ctest --test-dir build -R tst_carousel --output-on-failure
python3 tools/api-check/check_carousel_release.py
```
