# Agent Project Context

## Project: Game Guide Production System (vibejam3)

### Description

Free, open-source, provider-agnostic, BYOK game guide production system.
Produces durable, book-grade, personalized unofficial strategy guides.

### Key Files and Folders

- `COPILOT-PRD-vibe1/` — Full pre-implementation pack (22 documents)
  - `02-requirements.md` — FR-13 to FR-16 define preview-before-full-build and explicit approval
  - `07-data-model-and-schema-definitions.md` — `Project`, `Blueprint`, `PreviewPackage`, `GuideEdition`
  - `14-ux-flow-and-state-model.md` — state model including `blueprint-ready`, `preview-ready`, and `revision-requested`
  - `19-acceptance-criteria-matrix.md` — AC-04 requires generated preview plus explicit approval before full build; AC-05 requires bounded full-build units before freeze
- `src/` — React/Vite/TypeScript web workbench kept in step with the desktop path
- `src/domain/schemas/knowledge-entity.ts` — normalized knowledge/provenance/conflict schema for visible guide facts and hidden codex notes
- `src/validation/knowledge-entity.ts` — parsing/formatting helpers for aliases, attributes, and knowledge summaries
- `src/domain/schemas/guide-build-bundle.ts` — durable schema for the bounded full-build artifact that exists before any immutable edition freeze
- `src/orchestration/full-build-factory.ts` — deterministic build-bundle unit generation from manifest, blueprint, preview, and knowledge
- `src/validation/full-build.ts` — freeze gate validation outcome for full-build bundles on the web path
- `qt/` — Qt 6 Widgets desktop app for `MY PLAITHROUGH`
- `qt/src/knowledge_workbench_dialog.cpp` — native structured-knowledge editor for the desktop path
- `qt/src/build_bundle_review_dialog.cpp` — native review surface for the durable full-build bundle and its validation outcome
- `docs/design/ui-ux-design-spec.md` — premium editorial dark-mode design direction
- `agent-test-log.md` — proof trail for red/green/build/smoke/browser verification

### Current Implementation Status

- Web and Qt are both active; Qt is the primary desktop path, while the web workbench remains a maintained parallel surface.
- Durable project manifests exist across both surfaces.
- Intake validation jobs and validation reports are implemented and persisted.
- Shared per-project CSS theme storage exists on the web path and remains a deferred deeper customization surface.
- Durable, versioned editorial blueprints are implemented across web and Qt.
- Durable, versioned preview packages are now implemented across web and Qt.
- Structured knowledge registry support now exists across web and Qt, including provenance notes, conflict markers, and visible-guide vs hidden-codex separation.
- Durable full-build bundle artifacts now exist across web and Qt, including approved-preview enforcement, bounded chapter/reference/appendix units, persisted validation reports, and observable full-build jobs.
- Preview approval is explicit; preview output is not treated as implicit approval.
- Project states now actively used in shipped code include `intake`, `knowledge-building`, `blueprint-ready`, `preview-ready`, `revision-requested`, `freeze-pending`, and `validation-failed`.

### Storage Reality

- Web runtime: durable browser `localStorage` via `LocalStorageProjectStorage`
- Node-side adapter: SQLite via `src/storage/sqlite-project-storage.ts`, including a durable `knowledge_entities` table
- Qt runtime: filesystem-backed per-project workspaces under `projects/<id>/`
  - `project-manifest.json`
  - `knowledge/*.json`
  - `blueprints/*.json`
  - `previews/*.json`
  - `build-bundles/*.json`
  - `jobs/*.json`
  - `validations/*.json`

### Tech Stack in Practice

- Web: TypeScript, React, Vite, Zod
- Desktop: Qt 6 Widgets / CMake
- Structured storage: SQLite + filesystem design remains aligned with the PRD
- Testing: Vitest for web service behavior, Qt Test for repository durability, browser/manual smoke for UI proof

### Verified Working Slices

- Create/edit/delete project manifests
- Run and persist intake validation
- Store shared theme CSS in project manifests
- Create, edit, and persist structured knowledge entries with required provenance
- Separate visible guide facts from hidden codex notes in both detail and lens surfaces
- Create and version editorial blueprints
- Generate deterministic preview packages from the latest blueprint
- Record explicit preview approval or changes-requested review state
- Run a real post-preview full build that produces durable bounded guide units instead of pretending a frozen edition already exists
- Visually verify the preview flow and the structured-knowledge flow in the live browser path

### Likely Next Slice

- Turn a validated build bundle into the first immutable frozen edition snapshot, then prove reopen behavior never regenerates or mutates that edition silently.

### Triggers

- `vibejam3` `game guide` `guide production` `strategy guide` `master codex`
