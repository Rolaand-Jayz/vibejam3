# MY PLAITHROUGH Qt migration plan

## Why this migration exists

The initial `vibejam3` web prototype proved the basic project lifecycle, but its UI direction is the wrong medium for the product. The new target is a native Qt desktop application named **MY PLAITHROUGH** with a more serious editorial desktop feel.

## Current state

- Existing path: React + Vite prototype in `src/`
- Current behavior already proven:
  - create a guide project
  - list projects
  - inspect project details
  - delete a project
- Limitation: web-first presentation feels wrong for the product direction

## Target state

- New path: Qt 6 Widgets desktop app in `qt/`
- Application name: `MY PLAITHROUGH`
- Desktop layout:
  - left rail for project navigation
  - central reading/detail work area
  - right-side summary/trust panel
  - native menu / toolbar / status bar feel
- Real persistence: local desktop storage for project manifests

## Migration strategy

This is an **incremental coexistence migration**.

1. Keep the React/Vite prototype intact as the proven fallback path.
2. Build the Qt desktop shell in parallel.
3. Prove the Qt path by configuring and compiling it locally.
4. Only after the Qt path is validated should the web prototype be treated as legacy.

## Slice being implemented now

### Scope

A real Qt foundation slice that preserves the proven behavior of the prototype:

- create a project
- list projects
- view project details
- delete a project
- persist projects locally between launches

### Out of scope for this slice

- provider adapters
- edition pipeline
- content generation jobs
- asset provenance workflow
- frozen-edition semantics

## Rollback points

### Rollback point 1: before Qt files are introduced

- Revert `qt/` and migration docs only.
- Web prototype remains usable.

### Rollback point 2: after Qt files exist but before build validation

- Revert `qt/` plus related build helper changes.
- Web prototype remains the active UI.

### Rollback point 3: after Qt build validates

- Keep both paths until the Qt app is accepted as the primary shell.
- No destructive removal of the web path in this slice.

## Validation gates

### Gate A — toolchain

- `Qt6Widgets` must be installed and discoverable.
- CMake must configure the Qt app successfully.

### Gate B — compile

- The Qt app must build cleanly.

### Gate C — runtime shell

- The app must launch and show:
  - left project list
  - project creation flow
  - detail view
  - delete flow

### Gate D — persistence

- Created projects must be written to local storage.

## Risks

### Risk: Qt found, but Widgets missing

- Mitigation: verify `Qt6Widgets` before coding.

### Risk: web and desktop paths drift

- Mitigation: keep the Qt slice behaviorally aligned to the currently proven project lifecycle.

### Risk: persistence format becomes throwaway

- Mitigation: use a real persisted repository, not in-memory or mock data.

## Immediate next step

Implement the Qt desktop shell and verify it builds locally as the new `MY PLAITHROUGH` foundation path.
