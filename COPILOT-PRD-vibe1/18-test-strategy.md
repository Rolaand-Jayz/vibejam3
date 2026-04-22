# 18 — Test Strategy

## Purpose

This product needs tests that prove durable guide behavior, not just functions that return objects.

The test strategy should focus on the behaviors that matter most to trust: edition stability, provenance, policy handling, and consistent long-form output.

## Test levels

### Unit tests

Use for:

- manifest validation
- schema validation
- conflict tracking logic
- provider capability normalization
- data transformation and artifact naming rules

### Integration tests

Use for:

- SQLite persistence
- file system artifact writes and reopen behavior
- job lifecycle transitions
- provider adapter-to-orchestrator behavior
- asset and policy evaluation across multiple components

### End-to-end tests

Use for:

- project intake to preview to freeze workflow
- edition reopen without regeneration
- reader plus chat companion behavior
- validation failure and approval remediation flows

### Load / resilience tests

Use for:

- long-running guide production jobs
- repeated reopen operations
- build interruption and recovery scenarios

## Testable behaviors by risk area

### Edition stability

- reopening a frozen edition returns the same edition snapshot
- a new edition is created instead of mutating the old one
- preview state cannot masquerade as frozen state

### Provenance and policy

- every factual record retains source references
- assets without approval are rejected or scrubbed
- map assets require explicit provenance and policy status

### Consistency

- terminology remains stable across generated sections
- cross-section contradictions are detected before freeze
- hidden codex material does not leak into user-visible guide text

### Provider abstraction

- provider-specific capabilities are normalized correctly
- unsupported capabilities fail explicitly
- provider errors propagate in a readable way

### UX and state

- the user can tell draft, preview, frozen, and reopened states apart
- chat is clearly grounded in the selected edition
- validation failure is shown as a real state

## Mock boundaries

### Mock these

- external provider network calls
- time and randomness where deterministic output is needed
- non-essential third-party services

### Keep real where practical

- SQLite persistence in integration tests
- filesystem artifact behavior
- core orchestration logic
- schema validation boundaries

## Coverage targets

Coverage should be risk-based rather than vanity-based.

- high-risk trust behaviors: very high coverage and multiple test levels
- medium-risk behaviors: meaningful unit + integration coverage
- low-risk presentation details: selective coverage only

## What not to test

- third-party library internals
- trivial getters and setters with no logic
- generated content style minutiae that do not affect correctness
- framework boilerplate that does not belong to the product logic

## Priority order for implementation

1. project manifest and schema validation
2. edition persistence and reopen behavior
3. preview-to-freeze workflow
4. provider adapter normalization
5. policy and provenance handling
6. reader and chat grounding
7. long-running build resilience

## Testing anti-patterns to avoid

- testing only the happy path
- mocking away the storage layer so nothing real is proven
- allowing provider mocks to hide integration mistakes
- measuring coverage without proving trust-critical behavior
