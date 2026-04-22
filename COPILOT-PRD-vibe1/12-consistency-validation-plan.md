# 12 — Consistency Validation Plan

## Purpose

This product lives or dies on whether its long-form output stays consistent.

The validation plan exists so the system can catch contradictions, policy problems, and structural drift before a guide is frozen as an edition.

## Validation philosophy

- validate at multiple levels
- separate deterministic checks from model-assisted checks
- treat failed validation as a real product state, not an embarrassment to hide
- make the output of validation readable enough for a human to act on

## Validation layers

### Unit-level validators

These validate a section, chapter family, or asset in isolation.

Examples:

- terminology consistency within a section
- required section fields present
- citation/provenance attached
- spoiler policy respected
- policy-allowed asset included

### Cross-unit validators

These compare neighboring or related guide units.

Examples:

- one chapter does not contradict another chapter’s stated mechanics
- terminology remains stable across sections
- a build step does not drop required references

### Global validators

These inspect the whole guide edition.

Examples:

- overall coverage is complete enough for the intended guide type
- the frozen edition has no unresolved blocking conflicts
- the visible guide and hidden codex remain distinct
- edition freeze preconditions are satisfied

## Suggested validator set

### 1. Terminology validator

Checks that terms, names, and labels are used consistently.

### 2. Coverage validator

Checks that the guide covers the planned chapters, references, and appendices.

### 3. Provenance validator

Checks that factual claims and assets carry source references.

### 4. Conflict validator

Checks that contradictions are recorded and not silently flattened.

### 5. Spoiler policy validator

Checks that content matches the chosen spoiler boundaries.

### 6. Asset policy validator

Checks that non-text assets satisfy rights and policy rules.

### 7. Map policy validator

Checks that map-like content has explicit provenance and allowed-use rationale.

### 8. Edition readiness validator

Checks that the edition can be frozen without missing prerequisites.

## Blocking vs warning policy

Not every issue should block the build, but the line must be explicit.

### Blocking issues

- missing provenance for required factual content
- unsupported or disallowed assets
- unresolved contradictions that affect guide correctness
- edition freeze preconditions not met
- guide content that violates spoiler policy

### Warning issues

- wording that could be improved without affecting correctness
- lower-priority gaps in optional reference depth
- non-blocking style inconsistencies that are still visible to reviewers

## Validation flow

1. validate each unit as it is produced
2. accumulate findings in structured reports
3. compare related units for cross-guide consistency
4. run global validation before freeze
5. block freeze until required checks pass
6. store the validation report with the edition

## Remediation workflow

When validation fails, the system should be able to answer:

- what failed
- where it failed
- whether the failure is blocking
- what needs to be regenerated or corrected
- whether the issue belongs to the guide, the blueprint, the knowledge base, or the asset set

## Human review handoff

Some issues are best resolved by humans, especially:

- ambiguous source interpretation
- unresolved policy questions
- conflicting knowledge where no clear authority exists

The system should surface these clearly rather than pretending they are solved.

## Anti-patterns to avoid

- one validation pass at the very end only
- blending warnings and blockers into one unreadable list
- hiding validation failures behind successful build states
- validating style while ignoring provenance
- freezing editions that still have unresolved major conflicts
