# Game Guide Production System — Production Specification

## 1. Product Definition

Build a free and open-source, provider-agnostic, BYOK game guide production system that creates durable, book-grade, personalized unofficial strategy guides for a specific game, a specific player goal, and a specific run style.

This is **not** a live-regenerating chatbot and **not** a public guide repository.

It is a **guide production pipeline** that:

1. gathers and normalizes game knowledge,
2. produces a preview slice for user review,
3. generates a full end-to-end guide with consistent writing and art style,
4. freezes the approved guide as a durable edition,
5. lets the user reopen the guide exactly as saved,
6. provides an in-guide chat mode during play,
7. uses a hidden, deeper “master codex” knowledge layer for chat and consistency,
8. supports later edition forks without overwriting prior editions.

---

## 2. Core Product Promise

The product should feel like:

> “Give me the exact premium strategy guide I would have wanted for this specific run, and let me keep using it throughout the entire game without it changing under me.”

The product must:

- feel as rich and structured as classic physical strategy guides,
- remain stable across dozens of hours of play,
- preserve internal consistency from first page to last appendix,
- support a playable companion chat experience without mutating the saved guide,
- separate visible player-facing content from hidden deeper knowledge used for chat and QA.

---

## 3. Product Scope

### 3.1 In Scope

- Personalized full-guide generation
- Preview-before-full-build workflow
- Durable saved guide editions
- Consistent writing style across entire guide
- Consistent art and layout system across entire guide
- Player-facing reading mode
- Player-facing chat mode grounded in the saved guide plus hidden master codex
- Structured encyclopedic coverage:
  - walkthrough
  - bestiary
  - inventory / item compendium
  - weapons / armor / accessories
  - jobs / classes / skills / spells / abilities
  - build guidance
  - progression routing
  - collectibles / completion tracking
  - maps support with strict source policy
  - appendices / reference tables
- Guide editioning and project persistence
- Provider-agnostic BYOK model integration
- Full artifact persistence so guides reopen exactly as saved

### 3.2 Out of Scope for v1

- Public guide-sharing platform
- Marketplace
- Multiplayer collaborative editing
- Automatic legal clearance for all copyrighted assets
- Universal automatic map extraction for every game
- Universal OCR-heavy ingestion of arbitrary scans as primary pipeline
- Real-time cloud sync as a hard requirement

---

## 4. Non-Negotiable Product Constraints

1. **No silent regeneration on reopen.** A saved guide must reopen exactly as last approved.
2. **Preview before full production.** The user must review representative output before full build.
3. **Full build is a frozen edition.** Changes after approval produce a new edition, not overwrite.
4. **Long-form consistency is system-enforced, not model-memory-dependent.**
5. **Chat must not rewrite the guide.** Chat is grounded to the saved guide + hidden master codex.
6. **Player-facing guide and hidden master codex are separate artifacts.**
7. **Maps cannot be hallucinated.** If maps are included, they must come from allowed sources and be provenance-tracked.
8. **All visual assets must follow an asset-rights policy.**
9. **The app must remain provider-agnostic and BYOK.**
10. **Open-source application logic, orchestration, schemas, and policies must be independent of any single model provider.**

---

## 5. Primary User Stories

### 5.1 Guide Creation
- As a player, I want to define the exact kind of guide I need for a specific game and run style.
- As a player, I want to preview the guide direction before committing to a long build.
- As a player, I want the full guide to match the quality, depth, and structure of classic premium guides.

### 5.2 Durability
- As a player, I want to reopen my guide later and see it exactly as it was.
- As a player, I do not want model drift to change my guide.
- As a player, I want edition history so that updates do not destroy the original guide.

### 5.3 In-Play Use
- As a player, I want to chat with the guide while I play.
- As a player, I want the chat to know both my current playthrough guide and the broader game knowledge.
- As a player, I want the chat to answer without contradicting the guide edition I approved.

