# 19 — Acceptance Criteria Matrix

## Purpose

This matrix ties the product requirements to concrete acceptance criteria and the most useful proof method.

## Matrix

| Criterion | Covered requirements | Acceptance condition | Suggested proof |
| --- | --- | --- | --- |
| AC-01 Project intake is durable | FR-01 to FR-05 | A user can create a project with the required manifest fields and reopen it later with the same values | integration + e2e |
| AC-02 Knowledge is structured | FR-06 to FR-09 | Source facts are stored as normalized entities with provenance and conflict markers | unit + integration |
| AC-03 Blueprint exists before generation | FR-10 to FR-12 | The system compiles a versioned editorial blueprint before preview or full production | integration |
| AC-04 Preview is mandatory | FR-13 to FR-16 | Full build cannot proceed without a generated, reviewable preview and explicit approval | e2e |
| AC-05 Controlled full build exists | FR-17 to FR-20 | The guide is produced in bounded units and only frozen after validation passes | integration + e2e |
| AC-06 Frozen editions are immutable | FR-21 to FR-23 | Reopening a guide shows the exact frozen edition and does not regenerate content silently | e2e |
| AC-07 Chat is edition-grounded | FR-24 to FR-27 | Chat answers are grounded in the selected frozen edition plus hidden codex and do not mutate the edition | integration + e2e |
| AC-08 Asset policy is enforced | FR-28 to FR-31 | Unapproved assets are rejected or scrubbed and map assets require provenance | integration |
| AC-09 Durable persistence exists | FR-32 to FR-34 | Core project, edition, preview, validation, and export artifacts survive restart and can be reopened | integration |
| AC-10 Trust signals are visible | EX-01 to EX-03 | The interface clearly distinguishes draft, preview, frozen, and chat states | e2e |
| AC-11 Consistency is validated | NFR-04 to NFR-06 | The system detects terminology drift and cross-section contradictions before freeze | integration |
| AC-12 Provider abstraction is portable | NFR-10 to NFR-12 | A different provider can be used through the same internal contract without changing domain logic | contract tests |
| AC-13 Long jobs are observable | NFR-07 to NFR-09 | Production jobs expose progress and can be resumed or inspected after interruption | integration |

## Notes on proof quality

Acceptance criteria should not be validated by hand-wavy screenshots alone. The strongest proof combines:

- schema and contract checks for correctness
- integration tests for storage and job lifecycle behavior
- end-to-end tests for the user-visible story

## Criteria that must remain blocking

- edition immutability
- preview-before-full-build enforcement
- provenance retention
- map and asset policy enforcement
- chat edition grounding
