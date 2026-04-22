# vibejam3 — Game Guide Production System

## What this is

Free, open-source, provider-agnostic, BYOK game guide production system.
Produces durable, book-grade, personalized unofficial strategy guides.

## Tech stack

- TypeScript / Node.js, React, Vite
- SQLite + filesystem for storage
- Zod for schema validation
- Provider adapter interface for LLM integration (OpenAI, Anthropic, Google, Local)
- Vitest + Playwright for testing

## Build and test commands

```bash
npm run dev          # Start dev server (port 5173)
npm run build        # Typecheck + Vite build
npm run typecheck    # TypeScript type checking only
npm run preview      # Preview production build (port 4173)
```

No test runner installed yet. When tests are added: `npx vitest` for unit/integration.

## PRD pack

Full pre-implementation specification lives in `COPILOT-PRD-vibe1/` (22 documents).
See `COPILOT-PRD-vibe1/README.md` for reading order and approval gates.

Key docs:
- Architecture: `COPILOT-PRD-vibe1/05-technical-architecture.md`
- Data model: `COPILOT-PRD-vibe1/07-data-model-and-schema-definitions.md`
- Milestone plan: `COPILOT-PRD-vibe1/16-milestone-plan.md`
- Acceptance criteria: `COPILOT-PRD-vibe1/19-acceptance-criteria-matrix.md`
- UI/UX design: `docs/design/ui-ux-design-spec.md`

## Architecture

6-layer modular design: Presentation → Application Orchestration → Domain → Provider Adapter → Validation & Policy → Storage.

Source code lives in `src/`. Current scaffold is a single `App.tsx` with premium editorial dark-mode styling.

## MANDATORY skill usage rules

**These rules are non-negotiable. Violating them wastes context, skips validation, and produces untrustworthy output.**

### Rule 1: Always load the matching skill before acting

When a project-local or shared skill matches the current task, you MUST `read_file` the corresponding SKILL.md as your FIRST action, BEFORE writing any code, making any plan, or producing any output.

Skills are listed in the **Project-local skills** section below and in the global shared skill pack. Match by description keywords.

If multiple skills match, load the most specific project-local one first. Load secondary skills only when they add clear value to the current step.

### Rule 2: Follow the skill's procedure exactly

After loading a skill, follow its step-by-step procedure. Do not skip steps, summarize steps away, or replace the skill's workflow with generic behavior.

If a skill says "load X first" or "run Y before proceeding", do exactly that.

### Rule 3: Do not improvise around missing skills

If a skill should exist for the current task but is not listed, do not silently proceed without it. State explicitly that the skill is missing and what gap it creates, then ask whether to proceed without it or stop.

### Rule 4: Phase-gated execution

Each milestone phase has dedicated skills. Use them in order:

1. **Before any new phase**: `vibejam3-phase0-environment-inspector` — verify workspace readiness
2. **Before coding a slice**: `vibejam3-readiness-gate` — go/no-go check
3. **During coding**: `vibejam3-app-design` for any UI work
4. **Before claiming done**: `vibejam3-completion-gate` — honest verdict
5. **Before shipping**: `vibejam3-release-readiness` — professional go/no-go

Do not skip gates. A gate failure means stop and fix, not note and continue.

### Rule 5: Use the PRD accelerator for autonomous work

When running autonomously or asked to "implement Phase N", load `vibejam3-prd-accelerator` first. It routes through the full preimplementation chain (optimizer → gap closure → slice selector → readiness gate → implementation).

For full end-to-end autonomous execution, load `vibejam3-autonomy-wrapper` or `vibejam3-e2e-stack` instead.

## Project-local skills

All skills below are project-optimized versions of shared global skills. They are scoped to vibejam3's PRD pack, tech stack, and acceptance criteria. **Always prefer these over the generic shared equivalents when working on this project.**

### Phase 0 — Environment inspection

- **`vibejam3-phase0-environment-inspector`** — MANDATORY before any new phase. Inspects workspace for build/test/runtime readiness. Load before starting any implementation work.

### Preimplementation — PRD chain

- **`vibejam3-prd-optimizer`** — Load at session start or when planning the next implementation slice. Optimizes the active skill set around the PRD pack.
- **`vibejam3-prd-gap-closure`** — Load before slicing when gaps remain. Finds blockers, confirms assumptions, classifies unresolved questions against the PRD pack.
- **`vibejam3-slice-selector`** — Load when the PRD is ready but the cut line is unclear. Cuts the narrowest complete implementation slice.
- **`vibejam3-readiness-gate`** — MANDATORY before a slice enters coding. Checks storage boundaries, provider contracts, edition rules, and acceptance criteria.
- **`vibejam3-prd-accelerator`** — Load for autonomous PRD-to-code routing. Drives the fastest safe path through the full project-local PRD chain.

### Implementation — Coding and autonomy

- **`vibejam3-autonomy-wrapper`** — Load for full end-to-end autonomous work. Routes multi-phase execution through the project-local skill chain.
- **`vibejam3-e2e-stack`** — Load when the agent needs a clear execution stack for autonomous vibejam3 execution.

### Design

- **`vibejam3-app-design`** — MANDATORY for any UI, screen, workflow, or visual component work. Specializes anti-slop app design for vibejam3's premium editorial, book-like guide-production UI.

### Completion and handoff

- **`vibejam3-completion-gate`** — MANDATORY before claiming a phase is done. Truthful complete/blocked/incomplete verdict against acceptance criteria.
- **`vibejam3-handoff-pack`** — Load when pausing or switching agents. Produces resumable handoff state.

### Release

- **`vibejam3-quality-coach`** — Load before sharing or shipping. Checks whether the app is broken, risky, messy, or ready.
- **`vibejam3-release-readiness`** — Load for professional ship go/no-go. Checks build, tests, docs, packaging, and trust surfaces.

## Global shared skills

The canonical shared skill pack at `/home/rolaandjayz/.agents/skills/` is also available. Route through `shared-skill-index` when unsure which specialist applies.

## Current status

- PRD pack: complete and handoff-ready
- App scaffold: Vite/React/TS initialized, `npm run build` passes
- Phase 1 implementation: not started yet

## Triggers

- `vibejam3` `game guide` `guide production` `strategy guide` `master codex`
