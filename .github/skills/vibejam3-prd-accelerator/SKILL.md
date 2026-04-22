---
name: vibejam3-prd-accelerator
description: "Project-local PRD-to-deploy accelerator tuned to the vibejam3 guide production system. Drives the fastest safe path from PRD to working code without skipping proof."
argument-hint: "Current vibejam3 phase, milestone, or request to drive from spec to implementation"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 PRD Accelerator

Use this skill when vibejam3 needs to move from PRD to implementation as fast as possible without sacrificing correctness.

This is a project-specialized version of `shared-prd-to-deploy-accelerator`. It assumes the PRD pack at `COPILOT-PRD-vibe1/` as the source of truth and routes through the vibejam3-specific skill chain.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Treats the vibejam3 PRD pack as the source of truth.
- Routes through project-local companion skills instead of generic ones.
- Picks the smallest complete path to a working slice.
- Preserves validation, integration checks, and release gates.

## Required workflow

### 1. Read the PRD pack

Use `COPILOT-PRD-vibe1/` as the first source of truth. Pull out:
- requirements from `02-requirements.md`
- constraints from `03-constraints-and-assumptions.md`
- acceptance criteria from `19-acceptance-criteria-matrix.md`
- milestone boundaries from `16-milestone-plan.md`
- open questions from `20-open-questions.md`

### 2. Close preimplementation gaps

Use `vibejam3-prd-gap-closure` instead of the generic gap closure. Resolve only what blocks the current slice.

### 3. Choose the smallest safe slice

Use `vibejam3-slice-selector` instead of the generic selector. Cut to the narrowest guide-production value path.

### 4. Gate readiness

Use `vibejam3-readiness-gate` instead of the generic gate. Check storage boundaries, provider contracts, edition rules, and acceptance criteria.

### 5. Implement with evidence

Use the coding stack (`e2e-coding`, `result-driven-tdd`) when the gate passes.

### 6. Confirm completion

Use `vibejam3-completion-gate` and `vibejam3-handoff-pack` when pausing or finishing.

## Speed rules for vibejam3

- Speed means fewer wasted steps, not fewer proofs.
- Keep the active skill set small: one primary skill per phase.
- Cut scope before adding complexity.
- Do not build future milestones just because they are nearby.
- Respect the SQLite+filesystem boundary even when cutting scope.

## Correctness gates for vibejam3

Never skip these:
- PRD gap check before coding
- acceptance criteria mapped to `19-acceptance-criteria-matrix.md`
- edition immutability respected
- provider adapter interface honored
- validation path credible

## Routing rules

- Gaps blocking coding → `vibejam3-prd-gap-closure`
- Slice cut needed → `vibejam3-slice-selector`
- Go/no-go before coding → `vibejam3-readiness-gate`
- Implementation → `e2e-coding` + `result-driven-tdd`
- Completion check → `vibejam3-completion-gate`
- Pause or handoff → `vibejam3-handoff-pack`
- Design needed → `vibejam3-app-design`
- Quality check → `vibejam3-quality-coach`
- Release check → `vibejam3-release-readiness`

## Output required

Return all of these:

- current vibejam3 milestone
- PRD evidence used
- why the chosen phase is the fastest safe next step
- next tiny step
- next skill in the chain
- blockers or assumptions still unresolved

## Completion standard

This skill is complete only when vibejam3 can move from PRD to implementation without skipping preimplementation, without hiding risk, and without letting speed erase proof.
