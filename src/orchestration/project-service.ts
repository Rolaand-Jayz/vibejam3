import type {
  BuildJob,
  EditorialBlueprint,
  PreviewApprovalStateValue,
  PreviewPackage,
  ProjectManifest,
  ValidationReport,
} from '../domain/index.js';
import type { ProjectStorage } from '../storage/index.js';
import { buildPreviewPackageInput } from './preview-package-factory.js';
import { createIntakeValidationArtifacts } from '../validation/intake-validation.js';

export type ProjectResult<T> =
  | { ok: true; data: T }
  | { ok: false; error: string };

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
