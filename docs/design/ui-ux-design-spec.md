# vibejam3 UI/UX Design Specification

## Visual personality

The product should feel like a **premium strategy guide publishing studio** crossed with a **serious reference reader**. Think Penguin Classics meets a tactical operations manual — not a chatbot demo.

**Tone**: calm, editorial, information-rich, durable.
**Not**: bubbly, gradient-heavy, chat-first, disposable-feeling.

### Typography

- **Display/headers**: strong serif or high-contrast sans-serif (think Playfair Display, Source Serif Pro, or similar editorial weight). Headers earn their weight.
- **Body text**: clean, highly readable serif or serif-adjacent sans for long-form reading. Line height generous (1.6–1.7). Max line width capped at ~70 characters.
- **UI labels and controls**: compact, restrained sans-serif. Small size, muted color, no decorative weight.
- **Trust/status signals**: monospace or slab-serif for edition identifiers, validation codes, and provenance stamps.

### Color

- **Base**: warm off-white or paper-cream backgrounds for reading surfaces (`#FAFAF5` to `#F5F0E8` range).
- **Structure**: near-black for text (`#1A1A1A`), muted charcoal for secondary (`#4A4A4A`).
- **Accent**: single restrained editorial accent — deep ink blue (`#2C3E6B`) or dark copper (`#8B5E3C`). Used sparingly for active states, links, and primary actions.
- **Status signals**: small, deliberate colored dots or bars — green (validated), amber (attention needed), red (blocked/failed). No neon. No gradients.
- **Chat surfaces**: slightly cooler off-white (`#F8F9FA`) to visually separate conversation from guide content without shouting "AI panel!"

### Spacing and rhythm

- Reading surfaces use generous padding and clear section breaks.
- Guide chapters breathe with deliberate spacing — not cramped cards.
- Navigation is compact and stays out of the reading rhythm.
- Chat panels are clearly bounded — they do not bleed into the guide area.

---

## Screen architecture

The app has one persistent layout shell with two modes:

1. **Studio mode** — guide creation, review, and production
2. **Reader mode** — consuming the frozen guide with optional chat

### Layout shell

```
┌─────────────────────────────────────────────────────┐
│  Top bar: Project name | Edition badge | Mode switch │
├──────────┬──────────────────────────────┬───────────┤
│          │                              │           │
│  Nav     │       Content area           │  Sidecar  │
│  rail    │       (main stage)           │  (context)│
│          │                              │           │
│          │                              │           │
├──────────┴──────────────────────────────┴───────────┤
│  Status bar: Pipeline state | Validation | Provider  │
└─────────────────────────────────────────────────────┘
```

- **Nav rail** (left, ~48–64px collapsed, ~200px expanded): project navigation, chapter outline, section list. Collapses to icons in reader mode.
- **Content area** (center, flexible): the main stage. Shows whatever the current step needs — intake form, blueprint review, preview reader, full guide, build progress.
- **Sidecar** (right, ~320–400px, dismissible): context-dependent panel. In studio mode it shows the **creation chat**. In reader mode it shows the **reader chat**. Can be toggled open/closed.
- **Status bar** (bottom, ~28px): pipeline stage indicator, validation status, provider info, edition identity. Always visible, never intrusive.

---

## Screen-by-screen design

### 1. Dashboard — Project selector

**Purpose**: Entry point. Show existing projects and their state.

**Layout**: Grid of project cards (not a table). Each card shows:
- Game title
- Guide type (walkthrough, reference, tactical, etc.)
- Edition badge (draft / preview / frozen v1.2 / etc.)
- Last updated date
- Visual progress indicator (not a percentage bar — a small state word like "Frozen" or "Building")

**Empty state**: Single centered message explaining what the user can create, with a clear "New Guide" action. Plain language. No illustration of a robot holding a book.

**Anti-pattern avoidance**: No interchangeable KPI cards. No "Recent Activity" feed. No AI-generated summary of what the user did last time.

