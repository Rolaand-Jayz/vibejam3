# 11 — Prompt Orchestration Design

## Purpose

The orchestration layer turns project intent, structured knowledge, style decisions, and policy rules into controlled model requests.

It exists so the system can generate a guide edition deliberately instead of relying on one huge prompt and hope.

## Orchestration principles

- **Stage-aware:** each generation step should know what it is producing.
- **Versioned:** prompt templates and compilation logic should be versioned.
- **Bounded:** no uncontrolled “generate everything” call.
- **Inspectable:** prompt bundles and their inputs should be auditable.
- **Policy-aware:** the orchestrator must honor spoiler, asset, and map restrictions.

## Prompt stages

### 1. Intake synthesis prompt

Converts the user’s request into a structured project intent summary.

Output:

- project focus
- guide type
- run style assumptions
- spoiler policy summary
- unresolved questions

### 2. Blueprint compilation prompt

Turns the intake plus normalized knowledge into an editorial plan.

Output:

- table of contents
- section purposes
- detail targets
- cross-reference strategy
- terminology rules
- visual pack intent

### 3. Preview generation prompt

Produces a representative slice that exercises the key user-visible behaviors of the guide.

Output:

- sample chapter prose
- sample reference blocks
- sample checklist/callout behavior
- tone and layout signal

### 4. Section generation prompts

Produce controlled portions of the full guide.

The orchestrator should generate one bounded unit at a time, such as:

- a chapter group
- a reference family
- an appendix family

### 5. Validation prompts

Use model-assisted checks only where they add value, such as style consistency or coverage review, while keeping deterministic validators in place for the actual gatekeeping.

### 6. Chat grounding prompts

Build response context for in-guide questions using the frozen edition and hidden codex material.

## Context assembly rules

The orchestrator should assemble prompt bundles from clear ingredients:

- project manifest
- editorial blueprint
- relevant knowledge entities
- conflict summaries
- style bible
- spoiler policy
- edition history where relevant

It should not blindly dump the entire project into every request.

## Context budgeting

The product needs explicit budget discipline so it can scale to long guides without losing control.

The prompt orchestrator should respect:

- token budgets per stage
- section-specific context limits
- hidden codex retrieval limits
- preview versus full-build differences

Budgets should be treated as part of the design, not as a last-minute optimization.

## Prompt artifact versioning

Every major prompt template should be versioned so the team can trace output changes back to a design decision.

Artifact examples:

- style bible version
- blueprint template version
- preview template version
- section template version
- chat grounding template version

## Generation control rules

- The system should never generate the entire guide in one uncontrolled stream.
- Each produced unit should be validated before the next unit inherits its assumptions.
- The orchestration layer should be able to stop, report a problem, and resume from a known checkpoint.

## Hidden master codex usage

The hidden codex should be used as a grounding and QA surface, not as a hidden way to rewrite visible guide content.

The orchestration layer should keep these purposes separate:

- visible guide generation
- internal consistency checking
- chat response grounding
- unresolved conflict interpretation

## Failure handling

Prompt orchestration should make failures legible:

- missing knowledge should be reported
- provider refusal should be captured
- policy rejection should block the relevant unit
- inconsistent context should trigger validation, not silent continuation

## Anti-patterns to avoid

- one giant prompt with no checkpoints
- copying the whole knowledge graph into every request
- letting hidden codex material bleed into the visible guide without review
- using chat prompts to patch over broken editorial planning
- treating prompt text as if it were a substitute for validation logic
