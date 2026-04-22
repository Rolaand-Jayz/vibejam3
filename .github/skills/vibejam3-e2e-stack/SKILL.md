---
name: vibejam3-e2e-stack
description: "Project-local end-to-end autonomy stack tuned to vibejam3's PRD-first, guide-production workflow. Selects the minimum skill stack for autonomous execution."
argument-hint: "Current vibejam3 task, milestone, and whether the work is planning, coding, or full end-to-end"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 E2E Stack

Use this skill when vibejam3 needs autonomous end-to-end execution that stays aligned with the repo's PRD-first workflow.

This is a project-specialized version of `shared-e2e-autonomy-stack`. It routes through vibejam3-specific skills instead of generic ones.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Chooses the minimum stack for autonomous vibejam3 execution.
- Routes through project-local skills instead of generic shared ones.
- Prevents skill overload while preserving proof and recoverability.

## Phase routing for vibejam3

### Phase 0 (environment setup)

1. `vibejam3-phase0-environment-inspector`
2. `vibejam3-prd-optimizer`

### Preimplementation

1. `vibejam3-prd-gap-closure`
2. `vibejam3-slice-selector`
3. `vibejam3-readiness-gate`
4. `vibejam3-prd-accelerator`

### Implementation

1. `e2e-coding`
2. `result-driven-tdd`
3. `hardening-optimization`

### Design (when screens or flows need direction)

1. `vibejam3-app-design`

### Completion and handoff

1. `vibejam3-completion-gate`
2. `vibejam3-handoff-pack`

### Release

1. `vibejam3-quality-coach`
2. `vibejam3-release-readiness`

## Always-on meta skills

Keep these active for autonomous work:
- `general-autonomy`
- `consistency`
- `tool-usage`

Add when the task shape requires:
- `autonomous-planning` for multi-step work
- `long-task-support` for long-running work
- `traceability` for meaningful changes

## Selection rules for vibejam3

- Prefer the smallest stack that owns the current milestone phase.
- Do not start coding if the PRD chain is not complete.
- Do not call the project end-to-end complete until validation and hardening passed.
- Do not end a long task without either a completion verdict or a handoff pack.

## Hard bans

- No skill pileups without a phase reason.
- No autonomous flow that skips PRD chain on multi-step work.
- No coding path without proof.
- No hardening skipped on guide-production code.

## Output required

Return all of these:

- current vibejam3 milestone
- active autonomy stack
- worker skills selected for the current phase
- why this is the minimum sufficient stack
- current phase goal
- next proof step
- what condition advances to the next phase

## Completion standard

This skill is complete only when the task has a clear autonomous stack that is small enough to stay sharp and strong enough to carry the work end to end without guesswork.
