import type {
  BuildJob,
  ConflictRecord,
  EditorialBlueprint,
  GuideBuildBundle,
  KnowledgeAttribute,
  KnowledgeEntity,
  PreviewApprovalStateValue,
  PreviewPackage,
  ProjectManifest,
  SourceReference,
  ValidationReport,
} from '../domain/index.js';
import type { ProjectStorage } from '../storage/index.js';
import { buildPreviewPackageInput } from './preview-package-factory.js';
import { buildGuideBuildBundleInput } from './full-build-factory.js';
import { createIntakeValidationArtifacts } from '../validation/intake-validation.js';
import { createFullBuildValidationArtifacts } from '../validation/full-build.js';

export type ProjectResult<T> =
  | { ok: true; data: T }
  | { ok: false; error: string };

type KnowledgeSourceInput = Omit<SourceReference, 'id'>;
type KnowledgeConflictInput = Omit<ConflictRecord, 'id'>;

function generateUuid(): string {
  if (typeof crypto !== 'undefined' && typeof crypto.randomUUID === 'function') {
    return crypto.randomUUID();
  }

  return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, (character) => {
    const random = (Math.random() * 16) | 0;
    const value = character === 'x' ? random : (random & 0x3) | 0x8;
    return value.toString(16);
  });
}

function normalizeList(values: string[] | undefined): string[] {
  return (values ?? [])
    .map((value) => value.trim())
    .filter((value, index, items) => value.length > 0 && items.indexOf(value) === index);
}

function nextKnowledgeProjectState(currentState: ProjectManifest['projectState']): ProjectManifest['projectState'] {
  if (currentState === 'empty' || currentState === 'intake' || currentState === 'knowledge-building') {
    return 'knowledge-building';
  }

  return currentState;
}

function materializeSourceRefs(sourceRefs: KnowledgeSourceInput[]): SourceReference[] {
  return sourceRefs.map((sourceRef) => ({
    id: generateUuid(),
    sourceType: sourceRef.sourceType.trim(),
    sourceTitle: sourceRef.sourceTitle.trim(),
    sourceUri: sourceRef.sourceUri?.trim() ?? '',
    excerpt: sourceRef.excerpt.trim(),
    retrievalDate: sourceRef.retrievalDate,
    trustClassification: sourceRef.trustClassification,
  }));
}

function materializeConflictMarkers(conflictMarkers: KnowledgeConflictInput[] | undefined): ConflictRecord[] {
  return (conflictMarkers ?? []).map((conflict) => ({
    id: generateUuid(),
    conflictSummary: conflict.conflictSummary.trim(),
    conflictType: conflict.conflictType.trim(),
    severity: conflict.severity,
    resolutionStatus: conflict.resolutionStatus,
    recommendedHandling: conflict.recommendedHandling.trim(),
  }));
}

export class ProjectService {
  constructor(private storage: ProjectStorage) {}

  initialize(): void {
    this.storage.initialize();
  }

  createProject(input: {
    gameTitle: string;
    platform?: string;
    guideIntent: string;
    playerGoal?: string;
    runStyle: string;
    spoilerPolicy: string;
    depthPreference: string;
    providerProfile?: {
      providerId: string;
      modelId: string;
      keyRef?: string;
      endpoint?: string;
    };
    policyProfile?: {
      allowFanArt?: boolean;
      allowUserMaps?: boolean;
      requireProvenance?: boolean;
      contentRating?: string;
    };
    templateVersion?: string;
    sharedThemeCss?: string;
  }): ProjectResult<ProjectManifest> {
    if (!input.gameTitle.trim()) {
      return { ok: false, error: 'Game title is required' };
    }

    if (!input.guideIntent.trim()) {
      return { ok: false, error: 'Guide intent is required' };
    }

    try {
      const manifest = this.storage.createProject({
        projectState: 'intake',
        gameTitle: input.gameTitle,
        platform: input.platform ?? '',
        guideIntent: input.guideIntent,
        playerGoal: input.playerGoal ?? '',
        runStyle: input.runStyle,
        spoilerPolicy: input.spoilerPolicy as ProjectManifest['spoilerPolicy'],
        depthPreference: input.depthPreference as ProjectManifest['depthPreference'],
        providerProfile: input.providerProfile,
        policyProfile: {
          allowFanArt: input.policyProfile?.allowFanArt ?? false,
          allowUserMaps: input.policyProfile?.allowUserMaps ?? true,
          requireProvenance: input.policyProfile?.requireProvenance ?? true,
          contentRating: (input.policyProfile?.contentRating ?? 'everyone') as ProjectManifest['policyProfile']['contentRating'],
        },
        templateVersion: input.templateVersion ?? '0.1.0',
        sharedThemeCss: input.sharedThemeCss ?? '',
      });

      return { ok: true, data: manifest };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error creating project';
      return { ok: false, error: message };
    }
  }

