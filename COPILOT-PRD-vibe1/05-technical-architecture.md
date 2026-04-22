# 05 — Technical Architecture

## Architecture summary

The system should be built as an artifact-first, job-oriented web application with a clean separation between UI, orchestration, domain logic, provider adapters, validation, and storage.

The architecture should not let vendor APIs, preview state, or chat behavior leak directly into the core guide model.

## Recommended stack direction

The current product direction points to:

- **Backend/runtime:** TypeScript on Node.js
- **Frontend:** React
- **Build tooling:** Vite
- **Schema validation:** Zod
- **Structured persistence:** SQLite
- **Large artifacts:** file system storage
- **Testing:** Vitest and Playwright
- **Initial packaging posture:** web-first local/server deployment, with desktop shell deferred

## Architectural style

The product should use a layered modular architecture rather than a giant prompt script.

### 1. Presentation layer

Responsible for:

- project setup UI
- preview review UI
- guide reader UI
- chat UI
- build status and validation surfaces
- edition history and policy review surfaces

### 2. Application orchestration layer

Responsible for:

- starting and tracking guide jobs
- moving from intake to blueprint to preview to full build
- coordinating validation steps
- freezing editions
- managing replay/reopen behavior

### 3. Domain layer

Responsible for:

- guide project state
- edition semantics
- knowledge entities and relationships
- provenance and conflict tracking
- editorial blueprint logic
- policy decisions

### 4. Provider adapter layer

Responsible for:

- normalizing provider-specific model calls
- handling capability differences cleanly
- keeping provider request/response shapes out of the domain model

### 5. Validation and policy layer

Responsible for:

- consistency checks
- asset rights checks
- map policy enforcement
- output sanity checks
- edition freeze readiness checks

### 6. Storage layer

Responsible for:

- durable project state
- guide editions
- knowledge records
- generated artifacts
- validation evidence

## System shape

The product should behave like a pipeline of durable transformations:

1. intake and manifest creation
2. knowledge normalization
3. editorial blueprint compilation
4. preview generation
5. user review and revision
6. controlled full production
7. validation and freeze
8. reopen and chat grounding

Each stage should produce a tangible artifact that can be inspected later.

## Key architectural principles

- **Artifact-first:** every meaningful stage produces a stored artifact, not just transient state.
- **Job-centric:** guide production is a long-running job with explicit status and checkpoints.
- **Adapter-bound:** external providers are isolated behind strict interfaces.
- **Versioned:** project, blueprint, and edition state must be version-aware.
- **Deterministic where possible:** where inputs are stable, outputs and validation should be reproducible enough to reason about.
- **Inspectable:** it should be possible to tell why a guide exists in a particular form.

## Major flows

### Intake flow

The UI collects structured project intent and creates a project manifest.

### Knowledge flow

Structured facts are ingested, normalized, connected, and scored for confidence/provenance.

### Blueprint flow

The system compiles editorial decisions into an explicit chapter/section plan before generation.

### Preview flow

The system generates a sample slice for review so the user can confirm tone and scope.

### Full production flow

The system generates the guide in bounded units and validates each unit before final freeze.

### Chat grounding flow

The chat system retrieves from the frozen edition plus hidden codex data without altering the edition.

## Architecture decisions that matter

### Why not a single giant prompt pipeline?

Because the product needs durability, reopenability, validation, editioning, and policy discipline. A single opaque prompt script would be hard to test, hard to revisit, and hard to trust.

### Why SQLite + filesystem first?

Because the product needs durable local inspectability more than it needs distributed complexity at the beginning.

### Why strict provider adapters?

Because the product’s long-term value is in its domain behavior, not in one provider’s API shape.

## Runtime concerns

- long-running jobs must survive interruptions well enough to be inspectable
- validation failures must be recorded, not erased
- edition freeze must be a real boundary, not a label change
- chat requests must respect the current edition context
- provider failures should not corrupt project state

## Architectural non-goals

- no assumption of a public multi-tenant service in v1
- no assumption of live regeneration as the default open action
- no assumption that one provider’s response schema is the canonical internal schema
- no assumption that all assets can be sourced automatically without policy review
