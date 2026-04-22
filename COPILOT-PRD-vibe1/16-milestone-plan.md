# 16 — Milestone Plan

## Planning principles

- each milestone must produce something testable and useful
- each milestone must preserve edition truth and artifact durability
- each milestone should reduce risk rather than merely increase code volume
- the plan should keep the first useful slice small enough to ship safely

## Phase 1 — Foundation and project skeleton

### Phase 1 goal

Create a runnable application shell and the baseline storage/model structure.

### Phase 1 outputs

- project manifest model
- base UI shell
- storage scaffolding
- job tracking primitives
- directory/artifact layout

### Phase 1 exit criteria

- app starts locally
- project can be created and persisted
- storage can survive restart

### Phase 1 key risks

- setting the wrong architectural boundary too early
- coupling UI state to ephemeral memory

## Phase 2 — Intake and project manifest

### Phase 2 goal

Capture the guide’s intent in durable structured form.

### Phase 2 outputs

- project intake flow
- editable manifest
- explicit spoiler and depth settings
- persisted project metadata

### Phase 2 exit criteria

- user can create a project with all required fields
- manifest survives reopening

### Phase 2 key risks

- vague intake fields that are not strong enough to drive generation

## Phase 3 — Knowledge assembly and provenance tracking

### Phase 3 goal

Normalize source knowledge into structured entities with provenance and conflict tracking.

### Phase 3 outputs

- knowledge entity schema
- relationship links
- conflict records
- source reference tracking

### Phase 3 exit criteria

- knowledge can be stored, queried, and reviewed with provenance intact

### Phase 3 key risks

- letting source ambiguity disappear into unstructured notes

## Phase 4 — Editorial blueprint and preview slice

### Phase 4 goal

Build the plan that determines what the final guide will contain, then generate a representative preview for review.

### Phase 4 outputs

- editorial blueprint
- style bible
- preview package
- review and revision loop

### Phase 4 exit criteria

- preview is reviewable before full build
- preview is sufficiently representative to validate the product direction

### Phase 4 key risks

- preview too small to be trustworthy
- blueprint too vague to govern a real full build

## Phase 5 — Controlled full production and freeze

### Phase 5 goal

Generate the complete guide in bounded units and freeze the first approved edition.

### Phase 5 outputs

- chapter and appendix generation workflow
- unit validation reports
- global consistency validation
- frozen edition snapshot

### Phase 5 exit criteria

- full guide can be produced in sections
- edition can be frozen without mutation later

### Phase 5 key risks

- long-form inconsistency
- build failures that are hard to resume

## Phase 6 — Reader and chat companion

### Phase 6 goal

Make the frozen guide useful during play and keep chat grounded in the approved edition.

### Phase 6 outputs

- book-like guide reader
- edition navigation
- in-guide chat surface
- edition-aware chat grounding

### Phase 6 exit criteria

- user can read a frozen edition and ask questions without changing it

### Phase 6 key risks

- chat acting like a hidden editor
- the reader becoming too much like a chatbot

## Phase 7 — Asset policy, maps, and validation hardening

### Phase 7 goal

Make rights, provenance, and trust signals strong enough for real use.

### Phase 7 outputs

- asset policy engine
- map policy checks
- scrubber output
- stronger validation reporting

### Phase 7 exit criteria

- assets and maps cannot enter the edition without policy approval

### Phase 7 key risks

- policy logic being too weak to matter or too strict to be useful

## Phase 8 — Export, reopen, and release readiness

### Phase 8 goal

Make the edition durable enough to leave the original session and still behave correctly.

### Phase 8 outputs

- export packages
- reopen behavior
- edition history
- release checklist and hardening pass

### Phase 8 exit criteria

- frozen editions reopen exactly as saved
- exported artifacts remain traceable to their edition

### Phase 8 key risks

- export formats losing critical metadata
- hidden regeneration on reopen

## Deferred scope

The following should remain out of the first milestone chain unless later evidence demands otherwise:

- public sharing platform
- collaborative editing
- automatic legal clearance of every asset type
- broad multi-device sync
- elaborate desktop packaging
