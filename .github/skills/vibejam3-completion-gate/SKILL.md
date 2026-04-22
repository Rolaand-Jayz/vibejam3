---
name: vibejam3-completion-gate
description: "Project-local completion gate tuned to vibejam3's acceptance criteria and trust-critical surfaces. Decides whether a slice or phase is honestly complete."
argument-hint: "Current vibejam3 slice or phase, completed work, evidence, and remaining blockers"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Completion Gate

Use this skill when vibejam3 work needs a truthful complete/blocked/incomplete verdict.

This is a project-specialized version of `shared-completion-gate`. It checks against vibejam3 acceptance criteria and trust surfaces instead of generic standards.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Checks whether the requested slice or phase is actually complete.
- Compares delivered work against `19-acceptance-criteria-matrix.md`.
- Checks vibejam3-specific trust surfaces: edition immutability, provider contracts, storage boundaries, validation coverage, rights compliance.
- Produces a verdict that another agent or user can trust.

## Required workflow

1. Restate the requested slice scope.
2. List the evidence that exists.
3. Check against vibejam3 acceptance criteria from the matrix.
4. Check vibejam3-specific trust surfaces:
   - Are edition immutability rules respected?
   - Are provider adapter interfaces honored?
   - Are storage boundaries correct (SQLite vs filesystem)?
   - Are consistency validators passing?
   - Are asset rights and provenance rules followed?
5. List what remains unresolved.
6. Issue one verdict:
   - complete
   - complete_with_followups
   - blocked
   - incomplete
7. Explain what condition would change the verdict.

## Completion rules for vibejam3

A slice is complete only when:
- the requested scope was actually delivered
- acceptance criteria from the matrix are met
- edition immutability is preserved
- provider contracts are honored
- validations run and pass
- no blockers are hidden in follow-up notes

## Hard bans

- No "done for now" masquerading as complete.
- No optimistic wording that outruns evidence.
- No hiding failed validation checks.
- No calling future work optional when it blocks the slice.

## Output required

Return all of these:

- verdict
- slice scope checked
- evidence used (specific PRD docs and test results)
- blockers
- follow-ups if any
- exact condition for a better verdict

## Completion standard

This skill is complete only when the current vibejam3 slice has a truthful verdict backed by visible evidence against the acceptance criteria matrix.