### 5.4 Completion and Reference
- As a player, I want reference sections for all major game systems and content.
- As a player, I want bestiary, items, skills, classes, weapons, armor, spells, jobs, and related systems documented like old print guides.
- As a player, I want maps when legally and technically supportable.

### 5.5 Trust
- As a player, I want to know where knowledge came from internally.
- As a player, I want conflicts and uncertainty handled consistently.
- As a player, I want the guide to remain coherent from beginning to end.

---

## 6. Core Architecture Overview

The system must be designed as a **multi-layer production pipeline** with durable artifacts.

### 6.1 Major Layers

1. **Project Layer**
   - stores project metadata, user intent, build settings, edition history

2. **Knowledge Layer**
   - stores structured facts, entities, relationships, provenance, conflicts, version info

3. **Blueprint Layer**
   - stores the editorial plan, outline, style bible, terminology bible, coverage targets, page logic

4. **Asset Layer**
   - stores approved art assets, diagrams, icon packs, map assets, screenshot provenance, style pack metadata

5. **Production Layer**
   - generates preview slice and full guide sections using controlled prompts and validation

6. **Frozen Output Layer**
   - stores final approved guide content and render artifacts

7. **Chat Grounding Layer**
   - stores chat retrieval views over the saved guide + hidden master codex + user playthrough state

---

## 7. Key Artifacts

Each guide project must persist the following durable artifacts:

### 7.1 Project Manifest
Contains:
- project id
- game title
- platform/version if known
- user’s requested guide type
- run constraints
- spoiler settings
- provider/model metadata
- generation settings
- template/theme version
- policy versions used at build time

### 7.2 Knowledge Graph / Codex Database
Contains normalized knowledge entities such as:
- locations
- quests / missions / chapters
- bosses / enemies / NPCs
- items / equipment / crafting materials
- spells / abilities / skills / talents
- classes / jobs / archetypes / builds
- stats / status effects / elemental systems
- collectibles / missables
- vendors / drops / unlock paths
- progression dependencies
- map regions / area relationships
- combat mechanics / system rules

Each entity must support:
- canonical id
- aliases
- category
- structured attributes
- source references
- confidence score
- version tags
- conflict markers
- related entities

### 7.3 Hidden Master Codex
This is a **deeper-than-player guide** internal artifact used by chat, QA, and consistency enforcement.

It must include:
- exhaustive entity coverage beyond the visible guide
- strategy variants beyond the user’s chosen run
- alternative builds and branches
- broad system knowledge
- rare edge cases
- hidden mechanics where verified
- generalized game reference material
- unresolved conflicts and interpretation notes

This artifact is **not shown directly to the player**.
It is used to make the chat mode intelligent and to stop shallow or contradictory answers.

### 7.4 Editorial Blueprint
Contains:
- complete chapter outline
- section hierarchy
- intended page/section purpose
- detail targets per section
- cross-reference plan
- glossary / terminology rules
- style bible
- spoiler policy rules
- recurring callout rules
- icon legend plan
- checklist plan
- appendix plan
- visual pack selection

### 7.5 Preview Package
Contains:
- representative chapter slice
- sample reference pages
- sample visual treatment
- sample callouts / checklists / diagrams
- review notes and user approval state

### 7.6 Frozen Guide Edition
Contains:
- final guide text by chapter/section
- final reference sections
- final appendices
- final art and diagrams
- layout/render data
- export outputs
- validation reports
- immutable edition id

---

## 8. Guide Generation Workflow

### 8.1 Step 1 — Intake
The user defines:
- game
- platform/version if relevant
- guide type
- spoiler tolerance
- difficulty/skill level
- target playstyle
- run constraints
- goals and priorities
- desired depth
- optional policy settings for screenshots/maps if supported

### 8.2 Step 2 — Research / Knowledge Assembly
System ingests and normalizes knowledge from supported inputs and sources.

Outputs:
- initial knowledge graph
- internal provenance map
- conflict report
- candidate chapter structure inputs
- candidate asset requirements

