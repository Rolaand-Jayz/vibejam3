# 17 — Risk Register

## Risk scoring approach

The register uses a simple qualitative view:

- **Likelihood:** low / medium / high
- **Impact:** low / medium / high
- **Priority:** derived from the combination of the two

## Risks

| ID | Risk | Likelihood | Impact | Early warning | Mitigation |
| --- | --- | --- | --- | --- | --- |
| R-01 | Provider output quality varies too much across vendors | high | high | preview tone or structure drifts by provider | strict adapter layer, provider-specific capability checks, preview review gates |
| R-02 | The hidden master codex leaks into user-facing content | medium | high | visible guide starts referencing internal-only knowledge structures | hard separation of artifacts, validation checks, explicit grounding boundaries |
| R-03 | Frozen editions accidentally regenerate on reopen | medium | high | reopened content changes without a new edition id | immutable edition snapshots, reopen-by-reference only, regression tests |
| R-04 | Long-form consistency degrades across sections | high | high | terminology or recommendations conflict across chapters | unit + cross-unit + global validators, terminology bible, section checkpoints |
| R-05 | Asset rights or map provenance are incomplete | medium | high | assets lack source or usage status | asset policy engine, scrubber, hard rejection of unapproved items |
| R-06 | Guide production jobs become hard to resume | medium | high | build retries create duplicate or partial artifacts | job records, checkpoints, artifact versioning, idempotent stages |
| R-07 | Storage model becomes too clever and hard to inspect | medium | high | docs or code cannot explain where truth lives | SQLite + filesystem first, explicit schemas, no hidden blob magic |
| R-08 | Chat answers contradict the frozen edition | medium | high | user sees guidance that disagrees with approved content | edition-aware grounding, chat validation, response policy |
| R-09 | Intake is too vague to drive a meaningful guide | medium | medium | preview output feels generic or misaligned | stronger intake fields, preview review, explicit unresolved questions |
| R-10 | Scope creep turns the first version into a platform project | high | high | roadmap fills up with sync, sharing, collaboration, and marketplace work | milestone discipline, explicit non-goals, devil’s-advocate review |
| R-11 | Provider credentials handling becomes insecure or confusing | medium | high | secret data appears in logs or settings UI | strict credential boundary, redaction, secure storage discipline |
| R-12 | Validation becomes a formality instead of a real gate | medium | high | edition freezes despite unresolved blockers | blocking rules, visible validation report, freeze precondition checks |
| R-13 | The preview slice is not representative enough to trust | medium | medium | user approves preview but dislikes full guide tone | sample breadth checks, preview review rubric, style bible |
| R-14 | Performance and memory pressure appear during long builds | medium | medium | build jobs slow down or crash on large guides | bounded units, progress tracking, artifact segmentation |
| R-15 | Future maintainers cannot tell why a guide exists in a certain form | medium | medium | assumptions are buried in prompt text or memory only | inference ledger, open questions, versioned blueprint and validation logs |

## Highest-priority risks

The most dangerous risks are the ones that damage trust rather than merely causing bugs:

1. hidden regeneration
2. edition inconsistency
3. provider drift
4. policy or rights failure
5. hidden codex leakage

## Risk response principles

- Fail visibly rather than silently.
- Block freeze if trust-critical gates are unresolved.
- Preserve enough evidence to explain every important build decision.
- Prefer simplification when a risk comes from unnecessary scope.
