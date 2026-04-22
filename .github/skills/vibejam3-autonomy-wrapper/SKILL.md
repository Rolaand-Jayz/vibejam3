---
name: vibejam3-autonomy-wrapper
description: "Project-local wrapper that applies the shared end-to-end autonomy stack to vibejam3 using the repo’s phase-0 inspection, PRD pack, and phase-specific skill chain."
argument-hint: "Current vibejam3 task, repo phase, and whether the work is preimplementation, implementation, or handoff"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Autonomy Wrapper

Use this skill when `vibejam3` needs autonomous execution that stays aligned with the repo’s PRD-first workflow.

This is the project-local wrapper for `shared-e2e-autonomy-stack`.

## Non-bypassable no-stub rule

- This skill inherits a global, non-bypassable no-stub doctrine.
- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- Do **not** present scaffolding, temporary wiring, or speculative future behavior as finished product behavior.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## Required repo-specific route

### Phase 0

Start with:

1. `vibejam3-phase0-environment-inspector`
2. `vibejam3-prd-optimizer`

### Preimplementation phase

Then use:

1. `vibejam3-prd-gap-closure`
2. `vibejam3-slice-selector`
3. `vibejam3-readiness-gate`
4. `vibejam3-prd-accelerator`

### Implementation phase

Only after the slice is truly ready:

1. `vibejam3-e2e-stack`
2. `e2e-coding`
3. `result-driven-tdd`
4. `hardening-optimization`
5. `vibejam3-completion-gate`
6. `vibejam3-handoff-pack` when pausing or switching agents

### Release phase

1. `vibejam3-quality-coach`
2. `vibejam3-release-readiness`

## Repo-specific sources of truth

Read these first when relevant:

- `agent-project-context.md`
- `agent-plan-progress.md`
- `COPILOT-PRD-vibe1/`
- `game_guide_production_system_spec.md`

## Hard rules

- Do not bypass the PRD chain on multi-step work.
- Do not let `shared-e2e-autonomy-stack` replace the repo’s phase-0 and PRD-specific routing.
- Do not start implementation until the readiness gate is truthful.
- Use `shared-completion-gate` before claiming a phase is done.
- Use `shared-handoff-pack` before pausing long work.

## Output required

Return all of these:

- current vibejam3 phase
- active autonomy stack
- repo-specific worker skills in order
- proof required before the next phase
- next exact action
- whether handoff or completion gating is needed now

## Completion standard

This skill is complete only when the shared autonomy stack has been specialized to the actual vibejam3 repo flow and the next autonomous phase can proceed without guesswork.