import { z } from 'zod';

export const ProjectState = z.enum([
  'empty',
  'intake',
  'knowledge-building',
  'blueprint-ready',
  'preview-generating',
  'preview-ready',
  'revision-requested',
  'full-build-running',
  'validation-failed',
  'freeze-pending',
  'frozen',
  'reopened-frozen',
  'forked-edition',
]);

export const BuildJobStatus = z.enum([
  'queued',
  'running',
  'blocked',
  'failed',
  'completed',
  'requires-review',
]);

/**
 * Spoiler tolerance levels for guide production.
 * Controls how much plot-revealing information the guide contains.
 */
export const SpoilerPolicy = z.enum([
  'none',
  'minimal',
  'moderate',
  'full',
]);

/**
 * How detailed the produced guide should be.
 * Maps to the PRD's depth-by-intent system.
 */
export const DepthPreference = z.enum([
  'essential',
  'standard',
  'comprehensive',
  'completionist',
  'reference',
]);

/**
 * Provider adapter selection and BYOK metadata.
 * The system is provider-agnostic; this records which provider
 * the user has configured and their key reference.
 */
export const ProviderProfile = z.object({
  providerId: z.string().min(1),
  modelId: z.string().min(1),
  keyRef: z.string().optional(),
  endpoint: z.string().url().optional(),
});

/**
 * Asset, map, and content policy settings.
 * Governs what content is allowed and how assets are handled.
 */
export const PolicyProfile = z.object({
  allowFanArt: z.boolean().default(false),
  allowUserMaps: z.boolean().default(true),
  requireProvenance: z.boolean().default(true),
  contentRating: z.enum(['everyone', 'teen', 'mature']).default('everyone'),
});

export const BuildJobSchema = z.object({
  id: z.string().uuid(),
  projectId: z.string().uuid(),
  jobType: z.string().min(1),
  status: BuildJobStatus,
  progress: z.number().int().min(0).max(100),
  startedAt: z.string().datetime(),
  endedAt: z.string().datetime(),
  humanSummary: z.string().min(1),
  validationReportId: z.string().uuid(),
});

export const ValidationReportSchema = z.object({
  id: z.string().uuid(),
  projectId: z.string().uuid(),
  targetType: z.string().min(1),
  targetId: z.string().uuid(),
  checksRun: z.array(z.string().min(1)),
  blockingFindings: z.array(z.string().min(1)),
  warningFindings: z.array(z.string().min(1)),
  blockingCount: z.number().int().min(0),
  warningCount: z.number().int().min(0),
  createdAt: z.string().datetime(),
  summary: z.string().min(1),
});

/**
 * ProjectManifest — the root guide project configuration.
 *
 * This is the first entity created when a user starts a new guide.
 * It captures the guide's intent, style, and policy in durable structured form.
 *
 * PRD reference: COPILOT-PRD-vibe1/07-data-model-and-schema-definitions.md
 */
export const ProjectManifestSchema = z.object({
  id: z.string().uuid(),
  projectState: ProjectState.default('intake'),
  gameTitle: z.string().min(1),
  platform: z.string().default(''),
  guideIntent: z.string().min(1),
  playerGoal: z.string().default(''),
  runStyle: z.string().min(1).default('casual'),
  spoilerPolicy: SpoilerPolicy.default('minimal'),
  depthPreference: DepthPreference.default('standard'),
  providerProfile: ProviderProfile.optional(),
  policyProfile: PolicyProfile.default({
    allowFanArt: false,
    allowUserMaps: true,
    requireProvenance: true,
    contentRating: 'everyone',
  }),
  templateVersion: z.string().min(1).default('0.1.0'),
  sharedThemeCss: z.string().default(''),
  createdAt: z.string().datetime(),
});

export type ProjectManifest = z.infer<typeof ProjectManifestSchema>;
export type ProjectStateValue = z.infer<typeof ProjectState>;
export type SpoilerPolicyValue = z.infer<typeof SpoilerPolicy>;
export type DepthPreferenceValue = z.infer<typeof DepthPreference>;
export type ProviderProfileType = z.infer<typeof ProviderProfile>;
export type PolicyProfileType = z.infer<typeof PolicyProfile>;
export type BuildJob = z.infer<typeof BuildJobSchema>;
export type BuildJobStatusValue = z.infer<typeof BuildJobStatus>;
export type ValidationReport = z.infer<typeof ValidationReportSchema>;
