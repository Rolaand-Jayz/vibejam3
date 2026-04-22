# 14 — UX Flow and State Model

## Purpose

This document defines how the user moves through the product and how the product state should change as the guide is created, reviewed, frozen, reopened, and chatted with.

The important thing is not just screen flow. The important thing is state truth.

## Primary states

### Project-level states

- `empty` — no project yet
- `intake` — user is setting up the guide
- `knowledge-building` — source knowledge is being normalized
- `blueprint-ready` — editorial plan exists
- `preview-generating` — preview output is being assembled
- `preview-ready` — reviewable preview exists
- `revision-requested` — user asked for changes
- `full-build-running` — the complete guide is being produced
- `validation-failed` — build output exists but cannot yet freeze
- `freeze-pending` — output is ready and awaiting final freeze conditions
- `frozen` — approved edition is immutable
- `reopened-frozen` — frozen edition is being viewed again
- `forked-edition` — a new edition is being prepared from an existing one

### Job-level states

- `queued`
- `running`
- `blocked`
- `failed`
- `completed`
- `requires-review`

## State transition principles

- Preview must exist before full build can be approved.
- Frozen editions must not be mutated in place.
- Reopening a frozen edition must not trigger regeneration.
- Validation failure must be a first-class state, not a hidden backend exception.
- Chat is allowed in frozen and reopened states without changing edition truth.

## Screen flow

### 1. Dashboard / project selector

User sees existing projects, their state, and the last known edition status.

### 2. New project intake

User defines the target game, run style, spoiler tolerance, and guide intent.

### 3. Blueprint and preview review

User reviews the editorial plan and representative preview.

### 4. Revision loop

User requests depth, style, scope, or policy changes before the full build continues.

### 5. Full build progress

User watches progress as the system produces controlled guide units.

### 6. Validation and freeze review

User reviews validation outcomes and approves the frozen edition.

### 7. Reader and chat mode

User reads the frozen guide and optionally asks questions in context.

### 8. Edition history / fork flow

User creates a new edition without harming the old one.

## UI state expectations

### Empty state

Should explain the product in plain language and show the next action.

### Loading state

Should show what is happening and why it may take time.

### Partial-complete state

Should explain what exists, what is still pending, and whether the user can continue.

### Error state

Should be specific enough for the user to act on instead of generic and decorative.

## Edition visibility rules

The interface should always make it obvious whether the user is looking at:

- a draft intake
- a preview
- a frozen edition
- a chat response grounded in the frozen edition
- a forked edition in progress

## Chat state rules

The chat panel should inherit the current edition identity.

It should not be possible for a user to accidentally chat against the wrong edition without seeing that choice.

## State anti-patterns to avoid

- hiding edition boundaries behind a generic “open guide” button
- making preview and frozen output look identical
- allowing chat to feel like a hidden editor
- burying validation failure inside a dashboard badge nobody understands
- re-running generation on reopen without telling the user
