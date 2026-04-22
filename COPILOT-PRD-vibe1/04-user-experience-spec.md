# 04 — User Experience Specification

## UX goals

The user experience should feel like a focused editorial tool wrapped around a premium reader.

The interface should communicate three things very clearly:

1. what the user is creating,
2. what state that creation is in,
3. how much confidence the system has in what it shows.

The UX should never force the user to guess whether they are seeing a draft, a preview, a frozen edition, or a chat-derived answer.

## UX principles

- **Book-like where it matters.** The reader should feel stable, structured, and calm.
- **Operator-visible where it matters.** Build state, validation state, and policy state must be visible when the user is managing the guide.
- **No sneaky mutation.** Approval and editioning should never feel accidental.
- **Trust signals are part of the interface.** Provenance, confidence, and edition identity are not back-office trivia.
- **The content should breathe.** Long-form material needs clear hierarchy, not dense wall-of-text dumping.

## Primary personas

### Player / reader

The player wants to use the guide while playing and occasionally ask questions without the guide losing its shape.

### Guide creator / reviewer

The creator wants to define the guide, review preview output, approve the full build, and inspect validation results.

### Maintainer / future editor

The maintainer wants to understand edition history, policy outcomes, and the reason a particular artifact exists in the current form.

## Core journeys

### 1. Create a new guide project

The user starts with a fresh project and supplies the target game, desired guide style, run constraints, spoiler tolerance, and goal.

The interface should:

- make required inputs obvious
- explain why each input matters in plain language
- store the intake as a durable project manifest
- immediately show the project as a distinct working object rather than a loose draft

### 2. Review the preview slice

The user sees a representative preview of what the final guide will feel like.

The preview should include:

- chapter tone
- sample tactical guidance
- sample reference formatting
- sample checklist or callout behavior
- sample visual or layout treatment where available

The review experience should support quick feedback such as:

- approve
- request more/less depth
- request style changes
- request spoiler-policy changes
- request scope corrections

### 3. Read the frozen guide

The reader should feel like a durable guidebook, not like a chat transcript.

Reader behavior should include:

- stable navigation hierarchy
- clear chapter and section transitions
- visible edition identity
- persistent reading position
- optional trust/provenance surface when the user wants deeper detail

### 4. Ask in-guide questions during play

The chat surface should help the player without taking over the reader.

The chat UX should:

- preserve the guide’s edition context
- answer from the frozen guide plus hidden grounding knowledge
- explain uncertainty when needed
- avoid mutating visible guide text
- distinguish “guide says” from “internal grounding suggests” when relevant

### 5. Inspect edition history

The user should be able to see when the guide was created, what version it is, and what changed when a new edition is forked.

The history UX should make it easy to answer:

- Which edition am I using?
- What changed from the previous one?
- Is this a draft, preview, or frozen output?
- Which state is safe to reopen without regeneration?

## Key screens

### Project dashboard

Shows existing projects, status, last updated state, and quick entry into the active guide.

### New project intake

Collects game details, guide intent, run style, and policy options.

### Blueprint and preview review

Shows the editorial plan and preview slice, with explicit approval controls.

### Build progress view

Shows pipeline stage, checkpoints, validation results, and any unresolved conflicts.

### Guide reader

The main consumption surface with chapter navigation, section hierarchy, annotations, and trust indicators.

### Chat companion

An anchored sidecar or split-pane mode that answers questions while keeping the frozen guide intact.

### Edition history and comparison

Shows frozen editions, preview states, and revision history.

### Asset and policy review

Shows provenance and policy state for maps, screenshots, and other visuals.

## Error and empty states

- Empty project state should explain what a user can create and why.
- Missing preview state should explain that preview is required before full build.
- Validation failure should show what failed, what it affects, and whether the user can continue.
- Policy rejection should say what was rejected and what alternative exists.
- Chat uncertainty should be explicit rather than hidden behind generic fallback phrasing.

## UX anti-patterns to avoid

- pretending a preview is the final guide
- hiding edition state
- burying provenance behind clicks the user would not know to make
- making the chat feel like it can silently rewrite the saved edition
- dumping raw validation output without interpretation
- using dense interface chrome that fights long-form reading

## Accessibility and readability expectations

- legible typography for long-form reading
- strong hierarchy and spacing
- keyboard-accessible navigation and controls
- clear contrast for trust/status signals
- readable error and warning language

## UX success criterion

The user should finish the workflow feeling that the guide is theirs, that it is stable, and that the system knows the difference between a draft thought, an approved edition, and a grounded answer.
