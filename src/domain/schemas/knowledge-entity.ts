import { z } from 'zod';

export const KnowledgeCategory = z.enum([
  'location',
  'item',
  'enemy',
  'quest',
  'class',
  'skill',
  'system',
  'vendor',
  'map-region',
  'route-note',
]);

export const KnowledgeVisibility = z.enum(['visible-guide', 'hidden-codex']);
export const KnowledgeScope = z.enum(['broad', 'run-specific']);
export const KnowledgeConfidence = z.enum(['low', 'medium', 'high']);
export const SourceTrustClassification = z.enum([
  'primary',
  'secondary',
  'community-verified',
  'working-notes',
  'uncertain',
]);
export const ConflictSeverity = z.enum(['warning', 'blocking']);
export const ConflictResolutionStatus = z.enum(['open', 'monitoring', 'resolved']);

export const KnowledgeAttributeSchema = z.object({
  key: z.string().min(1),
  value: z.string().min(1),
});

export const SourceReferenceSchema = z.object({
  id: z.string().uuid(),
  sourceType: z.string().min(1),
  sourceTitle: z.string().min(1),
  sourceUri: z.string().default(''),
  excerpt: z.string().min(1),
  retrievalDate: z.string().datetime(),
  trustClassification: SourceTrustClassification,
});

export const ConflictRecordSchema = z.object({
  id: z.string().uuid(),
  conflictSummary: z.string().min(1),
  conflictType: z.string().min(1),
  severity: ConflictSeverity,
  resolutionStatus: ConflictResolutionStatus,
  recommendedHandling: z.string().min(1),
});

export const KnowledgeEntitySchema = z.object({
  id: z.string().uuid(),
  projectId: z.string().uuid(),
  canonicalName: z.string().min(1),
  category: KnowledgeCategory,
  visibility: KnowledgeVisibility,
  scope: KnowledgeScope,
  confidence: KnowledgeConfidence,
  aliases: z.array(z.string().min(1)).default([]),
  summary: z.string().min(1),
  structuredAttributes: z.array(KnowledgeAttributeSchema).default([]),
  sourceRefs: z.array(SourceReferenceSchema).default([]),
  conflictMarkers: z.array(ConflictRecordSchema).default([]),
  versionTags: z.array(z.string().min(1)).default([]),
  createdAt: z.string().datetime(),
  updatedAt: z.string().datetime(),
});

export type KnowledgeCategoryValue = z.infer<typeof KnowledgeCategory>;
export type KnowledgeVisibilityValue = z.infer<typeof KnowledgeVisibility>;
export type KnowledgeScopeValue = z.infer<typeof KnowledgeScope>;
export type KnowledgeConfidenceValue = z.infer<typeof KnowledgeConfidence>;
export type SourceTrustClassificationValue = z.infer<typeof SourceTrustClassification>;
export type ConflictSeverityValue = z.infer<typeof ConflictSeverity>;
export type ConflictResolutionStatusValue = z.infer<typeof ConflictResolutionStatus>;
export type KnowledgeAttribute = z.infer<typeof KnowledgeAttributeSchema>;
export type SourceReference = z.infer<typeof SourceReferenceSchema>;
export type ConflictRecord = z.infer<typeof ConflictRecordSchema>;
export type KnowledgeEntity = z.infer<typeof KnowledgeEntitySchema>;
