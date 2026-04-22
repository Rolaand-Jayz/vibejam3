---
name: vibejam3-phase0-environment-inspector
description: "Project-local phase-0 skill for vibejam3 that inspects the actual workspace, detects the build/test/runtime stack, and reports readiness and missing pieces before implementation starts."
argument-hint: "Workspace path, current repo phase, or request to inspect vibejam3 before implementation"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Phase 0 Environment Inspector

Use this skill at the very start of `vibejam3` implementation work.

Its job is to inspect the real workspace and produce a truthful environment summary before any code is written.

## Non-bypassable no-stub rule

- This skill inherits a global, non-bypassable no-stub doctrine.
- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- Do **not** present scaffolding, temporary wiring, or speculative future behavior as finished product behavior.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Inspects the actual workspace layout.
- Detects whether the repo already has application code or is still docs-only.
- Identifies language, framework, package manager, build system, test system, and likely entry points.
- Finds the files the coding agent should consult first.
- Notes any missing pieces that block implementation startup.
- Produces a concise phase-0 report the next agent can use without guessing.

## When to use

Use this skill when:

- a new implementation phase is about to begin
- the repo state is unknown or partially known
- the coding agent needs a real baseline before editing files
- the user wants the fastest safe path from PRD to code

## Required workflow

1. Inspect the workspace root.
   - list directories and files
   - note whether the repo is docs-only, scaffolded, or already has app code

2. Detect the stack.
   - package manager files
   - build/test configs
   - frontend/backend framework markers
   - TypeScript / JavaScript / other language indicators
   - runtime or app entry points

3. Check project control files.
   - `agent-project-context.md`
   - `agent-plan-progress.md`
   - any repo-local instructions or notes

4. Identify implementation blockers.
   - missing manifest
   - missing source tree
   - no test commands
   - no build commands
   - no obvious entry point

5. Summarize the handoff readiness.
   - what exists
   - what is missing
   - what the next tiny step should be
   - whether the repo is ready for code or still needs scaffolding

## Hard bans

- No code changes.
- No creating missing app files just to make the inspection look successful.
- No assuming a framework that the workspace does not prove.
- No claiming implementation readiness without evidence from the workspace.
- No skipping the repo inspection because the PRD is already detailed.

## Output required

Return all of these:

- workspace shape
- detected stack or explicit absence of one
- key files found
- key files missing
- likely next phase
- coding handoff readiness
- blockers or assumptions

## Completion standard

This skill is complete only when the workspace has been inspected and the coding agent has a truthful phase-0 baseline to work from.