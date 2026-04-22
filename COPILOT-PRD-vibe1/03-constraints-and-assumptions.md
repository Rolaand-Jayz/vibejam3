# 03 — Constraints and Assumptions

## Why this document exists

This project has a lot of attractive possible directions. That is exactly why the constraints need to be explicit.

If a constraint is real, it should be visible before implementation. If an assumption is being used to move the project forward, it should be named so it can be challenged later.

## Hard constraints

### Product constraints

1. **No silent regeneration on reopen.** A saved guide must reopen exactly as the user approved it.
2. **Preview before full production.** The user must see representative output before the full build proceeds.
3. **Frozen editions are immutable.** Post-approval changes must create a new edition instead of mutating the old one.
4. **Chat must not rewrite the guide.** Chat is a grounded companion, not a hidden editor.
5. **Visible guide and hidden master codex are separate artifacts.** They may cooperate, but they must not collapse into one undifferentiated blob.
6. **Maps cannot be hallucinated.** If a map is shown, provenance and permission must exist.
7. **Asset use must obey a rights policy.** Decorative convenience does not outrank source permissions.
8. **Provider-agnostic core logic is mandatory.** The domain cannot become a thin wrapper around a single vendor’s API.

### Engineering constraints

1. The project must be buildable as a web-first product with a responsive reader and operator surface.
2. The product must support durable artifact storage, not only ephemeral in-memory state.
3. The system must model guide production as a sequence of explicit jobs and validation checkpoints.
4. The system must preserve version history for at least the project, blueprint, and guide edition layers.
5. The implementation must keep business rules, provider adapters, and storage concerns separable enough to test independently.

### Operational constraints

1. Build jobs may be long-running and should not assume a single request/response lifecycle.
2. Reopened guides may be viewed long after the original build environment is gone.
3. The system should remain inspectable enough that future maintainers can understand what was produced and why.

## Assumptions

### Assumption A — Web-first delivery

The first implementation should target a local web application experience with a responsive UI rather than starting with a native desktop shell.

- **Why this assumption is useful:** it keeps the first build simpler and reduces platform complexity.
- **Risk if wrong:** a future desktop shell may need extra packaging work.

### Assumption B — Local durable storage is the right first default

The initial storage model should rely on SQLite plus the file system for durable artifacts unless a later decision proves a stronger need for a different persistence layer.

- **Why this assumption is useful:** it keeps the storage model inspectable and practical.
- **Risk if wrong:** migration to a more distributed storage model may later be needed.

### Assumption C — Guide generation is asynchronous

The production pipeline should be treated as a long-running job with explicit states, checkpoints, and outputs.

- **Why this assumption is useful:** it matches the actual shape of guide production.
- **Risk if wrong:** a different execution model could affect orchestration and UI.

### Assumption D — Provider adapters are strict boundaries

Model provider APIs should be normalized at the edge so the rest of the system does not inherit vendor-specific request and response shapes.

- **Why this assumption is useful:** it preserves portability and testability.
- **Risk if wrong:** core logic could become coupled to a provider.

### Assumption E — The hidden master codex is internal-only

The deeper knowledge layer used for chat and QA should remain internal and should not be presented as a visible guide section.

- **Why this assumption is useful:** it preserves the distinction between user-facing editorial content and internal grounding material.
- **Risk if wrong:** users may confuse internal inference with approved guide content.

### Assumption F — All content is unofficial

The product is generating unofficial strategy guidance, not authoritative publisher content.

- **Why this assumption is useful:** it keeps the product truth surface honest.
- **Risk if wrong:** legal and branding review would need to tighten further.

## Dependencies the project relies on

- Reliable model provider access under user-supplied credentials.
- Enough source material or structured input to build a useful knowledge base.
- A safe asset policy for any included media or map-like materials.
- Stable local filesystem behavior for guide artifacts and exports.

## Future-phase decisions intentionally deferred from v1

These are not required to start implementation and should not block handoff.

- Whether later packaging includes a desktop shell.
- Whether the system eventually supports multi-user or cloud sync.
- Whether some knowledge ingestion paths become more automated over time.
- Whether additional export formats are needed beyond the initial frozen guide bundle.

## Risk posture

The biggest risk is not that the product will fail to generate text. The bigger risk is that it will generate a product that looks complete while quietly losing trust, edition stability, provenance clarity, or policy discipline.

This document exists to keep that from happening.
