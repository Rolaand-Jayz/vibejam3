# 01 — Project Brief

## Project summary

`vibejam3` is a free, open-source, provider-agnostic, BYOK game guide production system.

Its job is to create durable, book-grade, personalized unofficial strategy guides for a specific game, a specific player goal, and a specific run style. The product is not a generic chat assistant. It is a production pipeline for building a saved guide edition that can be reopened exactly as approved and used again and again without silently changing under the user.

## Core promise

The product should feel like a serious, premium strategy guide publishing tool with the added convenience of interactive help while the player is actively using the guide.

The promise is:

- define the exact guide the player wants
- preview the direction before committing to a long build
- generate a complete, coherent, end-to-end guide
- freeze the approved result as a durable edition
- reopen that edition exactly as saved
- support in-guide chat without mutating the frozen content

## What makes this product different

The product is different from a chatbot, a wiki, and a forum archive because it treats guide creation as an editorial production process.

That means:

- the output is not ephemeral
- the output is not regenerated every time the user opens it
- the visible guide and the hidden internal knowledge layer are separate artifacts
- consistency across long documents is a first-class requirement, not a nice-to-have
- provider-specific behavior must not leak into the core product logic

## Primary user value

The user should be able to say, in effect: “Give me the exact guide I wish I had for this game and this run, then let me keep using it without it drifting under my feet.”

The value is not just answer quality. The value is trust, durability, editorial coherence, and long-session usefulness.

## Primary audience

### Player / guide consumer

Someone who wants a tailored strategy guide for a specific game and playthrough style. They care about clarity, accuracy, depth, and stability over time.

### Guide creator / operator

Someone creating or refining the guide project, reviewing preview output, approving the full build, and checking validation and policy boundaries.

### Future maintainer

Someone who may extend the product later and needs the internal contracts to remain readable instead of reverse-engineering them from prompt behavior.

## v1 product shape

The first real version should support:

- project setup for a single guide instance
- preview-before-full-build workflow
- long-form guide generation in controlled units
- frozen edition storage
- reopenable saved guides
- in-guide chat grounded in the frozen guide plus hidden internal knowledge
- asset and map policy enforcement
- provider-agnostic LLM access through strict adapters
- durable storage for guide artifacts and validation evidence

## Explicit non-goals for v1

- public guide sharing platform
- marketplace or monetization features
- multiplayer collaboration on the same guide edition
- automatic legal clearance for all copyrighted assets
- universal automatic map extraction for every game
- universal OCR-heavy ingestion of arbitrary scans as the primary pipeline
- real-time cloud sync as a hard requirement

## Product success profile

A successful early version is one that:

- reliably produces coherent, structured, personalized guide editions
- keeps guide output stable after approval
- makes source and policy boundaries visible enough to trust
- supports a useful chat mode without allowing the chat to rewrite the edition
- preserves enough provenance that conflicts and uncertainty can be traced
- stays provider-agnostic so the product logic outlives any one model vendor

## Editorial stance

The product should be conservative where trust matters and ambitious where usefulness matters.

It should avoid pretending that uncertain knowledge is certain, but it should still be helpful, editorially polished, and complete enough to feel like a real guide rather than a pile of fragments.
