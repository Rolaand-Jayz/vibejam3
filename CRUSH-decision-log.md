# CRUSH Decision Log

- 2026-04-22 — Keep the Phase 5 starter slice honest by introducing a durable `GuideBuildBundle` instead of pretending a frozen edition already exists.
  - Alternatives considered:
    - Reusing `GuideEdition` early as a pre-freeze container
    - Delaying full-build work until the later freeze slice
  - Why this choice won:
    - Preserves edition immutability rules
    - Satisfies the need for a real post-preview artifact now
    - Keeps web and Qt in step with the same trust model
  - Evidence / constraints:
    - `COPILOT-PRD-vibe1/19-acceptance-criteria-matrix.md` requires AC-04 and AC-05 behavior
    - Web full-build tests passed
    - Qt repository tests passed

- 2026-04-22 — Add native Qt review surfaces for knowledge and full-build artifacts instead of leaving the desktop path storage-only.
  - Alternatives considered:
    - Repository-only implementation with no desktop visibility
    - Web-only visibility while Qt lagged behind
  - Why this choice won:
    - Matches the user’s requirement that web and Qt update together
    - Keeps trust signals visible on the primary Qt path
  - Evidence / constraints:
    - `qt/src/main_window.cpp` now exposes knowledge and full-build flows
    - `cmake --build build/qt` completed successfully