### 8.3 Step 3 — Blueprint Compilation
System compiles a full editorial plan.

Outputs:
- table of contents
- section detail targets
- terminology bible
- style bible
- chapter dependency map
- reference coverage map
- asset plan

### 8.4 Step 4 — Preview Generation
Generate a representative slice, not the whole guide.

The preview must include enough breadth to validate:
- writing tone
- layout approach
- checklist design
- tactical guidance density
- reference format
- art direction

### 8.5 Step 5 — User Review / Revision
User can:
- approve
- request style adjustments
- request depth changes
- request scope changes
- request spoiler policy changes

### 8.6 Step 6 — Full Production Build
System generates the full guide in controlled units.

**Never generate the entire guide in one uncontrolled stream.**

Instead, generate by:
- chapter group
- reference section family
- appendix family

Every unit must be validated before being locked.

### 8.7 Step 7 — Freeze Edition
After full build and validation, freeze the guide.

This creates:
- immutable edition snapshot
- reopenable saved state
- render/export bundle
- chat grounding bundle

### 8.8 Step 8 — Read / Chat / Fork
The player can:
- read the guide
- chat with the guide during play
- track progress
- fork a new edition later without mutating the old one

---

## 9. Guide Content Requirements

The visible guide must support classic print-guide grade structure.

### 9.1 Mandatory Sections

#### A. Front Matter
- cover / title page
- unofficial disclaimer
- build metadata (game/version/guide type)
- spoiler notice
- guide usage notes
- icon legend
- terminology notes

#### B. Walkthrough
- full progression path end to end
- chapter/area/mission order
- route optimization where relevant
- tactical warnings
- missables and branching notes
- optional alternative routes
- boss prep and encounter notes

#### C. Systems / Mechanics Guide
- combat systems
- progression systems
- crafting systems
- status/element systems
- economy systems
- reputation/karma systems where relevant
- traversal or vehicle systems where relevant

#### D. Bestiary / Enemy Compendium
Each entry should support:
- name / aliases
- location / encounter zones
- threat profile
- resistances/weaknesses if known
- behavior summary
- loot/drops if known
- tactical notes
- notable variants
- progression relevance

#### E. Inventory / Item Compendium
Must support full cataloging where source data permits.

Categories include:
- consumables
- crafting materials
- key items
- quest items
- tools
- ammunition
- upgrades
- accessories
- relics / trinkets / charms
- resources / currencies

Each entry should support:
- name
- category
- description summary
- acquisition sources
- use/effect
- upgrade/crafting relationships
- relevant constraints / missability

#### F. Equipment Reference
- weapons
- armor
- shields
- ranged weapons
- special gear
- vehicles / mounts if relevant
- upgrade paths where relevant

#### G. Build / Progression Reference
- jobs
- classes
- archetypes
- skills
- spells
- abilities
- talents / perks
- passive systems
- synergies
- recommended builds for user’s run
- broader alternatives if needed for context

#### H. Maps / Area Reference
Maps are required only when technically and legally supportable.

Rules:
- maps must not be hallucinated
- maps must be provenance-tracked
- maps must come from approved source class
- if maps are unavailable, the system must degrade gracefully to structured area diagrams, route tables, and coordinate/reference descriptions

#### I. Collectibles / Completion Reference
- checklist tables
- missables
- region/area tracking
- quest dependencies
- achievement/trophy support if desired

#### J. Appendices
- glossary
- quick-reference tables
- progression milestones
- recommended route summary
- optional spoiler appendix
- patch/version notes

---

## 10. Hidden Master Codex Requirements

The hidden master codex must be more exhaustive than the visible guide.

### 10.1 Purpose
Used for:
- in-guide chat
- consistency validation
- answering off-path player questions
- deeper lookup beyond the chosen run
- supporting “what if I do X instead?” conversations