  getProject(id: string): ProjectResult<ProjectManifest> {
    const manifest = this.storage.getProject(id);
    if (!manifest) {
      return { ok: false, error: `Project not found: ${id}` };
    }

    return { ok: true, data: manifest };
  }

  listProjects(): ProjectManifest[] {
    return this.storage.listProjects();
  }

  listJobs(projectId: string): BuildJob[] {
    return this.storage.listJobs(projectId);
  }

  listBlueprints(projectId: string): EditorialBlueprint[] {
    return this.storage.listBlueprints(projectId);
  }

  listGuideBuildBundles(projectId: string): GuideBuildBundle[] {
    return this.storage.listGuideBuildBundles(projectId);
  }

  listKnowledgeEntities(projectId: string): KnowledgeEntity[] {
    return this.storage.listKnowledgeEntities(projectId);
  }

  listPreviewPackages(projectId: string): PreviewPackage[] {
    return this.storage.listPreviewPackages(projectId);
  }

  listValidationReports(projectId: string): ValidationReport[] {
    return this.storage.listValidationReports(projectId);
  }

  createBlueprint(
    projectId: string,
    input: {
      chapterPlans: EditorialBlueprint['chapterPlans'];
      terminologyRules: string;
      styleBible: string;
      crossReferencePlan: string;
      checklistPlan: string;
      visualPackIntent: string;
      spoilerGuardrails: string;
    },
  ): ProjectResult<EditorialBlueprint> {
    const project = this.storage.getProject(projectId);
    if (!project) {
      return { ok: false, error: `Project not found: ${projectId}` };
    }

    if (input.chapterPlans.length === 0) {
      return { ok: false, error: 'At least one chapter plan is required.' };
    }

    const requiredTextFields = [
      ['terminology rules', input.terminologyRules],
      ['style bible', input.styleBible],
      ['cross-reference plan', input.crossReferencePlan],
      ['checklist plan', input.checklistPlan],
      ['visual pack intent', input.visualPackIntent],
      ['spoiler guardrails', input.spoilerGuardrails],
    ] as const;

    const missingField = requiredTextFields.find(([, value]) => !value.trim());
    if (missingField) {
      return { ok: false, error: `Blueprint ${missingField[0]} is required.` };
    }

    try {
      const latestBlueprint = this.storage.listBlueprints(projectId)[0] ?? null;
      const blueprint = this.storage.createBlueprint({
        projectId,
        version: latestBlueprint ? latestBlueprint.version + 1 : 1,
        chapterPlans: input.chapterPlans,
        terminologyRules: input.terminologyRules.trim(),
        styleBible: input.styleBible.trim(),
        crossReferencePlan: input.crossReferencePlan.trim(),
        checklistPlan: input.checklistPlan.trim(),
        visualPackIntent: input.visualPackIntent.trim(),
        spoilerGuardrails: input.spoilerGuardrails.trim(),
      });

      this.storage.updateProject(projectId, { projectState: 'blueprint-ready' });
      return { ok: true, data: blueprint };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error creating editorial blueprint';
      return { ok: false, error: message };
    }
  }

  createKnowledgeEntity(
    projectId: string,
    input: {
      canonicalName: string;
      category: KnowledgeEntity['category'];
      visibility: KnowledgeEntity['visibility'];
      scope: KnowledgeEntity['scope'];
      confidence: KnowledgeEntity['confidence'];
      aliases?: string[];
      summary: string;
      structuredAttributes?: KnowledgeAttribute[];
      sourceRefs: KnowledgeSourceInput[];
      conflictMarkers?: KnowledgeConflictInput[];
      versionTags?: string[];
    },
  ): ProjectResult<KnowledgeEntity> {
    const project = this.storage.getProject(projectId);
    if (!project) {
      return { ok: false, error: `Project not found: ${projectId}` };
    }

    if (!input.canonicalName.trim()) {
      return { ok: false, error: 'Knowledge entry name is required.' };
    }

    if (!input.summary.trim()) {
      return { ok: false, error: 'Knowledge summary is required.' };
    }

    if (project.policyProfile.requireProvenance && input.sourceRefs.length === 0) {
      return { ok: false, error: 'At least one source reference is required while provenance is enforced.' };
    }

    try {
      const knowledgeEntity = this.storage.createKnowledgeEntity({
        projectId,
        canonicalName: input.canonicalName.trim(),
        category: input.category,
        visibility: input.visibility,
        scope: input.scope,
        confidence: input.confidence,
        aliases: normalizeList(input.aliases),
        summary: input.summary.trim(),
        structuredAttributes: (input.structuredAttributes ?? [])
          .map((attribute) => ({ key: attribute.key.trim(), value: attribute.value.trim() }))
          .filter((attribute) => attribute.key.length > 0 && attribute.value.length > 0),
        sourceRefs: materializeSourceRefs(input.sourceRefs),
        conflictMarkers: materializeConflictMarkers(input.conflictMarkers),
        versionTags: normalizeList(input.versionTags),
      });

      const nextState = nextKnowledgeProjectState(project.projectState);
      if (nextState !== project.projectState) {
        this.storage.updateProject(projectId, { projectState: nextState });
      }

      return { ok: true, data: knowledgeEntity };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error creating knowledge entry';
      return { ok: false, error: message };
    }
  }

