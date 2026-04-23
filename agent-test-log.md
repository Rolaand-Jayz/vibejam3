# Agent Test Log

## Status

Qt repository durability tests now exist for the desktop path in `qt/tests/project_repository_test.cpp`.

## Entries

- [2026-04-22] `qt/tests/project_repository_test.cpp` — RED — Added repository durability proof for intake-state persistence and manifest editing across reopen. Initial build failed because `Project` had no visible state field and `ProjectRepository` had no update path.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Added `projectState`, repository update support, and reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Ran `timeout(5s) ./build/qt/my-plaithrough`; the app stayed up without runtime error output before timeout ended it.
- [2026-04-22] Web fallback build — GREEN — Re-ran the existing web production build and it completed successfully after the Qt slice changes.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — RED — Added storage-layout tests for per-project workspaces and legacy aggregate-store migration. The suite failed because the repository still wrote only the flat legacy JSON file.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Replaced the flat store with per-project workspaces under `projects/<id>/`, created artifact directories, migrated `projects.json` forward into workspace manifests, and reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Re-ran `timeout(5s) ./build/qt/my-plaithrough` after the storage rewrite; startup remained clean.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — RED — Added validation-job tests for persisted intake checks and validation reports. The new tests referenced missing repository APIs and failed the slice until job/report persistence existed.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Added persisted `validate` jobs and validation reports, plus repository listing APIs and UI wiring for intake checks; reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Re-ran `timeout(5s) ./build/qt/my-plaithrough` after wiring validation controls and status surfaces; startup remained clean.
- [2026-04-22] Web build — GREEN — Replaced the stubbed browser path with durable localStorage-backed manifests, persisted intake validation history, and live shared-theme CSS support; reran `npm run build`; production bundle completed successfully.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Synced Qt manifest persistence with shared theme CSS and aligned intake validation warnings with the web rule set; reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Re-ran `timeout(5s) ./build/qt/my-plaithrough` after the shared theme field sync; startup remained clean.
- [2026-04-22] `src/orchestration/project-service.test.ts` — RED — Added editorial blueprint service tests first; initial run failed because `ProjectService` had no `createBlueprint` API yet.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — RED — Added editorial blueprint durability tests for versioned blueprint persistence; initial Qt build failed because the repository had no blueprint types or APIs yet.
- [2026-04-22] `src/orchestration/project-service.test.ts` — GREEN — Added durable blueprint storage, versioning, and `blueprint-ready` project-state transitions on the web path; reran `npx vitest run src/orchestration/project-service.test.ts`; 2/2 tests passed.
- [2026-04-22] Web build — GREEN — Added the in-app editorial blueprint workflow, strict chapter-plan parsing, and latest-blueprint detail surfaces; reran `npm run build`; production bundle completed successfully.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Added real filesystem-backed blueprint artifacts, version sorting, and `blueprint-ready` state transitions for the Qt repository; reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Re-ran `timeout(5s) ./build/qt/my-plaithrough` after adding the blueprint dialog and desktop blueprint surfaces; startup remained clean.
- [2026-04-22] Browser smoke — GREEN — Reloaded the dev web app, drafted and saved a real blueprint for the existing Chrono Trigger project, and verified the state advanced to `BLUEPRINT-READY` with durable blueprint summary rendering.
- [2026-04-22] `src/orchestration/project-preview.test.ts` — RED — Added preview package service tests first; initial run failed because `ProjectService` had no preview generation or explicit review APIs yet.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — RED — Added preview durability and approval-state tests; initial Qt build failed because preview types and repository APIs did not exist yet.
- [2026-04-22] `src/orchestration/project-preview.test.ts` + `src/orchestration/project-service.test.ts` — GREEN — Added durable preview package storage, deterministic preview generation, and explicit approval review on the web path; reran `npx vitest run src/orchestration/project-preview.test.ts src/orchestration/project-service.test.ts`; 5/5 tests passed.
- [2026-04-22] Web build — GREEN — Added the preview generation/review UI, latest-preview detail surfaces, and approval-state trust signals; reran `npm run build`; production bundle completed successfully.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Added filesystem-backed preview artifacts, versioned regeneration, and explicit preview review state for the Qt repository; reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Re-ran `timeout(5s) ./build/qt/my-plaithrough` after adding the desktop preview/review flow; the app stayed up without runtime error output before timeout ended it.
- [2026-04-22] Browser smoke — GREEN — Created a Chrono Trigger project in the live web app, saved blueprint v1, generated preview v1, and explicitly approved it at `http://127.0.0.1:4174/`.
- [2026-04-22] `src/orchestration/project-knowledge.test.ts` — RED — Added structured knowledge/provenance tests first; initial run failed because `ProjectService` had no knowledge entity create/list/update APIs yet.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — RED — Added knowledge-registry durability tests; initial Qt build failed because the desktop path had no knowledge entity types, persistence methods, or repository APIs yet.
- [2026-04-22] `src/orchestration/project-knowledge.test.ts` + `src/orchestration/project-preview.test.ts` + `src/orchestration/project-service.test.ts` — GREEN — Added durable knowledge entity schemas, storage, and orchestration on the web path; reran targeted Vitest coverage and all 8/8 tests passed.
- [2026-04-22] Web build — GREEN — Added the browser knowledge registry UI, hidden-codex separation, and detail/lens trust surfaces; reran `npm run build`; production bundle completed successfully.
- [2026-04-22] `qt/tests/project_repository_test.cpp` — GREEN — Added filesystem-backed knowledge artifacts, provenance enforcement, deterministic ordering, and native repository coverage; reran `ctest --test-dir build/qt --output-on-failure`; 1/1 tests passed.
- [2026-04-22] Qt runtime smoke — GREEN — Rebuilt the full desktop app after wiring the native knowledge workbench and reran `timeout 5 ./build/qt/my-plaithrough`; the app launched cleanly for the smoke window.
- [2026-04-22] Browser smoke — RED — Visual QA of the live knowledge workbench caught a real regression: fresh entries could not save because an optional conflict note defaulted `conflictType`, making the validator think the contradiction form was half-filled.
- [2026-04-22] Browser smoke + builds — GREEN — Cleared the optional conflict default in both web and Qt forms, reran targeted Vitest + `npm run build` + Qt build/tests, then created and edited a real knowledge entry at `http://127.0.0.1:5173/`; the app correctly updated the trust surfaces from `1 visible / 0 hidden` to `0 visible / 1 hidden`.
- [2026-04-23] `src/orchestration/project-full-build.test.ts` — GREEN — Added post-preview full-build bundle coverage on the web path; reran `npx vitest run src/orchestration/project-full-build.test.ts` and all 3/3 tests passed.
- [2026-04-23] Web build — GREEN — Rebuilt the React/Vite workbench after landing the bounded full-build bundle slice; `npm run build` passed successfully.
- [2026-04-23] `qt/tests/project_repository_test.cpp` — GREEN — Added durable full-build bundle repository coverage for approved-preview gating, freeze-pending success, and validation-failed blocker handling; reran `ctest --test-dir build/qt --output-on-failure -R my_plaithrough_repository_test`; 1/1 tests passed.
- [2026-04-23] Qt desktop build — GREEN — Rebuilt the full Qt application after wiring the native full-build action and review dialog; `cmake --build build/qt` completed successfully.

## Template

When tests are added, log entries here:

- [date] test path — RED/GREEN/NOTE — description
