# 10 — Provider Abstraction Design

## Purpose

The provider abstraction exists so the product can remain open, portable, and testable across multiple model providers without rewriting the guide system every time the vendor surface changes.

## Design goals

- keep provider-specific details at the edge
- normalize request and response shapes internally
- preserve capability differences honestly rather than pretending all providers behave the same
- support BYOK without leaking secret handling into domain logic
- make provider failures visible enough to act on

## Providers in scope

The source product vision points to support for:

- OpenAI
- Anthropic
- Google
- Local model backends

The abstraction should support more than one provider from the start, even if not every feature is identically available on every backend.

## Core adapter responsibilities

### 1. Capability discovery

Each provider should report what it can do.

Examples:

- text generation
- structured output generation
- streaming support
- context window size
- tool or function support
- vision or multimodal support if relevant

### 2. Normalized generation

The core app should ask for a normalized generation result, not a provider-native payload.

The adapter should map:

- prompt bundles
- structured output requests
- chat context
- system instructions
- temperature or creativity policy

into provider-specific requests.

### 3. Normalized response handling

The adapter should normalize:

- text output
- token usage estimates if available
- stop reasons
- tool call traces if supported
- provider warnings or refusals
- error states

### 4. Secrets and credential boundaries

Provider credentials should be handled as BYOK inputs and should not leak into the core guide model.

## Recommended adapter interface shape

The exact code shape can vary, but the abstraction should support functions like:

- `describeCapabilities()`
- `generateText()`
- `generateStructuredOutput()`
- `streamText()` if streaming is supported
- `estimateLimits()` or `getModelLimits()`

Each call should return normalized result objects with explicit success and failure states.

## Capability mismatch policy

Not every provider must support every feature equally.

If a capability is missing, the product should:

- fail explicitly, or
- fall back only when the fallback is truthful and approved by design

It should not silently pretend a missing feature exists.

## Provider selection policy

The system should be able to choose a provider based on:

- user selection
- model availability
- capability requirements
- job type
- policy restrictions

The selection logic should be transparent enough to explain why one provider was chosen over another.

## Prompt and output normalization

Provider outputs should be normalized as close to the adapter as possible so that downstream guide logic sees a consistent structure.

That structure should preserve:

- the generated content
- the provider identity
- the model name
- relevant usage metadata
- any refusal or warning states

## Testing expectations

The provider layer should be tested with:

- adapter contract tests
- capability mismatch tests
- structured output tests
- error propagation tests
- BYOK handling tests where secrets are mocked safely

## Anti-patterns to avoid

- hard-coding one provider as the only real path
- leaking provider-native prompt formats into guide logic
- letting provider response oddities dictate the domain model
- treating unsupported capabilities as if they were available
- collapsing all provider errors into one generic failure bucket
