import { z } from 'zod';

export const BlueprintChapterPlanSchema = z.object({
  title: z.string().min(1),
  purpose: z.string().min(1),
});

export const EditorialBlueprintSchema = z.object({
  id: z.string().uuid(),
  projectId: z.string().uuid(),
  version: z.number().int().min(1),
  chapterPlans: z.array(BlueprintChapterPlanSchema).min(1),
  terminologyRules: z.string().min(1),
  styleBible: z.string().min(1),
  crossReferencePlan: z.string().min(1),
  checklistPlan: z.string().min(1),
  visualPackIntent: z.string().min(1),
  spoilerGuardrails: z.string().min(1),
  createdAt: z.string().datetime(),
  updatedAt: z.string().datetime(),
});

export type BlueprintChapterPlan = z.infer<typeof BlueprintChapterPlanSchema>;
export type EditorialBlueprint = z.infer<typeof EditorialBlueprintSchema>;