### 2. New project intake

**Purpose**: Collect game details, guide intent, run style, and policy options.

**Layout**: Single-column form in the content area. Nav rail shows project setup steps. Sidecar is closed by default.

**Form sections** (one at a time, wizard-style):
1. **Game**: target game, version/platform
2. **Intent**: what kind of guide, player goal, run type
3. **Style**: tone preference, depth preference, spoiler tolerance
4. **Policy**: asset handling preferences, content boundaries

**Creation chat role here**: The sidecar opens automatically during intake. The user can describe what they want in natural language and the system translates that into form values, OR the user can fill the form directly. The chat does not replace the form — it assists it. Form fields update as the chat understands intent. Chat messages show what was inferred: "Got it — you want a spoiler-light tactical walkthrough for Elden Ring, focusing on strength builds. I'll set that up."

**Key UX rule**: The intake must produce a durable project manifest. The user must see a clear "Create Project" confirmation that commits the intake — not a vague "let's get started" that disappears into the aether.

### 3. Blueprint review

**Purpose**: Show the editorial plan before preview generation.

**Layout**: Content area shows the blueprint (chapter outline, section list, depth targets, reference families). Sidecar shows creation chat for refinement.

**Creation chat role here**: The user can say things like "add a chapter on late-game scaling" or "merge equipment into the combat chapter" and the system updates the blueprint. Changes are reflected in the content area immediately. The chat is a refinement tool, not the primary view.

**Key UX rule**: Blueprint changes must be visible as edits to the blueprint document, not as chat message artifacts. The blueprint is the source of truth, not the chat log.

### 4. Preview review

**Purpose**: Show a representative preview slice for approval.

**Layout**: Content area renders the preview in full guide formatting — not a chat response, not a markdown dump. The preview looks like the real guide. Sidecar shows creation chat for feedback.

**Creation chat role here**: User can say "the tone is too casual" or "I want more depth on boss strategies." The system notes the feedback and offers to revise. Approval happens through a clear action button in the content area, not through chat.

**Feedback controls** (in the content area, not chat):
- Approve → advance to full build
- Request changes → specify what to change (depth, tone, scope, spoilers, style)

**Key UX rule**: Preview must look like the final guide, not like a chat response preview. The user is reviewing a book page, not a conversation.

### 5. Build progress

**Purpose**: Show pipeline status as the full guide is produced.

**Layout**: Content area shows a structured progress view — not a spinning loader, not a chat stream. Each production unit (chapter group, reference family, appendix) has its own row showing state: queued → generating → validating → locked.

**Sidecar**: Creation chat is available for questions ("why is chapter 3 taking so long?") but the chat does not control the build. Build progress is driven by the pipeline, not by chat commands.

**Key UX rule**: Build progress must show real state, not fake progress. If something fails, show the failure plainly with what was affected and what the user can do.

### 6. Validation and freeze review

**Purpose**: Show validation outcomes before final freeze.

**Layout**: Content area shows validation results grouped by type (consistency, completeness, policy compliance). Each result is pass/fail/warning with a plain-language explanation.

**Sidecar**: Creation chat available for questions about validation results.

**Freeze action**: Clear "Freeze Edition" button in the content area. Requires confirmation. Shows what will happen: edition identity, what becomes immutable, what the user can still do later (chat, fork).

**Key UX rule**: Validation failure is a first-class state, not a hidden error. The user must understand what failed and why before they can proceed.

### 7. Guide reader

**Purpose**: The main consumption surface. Reading the frozen guide.

**Layout**: 
- Nav rail expands to show full chapter/section tree (collapsible)
- Content area renders the guide in full book-like formatting
- Status bar shows edition identity (e.g., "Frozen Edition v1.0")
- Sidecar is closed by default, opens for reader chat