  updateKnowledgeEntity(
    entityId: string,
    input: {
      canonicalName: string;
      category: KnowledgeEntity['category'];
      visibility: KnowledgeEntity['visibility'];
      scope: KnowledgeEntity['scope'];
      confidence: KnowledgeEntity['confidence'];
      aliases?: string[];
      summary: string;
      structuredAttributes?: KnowledgeAttribute[];
      sourceRefs: KnowledgeSourceInput[];
      conflictMarkers?: KnowledgeConflictInput[];
      versionTags?: string[];
    },
  ): ProjectResult<KnowledgeEntity> {
    const existing = this.storage.getKnowledgeEntity(entityId);
    if (!existing) {
      return { ok: false, error: `Knowledge entry not found: ${entityId}` };
    }

    const project = this.storage.getProject(existing.projectId);
    if (!project) {
      return { ok: false, error: `Project not found: ${existing.projectId}` };
    }

    if (!input.canonicalName.trim()) {
      return { ok: false, error: 'Knowledge entry name is required.' };
    }

    if (!input.summary.trim()) {
      return { ok: false, error: 'Knowledge summary is required.' };
    }

    if (project.policyProfile.requireProvenance && input.sourceRefs.length === 0) {
      return { ok: false, error: 'At least one source reference is required while provenance is enforced.' };
    }

    try {
      const updatedEntity = this.storage.updateKnowledgeEntity(entityId, {
        canonicalName: input.canonicalName.trim(),
        category: input.category,
        visibility: input.visibility,
        scope: input.scope,
        confidence: input.confidence,
        aliases: normalizeList(input.aliases),
        summary: input.summary.trim(),
        structuredAttributes: (input.structuredAttributes ?? [])
          .map((attribute) => ({ key: attribute.key.trim(), value: attribute.value.trim() }))
          .filter((attribute) => attribute.key.length > 0 && attribute.value.length > 0),
        sourceRefs: materializeSourceRefs(input.sourceRefs),
        conflictMarkers: materializeConflictMarkers(input.conflictMarkers),
        versionTags: normalizeList(input.versionTags),
      });

      if (!updatedEntity) {
        return { ok: false, error: `Knowledge entry not found: ${entityId}` };
      }

      return { ok: true, data: updatedEntity };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error updating knowledge entry';
      return { ok: false, error: message };
    }
  }

  generatePreview(projectId: string): ProjectResult<PreviewPackage> {
    const project = this.storage.getProject(projectId);
    if (!project) {
      return { ok: false, error: `Project not found: ${projectId}` };
    }

    const latestBlueprint = this.storage.listBlueprints(projectId)[0] ?? null;
    if (!latestBlueprint) {
      return { ok: false, error: 'Draft an editorial blueprint before generating a preview.' };
    }

    try {
      const latestPreview = this.storage.listPreviewPackages(projectId)[0] ?? null;
      const preview = this.storage.createPreview(
        buildPreviewPackageInput(
          project,
          latestBlueprint,
          latestPreview ? latestPreview.version + 1 : 1,
        ),
      );

      this.storage.updateProject(projectId, { projectState: 'preview-ready' });
      return { ok: true, data: preview };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error generating preview package';
      return { ok: false, error: message };
    }
  }

  reviewPreview(
    previewId: string,
    input: {
      approvalState: PreviewApprovalStateValue;
      reviewNotes?: string;
    },
  ): ProjectResult<PreviewPackage> {
    const preview = this.storage.getPreview(previewId);
    if (!preview) {
      return { ok: false, error: `Preview not found: ${previewId}` };
    }

    const reviewNotes = input.reviewNotes?.trim() ?? '';
    if (input.approvalState === 'changes-requested' && !reviewNotes) {
      return { ok: false, error: 'Explain what needs to change before requesting a revision.' };
    }

    const project = this.storage.getProject(preview.projectId);
    if (!project) {
      return { ok: false, error: `Project not found: ${preview.projectId}` };
    }

    try {
      const updatedPreview = this.storage.updatePreview(previewId, {
        approvalState: input.approvalState,
        reviewNotes,
      });

      if (!updatedPreview) {
        return { ok: false, error: `Preview not found: ${previewId}` };
      }

      this.storage.updateProject(preview.projectId, {
        projectState: input.approvalState === 'changes-requested' ? 'revision-requested' : 'preview-ready',
      });

      return { ok: true, data: updatedPreview };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error reviewing preview package';
      return { ok: false, error: message };
    }
  }

