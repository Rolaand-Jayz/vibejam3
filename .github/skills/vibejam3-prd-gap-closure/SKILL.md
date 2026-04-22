---
name: vibejam3-prd-gap-closure
description: "Project-local PRD gap closure tuned to the vibejam3 PRD pack structure. Finds blockers, confirms assumptions, and classifies unresolved questions specifically against the COPILOT-PRD-vibe1 documents."
argument-hint: "Which PRD section or phase boundary still has unresolved gaps"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 PRD Gap Closure

Use this skill when the vibejam3 PRD pack still has open questions or conflicting assumptions that block the next implementation phase.

This is a project-specialized version of `shared-prd-gap-closure`. It assumes the PRD pack at `COPILOT-PRD-vibe1/` as the source of truth and skips all generic PRD framing.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Reads the vibejam3 PRD pack specifically (`COPILOT-PRD-vibe1/01` through `22`).
- Finds gaps that block the current vibejam3 implementation slice.
- Classifies each gap against the project's actual constraints (TypeScript/React/Vite, SQLite+filesystem, BYOK provider adapters, immutable frozen editions, preview-before-full-build).
- Produces a closure note scoped to the current slice, not the entire product.

## Required workflow

1. Read the relevant PRD docs for the current slice.
   - Always read: `02-requirements.md`, `03-constraints-and-assumptions.md`, `19-acceptance-criteria.md`
   - Read domain-specific docs based on slice focus:
     - Storage/data: `07-data-model.md`, `08-storage-strategy.md`
     - Provider/LLM: `10-provider-abstraction.md`, `11-prompt-orchestration.md`
     - UX/screens: `04-ux-spec.md`, `14-ux-flow-and-state-model.md`
     - Validation: `12-consistency-validation.md`
     - Rights/assets: `13-asset-rights.md`

2. Build a gap map against the current slice.
   - What the slice requires that the PRD does not yet specify
   - What the PRD states that contradicts the current workspace reality
   - What assumptions were confirmed during phase-0 inspection vs still unproven

3. Classify each gap.
   - **Blocker**: must resolve before coding this slice
   - **Needs confirmation**: verify with user but proceed on safest default
   - **Deferred**: affects future phases only, document and move on

4. Produce the closure note.

## Domain-specific gap patterns for vibejam3

Watch for these common gap types in this project:

- **Provider adapter gaps**: BYOK contract not specified for a particular provider behavior
- **Edition immutability gaps**: unclear whether a change violates frozen edition rules
- **Storage boundary gaps**: unclear whether data belongs in SQLite or filesystem
- **Validation gaps**: missing consistency validator for a new data path
- **Rights/provenance gaps**: unclear asset policy for a new asset type
- **State model gaps**: screen transitions not covered in UX flow

## Hard bans

- No implementation code.
- No expanding scope beyond the current slice.
- No inventing requirements the PRD does not contain.
- No hiding uncertainty inside confident prose.

## Output required

Return all of these:

- blocker list (specific to current slice)
- confirmed assumptions
- must-answer questions
- deferred decisions
- which PRD docs need updates
- readiness verdict: ready / ready-with-conditions / blocked

## Completion standard

This skill is complete only when the current slice's preimplementation gaps are explicitly classified and the next phase can proceed or stop with a truthful blocker list.