### 10.2 Content Rules
The master codex should include:
- all major entities in the game
- all supported systems and subsystems
- alternative builds and strategies
- optional content not emphasized in the visible guide
- references to broader playstyles
- conflict notes where sources disagree
- provenance-rich entity records

### 10.3 Separation Rules
- hidden master codex is not directly exposed as a standalone visible section
- chat may use it, but answers must still respect player spoiler settings and edition context
- chat should not contradict the frozen visible guide unless it explicitly identifies a broader-answer context

---

## 11. Chat Mode Requirements

### 11.1 Product Behavior
After a guide is frozen, the user must be able to chat with it while playing.

Chat must be grounded in:
1. the frozen visible guide edition,
2. the hidden master codex,
3. current user playthrough state,
4. spoiler rules,
5. known guide preferences and run constraints.

### 11.2 Chat Use Cases
- “What should I do next?”
- “Did I miss anything in this area?”
- “What is the best weapon for my current run?”
- “What is this enemy weak to?”
- “Can I still get item X?”
- “What happens if I switch to a stealth build?”
- “What route should I take from here?”
- “I’m trying one-hand any% now, what should change?”

### 11.3 Chat Rules
- must cite or internally ground to saved knowledge artifacts
- must honor spoiler policy
- must know when it is speaking from visible guide vs broader codex
- must not rewrite the frozen guide
- may suggest creating a forked edition if the player changes goals substantially
- must track session state when the user indicates current progress

### 11.4 Chat State Model
Store:
- current chapter/area
- recent user progress
- user-marked completed objectives
- optional inventory/progression checkpoints
- user-selected chat spoiler level

---

## 12. Map Policy and Technical Handling

### 12.1 Non-Negotiable Rule
Maps cannot be hallucinated.

### 12.2 Allowed Map Source Classes
Implementation must support explicit provenance classes such as:
- user-supplied map assets
- officially licensed/permitted map assets
- source-policy-approved community assets where permitted
- internally constructed abstract route diagrams clearly marked as non-canonical diagrams
- structured location graph visualizations generated from facts rather than pretending to be literal maps

### 12.3 Disallowed Behavior
- fabricate exact game maps
- generate “looks-right” fake maps presented as true maps
- hide uncertain provenance
- imply official accuracy where not supported

### 12.4 Graceful Fallback
When true maps are unavailable, the system must provide:
- area flow diagrams
n- route tables
- coordinate-like progression descriptions where available
- landmark-based pathing descriptions
- encounter/loot topology summaries

---

## 13. Asset and Art Policy

### 13.1 Core Rule
The visual system must evoke the category and guide intent without copying protected game identity or official publisher guide branding.

### 13.2 Art System Design
Use a three-part model:
1. **house style**
2. **guide-type style pack**
3. **sanitized game-atmosphere descriptor bundle**

### 13.3 House Style
Defines:
- typography hierarchy
- layout rhythm
- chapter opener rules
- sidebar/callout styles
- icon families
- table styles
- checklist styles
- diagram rules
- ornamental system

### 13.4 Guide-Type Style Pack Examples
- completionist ledger
- speedrun timing manual
- stealth dossier
- challenge-run survival manual
- motorsport telemetry handbook
- low-spoiler first-playthrough codex

### 13.5 Sanitized Descriptor Compiler
Game title may be used to identify the subject and compile internal atmosphere descriptors.
The raw game title must not be passed directly into art prompts for generation.

Example flow:
- input: game title + guide type + run intent
- style compiler output: abstract atmosphere descriptors + layout descriptors + banned-content filters
- generator receives only the sanitized descriptor bundle

### 13.6 Hard Visual Restrictions
Reject generation or inclusion of:
- official logos
- official wordmarks
- character likenesses
- creature likenesses
- faction emblems
- UI/HUD clones
- cover-art mimicry
- official guide trade dress mimicry
- “in the style of [game/publisher/artist]” requests

### 13.7 Visual Asset Classes
Supported classes:
- abstract chapter art
- original ornaments
- icon packs
- diagrams
- charts
- progression ladders
- dependency maps
- taxonomy layouts
- neutral texture backdrops
- non-franchise-specific mood art

