---
name: vibejam3-slice-selector
description: "Project-local slice selector tuned to the vibejam3 PRD pack and milestone plan. Cuts the narrowest complete implementation slice for guide production."
argument-hint: "Current vibejam3 milestone or the feature area that needs slicing"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Slice Selector

Use this skill when the vibejam3 PRD is ready but the next implementation step needs a disciplined cut line.

This is a project-specialized version of `shared-smallest-safe-slice-selector`. It assumes the PRD pack at `COPILOT-PRD-vibe1/` and the milestone plan at `16-milestone-plan.md` as the source of truth.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Reads the vibejam3 PRD pack and milestone plan.
- Identifies the current milestone boundary.
- Cuts the narrowest slice that still produces a real, testable guide-production outcome.
- Preserves the constraints that matter: immutable editions, BYOK provider contracts, preview-before-full-build, and trust signals.
- Removes everything that belongs to a later milestone.

## Required workflow

1. Read `COPILOT-PRD-vibe1/16-milestone-plan.md` and the gap-closure output.
2. Identify the current milestone and its primary user value.
3. Determine the smallest set of features that delivers that value end to end.
4. Check against vibejam3 constraints:
   - Does the slice respect the SQLite+filesystem storage boundary?
   - Does the slice respect the provider adapter interface?
   - Does the slice require new consistency validators?
   - Does the slice introduce new asset types that need rights policy?
5. Remove everything that belongs to a later milestone.
6. Attach the proof plan: tests, validation, integration checks.

## Slice selection rules for vibejam3

- One guide-production value path per slice.
- Include only the storage, provider, and validation dependencies the slice strictly requires.
- Keep acceptance criteria mapped to `19-acceptance-criteria-matrix.md`.
- Preserve edition immutability even when cutting scope.
- If the slice touches the hidden master codex, treat it as trust-critical.
- If the slice touches prompt orchestration, include token budget validation.

## Hard bans

- No mega-slice disguised as speed.
- No skipping the storage boundary decision.
- No removing edition immutability just to look smaller.
- No slice that cannot be tested against the acceptance criteria matrix.

## Output required

Return all of these:

- slice name
- included scope
- excluded scope (with milestone destination)
- dependencies on other slices or milestones
- proof plan
- acceptance criteria (from matrix)
- next skill or phase

## Completion standard

This skill is complete only when the next vibejam3 implementation slice is narrow, testable against the acceptance criteria matrix, and obviously safe enough to hand to coding.
