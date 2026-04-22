# COPILOT-PRD

This folder is the working PRD and pre-implementation pack for `vibejam3`.

It is intentionally exhaustive. The goal is not to sound complete; the goal is to leave the product, design, and engineering team with enough detail to build the first real version without guessing about the intended behavior.

## What this pack is for

- Capturing the product definition in a form that can survive implementation, review, and revision.
- Separating explicit facts from assumptions so the project does not quietly drift.
- Turning the source vision into a buildable plan with clear scope boundaries.
- Making the pre-implementation decisions visible before code exists.

## What this pack is not for

- It is not implementation code.
- It is not a marketing plan.
- It is not a loose brainstorm.
- It is not a substitute for validation with real source material, real provider behavior, or real asset rights checks.

## Inference posture

- Selected inference level for this pack: `very high`
- Reason: the source specification is already strong and opinionated, the user asked for exhaustive output, and the immediate need is a build-ready planning set rather than a clarification workshop.

## Reading order

1. `01-project-brief.md`
2. `02-requirements.md`
3. `03-constraints-and-assumptions.md`
4. `04-user-experience-spec.md`
5. `05-technical-architecture.md`
6. `06-system-component-map.md`
7. `07-data-model-and-schema-definitions.md`
8. `08-storage-and-infrastructure-strategy.md`
9. `09-api-and-integration-contracts.md`
10. `10-provider-abstraction-design.md`
11. `11-prompt-orchestration-design.md`
12. `12-consistency-validation-plan.md`
13. `13-asset-rights-and-policy-enforcement.md`
14. `14-ux-flow-and-state-model.md`
15. `15-tech-evaluation.md`
16. `16-milestone-plan.md`
17. `17-risk-register.md`
18. `18-test-strategy.md`
19. `19-acceptance-criteria-matrix.md`
20. `20-open-questions.md`
21. `21-inference-ledger.md`
22. `22-devils-advocate-review.md`

## Approval gates before implementation

Implementation should not begin until the following are stable enough to trust:

- product scope and v1 cut line
- project and edition persistence model
- provider abstraction contract
- preview and freeze workflow
- asset and map policy boundaries
- data model baseline
- test strategy and acceptance matrix

## Pack conventions

- Each document should be read as part of the whole. A requirement without a matching data model, UX behavior, or validation rule is incomplete.
- Explicit facts are always preferred over smart guesses.
- Assumptions should be written where they can be challenged.
- Anything that could affect scope, trust, legal exposure, or the long-term shape of the product should be made visible rather than buried.
