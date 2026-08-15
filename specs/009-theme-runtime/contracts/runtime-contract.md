# ThemeRuntime Contract

Effective theme context precedence:
1. explicit context assigned to the widget/binding;
2. nearest inherited/parent context;
3. optional application/global default provider;
4. deterministic library fallback if required by public contract.

ThemeManager is optional convenience/fallback. It is never queried by renderers and does not own serialization.
Every effective-theme change exposes sufficient revision/change information for ResolvedSpec invalidation.
