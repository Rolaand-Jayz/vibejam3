---
name: vibejam3-handoff-pack
description: "Project-local handoff pack tuned to vibejam3's PRD structure, milestone plan, and file layout. Produces resumable state for agent-to-agent transfers."
argument-hint: "Current vibejam3 task, milestone, completed work, changed files, evidence, blockers, and next action"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Handoff Pack

Use this skill when vibejam3 work needs to be handed to another agent, session, or phase.

This is a project-specialized version of `shared-handoff-pack`. It structures handoff state around vibejam3's actual file layout and PRD structure instead of a generic format.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Compresses active vibejam3 task state into a resumable pack.
- Captures decisions, changed files, validations, blockers, and next actions.
- Uses vibejam3-specific file references instead of generic paths.

## Required workflow

1. Restate the current vibejam3 milestone and slice.
2. List what was completed.
3. List files changed or created (relative to project root).
4. Record the strongest evidence produced.
5. Record blockers, assumptions, and unresolved questions.
6. State the exact next action.
7. State which vibejam3 skill or phase should run next.

## Handoff format for vibejam3

Structure the pack around:

- **Milestone**: which milestone from `16-milestone-plan.md`
- **Slice**: which slice from the slice selector output
- **PRD docs consulted**: specific document numbers
- **Files touched**: source, config, test, and doc files
- **Evidence**: build results, test results, validation outputs
- **Blockers**: gaps from `vibejam3-prd-gap-closure` still unresolved
- **Next action**: exact next step and which vibejam3 skill runs it

## Packaging rules

- Prefer factual bullets over narrative recap.
- Keep the pack small enough to scan in one pass.
- Preserve failed approaches when they matter.
- Reference PRD docs by number (`02-requirements.md`) not just topic.
- If the work is incomplete, say incomplete plainly.

## Hard bans

- No ceremonial handoff summary with no next action.
- No "everything should be clear from the files" laziness.
- No dropping blockers because they are awkward.
- No hiding unverified assumptions.

## Output required

Return all of these:

- current milestone and slice
- completed work
- files touched
- evidence
- blockers or assumptions
- next exact action
- next vibejam3 skill or phase

## Completion standard

This skill is complete only when another agent or future session could resume the vibejam3 work from the handoff without guesswork.
