# 20 — Open Questions

## Purpose

This document records the questions that used to be open and the v1 decisions that now close them out for implementation handoff.

The goal is not to stall the project. The goal is to keep the remaining future-phase choices visible without leaving v1 handoff gaps.

## Resolved v1 decisions

| ID | Question | V1 resolution | Confidence | Basis |
| --- | --- | --- | --- | --- |
| Q-01 | What is the first supported ingestion path for game knowledge? | Start with structured/manual intake plus curated source references. Do not make OCR-heavy or arbitrary scan ingestion the primary path in v1. | high | 02 requirements, 03 constraints, 11 orchestration |
| Q-02 | How deep should the preview slice be? | Use a bounded representative slice that exercises tone, layout, reference format, and checklist/callout behavior, without trying to equal a full guide. | high | 04 UX, 11 orchestration, 16 milestones |
| Q-03 | How strict should hidden codex exposure rules be in chat? | Keep the internal grounding invisible. Chat may explain uncertainty, but it must not expose raw hidden-codex structure or reasoning traces. | high | 04 UX, 11 orchestration, 14 state model |
| Q-04 | What is the minimum asset policy for v1? | Only provenance-backed assets may enter the edition. Reject or scrub anything without a clear policy-approved path. | high | 13 asset policy, 12 validation plan |
| Q-05 | How should map permissions be represented? | Every map asset needs provenance, rights status, and an allowed-use rationale. Default to deny until explicit approval exists. | high | 07 data model, 13 asset policy, 12 validation plan |
| Q-06 | How should provider credentials be stored? | Treat credentials as BYOK secrets that stay outside guide content, logs, exports, and the guide database. Use host/environment secret handling appropriate to the deployment. | high | 08 storage, 09 API, 10 provider abstraction |
| Q-07 | Is local-first the long-term product posture or only the first step? | v1 is local/web-first. Desktop shell packaging is deferred until the web-first product proves itself. | high | 03 constraints, 05 architecture, 15 tech evaluation |
| Q-08 | What export formats are required at v1? | The canonical export is a durable frozen-edition artifact bundle with metadata, validation evidence, and render outputs. Extra consumer formats such as PDF or EPUB are deferred. | high | 07 data model, 08 storage, 09 API, 16 milestones |
| Q-09 | How much of the hidden master codex should be editable by users? | None of it directly in v1. Users work through project intake, blueprint, preview, and edition workflows; the hidden codex stays internal-only. | high | 01 project brief, 04 UX, 11 orchestration |
| Q-10 | How should edition diffing be presented? | Show edition history and version identity first. Rich comparison views can wait until users prove they need them. | medium-high | 04 UX, 14 state model, 16 milestones |
| Q-11 | What degree of OCR or screenshot ingestion should exist in v1? | Keep OCR-heavy and arbitrary scan ingestion out of v1. Manual or curated inputs are the starting point. | high | 01 project brief, 03 constraints, 15 tech evaluation |
| Q-12 | Should the product later support cloud sync? | Not required for v1. Do not let sync shape the first storage design. | high | 01 project brief, 03 constraints, 08 storage |
| Q-13 | Should the app eventually ship as a desktop shell? | Defer. The first implementation is web-first, with desktop packaging only if later evidence justifies it. | high | 03 constraints, 05 architecture, 15 tech evaluation |
| Q-14 | How should human review be represented when validation fails? | Validation failure must be a first-class review state with blocking vs warning classification and an explicit resolution path before freeze. | high | 12 validation plan, 14 state model, 19 acceptance criteria |
| Q-15 | What is the minimum acceptable quality threshold for a frozen edition? | Freeze only when all mandatory validations and policy checks pass. Warnings may remain only if they are explicitly non-blocking. | high | 12 validation plan, 17 risk register, 19 acceptance criteria |

## Deferred future decisions

These are not v1 blockers; they are explicitly deferred until real demand or a later phase makes them worth revisiting:

- richer edition comparison UX
- broader export format support
- desktop shell packaging
- cloud sync
- richer OCR automation

## Handoff note

No v1 open-question blocker remains. The implementation handoff should treat the decisions above as locked unless the scope changes.
