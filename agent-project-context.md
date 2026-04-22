# Agent Project Context

## Project: Game Guide Production System (vibejam3)

### Description

Free, open-source, provider-agnostic, BYOK game guide production system.
Produces durable, book-grade, personalized unofficial strategy guides.

### Key Files

- `game_guide_production_system_spec.md` — Full production specification (945 lines)
- `CRUSH-PRD/` — Complete pre-implementation package (19 documents, 5,763 lines)
  - 01-project-brief.md — Executive summary
  - 02-product-requirements.md — Full PRD (387 lines)
  - 03-constraints-and-assumptions.md — Constraints, assumptions, inferences
  - 04-technical-architecture.md — Architecture with layers, data flow, deployment
  - 05-system-component-map.md — Component inventory, interfaces, dependency graph
  - 06-data-models-and-schemas.md — Complete data model definitions (1,102 lines)
  - 07-storage-strategy.md — SQLite + file system storage design
  - 08-provider-abstraction.md — BYOK provider adapter interface
  - 09-prompt-orchestration.md — Prompt compilation, token budgets, templates
  - 10-consistency-validation-plan.md — 7 validators, cross-unit and global validation
  - 11-asset-rights-policy.md — Asset policy, map provenance, output scrubber
  - 12-ux-flow-and-state-model.md — Screen specs, navigation, state model
  - 13-milestone-plan.md — 8 phases with milestones and dependencies
  - 14-risk-register.md — 14 risks with mitigation strategies
  - 15-test-strategy.md — Test levels, mock strategy, coverage targets
  - 16-acceptance-criteria-matrix.md — 15 acceptance criteria mapped to tests
  - 17-open-questions.md — 12 unresolved questions with defaults
  - 18-inference-ledger.md — 15 tracked inferences/assumptions

### Status

- Pre-implementation package COMPLETE — awaiting review and approval
- 19 documents covering every aspect of the system
- All 14 pre-implementation deliverables from spec Section 20 addressed
- Open questions and inference ledger ready for review
- Project-local skill added: `.github/skills/vibejam3-prd-optimizer/SKILL.md` — specialized wrapper that turns the PRD pack into the smallest safe skill chain before implementation
- Project-local skill added: `.github/skills/vibejam3-phase0-environment-inspector/SKILL.md` — phase-0 workspace inspection skill that establishes the real repo baseline before implementation
- Project-local skill added: `.github/skills/vibejam3-autonomy-wrapper/SKILL.md` — repo-specific wrapper for the shared end-to-end autonomy stack
- Project-local skill added: `.github/skills/vibejam3-app-design/SKILL.md` — repo-specific app design wrapper for vibejam3’s premium editorial product style
- Phase 0 inspection result: docs-only workspace so far; no `package.json`, `tsconfig`, Vite, Playwright, Vitest, or lockfile detected; no app source tree yet
- Workspace initialization result: TypeScript + React + Vite scaffold created, dependencies installed, and `npm run build` passes

### Tech Stack (from PRD)

- Backend: TypeScript / Node.js
- Frontend: React
- Structured Storage: SQLite (per-project databases)
- Large Artifacts: File system
- Schema Validation: Zod
- LLM Integration: Provider adapter interface (OpenAI, Anthropic, Google, Local)
- Build: Vite
- Testing: Vitest + Playwright
- Desktop Shell: Deferred (start with local web server)

### Triggers

- `vibejam3` `game guide` `guide production` `strategy guide` `master codex`