**Reading experience**:
- Persistent reading position (survives navigation and reopen)
- Clear chapter and section transitions with deliberate spacing
- Trust indicators inline: small provenance stamps, validation badges, edition markers
- No floating AI suggestion bubbles
- No "rewrite this section" affordances — this is a frozen edition

**Key UX rule**: The reader must feel like opening a saved book. Stable. Predictable. Not a dynamic document that might change.

### 8. Edition history

**Purpose**: Show frozen editions, preview states, and revision timeline.

**Layout**: Content area shows a timeline view — each edition is a node with its identity, creation date, and change summary. Selecting a node shows a diff or summary of what changed.

**Key UX rule**: Edition history must make it obvious which edition the user is currently viewing. No ambiguity.

---

## Chat surface design

The product has two distinct chat contexts. They share the same sidecar panel position but have different behavior, styling, and grounding.

### Creation chat (Studio mode)

**When it appears**: During intake, blueprint review, preview review, build progress, and validation. Always available in the sidecar during studio mode.

**What it does**:
- Helps the user define their guide intent during intake
- Accepts refinement requests during blueprint and preview review
- Answers questions about build progress and validation results
- Translates natural language into form values and editorial decisions

**What it does NOT do**:
- It does not generate guide content directly. Guide content is produced by the pipeline, not by chatting.
- It does not approve or freeze anything. Actions happen in the content area.
- It does not replace forms, blueprints, or previews. It assists them.

**Visual treatment**:
- Messages use a restrained chat bubble style — slight background tint for system messages, clean text for user messages
- No avatar icons. No sparkle animations. No typing indicators that look sentient.
- System messages clearly label what they are: "Intake", "Blueprint update", "Preview feedback", "Build status"
- When the chat changes something (updates a form field, modifies the blueprint), the change is shown as a concise confirmation: "Updated spoiler tolerance to 'minimal'." The actual change is visible in the content area.

**Grounding**:
- The creation chat is grounded in the current project manifest, blueprint state, and PRD-derived constraints.
- It knows what phase the project is in and what actions are available.
- It does not hallucinate capabilities the product doesn't have.

### Reader chat (Reader mode)

**When it appears**: After the guide is frozen. Available in the sidecar when the user is reading the guide.

**What it does**:
- Answers questions about the game and guide content
- Grounds answers in the frozen guide edition plus the hidden master codex
- Explains uncertainty when the guide doesn't cover something fully
- Tracks the user's current reading position for context
- Distinguishes between "the guide says..." and "the deeper knowledge suggests..."

**What it does NOT do**:
- It does NOT rewrite, edit, or append to the frozen guide.
- It does NOT suggest guide improvements or ask "would you like me to update..."
- It does NOT regenerate any content.
- It does NOT forget which edition it is anchored to.

**Visual treatment**:
- Slightly different header than creation chat — shows edition identity: "Chat — Elden Ring Guide v1.0 (Frozen)"
- Messages follow the same restrained style as creation chat
- When the chat cites the guide, it shows a small reference link that scrolls the reader to the cited section
- When the chat uses master codex knowledge beyond what's in the visible guide, it marks the response: "This comes from the deeper knowledge layer, not the visible guide."
- Uncertainty is explicit: "The guide doesn't cover this area in detail. Based on the broader knowledge, [answer], but this is not guide-verified."

**Grounding**:
- Frozen guide edition content (what the user approved)
- Hidden master codex (deeper knowledge used for consistency and chat intelligence)
- Current reading position and chapter context
- Spoiler tolerance settings from the project manifest
- Edition identity — the chat always knows which version it's attached to

### Chat panel mechanics (shared)

**Toggle**: The sidecar opens/closes with a button in the top bar or a keyboard shortcut. It does not auto-open unless the workflow specifically needs it (like during intake).

**Width**: ~360–400px. Does not collapse the guide content aggressively. The guide should still be readable with the chat open.

**Scroll**: Independent scroll from the guide content. Chat history persists within the session.

