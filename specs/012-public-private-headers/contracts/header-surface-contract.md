# Header Surface Contract v1

For the repository include tree:

```text
all headers
=
public manifest
∪
private manifest

public ∩ private = ∅
```

Package installation:

```text
installed qtmaterial headers
=
public manifest
```

Public headers may not include:
- a manifest-private project header;
- any `private/` or `_p.*` project header;
- `src/` implementation paths.

Private headers are unsupported downstream implementation details.

Header classification is an API-surface decision. ABI stability is governed separately.