### 13.8 Output Scrubber
All generated visual outputs should be checked for:
- trademark strings
- logo-like features
- recognizable character similarity
- UI-like framing
- suspicious franchise-specific artifacts

---

## 14. Writing Consistency System

Long-form consistency must be enforced by artifacts and validators, not by hoping the model remembers everything.

### 14.1 Locked Editorial Artifacts
Must maintain:
- style bible
- terminology bible
- outline/coverage plan
- continuity notes
- spoiler policy
- callout rules
- naming conventions

### 14.2 Chapter Generation Contract
Each chapter/unit generation must load:
- relevant facts from the knowledge layer
- guide-wide style rules
- relevant terminology rules
- continuity summary from prior locked units
- section goals from the blueprint
- relevant reference entities

### 14.3 Required Validators
For each generated unit, run:
- fact consistency validation
- terminology consistency validation
- spoiler-policy validation
- structure/template validation
- duplication/redundancy validation
- unresolved cross-reference validation
- user-intent alignment validation

### 14.4 Locking Rule
A chapter becomes locked only after validation passes or accepted exceptions are recorded.

---

## 15. Production Pipeline Design

### 15.1 Pipeline Stages
1. intake
2. source acquisition / normalization
3. knowledge graph build
4. master codex build
5. blueprint compilation
6. preview package generation
7. user review
8. section-by-section full production
9. validation
10. render assembly
11. edition freeze
12. chat index build

### 15.2 Production Unit Types
- walkthrough chapters
- systems chapters
- compendium families
- appendix families
- visual/diagram units

### 15.3 Concurrency Rules
Where practical, allow concurrent generation for independent units, but require a final global consistency pass before edition freeze.

### 15.4 Failure Handling
If a unit fails validation:
- retry with narrower prompts
- re-ground with stricter context package
- flag for manual review if repeated failure persists

---

## 16. Persistence and Editioning

### 16.1 Project Persistence
Reopening a project must load from saved artifacts, not from regenerated model output.

### 16.2 Edition Model
A project may have multiple editions.

Each edition must store:
- immutable edition id
- timestamp
- parent edition if forked
- artifact manifest
- provider/model manifest
- validation results
- asset manifest

### 16.3 Forking Rules
Fork when:
- user changes spoiler settings significantly
- user changes build/playstyle significantly
- user wants updated game version coverage
- user requests rewritten scope or depth

### 16.4 Never Overwrite Rule
No edition overwrite once frozen.

---

## 17. Reading Mode Requirements

The player-facing guide reader should support:
- stable section navigation
- bookmarks
- progress markers
- checklist interactions
- section search
- cross-reference jumps
- spoiler gating where appropriate
- optional compact/mobile-friendly view

It should be possible to reopen the exact guide and land where the user left off.

---

## 18. Data Model Requirements

At minimum, define schemas for:
- project manifest
- guide request
- guide blueprint
- chapter spec
- style bible
- terminology bible
- entity record
- source record
- conflict record
- asset record
- map provenance record
- validation report
- edition manifest
- chat state snapshot

---

## 19. Source / Provenance Requirements

Every important knowledge unit must have provenance metadata internally.

### 19.1 Required Provenance Fields
- source type
- source id/url/path
- acquisition timestamp
- extraction method
- confidence
- version relevance
- notes / conflict flags

### 19.2 Source Conflict Handling
Where sources disagree:
- record both claims internally
- choose a visible guide position only when justified
- expose uncertainty carefully when needed
- preserve disagreement notes in the master codex

---

## 20. PRD and Preimplementation Requirements for the Coding Agent

The coding agent must not jump straight to implementation.

It must first produce a preimplementation package including:

1. product requirements document
2. technical architecture document
3. system component map
4. data model / schema definitions
5. storage strategy
6. provider abstraction design
7. prompt orchestration design
8. consistency validation plan
9. asset rights/policy enforcement design
10. UX flow and state model
11. milestone plan
12. risk register
13. test strategy
14. acceptance criteria matrix