**History**: Chat history is saved per-project, per-edition. Reopening a guide restores the reader chat context.

**Input**: Simple text input at the bottom. No rich text, no file upload, no voice input for v1. Clean and minimal.

---

## Component language

### Edition badge

A small, consistent badge element that appears in the top bar, status bar, and edition history. Shows edition state with a word, not a color alone:
- `DRAFT` — muted gray
- `PREVIEW` — amber
- `BUILDING` — blue with subtle pulse
- `FROZEN v1.0` — dark ink with a small lock icon

### Progress tracker

Used in build progress view. A vertical list of production units, each with a state indicator:
- ○ Queued
- ● Generating...
- ✓ Locked
- ✗ Failed: [reason]

No progress bars. No percentages. Real state words.

### Validation result

A compact result row:
- ✓ Pass: [check name]
- ⚠ Warning: [check name] — [plain language explanation]
- ✗ Fail: [check name] — [what was affected] — [what the user can do]

### Navigation tree

Used in the nav rail for guide reading. Collapsible chapter/section tree. Current position highlighted with the accent color. Keyboard navigable. Not a sidebar search box.

### Form section (intake wizard)

Clean, single-column form fields. One section visible at a time. Progress shown as step indicators (not a full sidebar). Each field has a small explanation of why it matters.

---

## Interaction tone

- **Quiet by default.** The interface does not animate, pulse, or draw attention unless something needs the user's action.
- **Responsive when asked.** When the user takes an action, the response is immediate and clear.
- **Honest about state.** If something is loading, it says what is loading. If something failed, it says what failed. If something is uncertain, it says it's uncertain.
- **No false personality.** The chat does not pretend to have feelings. It does not say "I'd love to help!" It says what it will do and does it.

---

## Anti-generic avoid list for implementation

When building this UI, actively avoid:

1. Giant hero chat panels as the primary layout
2. Purple/blue neon gradients anywhere
3. Glassmorphism on any surface
4. Sparkle, star, aurora, or "magic" decorative motifs
5. Oversized pill buttons with no visual hierarchy
6. Default AI assistant sidebars that dominate the layout
7. Dashboard grids of interchangeable cards
8. Empty whitespace luxury that hurts information density
9. Copy that sounds like an LLM wrote it to impress another LLM
10. Over-rounded UI that removes the feeling of a serious reference tool
11. "AI-powered" badges or labels on obvious features
12. Chat streams that look like the product's primary output
13. Floating suggestion bubbles over guide content
14. Typing indicators that make the system look sentient
15. Gradient status bars or animated progress indicators that hide real state

---

## Trust signal strategy

Trust signals are first-class UI elements, not back-office trivia:

- **Edition identity**: always visible in the top bar and status bar. The user should never have to guess which version they're looking at.
- **Validation status**: shown after every build step. Pass/fail/warning with plain language.
- **Provenance stamps**: on guide assets (maps, screenshots), small indicators showing source and policy compliance.
- **Chat grounding source**: when the reader chat answers, it shows whether the answer came from the guide or from deeper knowledge.
- **State words over state colors**: "Frozen", "Building", "Preview" — always text, never color alone.

---

## Layout responsive behavior

- **Full width** (>1200px): nav rail + content + sidecar all visible
- **Medium** (800–1200px): nav rail collapsed to icons, content + sidecar share space
- **Compact** (<800px): nav rail hidden (hamburger), content full width, sidecar as overlay sheet

---

## Design success criteria

The design is successful when:

1. A new user can understand what the product does from the empty state without reading documentation.
2. A player can open a frozen guide and read it without ever noticing the chat panel if they don't want it.
3. A creator can produce a guide using the creation chat without feeling like they're having a conversation instead of building a product.
4. The interface communicates edition state, validation truth, and provenance without the user having to click through to find them.
5. The app looks like a publishing tool and a premium reader — not like an AI demo.
