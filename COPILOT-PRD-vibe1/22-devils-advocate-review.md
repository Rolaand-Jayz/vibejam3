# 22 — Devil’s Advocate Review

## Purpose

This review intentionally attacks the plan to find where it might be too ambitious, too vague, or too likely to become a beautiful but fragile pile of documents.

The goal is to reduce self-deception before implementation starts.

## The strongest objections

### 1. The product may be trying to do too many “truth” jobs at once

The system wants to be:

- a guide authoring pipeline
- a durable edition store
- a chat companion
- a provenance tracker
- a policy gate
- a hidden codex system

That is a lot of trust surface for one v1. If the team is not disciplined, the product can drift into a complicated editor that is hard to finish and even harder to validate.

### 2. The hidden master codex can become a second product

The internal codex sounds useful, but it can easily turn into an entire parallel knowledge system with its own schema, maintenance rules, and failure modes.

If it starts absorbing unresolved facts, alternate builds, hidden heuristics, and policy exceptions, it may become harder to understand than the visible guide itself.

### 3. Preview-before-full-build may be more expensive than it looks

The preview step is a good trust feature, but it can consume a surprising amount of engineering and orchestration effort if it tries to mimic the full product too closely.

The team should be careful that the preview is representative enough to be useful, but not so complex that it becomes a second production pipeline.

### 4. Provider-agnosticism is valuable, but only if the abstraction stays small

There is a real risk that “provider agnostic” turns into a large abstraction layer that tries to normalize every feature of every vendor.

That would be a mistake.

The abstraction should only cover the capabilities the product truly needs. Anything else is optional until proven useful.

### 5. The asset and map policy layer may become a tax if it is too broad

If the policy engine tries to solve every copyright question up front, it could slow the product to a crawl.

The first version should focus on strict, understandable rules:

- known provenance
- known rights status
- clear rejection when uncertain

### 6. Long-form consistency is a hard problem, not a checkbox

The plan correctly says consistency matters, but consistency across many sections of a guide is the kind of problem that can survive an impressive architecture and still bite the product in practice.

The team should expect iteration, human review, and hard tradeoffs here.

## Simplification advice

If the team needs to cut scope, the safest cuts are:

1. extra export formats
2. advanced edition comparison UX
3. wide asset variety
4. complex chat features beyond grounded Q&A
5. any future sync or collaboration feature

## What absolutely should not be cut

- preview before full build
- frozen edition immutability
- provenance tracking
- map policy enforcement
- explicit validation gates
- edition-aware chat grounding

## Bottom-line challenge

This product is worth building only if the team stays disciplined enough to make the guide feel durable and trustworthy instead of merely intelligent.

If the system cannot protect edition identity and provenance, it loses the reason to exist.
