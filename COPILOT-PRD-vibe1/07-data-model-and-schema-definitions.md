# 07 — Data Model and Schema Definitions

## Purpose

This document defines the core data objects the product needs in order to remain durable, inspectable, and edition-safe.

The data model is intentionally explicit because this product depends on long-lived artifacts, not just transient generation output.

## Schema design principles

- **Durable first.** If a piece of state matters after the process exits, it belongs in a persistent schema.
- **Versioned where needed.** Project settings, blueprints, and editions must be version-aware.
- **Provenance attached.** Anything factual should know where it came from.
- **Confidence visible.** Uncertainty should not disappear into a freeform text blob.
- **Immutable editions.** Frozen guide editions are append-only artifacts, not mutable drafts.

## Shared field conventions

Many records should use the following common fields where applicable:

- `id`: stable unique identifier
- `projectId`: owning project
- `editionId`: owning edition when relevant
- `createdAt` / `updatedAt`: timestamps
- `version`: semantic or ordinal version marker
- `status`: lifecycle state
- `sourceRefs`: provenance anchors
- `confidence`: numeric or categorical confidence marker
- `notes`: human-readable explanation field

## Core entities

### 1. ProjectManifest

Represents the root guide project configuration.

| Field | Type | Required | Meaning |
| --- | --- | --- | --- |
| `id` | string | yes | project identifier |
| `gameTitle` | string | yes | target game |
| `platform` | string | no | platform/version constraints |
| `guideIntent` | string | yes | what kind of guide is being created |
| `playerGoal` | string | yes | desired playthrough objective |
| `runStyle` | string | yes | build/style constraints |
| `spoilerPolicy` | string | yes | spoiler tolerance |
| `depthPreference` | string | yes | how detailed the guide should be |
| `providerProfile` | object | yes | provider selection and BYOK metadata |
| `policyProfile` | object | yes | asset/map/content policy settings |
| `templateVersion` | string | yes | pack/template version |
| `createdAt` | datetime | yes | creation time |

### 2. GuideProject

Represents the broader project record that ties together the manifest, editions, and working state.

Key relationships:

- one project can have many editions
- one project can have many builds
- one project owns multiple knowledge and asset artifacts

### 3. KnowledgeEntity

Represents a normalized game fact or concept.

Suggested fields:

- canonical name
- category (`location`, `item`, `enemy`, `quest`, `class`, `skill`, `system`, `vendor`, `map-region`, etc.)
- aliases
- structured attributes
- confidence score
- provenance references
- conflict markers
- version tags
- relationships to other entities

### 4. EntityRelationship

Represents a link between knowledge entities.

Examples:

- location contains area
- boss guards reward
- item upgrades into item
- class unlocks skill path
- quest depends on event or item

Suggested fields:

- `sourceEntityId`
- `targetEntityId`
- `relationshipType`
- `strength`
- `sourceRefs`
- `notes`

### 5. SourceReference

Represents provenance for a fact, claim, or asset.

Suggested fields:

- source type
- source title or URI
- excerpt or pointer
- retrieval date
- trust classification
- applicable entity ids

### 6. ConflictRecord

Represents a contradiction, ambiguity, or unresolved interpretation.

Suggested fields:

- `id`
- `projectId`
- `entityIds`
- `conflictSummary`
- `conflictType`
- `severity`
- `resolutionStatus`
- `sourceRefs`
- `recommendedHandling`

### 7. EditorialBlueprint

Represents the guide’s planned structure before final generation.

Suggested fields:

- `id`
- `projectId`
- `version`
- `toc`
- `sectionPlans`
- `terminologyRules`
- `styleBible`
- `crossReferencePlan`
- `spoilerPolicy`
- `assetPlan`

### 8. PreviewPackage

Represents the reviewable output slice.

Suggested fields:

- `id`
- `projectId`
- `blueprintVersion`
- `sampleSections`
- `reviewNotes`
- `approvalState`
- `generatedAt`

### 9. GuideEdition

Represents a frozen approved guide version.

Suggested fields:

- `id`
- `projectId`
- `editionNumber`
- `sourceBlueprintVersion`
- `approvalState`
- `frozenAt`
- `contentBundleRef`
- `validationReportRefs`
- `exportRefs`

### 10. BuildJob

Represents a long-running production action.

Suggested fields:

- `id`
- `projectId`
- `jobType` (`preview`, `full-build`, `validate`, `freeze`, `export`)
- `status`
- `progress`
- `startedAt`
- `endedAt`
- `inputs`
- `outputs`
- `errorSummary`

### 11. ValidationReport

Represents one validation pass or a bundle of validation outcomes.

Suggested fields:

- `id`
- `targetType`
- `targetId`
- `checksRun`
- `findings`
- `passFailSummary`
- `severityCounts`
- `createdAt`

### 12. AssetRecord

Represents a tracked image, diagram, icon, screenshot, or map-like asset.

Suggested fields:

- asset type
- file path or URI
- origin
- rights status
- provenance
- linked edition or section
- scrub status

### 13. MapAsset

Represents a map-specific asset with stricter provenance requirements.

Suggested fields:

- `assetRecordId`
- source provenance
- allowed-use rationale
- coverage notes
- policy decision

### 14. ChatGroundingView

Represents the retrieval surface used to answer in-guide questions.

Suggested fields:

- `projectId`
- `editionId`
- `visibleGuideRefs`
- `hiddenCodexRefs`
- `retrievalVersion`
- `safetyPolicy`

### 15. ExportArtifact

Represents a frozen output bundle or render result.

Suggested fields:

- artifact type
- file path
- checksum or hash
- edition reference
- generatedAt

## Important invariants

- A frozen edition must reference exactly one approved blueprint source version.
- A frozen edition must never be edited in place.
- Conflict records must remain attached even when a best interpretation exists.
- Provenance must remain attached to the knowledge record, not only the final prose.
- The hidden codex must not be merged into user-facing guide content by accident.

## Storage shape implications

The data model implies a split storage strategy:

- relational or structured records for project, build, blueprint, and edition state
- file-based artifact storage for large guide content and exports
- separate tracked storage for assets and render outputs

## Schema validation stance

The schemas should be validated at boundaries using explicit schema definitions, not only by hope and naming convention. The data model is too important to leave implicit.
