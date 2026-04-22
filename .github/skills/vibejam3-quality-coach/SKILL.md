---
name: vibejam3-quality-coach
description: "Project-local quality coach tuned to vibejam3's guide-production system. Checks whether the app is broken, risky, messy, or ready for the next phase."
argument-hint: "Current vibejam3 app state, milestone, or area you want checked"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Quality Coach

Use this skill to tell the truth about vibejam3's local readiness before sharing, shipping, or moving to the next phase.

This is a project-specialized version of `shared-local-quality-coach`. It checks against vibejam3-specific quality surfaces instead of generic criteria.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Checks whether vibejam3 builds and runs correctly.
- Checks whether guide-production workflows actually work end to end.
- Checks quality surfaces specific to this product: edition immutability, provider adapters, storage boundaries, validation coverage, rights compliance, UX flow.

## Required workflow

1. Run `npm run build` and check the result.
2. Run `npm run typecheck` if available and check the result.
3. Check the current milestone scope against `19-acceptance-criteria-matrix.md`.
4. Check vibejam3-specific quality surfaces:
   - **Build health**: does `npm run build` pass?
   - **Type safety**: does TypeScript compile cleanly?
   - **Storage integrity**: are SQLite operations correct?
   - **Provider contracts**: do adapter interfaces match the PRD?
   - **Edition rules**: is immutability preserved?
   - **Validation**: do consistency validators pass?
   - **UX flow**: do screen transitions match `14-ux-flow-and-state-model.md`?
   - **Rights/provenance**: are asset policies honored?
5. Issue a verdict.

## Verdict levels

- **broken**: app does not build, crashes, or produces wrong results
- **risky**: builds but has known gaps in trust-critical surfaces
- **messy but okay**: works but needs cleanup before the next phase
- **ready**: passes all checks for the current milestone

## Routing rules

- If broken → debug and fix
- If correctness is the main issue → `shared-correctness-hardening`
- If stability is the main issue → `shared-stability-hardening`
- If the app is close to shipping → `vibejam3-release-readiness`

## Output required

Return all of these:

- verdict
- evidence for that verdict (build output, test results, specific checks)
- the most important next fix or confirmation
- why that next step matters
- what it unlocks if it passes

## Completion standard

This skill is complete only when vibejam3's current state has an honest readiness verdict and the highest-priority next action is clear.
