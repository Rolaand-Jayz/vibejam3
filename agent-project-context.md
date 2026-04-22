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
  - `19-acceptance-criteria-matrix.md` — AC-04 requires generated preview plus explicit approval before full build
- `src/` — React/Vite/TypeScript web workbench kept in step with the desktop path
- `qt/` — Qt 6 Widgets desktop app for `MY PLAITHROUGH`
- `docs/design/ui-ux-design-spec.md` — premium editorial dark-mode design direction
- `agent-test-log.md` — proof trail for red/green/build/smoke/browser verification

### Current Implementation Status

- Web and Qt are both active; Qt is the primary desktop path, while the web workbench remains a maintained parallel surface.
- Durable project manifests exist across both surfaces.
- Intake validation jobs and validation reports are implemented and persisted.
- Shared per-project CSS theme storage exists on the web path and remains a deferred deeper customization surface.
- Durable, versioned editorial blueprints are implemented across web and Qt.
- Durable, versioned preview packages are now implemented across web and Qt.
- Preview approval is explicit; preview output is not treated as implicit approval.
- Project states now actively used in shipped code include `intake`, `blueprint-ready`, `preview-ready`, and `revision-requested`.

### Storage Reality

- Web runtime: durable browser `localStorage` via `LocalStorageProjectStorage`
- Node-side adapter: SQLite via `src/storage/sqlite-project-storage.ts`
- Qt runtime: filesystem-backed per-project workspaces under `projects/<id>/`
  - `project-manifest.json`
  - `blueprints/*.json`
  - `previews/*.json`
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
- Create and version editorial blueprints
- Generate deterministic preview packages from the latest blueprint
- Record explicit preview approval or changes-requested review state
- Visually verify the preview flow in the live browser path

### Likely Next Slice

- Gate the eventual full-build path behind approved preview state and begin the first real post-preview build artifact workflow.

### Triggers

- `vibejam3` `game guide` `guide production` `strategy guide` `master codex`
