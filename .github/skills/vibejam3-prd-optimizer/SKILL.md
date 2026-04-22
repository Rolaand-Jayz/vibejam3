---
name: vibejam3-prd-optimizer
description: "Project-local optimizer that turns the vibejam3 PRD pack into the smallest safe skill chain for preimplementation, implementation, hardening, and deploy readiness."
argument-hint: "Path to the vibejam3 PRD pack or the current project phase"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 PRD Optimizer

Use this skill when working on `vibejam3` and the goal is to optimize the active skill set around the project PRD before implementation starts.

This skill is project-local on purpose. It exists to specialize the shared skill chain for the exact docs and phase boundaries in this repository.

## Non-bypassable no-stub rule

- This skill inherits a global, non-bypassable no-stub doctrine.
- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- Do **not** present scaffolding, temporary wiring, or speculative future behavior as finished product behavior.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## Source of truth

Use the vibejam3 PRD pack as the primary input:

- `COPILOT-PRD-vibe1/README.md`
- `COPILOT-PRD-vibe1/01-project-brief.md`
- `COPILOT-PRD-vibe1/02-requirements.md`
- `COPILOT-PRD-vibe1/03-constraints-and-assumptions.md`
- `COPILOT-PRD-vibe1/04-user-experience-spec.md`
- `COPILOT-PRD-vibe1/05-technical-architecture.md`
- `COPILOT-PRD-vibe1/07-data-model-and-schema-definitions.md`
- `COPILOT-PRD-vibe1/08-storage-and-infrastructure-strategy.md`
- `COPILOT-PRD-vibe1/09-api-and-integration-contracts.md`
- `COPILOT-PRD-vibe1/10-provider-abstraction-design.md`
- `COPILOT-PRD-vibe1/11-prompt-orchestration-design.md`
- `COPILOT-PRD-vibe1/12-consistency-validation-plan.md`
- `COPILOT-PRD-vibe1/13-asset-rights-and-policy-enforcement.md`
- `COPILOT-PRD-vibe1/14-ux-flow-and-state-model.md`
- `COPILOT-PRD-vibe1/15-tech-evaluation.md`
- `COPILOT-PRD-vibe1/16-milestone-plan.md`
- `COPILOT-PRD-vibe1/17-risk-register.md`
- `COPILOT-PRD-vibe1/18-test-strategy.md`
- `COPILOT-PRD-vibe1/19-acceptance-criteria-matrix.md`
- `COPILOT-PRD-vibe1/20-open-questions.md`
- `COPILOT-PRD-vibe1/21-inference-ledger.md`
- `COPILOT-PRD-vibe1/22-devils-advocate-review.md`
- `game_guide_production_system_spec.md`

If the PRD pack is missing or contradictory, stop and repair the pack first.

## What this skill does

- Uses phase 0 environment inspection first when the actual repo baseline is not yet established.
- Reads the project PRD pack and identifies the exact phase boundary.
- Optimizes the active skill set for speed without losing correctness.
- Chooses the smallest safe chain from the shared PRD companion skills.
- Keeps unrelated optimization, release, and implementation skills out of the active set until they are actually needed.
- Makes the next step obvious for the coding agent.

## Phase routing for this project

### Preimplementation phase

Use this order when the PRD still needs tightening:

0. `vibejam3-phase0-environment-inspector`
1. `vibejam3-prd-gap-closure`
2. `vibejam3-slice-selector`
3. `vibejam3-readiness-gate`
4. `vibejam3-prd-accelerator`

### Implementation phase

Use `e2e-coding` only after the current slice is ready and the handoff gate is truthful.

### Design phase

When a vibejam3 screen, flow, or component system needs design direction:

1. `vibejam3-app-design`
2. `anti-slop-app-design`

### Hardening and release phase

Use these when the project is close to shipping:

1. `vibejam3-quality-coach` if the codebase or slice is messy or uncertain
2. `vibejam3-release-readiness` for the final go/no-go verdict

### Completion and handoff phase

Use these when a phase is ending or work is being passed to another agent:

1. `vibejam3-completion-gate`
2. `vibejam3-handoff-pack`

### Optimization phase

If the bottleneck is not preimplementation but runtime, logic, or CI, use `optimize-system`. The shared optimizer now routes preimplementation bottlenecks into the PRD companion chain first.

## Hard rules

- Do not keep the whole skill pack active when only one phase matters.
- Do not skip environment inspection before the PRD chain when the repo baseline is still unknown.
- Do not start coding before the PRD gap, slice, and readiness questions are clear enough for the current phase.
- Do not use a broad optimizer when the real problem is preimplementation friction.
- Do not let speed collapse into skipping proof.
- Do not let release-readiness replace preimplementation readiness.

## Output required

Return all of these:

- current vibejam3 phase
- PRD docs consulted
- optimized skill chain for this phase
- why that chain is the fastest safe path
- next tiny step
- unresolved blockers or assumptions
- whether the current slice is ready for coding handoff

## Completion standard

This skill is complete only when the vibejam3 PRD pack can be mapped to a small, specialized, phase-correct skill chain that a coding agent can use without guessing.