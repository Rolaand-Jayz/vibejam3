# Stub Sweep Findings

## Scope

- Requested scope: repository-wide stub audit for `vibejam3`, with emphasis on current runtime code and any deceptive placeholder behavior.
- Files reviewed: 76 user-authored text files discovered in the workspace inventory across `src/`, `qt/`, `docs/`, `COPILOT-PRD-vibe1/`, repo instructions, and agent logs.
- Explicit exclusions:
  - `build/**` — generated CMake/Qt build output
  - `node_modules/**` — vendored dependencies
  - `dist/**` — generated web build output (not present in current inventory)
  - `package-lock.json` — generated dependency lockfile
  - binary/runtime artifacts under project storage roots — not part of authored source
- Search patterns used:
  - explicit markers: `TODO|FIXME|XXX|HACK|stub|placeholder|temporary|NotImplementedError|not implemented`
  - behavioral markers: `return true;|return {}|return nullptr;|if (true)|if (false)|QMessageBox::information|disabled`
  - feature-surface markers: `preview|export|edition|build progress|sidecar|chat|companion|providerProfile|projectState`

## Summary

- Confirmed findings: 2
- Likely findings: 2
- False positives reviewed: 6

## Findings

| ID | Status | Path | Line / Symbol | Category | Evidence | Why stub-like | User impact | Next action |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| STUB-001 | confirmed | `src/App.tsx` | lines 3, 9-15, `const storage = new InMemoryProjectStorage()` | fake persistence path / placeholder UX | The web app imports `InMemoryProjectStorage`, comments that it is used for "Phase 1 foundation", and wires the whole UI to process-memory-only state. | The UI allows users to create and manage guide projects as if they are real project records, but a reload/restart loses everything. That makes the web flow a working demo, not a durable project system. | A user can believe they created a real guide project, then lose it on refresh. This directly conflicts with the PRD requirement that intake create a durable manifest. | Either wire durable browser-safe storage, or clearly demote the web shell to an ephemeral demo and remove durability implications. |
| STUB-002 | confirmed | `src/App.tsx` | lines 51-53 and detail view line 282 | fake constant metadata | New projects are stamped with `providerId: 'openai'` and `modelId: 'gpt-4o'`, and that pair is shown in the detail view even though the user never selected or configured a provider. | The metadata is invented to satisfy the schema/UI rather than reflecting real project configuration. | Users and future code can trust provider/model values that were never actually chosen. | Replace hardcoded provider metadata with real config capture, or omit provider display until configuration exists. |
| STUB-003 | likely | `qt/src/project_repository.cpp` | `artifactDirectoryNames()` lines 54-62 | scaffold-only future feature surface | The repository eagerly creates `editions/`, `previews/`, `assets/`, and `exports/` directories for each project, but the current source sweep found no producer/consumer flows for those directories. | This looks like honest scaffold infrastructure, but it is still future-feature groundwork with no runtime behavior behind most of it yet. | Low immediate user impact because the Qt UI does not currently expose those features, but the workspace layout implies capability that does not yet exist. | Keep as scaffold if intentional, but do not surface these as working features until read/write flows exist. |
| STUB-004 | likely | `qt/src/project.hpp`, `qt/src/project_repository.cpp` | `projectState`, `fromDraft()`, `update()`, `stateLabel()` | partial state-model stub | The project state field exists, is shown in the UI, and is always initialized or reset to `intake`; the current sweep found no transitions to later workflow states such as blueprint/preview/build. | The state model exists structurally, but only one real state is implemented. This is not a fake-success path yet, but it is a parked state machine. | The desktop shell can show a state badge, but the broader workflow state model is not actually alive yet. | Treat as an incomplete workflow model; only elevate to a confirmed stub if the app starts advertising downstream stages before they work. |

## Likely findings needing confirmation

| ID | Path | Line / Symbol | Reason to inspect further |
| --- | --- | --- | --- |
| STUB-L-001 | `qt/src/project_repository.cpp` | `artifactDirectoryNames()` | Empty workspace directories may be acceptable scaffolding, but they are currently unbacked by runtime flows. |
| STUB-L-002 | `qt/src/project.hpp` / `qt/src/project_repository.cpp` | `projectState` handling | The state badge is real, but only the intake state appears to exist in code today. |

## False positives reviewed

| Path | Line / Symbol | Why not a stub |
| --- | --- | --- |
| `src/App.tsx` | input `placeholder=` strings | These are normal form examples, not fake implementation. |
| `qt/src/new_project_dialog.cpp` | `setPlaceholderText(...)` | Same story: input hints, not runtime deception. |
| `src/App.tsx` | disabled create button when title is empty | Legitimate validation gate, not a dead affordance. |
| `qt/src/main_window.cpp` | `return {}` / `return nullptr` branches | These are normal guard/error/empty-state branches, not fake success paths. |
| `qt/src/main_window.cpp` | `Check Intake` action/button | This is now a real path backed by persisted jobs/reports, not a message-only stub. |
| `COPILOT-PRD-vibe1/**` and `docs/design/ui-ux-design-spec.md` | references to sidecar/chat/preview/build flows | These are specification documents and roadmap material, not claims that the current implementation already ships those features. |

## Notes

- The strongest current stub problem is **the old web path**, not the active Qt desktop path.
- I did **not** find `TODO`, `FIXME`, `NotImplementedError`, bare `pass`, or message-only fake-complete handlers in the current Qt app code.
- No remediation was performed during this sweep.
