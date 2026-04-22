# 15 — Tech Evaluation

## Purpose

This document evaluates the technology direction for the first serious implementation of `vibejam3`.

The conclusion is not meant to be trendy. It is meant to be coherent, testable, and compatible with the product’s durability requirements.

## Recommended stack direction

### Backend

- **TypeScript / Node.js**

Why:

- matches the desired full-stack language surface
- fits the orchestration-heavy nature of the product
- works well with structured validation and workflow code
- keeps provider adapters and domain logic in one language family

### Frontend

- **React**

Why:

- suitable for a complex, stateful guide reader and operator UI
- strong fit for split-pane, stateful, and long-form document interactions
- practical for preview/review/freeze workflows

### Build tooling

- **Vite**

Why:

- fast local development
- clean frontend build path
- sensible fit for a web-first app

### Validation and schemas

- **Zod**

Why:

- strong runtime schema validation
- good for boundary checks on manifest, edition, validation, and provider output shapes

### Persistence

- **SQLite** for structured data
- **File system storage** for large guide artifacts and exports

Why:

- durable without overengineering
- easy to inspect locally
- appropriate for a first edition-oriented product

### Testing

- **Vitest** for unit and integration-style tests
- **Playwright** for end-to-end UX coverage

Why:

- supports logic-heavy domain code
- supports UI state and guide-reader workflows

## Alternatives considered

### Electron-first desktop shell

Rejected for v1 because:

- adds packaging and runtime complexity too early
- does not improve the product’s core trust problem
- the source spec explicitly allows a web-first start

### Cloud-first distributed backend

Rejected for v1 because:

- the first version benefits more from inspectable local durability than from distributed scale
- the product’s value is not gated by multi-node infrastructure

### NoSQL-only persistence

Rejected for v1 because:

- the product needs precise relationships and versioned guide state
- SQLite is easier to reason about for a first durable edition model

### Single giant prompt pipeline

Rejected for v1 because:

- the product requires checkpoints, validation, and edition freeze behavior
- a single opaque flow would be harder to trust and hard to recover

## Why this stack fits the product

The product needs to do four difficult things well:

1. maintain structured state
2. produce long-form guide output in controlled units
3. preserve edition identity
4. stay provider-agnostic

The recommended stack supports all four without forcing premature infrastructure complexity.

## What this stack does not solve by itself

The technology choices do not automatically solve:

- source quality
- map and asset rights policy
- long-form editorial consistency
- hidden codex governance
- provider output quality

Those problems still require strong product and orchestration design.

## Recommendation summary

The best first implementation path is a web-first TypeScript app using React, Vite, Zod, SQLite, filesystem artifact storage, Vitest, and Playwright, with provider-agnostic adapters and no early dependency on a desktop shell.
