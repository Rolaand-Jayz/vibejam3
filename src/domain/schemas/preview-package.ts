import { z } from 'zod';

export const PreviewApprovalState = z.enum([
  'requires-review',
  'approved',
  'changes-requested',
]);

export const PreviewSectionKind = z.enum([
  'front-matter',
  'walkthrough',
  'checklist',
  'reference',
]);

export const PreviewSampleSectionSchema = z.object({
  title: z.string().min(1),
  kind: PreviewSectionKind,
  summary: z.string().min(1),
  body: z.string().min(1),
});

export const PreviewPackageSchema = z.object({
  id: z.string().uuid(),
  projectId: z.string().uuid(),
  blueprintVersion: z.number().int().min(1),
  version: z.number().int().min(1),
  sampleSections: z.array(PreviewSampleSectionSchema).min(1),
  reviewNotes: z.string().default(''),
  approvalState: PreviewApprovalState.default('requires-review'),
  generatedAt: z.string().datetime(),
  updatedAt: z.string().datetime(),
});

export type PreviewApprovalStateValue = z.infer<typeof PreviewApprovalState>;
export type PreviewSectionKindValue = z.infer<typeof PreviewSectionKind>;
export type PreviewSampleSection = z.infer<typeof PreviewSampleSectionSchema>;
export type PreviewPackage = z.infer<typeof PreviewPackageSchema>;
