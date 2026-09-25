# Component-local theme overrides

`ComponentTokenOverrides` is the typed customization layer between global theme tokens and resolved component specs.

## Precedence

Resolvers apply component IDs from the broadest family to the most specific variant.
For a filled button the contract is:

```text
Material defaults
  -> global Theme tokens
  -> ComponentId::Button
  -> ComponentId::ButtonFilled
  -> explicit component-local concrete values
```

Later component IDs replace earlier values for the same semantic token. Unrelated values are merged.
The order passed to a resolver is therefore part of the component compatibility contract.

Typed maps (`colors`, `typography`, `shapes`, `elevations`, `motion`, `density`, and `iconSizes`) override matching semantic roles.
`stateLayer` replaces the inherited state layer when present. `custom` contains component-specific concrete values and is the final component override layer.

## Serialization

First-party component IDs serialize with canonical names owned by ThemeIO internals.
Applications should use `ThemeSerializer`; the textual mapping helper is not part of the public API.

Unknown third-party component names are kept opaque inside ThemeIO. They survive a Theme JSON read/write round-trip without adding QString-key APIs to ThemeModel.

## Compatibility rule

Compatible additions may add optional component override tokens. Breaking persistence changes require an explicit Theme JSON format-version change.
