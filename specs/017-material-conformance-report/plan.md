# Implementation Plan

## Phase 017.A — Deterministic aggregate report

1. Define a closed schema for gates, evidence dimensions, headline statuses and
   registry-wide summaries.
2. Load the active catalog, model, evidence suites, visual contract and complete
   registry without Qt dependencies.
3. Invoke the four strict source checkers directly and propagate failures to the
   dimensions they own.
4. Generate an ordered, byte-stable report that distinguishes pass, failure,
   pending runtime, not evaluated and not modeled.
5. Validate schema closure, freshness, current evidence counts and absence of
   premature conformance claims with an independent checker.
6. Add focused mutation tests and make the harness a repository-health gate.

## Next increment

After 016.C2 produces one complete reviewed target comparison, define 017.B's
closed comparison-ingestion inventory. Keep all other targets pending and do not
broaden component-family modeling in the same increment.
