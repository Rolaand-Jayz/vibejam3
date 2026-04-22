---
description: "Use when implementing features, writing code, building UI, creating models, or doing any development work in the vibejam3 project. Covers architecture constraints, coding conventions, no-stubs policy, and PRD-first implementation flow."
applyTo: "src/**"
---

# Implementation workflow for vibejam3

## Architecture constraints

6-layer modular design. Keep layers separated:

1. **Presentation** (`src/components/`, `src/screens/`) — React UI only
2. **Application Orchestration** (`src/orchestration/`) — job tracking, workflow coordination
3. **Domain** (`src/domain/`) — guide state, edition semantics, knowledge entities, provenance
4. **Provider Adapter** (`src/providers/`) — LLM normalization, capability abstraction
5. **Validation & Policy** (`src/validation/`) — consistency checks, asset rights, map policy
6. **Storage** (`src/storage/`) — projects, editions, knowledge, artifacts

Never import a higher layer from a lower layer. Dependencies flow downward.

## PRD-first implementation

Every implementation slice must:
1. Start from a PRD requirement — trace code back to a specific FR/NFR/AC number
2. Pass the readiness gate before coding begins
3. Satisfy acceptance criteria from `COPILOT-PRD-vibe1/19-acceptance-criteria-matrix.md`

## No stubs policy

- No `pass`, `NotImplementedError`, TODO-backed user flows, or placeholder behavior in app code
- No fake completion paths or mock implementations presented as real
- If a real implementation cannot be completed, state that clearly and stop
- Prefer explicit "not implemented yet" over deceptive fallback logic

## Coding conventions

- TypeScript strict mode — no `any` types
- Zod schemas for all data boundaries (API inputs, storage reads, config)
- React functional components with hooks — no class components
- File naming: `kebab-case.tsx` for components, `kebab-case.ts` for utilities
- Each component file exports one named export matching its purpose

## Data model reference

See `COPILOT-PRD-vibe1/07-data-model-and-schema-definitions.md` for the full schema.
Key entities: Project, Edition, KnowledgeEntity, Blueprint, GuideUnit, ValidationEvidence.

## Design system

See `docs/design/ui-ux-design-spec.md` for the full design system.
Key rules:
- Premium editorial aesthetic — calm, authoritative, not bubbly
- Dark mode base with warm dark gradients
- No purple/blue neon, no glassmorphism, no sparkle motifs
- Nav rail (left), content area (center), sidecar (right), status bar (bottom)
