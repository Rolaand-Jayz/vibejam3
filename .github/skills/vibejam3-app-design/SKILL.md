---
name: vibejam3-app-design
description: "Project-local design wrapper that specializes anti-slop app design for vibejam3’s premium editorial, book-like guide-production workflow."
argument-hint: "Current vibejam3 screen, workflow, or UI slice to design"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 App Design

Use this skill when designing `vibejam3` screens, flows, components, or interface systems.

This is the project-local wrapper for `anti-slop-app-design`.

## Non-bypassable no-stub rule

- This skill inherits a global, non-bypassable no-stub doctrine.
- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- Do **not** present scaffolding, temporary wiring, or speculative future behavior as finished product behavior.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## Product-specific design stance

The product should feel like:

- a serious editorial tool
- a premium guide reader
- a durable, book-like system
- a calm but information-rich environment

It should **not** feel like:

- a generic AI copilot shell
- a neon-gradient chatbot dashboard
- a startup demo surface dressed up as a publishing tool
- a glassy prompt box with the rest of the product orbiting around it

## Required source docs

Use these docs as design truth:

- `COPILOT-PRD-vibe1/01-project-brief.md`
- `COPILOT-PRD-vibe1/04-user-experience-spec.md`
- `COPILOT-PRD-vibe1/13-asset-rights-and-policy-enforcement.md`
- `COPILOT-PRD-vibe1/14-ux-flow-and-state-model.md`
- `game_guide_production_system_spec.md`

## Product-specific design rules

- The reader should feel book-like, stable, and structured.
- The operator surfaces should feel editorial and operational, not chat-first.
- Trust signals such as edition state, provenance, validation status, and policy outcomes must be first-class UI elements.
- Long-form reading should breathe through hierarchy, spacing, and typography rather than decorative chrome.
- Chat must remain a subordinate sidecar, not the visual center of gravity.
- Asset and visual treatment must respect the product’s rights and provenance rules.

## Active anti-generic steering for vibejam3

Actively steer away from:

- giant assistant panels as the primary layout skeleton
- tokenized AI dashboard cards with interchangeable KPI aesthetics
- playful gradient branding that undermines editorial seriousness
- decorative sparkles, magic icons, or pseudo-intelligence motifs
- over-soft, over-rounded UI that removes the feeling of a serious reference tool
- lightweight note-app visuals that make long-form guide use feel disposable

Prefer:

- strong typography
- clear reading rhythm
- structured panels with explicit roles
- durable navigation
- visible edition identity
- design language that feels closer to publishing, field manuals, reference systems, and editorial tooling than to chat demos

## Output required

Return all of these:

- vibejam3 visual personality
- screen architecture for the requested flow
- component language specific to this product
- trust-signal strategy
- anti-generic avoid list for implementation
- why the design fits the repo’s PRD

## Completion standard

This skill is complete only when the resulting UI direction is unmistakably specific to vibejam3 and clearly avoids generic AI-product design habits.