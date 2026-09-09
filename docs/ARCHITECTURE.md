# Architecture

## Core rule

HTTP transport, domain/application logic and persistence responsibilities are separate.

```text
routes / API
    ↓
services
    ↓
models + persistence
```

## Responsibilities

### Routes

- Parse HTTP input.
- Invoke forms/validation.
- Call application services.
- Select response / redirect / template.
- Do not own business transactions.

### Services

- Implement use cases.
- Enforce workflow-level business rules.
- Own transaction boundaries.
- Coordinate multiple models/repositories/queries when needed.

### Models

- Represent persistent domain state.
- Enforce local invariants.
- Do not call `db.session.commit()`.
- Do not depend on Flask request globals.

### Queries

- Encapsulate non-trivial read models and filtering.
- Use SQLAlchemy 2.x `select()` style.
- Do not become a generic repository abstraction without a concrete need.

### Forms / schemas

- Validate transport-level input.
- Do not implement workflows.

### Templates

- Presentation only.
- No business queries.

## Application factory

The package exports:

```python
create_app(environment=None, test_config=None)
```

Extensions are created unbound and initialized with `init_app()`.

## Database

SQLite is the default backend.

The application:

- uses SQLAlchemy 2.x;
- uses deterministic SQLAlchemy constraint naming for Alembic;
- explicitly enables `PRAGMA foreign_keys=ON` on SQLite connections;
- never creates or seeds application data at import time.

Database schema changes are performed only through migrations.

## Configuration

Supported environments:

- `development`
- `testing`
- `production`

Production requires an explicit `SECRET_KEY`.

Secrets are never committed.

## Domain modules planned for v1

```text
auth
users
books
authors
catalog/search
personal_library
loans
reservations
reviews
notifications
api
admin
audit
```

Domain modules may depend on common infrastructure, but should avoid circular
dependencies between web blueprints.

## Testing strategy

```text
unit
integration
functional
api
legacy-contract
```

Legacy behavior is reproduced through tests, not by copying the old architecture.
Known historical bugs become regression tests with the corrected behavior.


## Identity and RBAC

Identity is intentionally small and explicit:

- `Role` is a stable application enum (`user`, `moderator`, `administrator`).
- `Permission` is an `IntFlag`.
- `ROLE_PERMISSIONS` maps roles to capabilities.
- Views authorize through `permission_required(...)`, not role-name checks.
- User email has two representations:
  - `email`: standards-aware normalized display/storage form.
  - `email_key`: case-folded unique lookup key used for authentication and uniqueness.
- Public profiles never expose account email addresses by default.
- Password policy is length-based and accepts passphrases; passwords are hashed with Werkzeug scrypt.

### Test database lifecycle

Tests use an in-memory SQLite database. Each application fixture:

1. creates the schema;
2. removes the scoped session after the test;
3. drops the schema;
4. removes the session again;
5. disposes the SQLAlchemy engine.

This keeps SQLite DBAPI connections from surviving until garbage collection and
prevents `ResourceWarning: unclosed database` noise on Windows.

### Static typing boundaries

`mypy --strict` is kept enabled for the project. A few Flask extensions do not
currently expose complete PEP 561 typing information, so their dynamic APIs are
contained behind narrow local boundaries instead of weakening the global mypy
configuration:

- `common.forms.AppForm` contains the Flask-WTF boundary.
- `users.session` contains the Flask-Login boundary and exposes typed helpers.
- `User` implements Flask-Login's required user interface explicitly rather than
  inheriting from the optional untyped `UserMixin`.
- `db.Model` keeps Flask-SQLAlchemy's documented model pattern; its dynamic model
  attribute is suppressed on the model declaration only.

Do not add global `ignore_missing_imports = true` to make a new dependency pass
mypy. Prefer a typed dependency, a small adapter, or a narrowly-scoped ignore at
the external-library boundary.

### Blank environment values

Environment variables are normalized through `_environment_value()`.

A blank value such as `SECRET_KEY=` or `DATABASE_URL=` is treated as missing.
This matters because `.env.example` is intentionally copyable as-is:

- development falls back to an explicitly insecure development-only secret;
- production still refuses to start without a non-blank `SECRET_KEY`;
- a blank `DATABASE_URL` falls back to the default SQLite database.

This prevents Flask sessions/CSRF from silently receiving an empty secret.


## Catalogue domain

The v1 catalogue models bibliographic contributors explicitly.

```text
Book
├── BookAuthor ── Author
│   ├── role: author/editor/translator
│   └── position
├── Publisher
├── Series
├── Tag *
└── Category *
```

`BookAuthor` is an association object rather than a plain many-to-many join
because the relationship itself carries `role` and `position`.

Catalog names use a display form plus a normalized `name_key` for
case-insensitive reuse. ISBN-10 and ISBN-13 are checksum-validated and stored
without spaces or hyphens.

Archive/restore uses `archived_at`; public catalogue queries exclude archived
books by default. Destructive/archive commands remain POST-only.

`total_copies` is introduced in FBP-060. Available-copy calculation is
deliberately deferred to FBP-080, where active loans provide the source of
truth.


### Third-party typing boundaries

The project keeps strict mypy checks enabled.

- Flask-WTF remains behind the local `AppForm` boundary because Flask-WTF itself
  is not typed.
- WTForms field/validator APIs use the `types-WTForms` development stub package.
- Flask-Login remains behind `users.session`.
- SQLAlchemy eager-loading options are applied directly to statements rather
  than exposing SQLAlchemy's private/internal loader option types through a
  helper return annotation.

This keeps ignores local to genuinely untyped framework boundaries rather than
disabling type checks project-wide.


### Form/service typing boundary

WTForms validators establish runtime invariants that static typing cannot infer.
Routes therefore use `required_data(...)` to narrow required field values after
validation and `add_field_error(...)` to mutate WTForms' dynamic error
collection. Both helpers live at the Flask-WTF boundary.

### Catalog unit-of-work lifecycle

New `Book` objects enter the SQLAlchemy session before relationship graph
construction. This ensures `Publisher`, `Series`, `BookAuthor`, `Tag` and
`Category` cascades participate in the same unit of work before lookup queries
can trigger autoflush.

Catalog create/update services roll back on every exception. A regression test
turns SQLAlchemy `SAWarning` into an error during graph creation.