Only after approval of the preimplementation package should full coding begin.

---

## 21. Testing Requirements

### 21.1 Unit Tests
- schema validation
- policy filters
- provenance tracking
- edition persistence
- prompt compilation
- chat grounding selection

### 21.2 Integration Tests
- intake → blueprint → preview
- preview → full build → freeze
- reopen frozen edition
- chat grounded to saved guide + master codex
- fork edition without mutating original

### 21.3 Content Consistency Tests
- terminology stability across chapters
- no contradictions across locked sections
- spoiler-rule compliance
- cross-reference integrity
- section coverage completeness

### 21.4 Asset Policy Tests
- reject banned visual prompts
- reject unapproved asset classes
- handle map provenance correctly
- degrade gracefully when maps are unavailable

### 21.5 Regression Tests
- saved guide remains byte-stable / structurally stable across reopen
- build reproducibility metadata persists
- older edition artifacts remain readable after app updates

---

## 22. Acceptance Criteria

The system is acceptable only if all of the following are true:

1. a user can define a guide request in structured form;
2. the system can generate a representative preview package;
3. the user can approve and trigger full production;
4. the system can generate a long-form guide in controlled units;
5. the final guide is internally consistent by validation standards;
6. the guide can be frozen as an immutable edition;
7. reopening the guide restores the exact same edition content;
8. chat can answer from the frozen guide plus hidden master codex;
9. chat does not mutate the frozen guide;
10. asset generation and inclusion respect the asset policy;
11. maps are only included when provenance supports them;
12. the visible guide includes walkthrough + encyclopedic reference coverage;
13. the hidden master codex exceeds visible guide depth;
14. the system remains provider-agnostic and BYOK;
15. the application logic can be implemented end-to-end from the approved preimplementation package.

---

## 23. Recommended Implementation Phases

### Phase 0 — PRD / Architecture / Schemas
Deliver full preimplementation package.

### Phase 1 — Core Project and Persistence
- project model
- edition model
- artifact storage
- reopen exact edition

### Phase 2 — Knowledge Layer
- ingestion
- normalization
- entity schemas
- source provenance
- conflict tracking

### Phase 3 — Blueprint + Preview
- outline compiler
- style bible compiler
- preview generator
- review loop

### Phase 4 — Full Guide Production
- chapter unit generation
- validators
- lock/freeze pipeline
- render system

### Phase 5 — Reference Compendia
- bestiary
- item/equipment compendia
- classes/skills/spells/jobs
- appendices

### Phase 6 — Chat Grounding
- hidden master codex
- playthrough state
- retrieval/orchestration
- spoiler-aware answers

### Phase 7 — Maps / Asset Policy Layer
- provenance-tracked map support
- asset policy engine
- art descriptor compiler
- output scrubber

---

## 24. Developer Guidance for the Coding Agent

The coding agent should treat this product as:
- a durable knowledge artifact generator,
- a book-production pipeline,
- a retrieval-grounded companion system,
- a strict persistence and validation problem,
not as a generic chatbot app.

The coding agent must optimize for:
- determinism of saved outputs,
- strong artifact schemas,
- separation of visible guide and hidden codex,
- validation-heavy generation,
- policy-enforced assets,
- explicit editioning,
- future maintainability.

The coding agent must avoid:
- relying on conversation history as persistent truth,
- giant monolithic prompts for full-book generation,
- regeneration-on-open patterns,
- ad hoc art generation,
- hidden mutation of approved artifacts,
- source/provenance ambiguity.

---

## 25. One-Sentence Build Target

Build an open-source, BYOK, edition-based game guide production system that compiles structured game knowledge into a previewable, fully generated, frozen, premium-feeling unofficial strategy guide with deep encyclopedic reference sections and a grounded companion chat powered by a hidden master codex.

