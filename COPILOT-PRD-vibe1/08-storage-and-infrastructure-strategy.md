# 08 — Storage and Infrastructure Strategy

## Strategy summary

The storage model should favor boring, inspectable persistence over clever hidden state.

The product needs to preserve project data, edition history, validation evidence, and large rendered artifacts in a way that can be reopened later without regenerating the guide by accident.

## Core storage decisions

### Structured state

Use SQLite for structured project, blueprint, edition, job, provenance, and validation records.

Why this is the right starting point:

- it is easy to inspect locally
- it supports durable transactional updates
- it is strong enough for the v1 project shape
- it keeps the data model tangible rather than implied

### Large artifacts

Use the file system for large content bundles and render outputs.

Typical examples:

- frozen guide markdown or HTML bundles
- preview packages
- validation reports when stored as files for inspection
- exported guide packages
- render assets and associated metadata files

### Asset storage

Store assets with explicit provenance and policy status alongside the binary file or URI reference.

## Suggested storage layout

The product should separate logical concerns even if they live in one workspace:

- `projects/` for per-project state or project-local bundles
- `editions/` for frozen approved guide editions
- `previews/` for review slices and reviewer artifacts
- `assets/` for visual assets, map assets, and provenance manifests
- `validations/` for validation output and audit evidence
- `exports/` for handoff-ready output packages

The exact directory naming can evolve, but the separation between structured data and large artifacts should remain intact.

## Environment strategy

### Development

The dev environment should be simple to start, inspect, and reset.

Desired characteristics:

- local SQLite database file
- filesystem-backed artifacts in a predictable workspace path
- environment variables for provider credentials and optional policy flags
- easy reset behavior for fresh test runs

### Production-like deployment

The v1 architecture should still work if the app is later hosted in a more formal environment.

That means:

- no hidden dependency on local-only state that cannot be represented in files or SQLite
- no assumption that generated state only exists in RAM
- no coupling between UI rendering and storage location

## Secrets and credentials

BYOK means the system must be designed to accept user-supplied provider credentials.

Storage implications:

- credentials must not be sprayed across logs
- provider settings should be isolated from guide content
- any persisted secret handling should be treated as sensitive design work, not an afterthought

## Persistence invariants

- A frozen edition must still exist after the build process ends.
- A reopened guide must point at the exact edition snapshot, not a fresh generation pass.
- Validation reports must remain associated with the edition or job they validated.
- Provenance and policy data must not disappear when content is exported.

## Operational concerns

### Backup and recovery

The storage model should make backup and restore straightforward.

Minimum expectations:

- SQLite database can be copied safely using standard backup procedures
- artifact folders can be archived with their metadata intact
- project state can be restored without re-running the original generation pipeline

### Migration readiness

If later scale requirements force a move beyond SQLite, the schema should still be readable enough to migrate.

That means no one-off hidden data blobs for important state.

## Infrastructure posture

The first implementation should keep infrastructure modest:

- local web server for the app shell
- file system for artifacts
- SQLite for structured records
- provider access through normal outbound network calls

The product should not require distributed infrastructure just to create a guide edition.

## Storage anti-patterns to avoid

- putting frozen guide content only in RAM
- mixing generated content with source provenance in a way that cannot be separated later
- storing assets without a policy status
- treating an export as the only durable copy of a guide
- using a cache as if it were the source of truth
