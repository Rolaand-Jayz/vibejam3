# 13 — Asset Rights and Policy Enforcement

## Purpose

The product may use visual materials, but it must do so with discipline.

This document defines how the system should treat assets, maps, provenance, and policy enforcement so the output stays honest about what is allowed and what is not.

## Policy goals

- preserve source provenance
- avoid unauthorized asset use
- keep map handling stricter than ordinary decorative art
- allow the system to reject or scrub risky assets
- make policy decisions visible enough for reviewers to understand

## Asset categories

### Generic visual assets

Examples:

- icons
- UI decorations
- diagrams
- chapter illustrations

### Map-related assets

Examples:

- zone maps
- region diagrams
- route maps
- area relationship visuals

### Screenshot-like or source-captured assets

Examples:

- in-game screenshots
- cropped UI references
- annotated scene captures

## Rights and provenance requirements

Every asset should know:

- what it is
- where it came from
- whether it is allowed to be used
- whether it has been transformed
- where it appears in the guide
- whether it passed scrub and policy checks

## Policy decisions the system must support

### Allow

Assets that have clear provenance and are approved under the project’s policy rules.

### Reject

Assets that are missing provenance, violate policy, or cannot be justified.

### Scrub

Assets that are otherwise usable but need sensitive details removed or transformed before inclusion.

### Defer

Assets that might be usable later but should not enter the current edition yet.

## Map policy

Maps deserve special handling because they are easy to overclaim.

Rules:

- maps cannot be invented by the system
- map assets must carry provenance
- map permissions or allowed-use rationale must be recorded
- if the source of a map is uncertain, the map must not be presented as verified

## Output scrubber responsibilities

The scrubber should remove or neutralize anything that violates policy before the guide is frozen.

This may include:

- unapproved visual references
- missing attribution markers
- unsupported map snippets
- assets with ambiguous source status

## Policy metadata

The system should store policy metadata with each asset:

- asset type
- provenance status
- rights status
- review status
- map eligibility
- scrub status
- edition linkage

## Review workflow

1. asset is introduced
2. provenance is recorded
3. policy engine evaluates it
4. asset is either approved, scrubbed, deferred, or rejected
5. policy outcome is stored with the project
6. frozen editions only include approved assets

## Anti-patterns to avoid

- “we probably can use it” as a policy category
- map inclusion without provenance
- hiding rejection reasons from reviewers
- assuming all transformed assets are automatically safe
- using a policy layer only as a post-hoc excuse instead of a real gate
