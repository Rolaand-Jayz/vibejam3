---
name: vibejam3-readiness-gate
description: "Project-local readiness gate tuned to vibejam3's PRD pack, acceptance criteria, and trust-critical surfaces. Decides whether the current slice can enter coding."
argument-hint: "Current vibejam3 slice, its acceptance criteria, and any remaining open questions"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Readiness Gate

Use this skill when a vibejam3 implementation slice needs a truthful go/no-go before coding starts.

This is a project-specialized version of `shared-deploy-readiness-gate`. It checks the slice against vibejam3-specific requirements instead of generic criteria.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Checks whether the current slice has enough clarity from the PRD pack to enter coding.
- Verifies that acceptance criteria from `19-acceptance-criteria-matrix.md` are testable.
- Confirms that provider adapter, storage, validation, and rights constraints are explicit.
- Produces a go / go-with-conditions / no-go verdict.

## Required workflow

1. Read the slice definition and the relevant PRD docs.
2. Check these vibejam3-specific readiness signals:
   - **Storage boundary**: is it clear what goes in SQLite vs filesystem?
   - **Provider contract**: does the slice need a specific provider adapter, and is the interface specified?
   - **Edition rules**: does the slice touch frozen editions, and are immutability rules clear?
   - **Validation**: does the slice introduce data that needs consistency checks, and are the validators specified?
   - **Rights/provenance**: does the slice handle assets, and are the policy rules clear?
   - **UX flow**: does the slice touch screens, and are the state transitions in `14-ux-flow-and-state-model.md`?
3. Check that acceptance criteria are testable.
4. Check that open questions do not block the slice.
5. Issue the verdict.

## Readiness rules for vibejam3

A slice is ready only when:

- the scope is explicit and mapped to the milestone plan
- storage boundaries are decided
- provider contracts are clear or the slice does not need them yet
- acceptance criteria are testable against `19-acceptance-criteria-matrix.md`
- edition immutability is respected
- validation paths are credible
- any rights/provenance concerns are addressed

## Hard bans

- No rubber-stamping.
- No confusing implementation-ready with release-ready.
- No calling a slice ready while hidden blockers remain in the PRD.
- No skipping the storage boundary check.

## Output required

Return all of these:

- verdict: go / go-with-conditions / no-go
- blockers
- conditions if any
- evidence used (specific PRD docs)
- next required skill or phase

## Completion standard

This skill is complete only when the current slice has a truthful go/no-go that the coding agent can act on without guessing.
