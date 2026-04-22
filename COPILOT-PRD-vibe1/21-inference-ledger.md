# 21 — Inference Ledger

## Purpose

This ledger records the major assumptions and design inferences that were used to turn the source vision into a buildable pre-implementation plan.

The point is not to hide uncertainty. The point is to make the uncertainty visible enough that the team can challenge it later.

## Ledger

| ID | Inference | Confidence | Evidence | If wrong |
| --- | --- | --- | --- | --- |
| I-01 | The product should be web-first before any desktop shell | high | source context explicitly allows a web-first start | packaging plan may need revision |
| I-02 | SQLite + filesystem is the right first durable storage approach | high | product needs inspectable durable artifacts more than distributed scale | storage migration work may be needed later |
| I-03 | Guide production is a long-running job rather than a simple request/response action | high | preview, full build, freeze, and validation all imply checkpoints | orchestration model may need adjustment |
| I-04 | Provider abstraction should be strict and not leak vendor APIs into domain logic | high | product must remain provider-agnostic and BYOK | domain boundaries would be weaker |
| I-05 | The hidden master codex should remain internal-only | high | source spec distinguishes visible guide content from hidden grounding material | UX and trust model would change |
| I-06 | The preview slice should be representative rather than exhaustive | high | the source spec explicitly calls for preview-before-full-build | preview design would need rethinking |
| I-07 | Frozen editions must be immutable and reopenable exactly as saved | high | this is a core non-negotiable product rule | edition model would become unstable |
| I-08 | Cross-unit and global validation are necessary for long-form consistency | high | the product promises long-running editorial coherence | validation scope would shrink |
| I-09 | Map handling needs a stricter policy than ordinary art assets | high | map hallucination is explicitly disallowed | asset policy would be too weak |
| I-10 | The guide should feel book-like rather than chat-like | medium | source promise references classic premium strategy guides | reader UX might become more conversational |
| I-11 | The first implementation does not need a public guide-sharing platform | high | explicitly out of scope in source material | roadmap complexity would increase |
| I-12 | Collaborative editing should not be in v1 | high | out of scope in source context | state model and permissions would grow substantially |
| I-13 | OCR-heavy arbitrary scan ingestion should be deferred | medium | explicitly out of scope in source material | ingestion pipeline would become much broader |
| I-14 | The product should preserve version history for blueprints and editions | high | editioning and fork behavior require it | provenance and comparison would weaken |
| I-15 | Validation failures should block freeze unless explicitly marked non-blocking | high | trust-critical output cannot be frozen with unresolved blockers | freeze semantics would be unsafe |
| I-16 | Provider credentials must stay outside guide artifacts and logs, with deployment-appropriate secret handling only | high | BYOK plus durable artifact storage requires a hard secret boundary | credential leakage or insecure persistence |
| I-17 | The v1 export target is a canonical frozen-edition artifact bundle, not a broad consumer-format matrix | high | durable reopenability matters more than format proliferation | later export work may need extension |
| I-18 | Edition history and version identity should be surfaced before rich diff/comparison UX | medium-high | current UX and milestone docs prioritize clarity over comparison complexity | comparison UX can be added later |
| I-19 | Cloud sync is not required for v1 and should not shape the initial storage model | high | source scope explicitly excludes real-time cloud sync as a hard requirement | sync work can be planned later if needed |

## Assumption handling rules

- If an inference is not strong enough to defend, it belongs in open questions.
- If an inference affects trust, rights, or edition stability, it should not be buried.
- If a future decision changes an inference, update the ledger rather than leaving stale rationale in old prose.