  runFullBuild(projectId: string): ProjectResult<{
    bundle: GuideBuildBundle;
    job: BuildJob;
    report: ValidationReport;
  }> {
    const project = this.storage.getProject(projectId);
    if (!project) {
      return { ok: false, error: `Project not found: ${projectId}` };
    }

    const latestBlueprint = this.storage.listBlueprints(projectId)[0] ?? null;
    if (!latestBlueprint) {
      return { ok: false, error: 'Draft an editorial blueprint before running the full build.' };
    }

    const latestPreview = this.storage.listPreviewPackages(projectId)[0] ?? null;
    if (!latestPreview) {
      return { ok: false, error: 'Generate and approve a preview before running the full build.' };
    }

    if (latestPreview.approvalState !== 'approved') {
      return { ok: false, error: 'Approve the latest preview before running the full build.' };
    }

    if (latestPreview.blueprintVersion !== latestBlueprint.version) {
      return { ok: false, error: 'Generate and approve a fresh preview for the latest blueprint before running the full build.' };
    }

    try {
      const knowledgeEntities = this.storage.listKnowledgeEntities(projectId);
      const visibleKnowledge = knowledgeEntities.filter((entity) => entity.visibility === 'visible-guide');
      const hiddenKnowledge = knowledgeEntities.filter((entity) => entity.visibility === 'hidden-codex');
      const latestBundle = this.storage.listGuideBuildBundles(projectId)[0] ?? null;
      const now = new Date().toISOString();

      const bundle: GuideBuildBundle = {
        ...buildGuideBuildBundleInput(
          project,
          latestBlueprint,
          latestPreview,
          visibleKnowledge,
          hiddenKnowledge,
          latestBundle ? latestBundle.version + 1 : 1,
        ),
        id: generateUuid(),
        generatedAt: now,
        validationState: 'freeze-pending',
        summary: 'Full build bundle generated and awaiting validation summary.',
      };

      const validation = createFullBuildValidationArtifacts({
        project,
        blueprint: latestBlueprint,
        preview: latestPreview,
        bundle,
        visibleKnowledge,
        hiddenKnowledge,
      });

      const finalizedBundle: GuideBuildBundle = {
        ...bundle,
        validationState: validation.nextProjectState === 'validation-failed' ? 'validation-failed' : 'freeze-pending',
        summary: validation.summary,
      };

      const savedBundle = this.storage.saveGuideBuildBundle(finalizedBundle);
      const savedReport = this.storage.saveValidationReport(validation.report);
      const savedJob = this.storage.saveJob({
        id: generateUuid(),
        projectId,
        jobType: 'full-build',
        status: validation.jobStatus,
        progress: 100,
        startedAt: now,
        endedAt: now,
        humanSummary: validation.summary,
        validationReportId: savedReport.id,
      });

      this.storage.updateProject(projectId, { projectState: validation.nextProjectState });
      return { ok: true, data: { bundle: savedBundle, job: savedJob, report: savedReport } };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error running full build';
      return { ok: false, error: message };
    }
  }

  updateProject(id: string, patch: Partial<Omit<ProjectManifest, 'id' | 'createdAt'>>): ProjectResult<ProjectManifest> {
    try {
      const result = this.storage.updateProject(id, patch);
      if (!result) {
        return { ok: false, error: `Project not found: ${id}` };
      }

      return { ok: true, data: result };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error updating project';
      return { ok: false, error: message };
    }
  }

  deleteProject(id: string): ProjectResult<boolean> {
    const deleted = this.storage.deleteProject(id);
    if (!deleted) {
      return { ok: false, error: `Project not found: ${id}` };
    }

    return { ok: true, data: true };
  }

  runIntakeValidation(id: string): ProjectResult<{ job: BuildJob; report: ValidationReport }> {
    const project = this.storage.getProject(id);
    if (!project) {
      return { ok: false, error: `Project not found: ${id}` };
    }

    try {
      const { job, report } = createIntakeValidationArtifacts(project);
      const savedReport = this.storage.saveValidationReport(report);
      const savedJob = this.storage.saveJob(job);
      return { ok: true, data: { job: savedJob, report: savedReport } };
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Unknown error validating intake';
      return { ok: false, error: message };
    }
  }

  shutdown(): void {
    this.storage.shutdown();
  }
}
