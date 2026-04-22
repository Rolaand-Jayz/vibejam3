# 02 — Requirements

## Requirement structure

The requirements are grouped by workflow stage and by product concern. Each item should be readable as a testable statement, not as a vague aspiration.

Where helpful, the requirements use short IDs so they can be traced into architecture, data modeling, test strategy, and acceptance criteria.

## Functional requirements

### Project setup and intake

- **FR-01** The system must let a user create a new guide project for a specific game and guide intent.
- **FR-02** The intake flow must capture the guide’s target game, player goal, preferred run style, spoiler tolerance, and desired depth.
- **FR-03** The intake flow must capture any known platform or version constraints that affect accuracy.
- **FR-04** The system must preserve the original intake values as part of the project manifest.
- **FR-05** The system must make it obvious which fields are required, which are optional, and which are still unresolved.

### Knowledge assembly

- **FR-06** The system must support normalization of game knowledge into structured entities rather than only freeform notes.
- **FR-07** The system must preserve source references, confidence, and conflict markers for knowledge entries.
- **FR-08** The system must keep the hidden master codex separate from the visible guide edition.
- **FR-09** The system must support both broad knowledge coverage and run-specific knowledge narrowed to the player’s requested intent.

### Blueprint and editorial planning

- **FR-10** The system must compile an editorial blueprint before full guide generation begins.
- **FR-11** The blueprint must include chapter order, section purpose, cross-reference logic, terminology rules, spoiler policy, and visual pack intent.
- **FR-12** The blueprint must be versioned so later editions can be compared to earlier ones.

### Preview-before-full-build

- **FR-13** The system must generate a representative preview slice before any full guide build is approved.
- **FR-14** The preview must be broad enough to validate tone, density, layout style, checklist behavior, and reference formatting.
- **FR-15** The preview must be reviewable and revisable before the full build is allowed to continue.
- **FR-16** The system must record approval state explicitly and must not treat preview output as an implicit approval.

### Full guide production

- **FR-17** The system must generate the guide in controlled units rather than one uncontrolled stream.
- **FR-18** Each production unit must be validated before it is locked into the edition.
- **FR-19** The system must support chapter sections, reference families, and appendices as separately validated outputs.
- **FR-20** The system must preserve the final guide content as an immutable frozen edition after approval.

### Reopen, fork, and edition history

- **FR-21** Reopening a saved guide must show the exact approved edition rather than silently regenerating it.
- **FR-22** The system must support edition history so later changes can become new editions without destroying earlier ones.
- **FR-23** The system must make it obvious when the user is viewing a frozen edition versus a draft or preview state.

### In-guide chat

- **FR-24** The system must support chat grounded in the frozen guide plus the hidden master codex.
- **FR-25** Chat responses must not overwrite or rewrite the frozen edition.
- **FR-26** The chat experience must remain aligned with the approved guide version the user is currently viewing.
- **FR-27** Chat must be able to explain uncertainty or conflicts instead of smoothing them away.

### Asset and map handling

- **FR-28** The system must track asset provenance and policy status for all included visual materials.
- **FR-29** The system must not hallucinate maps or present unsupported map assets as verified.
- **FR-30** If maps are included, the system must record where they came from and why they are allowed.
- **FR-31** The system must be able to scrub or reject assets that fail rights or policy checks.

### Persistence and export

- **FR-32** The system must persist the project manifest, knowledge artifacts, blueprint, preview, frozen edition, and validation reports durably.
- **FR-33** The system must preserve enough artifact structure that the guide can be reopened exactly as saved.
- **FR-34** The system must support export of the frozen edition in usable artifact form.

## Non-functional requirements

### Trust and correctness

- **NFR-01** The system must distinguish explicit facts, inferred assumptions, and unresolved questions.
- **NFR-02** The system must record conflicts rather than bury them.
- **NFR-03** The system must avoid presenting uncertain knowledge as certain.

### Consistency and editorial quality

- **NFR-04** Long-form content must remain consistent in terminology, style, and tactical advice across the guide.
- **NFR-05** The system must validate cross-section consistency before freezing an edition.
- **NFR-06** The system must keep the hidden knowledge layer and the visible guide layer separate.

### Performance and operational behavior

- **NFR-07** Guide production must be treated as a long-running job with explicit progress and intermediate outputs.
- **NFR-08** The system must not require the user to wait on a single giant generation pass that has no checkpoints.
- **NFR-09** The system must remain usable for reopened guides even when the original build session is gone.

### Provider and implementation independence

- **NFR-10** The application logic must remain provider-agnostic.
- **NFR-11** Core domain behavior must not depend on vendor-specific request or response shapes.
- **NFR-12** The product must remain open-source friendly and understandable without private vendor internals.

## Experience requirements

- **EX-01** The guide reader must feel calm, structured, and book-like rather than chat-like.
- **EX-02** The system must make review and approval feel deliberate, not accidental.
- **EX-03** Important trust signals such as edition state, source confidence, and policy flags must be visible when they matter.

## Scope boundaries

- The product is unofficial.
- The product is not a public wiki.
- The product is not a real-time collaborative writing platform.
- The product is not allowed to treat missing source evidence as if it were proof.
