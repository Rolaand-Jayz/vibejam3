# 09 — API and Integration Contracts

## Contract philosophy

The product should have small, explicit contracts between the UI, orchestration layer, storage layer, and external integrations.

If the implementation later uses HTTP endpoints, local RPC, or another internal transport, the resource shapes should remain stable.

## Internal application contract surface

### Project lifecycle

- `createProject`
- `getProject`
- `updateProjectManifest`
- `listProjectEditions`
- `getProjectStatus`

### Preview and build lifecycle

- `createPreviewJob`
- `getPreviewJob`
- `approvePreview`
- `createFullBuildJob`
- `getBuildJob`
- `getBuildArtifacts`
- `freezeEdition`

### Guide consumption

- `getEdition`
- `getEditionContent`
- `getEditionNavigation`
- `getEditionValidationSummary`
- `getEditionProvenance`

### Chat grounding

- `createChatSession`
- `sendChatMessage`
- `getChatTranscript`
- `getChatGroundingSummary`

### Asset and policy management

- `uploadAsset`
- `getAsset`
- `listAssets`
- `evaluateAssetPolicy`
- `evaluateMapPolicy`

## Recommended request/response conventions

### Request conventions

Requests should identify:

- the owning project
- the target edition or build job when relevant
- the version or revision context
- any user-selected policy settings

### Response conventions

Responses should return:

- an explicit status
- a machine-readable id for the created or fetched entity
- a human-readable summary
- any validation or policy flags that matter to the user
- artifact references instead of raw blobs where practical

## Integration boundaries

### Model provider integrations

The app must integrate with providers through adapter contracts rather than direct calls from the UI.

Provider integration data should be limited to:

- model selection
- credentials or BYOK profile reference
- capability metadata
- request payload built by the orchestrator
- normalized response returned by the adapter

### File system integration

File system access should be limited to a storage service or artifact manager that knows how to place, name, and preserve guide files safely.

### External source integrations

If the project later supports source ingestion from external locations, those integrations should be isolated behind ingestion contracts so the rest of the app does not depend on source-specific quirks.

## Contract rules by area

### Project APIs

- Project creation must be explicit and durable.
- Manifest updates must not silently change frozen editions.
- Versioned artifacts must refer back to the right project and edition.

### Build APIs

- Build start must create a job record before any heavy work begins.
- Job status must be queryable while the work is in progress.
- Validation failures must be visible in the job result rather than buried in logs.

### Chat APIs

- Chat requests must always know which edition they are grounded in.
- Chat responses must not rewrite frozen guide content.
- Chat should be able to report grounding strength or uncertainty when helpful.

### Asset APIs

- Asset ingestion must attach provenance.
- Policy evaluation must be explicit and explainable.
- Map assets should receive stricter checks than generic decorative assets.

## Expected error semantics

The contract should be able to express at least:

- validation failure
- policy rejection
- missing artifact or edition
- build in progress
- build blocked by upstream failure
- provider unavailability or quota exhaustion
- unsupported capability for a given provider

## Integration anti-patterns to avoid

- letting the UI talk directly to a provider SDK
- exposing vendor-specific request shapes all the way up the stack
- returning opaque success when validation or policy actually failed
- hiding edition identity from the chat surface
- treating a job as complete before the frozen artifacts exist
