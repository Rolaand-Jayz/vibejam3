# 06 — System Component Map

## Purpose

This document inventories the major components that need to exist for the product to behave like a real guide production system rather than a single monolith with hidden behavior.

## Component inventory

| Component | Responsibility | Primary inputs | Primary outputs | Notes |
| --- | --- | --- | --- | --- |
| Project Intake | Captures guide intent, game selection, run style, spoiler tolerance, and policy preferences | user input | project manifest draft | Must preserve explicit user choices |
| Project Manifest Store | Persists project-level metadata and versioned settings | intake data, build settings | durable project records | Serves as the root of trust for a project |
| Knowledge Normalizer | Converts loose facts into structured entities | source notes, structured imports, references | knowledge entities and relations | Keeps provenance and confidence attached |
| Conflict Tracker | Records contradictions and ambiguity | normalized knowledge, source references | conflict records | Must not hide disagreements |
| Editorial Blueprint Compiler | Converts intent and knowledge into a chapter/section plan | manifest, knowledge graph, style policy | editorial blueprint | Drives the preview and full build |
| Preview Generator | Produces representative sample output | blueprint, subset knowledge, style rules | preview package | Exists specifically for review before full build |
| Full Guide Producer | Builds the full guide in bounded units | approved blueprint, knowledge, style rules | frozen guide draft sections | Must not stream the whole thing as one opaque pass |
| Unit Validator | Checks each produced unit for internal consistency | section output, knowledge references, policy rules | validation result | Runs at section/chapter family granularity |
| Global Consistency Validator | Checks cross-guide coherence | all frozen sections, terminology map, conflict data | global validation report | Ensures long-form consistency |
| Edition Manager | Freezes and versions approved guides | validated outputs, approval state | immutable guide editions | Prevents silent regeneration on reopen |
| Reader UI | Presents the frozen guide for reading | frozen edition, navigation state | user-visible reader experience | Must feel book-like and stable |
| Chat Companion | Answers questions using the guide and hidden codex | current edition, chat context, codex retrieval | grounded responses | Must not mutate the edition |
| Hidden Master Codex | Internal deeper knowledge layer | knowledge graph, unresolved notes, variants | chat/QA grounding material | Not user-facing content |
| Asset Manager | Tracks visual assets and provenance | uploaded/referenced assets | asset records, policy status | Enforces rights and provenance discipline |
| Map Policy Engine | Applies map-specific sourcing rules | map candidates, provenance data | allow/deny decisions | “No hallucinated maps” lives here |
| Provider Registry | Catalogs supported model providers and capabilities | provider config | provider adapter instances | Keeps vendor logic isolated |
| Prompt Orchestrator | Compiles prompts and context bundles for jobs | blueprint, knowledge, settings | request payloads and prompt artifacts | Must remain versioned and inspectable |
| Exporter | Produces frozen guide exports and bundles | frozen edition, render assets | export artifacts | Needed for durable handoff/reopen |
| Storage Services | Manages SQLite and filesystem access | domain records, artifacts | persistent state | Must keep data and large blobs separate |

## Dependency map

### Foundational dependencies

- Project Intake feeds everything else.
- Project Manifest Store is the root record for a project.
- Knowledge Normalizer and Conflict Tracker feed the blueprint compiler.
- Provider Registry and Storage Services support most runtime components.

### Production dependencies

- Blueprint Compiler depends on manifest + normalized knowledge.
- Preview Generator depends on the blueprint and provider access.
- Full Guide Producer depends on approved blueprint and validation gates.
- Unit Validator and Global Consistency Validator depend on guide outputs and knowledge references.
- Edition Manager depends on validated output and explicit approval.

### Consumption dependencies

- Reader UI depends on frozen editions.
- Chat Companion depends on frozen editions plus hidden codex retrieval.
- Asset Manager and Map Policy Engine depend on provenance data and policy rules.

## Interface expectations

Each component should expose a small, clear contract:

- explicit inputs
- explicit outputs
- explicit failure states
- explicit version tags when the shape matters

## Component design rules

- No component should own unrelated concerns.
- No component should depend directly on a vendor API if an adapter boundary can exist.
- No component should treat unresolved conflict as resolved.
- No component should mutate frozen guide editions in place.
- No component should hide policy failures behind generic success states.

## Practical implementation note

The component map should be implemented in a way that a maintainer can inspect the product from intake to freeze without needing to mentally decode hidden prompt logic.
