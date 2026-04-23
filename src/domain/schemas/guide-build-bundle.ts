import { z } from 'zod';

export const GuideBuildValidationState = z.enum([
  'freeze-pending',
  'validation-failed',
]);

export const GuideUnitKind = z.enum([
  'front-matter',
  'walkthrough-chapter',
  'reference-family',
  'appendix',
]);

export const GuideBuildUnitSchema = z.object({
  id: z.string().uuid(),
  title: z.string().min(1),
  kind: GuideUnitKind,
  purpose: z.string().min(1),
  body: z.string().min(1),
  sourceKnowledgeIds: z.array(z.string().uuid()).default([]),
  blockingFindings: z.array(z.string().min(1)).default([]),
  warningFindings: z.array(z.string().min(1)).default([]),
});

export const GuideBuildBundleSchema = z.object({
  id: z.string().uuid(),
  projectId: z.string().uuid(),
  previewId: z.string().uuid(),
  blueprintVersion: z.number().int().min(1),
  version: z.number().int().min(1),
  validationState: GuideBuildValidationState,
  visibleKnowledgeIds: z.array(z.string().uuid()),
  hiddenCodexIds: z.array(z.string().uuid()),
  units: z.array(GuideBuildUnitSchema).min(1),
  generatedAt: z.string().datetime(),
  summary: z.string().min(1),
});

export type GuideBuildValidationStateValue = z.infer<typeof GuideBuildValidationState>;
export type GuideUnitKindValue = z.infer<typeof GuideUnitKind>;
export type GuideBuildUnit = z.infer<typeof GuideBuildUnitSchema>;
export type GuideBuildBundle = z.infer<typeof GuideBuildBundleSchema>;
