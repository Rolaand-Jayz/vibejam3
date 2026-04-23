import { useCallback, useEffect, useMemo, useState } from 'react';
import type {
  ConflictRecord,
  EditorialBlueprint,
  GuideBuildBundle,
  KnowledgeEntity,
  PreviewPackage,
  ProjectManifest,
  SourceReference,
} from './domain/index.js';
import { ProjectService } from './orchestration/index.js';
import { LocalStorageProjectStorage } from './storage/index.js';
import {
  blueprintSummary as summarizeBlueprint,
  formatChapterPlanText,
  parseChapterPlanText,
} from './validation/editorial-blueprint.js';
import {
  formatKnowledgeAttributeText,
  formatKnowledgeListText,
  knowledgeEntitySummary,
  parseKnowledgeAttributeText,
  parseKnowledgeListText,
} from './validation/knowledge-entity.js';
import { unresolvedFields } from './validation/intake-validation.js';

type CenterMode = 'detail' | 'form' | 'blueprint' | 'knowledge';
type FormMode = 'create' | 'edit';
type KnowledgeFormMode = 'create' | 'edit';

type ProjectFormState = {
  gameTitle: string;
  platform: string;
  guideIntent: string;
  playerGoal: string;
  runStyle: string;
  spoilerPolicy: ProjectManifest['spoilerPolicy'];
  depthPreference: ProjectManifest['depthPreference'];
  sharedThemeCss: string;
};

type BlueprintFormState = {
  chapterPlanText: string;
  terminologyRules: string;
  styleBible: string;
  crossReferencePlan: string;
  checklistPlan: string;
  visualPackIntent: string;
  spoilerGuardrails: string;
};

type KnowledgeFormState = {
  knowledgeId: string | null;
  mode: KnowledgeFormMode;
  canonicalName: string;
  category: KnowledgeEntity['category'];
  visibility: KnowledgeEntity['visibility'];
  scope: KnowledgeEntity['scope'];
  confidence: KnowledgeEntity['confidence'];
  aliasesText: string;
  summary: string;
  attributesText: string;
  versionTagsText: string;
  sourceType: string;
  sourceTitle: string;
  sourceUri: string;
  sourceExcerpt: string;
  sourceTrustClassification: SourceReference['trustClassification'];
  conflictSummary: string;
  conflictType: string;
  conflictSeverity: ConflictRecord['severity'];
  conflictResolutionStatus: ConflictRecord['resolutionStatus'];
  recommendedHandling: string;
};

const storage = new LocalStorageProjectStorage();
const service = new ProjectService(storage);
service.initialize();

function createDefaultFormState(): ProjectFormState {
  return {
    gameTitle: '',
    platform: '',
    guideIntent: 'complete walkthrough',
    playerGoal: '',
    runStyle: 'casual',
    spoilerPolicy: 'minimal',
    depthPreference: 'standard',
    sharedThemeCss: '',
  };
}

function createDefaultBlueprintFormState(
  project: ProjectManifest,
  latestBlueprint: EditorialBlueprint | null,
): BlueprintFormState {
  if (latestBlueprint) {
    return {
      chapterPlanText: formatChapterPlanText(latestBlueprint.chapterPlans),
      terminologyRules: latestBlueprint.terminologyRules,
      styleBible: latestBlueprint.styleBible,
      crossReferencePlan: latestBlueprint.crossReferencePlan,
      checklistPlan: latestBlueprint.checklistPlan,
      visualPackIntent: latestBlueprint.visualPackIntent,
      spoilerGuardrails: latestBlueprint.spoilerGuardrails,
    };
  }

  const playerGoal = project.playerGoal.trim() || 'the chosen run goal';

  return {
    chapterPlanText: [
      `Opening route :: Establish the early-game setup, constraints, and must-know warnings for ${project.gameTitle}.`,
      `Core progression :: Lay out the main route, power spikes, and decision points for a ${project.runStyle} run.`,
      `Cleanup and ending locks :: Track optional cleanup, missables, and end-state requirements tied to ${playerGoal}.`,
    ].join('\n'),
    terminologyRules: `Use official in-game names for ${project.gameTitle} locations, systems, and boss labels wherever source certainty is high.`,
    styleBible: `Write like a premium field manual: calm, tactical, specific, and ${project.spoilerPolicy} about story reveals.`,
    crossReferencePlan: 'Link route-critical warnings back to the relevant reference notes, missable checkpoints, and optional detour sections.',
    checklistPlan: 'End each planned chapter with irreversible checks, recommended prep, and a short “before you leave” checklist.',
    visualPackIntent: 'Keep the surface editorial and book-like: restrained callouts, warm accents, and compact tactical scannability.',
    spoilerGuardrails: 'Keep plot reveals out of headings, front matter, and route summaries unless the chosen spoiler policy explicitly allows them.',
  };
}

function createDefaultKnowledgeFormState(
  project: ProjectManifest,
  knowledgeEntity: KnowledgeEntity | null,
): KnowledgeFormState {
  if (knowledgeEntity) {
    const primarySource = knowledgeEntity.sourceRefs[0];
    const primaryConflict = knowledgeEntity.conflictMarkers[0];

    return {
      knowledgeId: knowledgeEntity.id,
      mode: 'edit',
      canonicalName: knowledgeEntity.canonicalName,
      category: knowledgeEntity.category,
      visibility: knowledgeEntity.visibility,
      scope: knowledgeEntity.scope,
      confidence: knowledgeEntity.confidence,
      aliasesText: formatKnowledgeListText(knowledgeEntity.aliases),
      summary: knowledgeEntity.summary,
      attributesText: formatKnowledgeAttributeText(knowledgeEntity.structuredAttributes),
      versionTagsText: formatKnowledgeListText(knowledgeEntity.versionTags),
      sourceType: primarySource?.sourceType ?? 'manual-entry',
      sourceTitle: primarySource?.sourceTitle ?? '',
      sourceUri: primarySource?.sourceUri ?? '',
      sourceExcerpt: primarySource?.excerpt ?? '',
      sourceTrustClassification: primarySource?.trustClassification ?? 'working-notes',
      conflictSummary: primaryConflict?.conflictSummary ?? '',
      conflictType: primaryConflict?.conflictType ?? '',
      conflictSeverity: primaryConflict?.severity ?? 'warning',
      conflictResolutionStatus: primaryConflict?.resolutionStatus ?? 'open',
      recommendedHandling: primaryConflict?.recommendedHandling ?? '',
    };
  }

  const playerGoal = project.playerGoal.trim() || 'the intended route';

  return {
    knowledgeId: null,
    mode: 'create',
    canonicalName: '',
    category: 'route-note',
    visibility: 'visible-guide',
    scope: project.playerGoal.trim() ? 'run-specific' : 'broad',
    confidence: 'medium',
    aliasesText: '',
    summary: `Capture a trustworthy fact the guide should preserve for ${playerGoal}.`,
    attributesText: ['Route role :: What this fact changes in the guide', 'Verification need :: What source proof supports it'].join('\n'),
    versionTagsText: project.platform.trim() ? project.platform.trim() : '',
    sourceType: 'manual-entry',
    sourceTitle: `${project.gameTitle} notes`,
    sourceUri: '',
    sourceExcerpt: '',
    sourceTrustClassification: 'working-notes',
    conflictSummary: '',
    conflictType: '',
    conflictSeverity: 'warning',
    conflictResolutionStatus: 'open',
    recommendedHandling: '',
  };
}

function formatDate(value: string): string {
  return new Date(value).toLocaleString();
}

function summaryLine(project: ProjectManifest): string {
  return `${project.platform.trim() || 'unspecified platform'} • ${project.depthPreference} • ${project.spoilerPolicy} spoilers`;
}

function themeSummary(sharedThemeCss: string): string {
  if (!sharedThemeCss.trim()) {
    return 'Default editorial theme';
  }

  const lineCount = sharedThemeCss.split('\n').filter((line) => line.trim().length > 0).length;
  return `Custom CSS defined (${lineCount} non-empty line${lineCount === 1 ? '' : 's'})`;
}

function describePreview(
  preview: PreviewPackage | null,
  latestBlueprint: EditorialBlueprint | null,
): string {
  if (!preview) {
    return 'No preview package generated yet.';
  }

  if (latestBlueprint && preview.blueprintVersion !== latestBlueprint.version) {
    return `Preview v${preview.version} targets blueprint v${preview.blueprintVersion}; generate a fresh preview for blueprint v${latestBlueprint.version}.`;
  }

  return `Preview v${preview.version} is ready for review against blueprint v${preview.blueprintVersion}.`;
}

function previewApprovalLabel(preview: PreviewPackage | null): string {
  return preview ? preview.approvalState.toUpperCase() : 'NOT GENERATED';
}

function previewSectionKindLabel(section: PreviewPackage['sampleSections'][number]): string {
  return section.kind.replace(/-/g, ' ');
}

function buildUnitKindLabel(unit: GuideBuildBundle['units'][number]): string {
  return unit.kind.replace(/-/g, ' ');
}

function knowledgeVisibilityLabel(visibility: KnowledgeEntity['visibility']): string {
  return visibility === 'hidden-codex' ? 'Hidden codex' : 'Visible guide';
}

function knowledgeCoverageSummary(knowledgeEntities: KnowledgeEntity[]): string {
  if (knowledgeEntities.length === 0) {
    return 'No structured knowledge recorded yet.';
  }

  const visibleCount = knowledgeEntities.filter((entity) => entity.visibility === 'visible-guide').length;
  const hiddenCount = knowledgeEntities.length - visibleCount;
  return `${knowledgeEntities.length} structured fact${knowledgeEntities.length === 1 ? '' : 's'} • ${visibleCount} visible • ${hiddenCount} hidden codex`;
}

function statusClass(status: string): string {
  return `status-${status.toLowerCase().replace(/[^a-z0-9]+/g, '-')}`;
}

function describeFullBuild(latestBuildBundle: GuideBuildBundle | null): string {
  if (!latestBuildBundle) {
    return 'No full build bundle generated yet.';
  }

  return latestBuildBundle.summary;
}

export default function App() {
  const [projects, setProjects] = useState<ProjectManifest[]>([]);
  const [selectedId, setSelectedId] = useState<string | null>(null);
  const [centerMode, setCenterMode] = useState<CenterMode>('detail');
  const [formMode, setFormMode] = useState<FormMode>('create');
  const [formState, setFormState] = useState<ProjectFormState>(createDefaultFormState);
  const [blueprintFormState, setBlueprintFormState] = useState<BlueprintFormState | null>(null);
  const [knowledgeFormState, setKnowledgeFormState] = useState<KnowledgeFormState | null>(null);
  const [previewReviewNotes, setPreviewReviewNotes] = useState('');
  const [error, setError] = useState<string | null>(null);
  const [notice, setNotice] = useState<string | null>(null);

  const refreshProjects = useCallback((preferredSelectedId?: string | null) => {
    const nextProjects = service.listProjects();
    setProjects(nextProjects);
    setSelectedId((currentSelectedId) => {
      const nextSelectedId = preferredSelectedId ?? currentSelectedId;
      if (nextSelectedId && nextProjects.some((project) => project.id === nextSelectedId)) {
        return nextSelectedId;
      }
      return nextProjects[0]?.id ?? null;
    });
  }, []);

  useEffect(() => {
    refreshProjects();
  }, [refreshProjects]);

  const selectedProject = useMemo(() => {
    if (!selectedId) {
      return null;
    }

    const result = service.getProject(selectedId);
    return result.ok ? result.data : null;
  }, [projects, selectedId]);

  const selectedBlueprints = selectedProject ? service.listBlueprints(selectedProject.id) : [];
  const selectedKnowledgeEntities = selectedProject ? service.listKnowledgeEntities(selectedProject.id) : [];
  const latestBlueprint = selectedBlueprints[0] ?? null;
  const selectedPreviews = selectedProject ? service.listPreviewPackages(selectedProject.id) : [];
  const latestPreview = selectedPreviews[0] ?? null;
  const selectedBuildBundles = selectedProject ? service.listGuideBuildBundles(selectedProject.id) : [];
  const latestBuildBundle = selectedBuildBundles[0] ?? null;
  const selectedJobs = selectedProject ? service.listJobs(selectedProject.id) : [];
  const selectedReports = selectedProject ? service.listValidationReports(selectedProject.id) : [];
  const latestJob = selectedJobs[0] ?? null;
  const latestReport = selectedReports[0] ?? null;
  const validationStatus = latestJob?.status.toUpperCase() ?? 'NOT RUN';
  const validationSummary = latestReport?.summary ?? 'No intake validation has been recorded yet.';
  const lastChecked = latestJob?.endedAt ?? latestReport?.createdAt ?? 'Not yet';
  const activeThemeCss = centerMode === 'form' ? formState.sharedThemeCss : selectedProject?.sharedThemeCss ?? '';
  const visibleKnowledgeCount = selectedKnowledgeEntities.filter((entity) => entity.visibility === 'visible-guide').length;
  const hiddenKnowledgeCount = selectedKnowledgeEntities.length - visibleKnowledgeCount;
  const previewMatchesLatestBlueprint = latestPreview != null && latestBlueprint != null
    ? latestPreview.blueprintVersion === latestBlueprint.version
    : false;
  const fullBuildReady = latestBlueprint != null
    && latestPreview != null
    && previewMatchesLatestBlueprint
    && latestPreview.approvalState === 'approved';

  useEffect(() => {
    setPreviewReviewNotes(latestPreview?.reviewNotes ?? '');
  }, [latestPreview?.id, latestPreview?.reviewNotes]);

  useEffect(() => {
    const styleId = 'my-plaithrough-project-theme';
    let styleElement = document.getElementById(styleId) as HTMLStyleElement | null;
    if (!styleElement) {
      styleElement = document.createElement('style');
      styleElement.id = styleId;
      document.head.appendChild(styleElement);
    }

    styleElement.textContent = activeThemeCss;
  }, [activeThemeCss]);

  const updateFormField = <K extends keyof ProjectFormState>(field: K, value: ProjectFormState[K]) => {
    setFormState((current) => ({
      ...current,
      [field]: value,
    }));
  };

  const updateBlueprintField = <K extends keyof BlueprintFormState>(field: K, value: BlueprintFormState[K]) => {
    setBlueprintFormState((current) => (current
      ? {
          ...current,
          [field]: value,
        }
      : current));
  };

  const updateKnowledgeField = <K extends keyof KnowledgeFormState>(field: K, value: KnowledgeFormState[K]) => {
    setKnowledgeFormState((current) => (current
      ? {
          ...current,
          [field]: value,
        }
      : current));
  };

  const describeValidation = (projectId: string) => {
    const jobs = service.listJobs(projectId);
    const reports = service.listValidationReports(projectId);
    return {
      status: jobs[0]?.status.toUpperCase() ?? 'NOT RUN',
      summary: reports[0]?.summary ?? 'No intake validation recorded yet.',
      lastChecked: jobs[0]?.endedAt ?? reports[0]?.createdAt ?? 'Not yet',
    };
  };

  const startCreate = () => {
    setError(null);
    setNotice(null);
    setFormMode('create');
    setFormState(createDefaultFormState());
    setCenterMode('form');
  };

  const startEdit = () => {
    if (!selectedProject) {
      return;
    }

    setError(null);
    setNotice(null);
    setFormMode('edit');
    setFormState({
      gameTitle: selectedProject.gameTitle,
      platform: selectedProject.platform,
      guideIntent: selectedProject.guideIntent,
      playerGoal: selectedProject.playerGoal,
      runStyle: selectedProject.runStyle,
      spoilerPolicy: selectedProject.spoilerPolicy,
      depthPreference: selectedProject.depthPreference,
      sharedThemeCss: selectedProject.sharedThemeCss,
    });
    setCenterMode('form');
  };

  const startBlueprintDraft = () => {
    if (!selectedProject) {
      return;
    }

    setError(null);
    setNotice(null);
    setBlueprintFormState(createDefaultBlueprintFormState(selectedProject, latestBlueprint));
    setCenterMode('blueprint');
  };

  const startKnowledgeWorkbench = (knowledgeEntity?: KnowledgeEntity | null) => {
    if (!selectedProject) {
      return;
    }

    setError(null);
    setNotice(null);
    setKnowledgeFormState(createDefaultKnowledgeFormState(selectedProject, knowledgeEntity ?? null));
    setCenterMode('knowledge');
  };

  const handleSaveProject = () => {
    setError(null);
    setNotice(null);

    if (formMode === 'create') {
      const result = service.createProject({
        gameTitle: formState.gameTitle,
        platform: formState.platform,
        guideIntent: formState.guideIntent,
        playerGoal: formState.playerGoal,
        runStyle: formState.runStyle,
        spoilerPolicy: formState.spoilerPolicy,
        depthPreference: formState.depthPreference,
        sharedThemeCss: formState.sharedThemeCss,
      });

      if (!result.ok) {
        setError(result.error);
        return;
      }

      refreshProjects(result.data.id);
      setCenterMode('detail');
      setNotice(`Created ${result.data.gameTitle}.`);
      return;
    }

    if (!selectedProject) {
      setError('Select a project before editing it.');
      return;
    }

    const result = service.updateProject(selectedProject.id, {
      projectState: selectedProject.projectState,
      gameTitle: formState.gameTitle,
      platform: formState.platform,
      guideIntent: formState.guideIntent,
      playerGoal: formState.playerGoal,
      runStyle: formState.runStyle,
      spoilerPolicy: formState.spoilerPolicy,
      depthPreference: formState.depthPreference,
      providerProfile: selectedProject.providerProfile,
      policyProfile: selectedProject.policyProfile,
      templateVersion: selectedProject.templateVersion,
      sharedThemeCss: formState.sharedThemeCss,
    });

    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(result.data.id);
    setCenterMode('detail');
    setNotice(`Updated ${result.data.gameTitle}.`);
  };

  const handleDeleteSelected = () => {
    if (!selectedProject) {
      return;
    }

    const result = service.deleteProject(selectedProject.id);
    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects();
    setCenterMode('detail');
    setNotice(`Deleted ${selectedProject.gameTitle}.`);
  };

  const handleRunIntakeCheck = () => {
    if (!selectedProject) {
      return;
    }

    const result = service.runIntakeValidation(selectedProject.id);
    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(selectedProject.id);
    setCenterMode('detail');
    setNotice(`Intake check finished for ${selectedProject.gameTitle}.`);
  };

  const handleSaveBlueprint = () => {
    if (!selectedProject || !blueprintFormState) {
      setError('Select a project before drafting an editorial blueprint.');
      return;
    }

    setError(null);
    setNotice(null);

    const chapterPlanResult = parseChapterPlanText(blueprintFormState.chapterPlanText);
    if (!chapterPlanResult.ok) {
      setError(chapterPlanResult.error);
      return;
    }

    const result = service.createBlueprint(selectedProject.id, {
      chapterPlans: chapterPlanResult.data,
      terminologyRules: blueprintFormState.terminologyRules,
      styleBible: blueprintFormState.styleBible,
      crossReferencePlan: blueprintFormState.crossReferencePlan,
      checklistPlan: blueprintFormState.checklistPlan,
      visualPackIntent: blueprintFormState.visualPackIntent,
      spoilerGuardrails: blueprintFormState.spoilerGuardrails,
    });

    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(selectedProject.id);
    setCenterMode('detail');
    setNotice(`Saved blueprint v${result.data.version} for ${selectedProject.gameTitle}.`);
  };

  const handleSaveKnowledge = () => {
    if (!selectedProject || !knowledgeFormState) {
      setError('Select a project before editing the knowledge base.');
      return;
    }

    setError(null);
    setNotice(null);

    if (!knowledgeFormState.canonicalName.trim()) {
      setError('Knowledge entry name is required.');
      return;
    }

    if (!knowledgeFormState.summary.trim()) {
      setError('Knowledge summary is required.');
      return;
    }

    const attributeResult = parseKnowledgeAttributeText(knowledgeFormState.attributesText);
    if (!attributeResult.ok) {
      setError(attributeResult.error);
      return;
    }

    const hasSourceDetails = [
      knowledgeFormState.sourceType,
      knowledgeFormState.sourceTitle,
      knowledgeFormState.sourceUri,
      knowledgeFormState.sourceExcerpt,
    ].some((value) => value.trim().length > 0);

    if (hasSourceDetails && (!knowledgeFormState.sourceType.trim() || !knowledgeFormState.sourceTitle.trim() || !knowledgeFormState.sourceExcerpt.trim())) {
      setError('Source type, source title, and source excerpt are required when a provenance note is recorded.');
      return;
    }

    if (!hasSourceDetails && selectedProject.policyProfile.requireProvenance) {
      setError('At least one provenance note is required for each knowledge entry.');
      return;
    }

    const hasConflictDetails = [
      knowledgeFormState.conflictSummary,
      knowledgeFormState.conflictType,
      knowledgeFormState.recommendedHandling,
    ].some((value) => value.trim().length > 0);

    if (hasConflictDetails && (!knowledgeFormState.conflictSummary.trim() || !knowledgeFormState.conflictType.trim() || !knowledgeFormState.recommendedHandling.trim())) {
      setError('Conflict summary, conflict type, and recommended handling must all be filled in together.');
      return;
    }

    const sourceRefs = hasSourceDetails
      ? [{
          sourceType: knowledgeFormState.sourceType.trim(),
          sourceTitle: knowledgeFormState.sourceTitle.trim(),
          sourceUri: knowledgeFormState.sourceUri.trim(),
          excerpt: knowledgeFormState.sourceExcerpt.trim(),
          retrievalDate: new Date().toISOString(),
          trustClassification: knowledgeFormState.sourceTrustClassification,
        }]
      : [];

    const conflictMarkers = hasConflictDetails
      ? [{
          conflictSummary: knowledgeFormState.conflictSummary.trim(),
          conflictType: knowledgeFormState.conflictType.trim(),
          severity: knowledgeFormState.conflictSeverity,
          resolutionStatus: knowledgeFormState.conflictResolutionStatus,
          recommendedHandling: knowledgeFormState.recommendedHandling.trim(),
        }]
      : [];

    const knowledgeInput = {
      canonicalName: knowledgeFormState.canonicalName.trim(),
      category: knowledgeFormState.category,
      visibility: knowledgeFormState.visibility,
      scope: knowledgeFormState.scope,
      confidence: knowledgeFormState.confidence,
      aliases: parseKnowledgeListText(knowledgeFormState.aliasesText),
      summary: knowledgeFormState.summary.trim(),
      structuredAttributes: attributeResult.data,
      sourceRefs,
      conflictMarkers,
      versionTags: parseKnowledgeListText(knowledgeFormState.versionTagsText),
    };

    const result = knowledgeFormState.mode === 'edit' && knowledgeFormState.knowledgeId
      ? service.updateKnowledgeEntity(knowledgeFormState.knowledgeId, knowledgeInput)
      : service.createKnowledgeEntity(selectedProject.id, knowledgeInput);

    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(selectedProject.id);
    setKnowledgeFormState(createDefaultKnowledgeFormState(selectedProject, result.data));
    setCenterMode('knowledge');
    setNotice(
      knowledgeFormState.mode === 'edit'
        ? `Updated knowledge entry “${result.data.canonicalName}”.`
        : `Added knowledge entry “${result.data.canonicalName}”.`,
    );
  };

  const handlePreviewAction = () => {
    if (!selectedProject) {
      setError('Select a project before generating or reviewing a preview.');
      return;
    }

    if (!latestBlueprint) {
      setError('Draft an editorial blueprint before generating a preview.');
      return;
    }

    setError(null);
    setNotice(null);

    const shouldGeneratePreview = !latestPreview
      || !previewMatchesLatestBlueprint
      || latestPreview.approvalState === 'changes-requested'
      || selectedProject.projectState === 'blueprint-ready';

    if (!shouldGeneratePreview) {
      setCenterMode('detail');
      setNotice(`Preview v${latestPreview.version} already matches blueprint v${latestBlueprint.version}. Review it below.`);
      return;
    }

    const result = service.generatePreview(selectedProject.id);
    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(selectedProject.id);
    setCenterMode('detail');
    setNotice(`Generated preview v${result.data.version} for ${selectedProject.gameTitle}.`);
  };

  const handlePreviewReview = (approvalState: 'approved' | 'changes-requested') => {
    if (!selectedProject || !latestPreview) {
      setError('Generate a preview before recording a review decision.');
      return;
    }

    setError(null);
    setNotice(null);

    const result = service.reviewPreview(latestPreview.id, {
      approvalState,
      reviewNotes: previewReviewNotes,
    });

    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(selectedProject.id);
    setCenterMode('detail');
    setNotice(
      approvalState === 'approved'
        ? `Approved preview v${result.data.version} for ${selectedProject.gameTitle}.`
        : `Marked preview v${result.data.version} for revision on ${selectedProject.gameTitle}.`,
    );
  };

  const handleFullBuildAction = () => {
    if (!selectedProject) {
      setError('Select a project before running the full build.');
      return;
    }

    setError(null);
    setNotice(null);

    const result = service.runFullBuild(selectedProject.id);
    if (!result.ok) {
      setError(result.error);
      return;
    }

    refreshProjects(selectedProject.id);
    setCenterMode('detail');
    setNotice(
      result.data.bundle.validationState === 'validation-failed'
        ? `Built guide bundle v${result.data.bundle.version}, but freeze is blocked until the new validation findings are cleared.`
        : `Built guide bundle v${result.data.bundle.version}; it is ready for freeze review when you are.`
    );
  };

  return (
    <main className="app-shell guide-theme-surface">
      <section className="hero-card guide-panel">
        <div className="hero-copy">
          <p className="eyebrow">MY PLAITHROUGH</p>
          <h1>Web workbench kept in step with the Qt desktop app.</h1>
          <p className="lead">
            Project manifests, structured knowledge records, editorial blueprints, reviewable preview packages, and
            first-pass full build bundles are now durable in the browser, while each project keeps its own shared CSS theme for the app and future guide surfaces.
          </p>
        </div>

        <div className="hero-badge guide-panel">
          <span className="badge-label">current slice</span>
          <strong>Approved preview gate · full build bundle · freeze review handoff</strong>
          <span>The guide can now produce a durable, bounded build bundle without pretending it is already a frozen edition.</span>
        </div>
      </section>

      {error && (
        <section className="message-banner error-banner guide-panel">
          <strong>Problem:</strong> {error}
        </section>
      )}

      {notice && (
        <section className="message-banner notice-banner guide-panel">
          <strong>Update:</strong> {notice}
        </section>
      )}

      <section className="workspace-grid">
        <aside className="panel rail-panel guide-panel">
          <div className="panel-heading">
            <div>
              <p className="section-eyebrow">Guide projects</p>
              <h2>Project selector</h2>
            </div>
            <span className="muted-text">{projects.length} total</span>
          </div>

          <div className="action-row compact-actions">
            <button className="btn-primary" onClick={startCreate}>
              New Project
            </button>
            <button className="btn-secondary" onClick={startEdit} disabled={!selectedProject}>
              Edit
            </button>
            <button className="btn-secondary" onClick={handleRunIntakeCheck} disabled={!selectedProject}>
              Check Intake
            </button>
            <button className="btn-secondary" onClick={startBlueprintDraft} disabled={!selectedProject}>
              Draft Blueprint
            </button>
            <button className="btn-secondary" onClick={() => startKnowledgeWorkbench()} disabled={!selectedProject}>
              Knowledge Base
            </button>
            <button className="btn-secondary" onClick={handlePreviewAction} disabled={!selectedProject || !latestBlueprint}>
              Preview / Review
            </button>
            <button className="btn-secondary" onClick={handleFullBuildAction} disabled={!selectedProject || !fullBuildReady}>
              Run Full Build
            </button>
            <button className="btn-ghost" onClick={handleDeleteSelected} disabled={!selectedProject}>
              Delete
            </button>
          </div>

          <div className="project-list" role="list">
            {projects.length === 0 ? (
              <div className="empty-state subtle-empty">
                <h3>No guide projects yet</h3>
                <p>Create a project to begin shaping a durable guide manifest and shared theme.</p>
              </div>
            ) : (
              projects.map((project) => {
                const projectValidation = describeValidation(project.id);

                return (
                  <button
                    key={project.id}
                    type="button"
                    className={`project-list-item ${selectedId === project.id ? 'selected' : ''}`}
                    onClick={() => {
                      setSelectedId(project.id);
                      setCenterMode('detail');
                      setError(null);
                    }}
                  >
                    <div className="project-list-head">
                      <strong>{project.gameTitle}</strong>
                      <span className={`status-pill ${statusClass(projectValidation.status)}`}>
                        {projectValidation.status}
                      </span>
                    </div>
                    <p>{summaryLine(project)}</p>
                    <p className="list-subnote">{projectValidation.summary}</p>
                    <div className="inline-meta">
                      <span>{themeSummary(project.sharedThemeCss)}</span>
                      <span>{project.projectState.toUpperCase()}</span>
                    </div>
                  </button>
                );
              })
            )}
          </div>
        </aside>

        <section className="panel detail-panel guide-panel">
          {centerMode === 'form' ? (
            <div className="editor-column">
              <div className="panel-heading">
                <div>
                  <p className="section-eyebrow">Manifest editor</p>
                  <h2>{formMode === 'create' ? 'New web + guide project' : 'Edit manifest + theme CSS'}</h2>
                </div>
                <span className="muted-text">{formMode === 'create' ? 'Create' : 'Update'} project</span>
              </div>

              <p className="support-copy">
                The custom CSS below is stored with the project manifest and applied live to the web workspace.
                That gives you one place to define a theme the app and future guide surfaces can share.
              </p>

              <div className="form-grid">
                <label>
                  <span>Game title *</span>
                  <input
                    type="text"
                    value={formState.gameTitle}
                    onChange={(event) => updateFormField('gameTitle', event.target.value)}
                    placeholder="e.g. Final Fantasy X-2"
                  />
                </label>

                <label>
                  <span>Platform / version</span>
                  <input
                    type="text"
                    value={formState.platform}
                    onChange={(event) => updateFormField('platform', event.target.value)}
                    placeholder="e.g. PS2, Switch, PC"
                  />
                </label>

                <label>
                  <span>Guide intent *</span>
                  <input
                    type="text"
                    value={formState.guideIntent}
                    onChange={(event) => updateFormField('guideIntent', event.target.value)}
                    placeholder="e.g. complete walkthrough"
                  />
                </label>

                <label>
                  <span>Player goal</span>
                  <input
                    type="text"
                    value={formState.playerGoal}
                    onChange={(event) => updateFormField('playerGoal', event.target.value)}
                    placeholder="e.g. 100% completion"
                  />
                </label>

                <label>
                  <span>Run style</span>
                  <select
                    value={formState.runStyle}
                    onChange={(event) => updateFormField('runStyle', event.target.value)}
                  >
                    <option value="casual">Casual</option>
                    <option value="optimized">Optimized</option>
                    <option value="challenge">Challenge run</option>
                    <option value="speedrun">Speedrun</option>
                  </select>
                </label>

                <label>
                  <span>Spoiler policy</span>
                  <select
                    value={formState.spoilerPolicy}
                    onChange={(event) => updateFormField('spoilerPolicy', event.target.value as ProjectManifest['spoilerPolicy'])}
                  >
                    <option value="none">No spoilers</option>
                    <option value="minimal">Minimal</option>
                    <option value="moderate">Moderate</option>
                    <option value="full">Full spoilers</option>
                  </select>
                </label>

                <label>
                  <span>Depth preference</span>
                  <select
                    value={formState.depthPreference}
                    onChange={(event) => updateFormField('depthPreference', event.target.value as ProjectManifest['depthPreference'])}
                  >
                    <option value="essential">Essential — main path only</option>
                    <option value="standard">Standard — main + key extras</option>
                    <option value="comprehensive">Comprehensive — thorough coverage</option>
                    <option value="completionist">Completionist — everything</option>
                    <option value="reference">Reference — encyclopedic data tables</option>
                  </select>
                </label>
              </div>

              <label className="theme-editor">
                <span>Shared web / guide CSS</span>
                <textarea
                  value={formState.sharedThemeCss}
                  onChange={(event) => updateFormField('sharedThemeCss', event.target.value)}
                  placeholder={['.guide-theme-surface {', '  --app-bg: #171717;', '  --panel-bg: rgba(35, 24, 18, 0.92);', '  --accent: #f0b35a;', '  font-family: "Libre Baskerville", serif;', '}', '', '.theme-preview-card h3 {', '  letter-spacing: 0.04em;', '}'].join('\n')}
                />
              </label>

              <div className="action-row">
                <button className="btn-primary" onClick={handleSaveProject} disabled={!formState.gameTitle.trim() || !formState.guideIntent.trim()}>
                  {formMode === 'create' ? 'Create project' : 'Save changes'}
                </button>
                <button className="btn-secondary" onClick={() => updateFormField('sharedThemeCss', '')}>
                  Clear CSS
                </button>
                <button className="btn-ghost" onClick={() => setCenterMode('detail')}>
                  Cancel
                </button>
              </div>
            </div>
          ) : centerMode === 'blueprint' && selectedProject && blueprintFormState ? (
            <div className="editor-column blueprint-editor-column">
              <div className="panel-heading">
                <div>
                  <p className="section-eyebrow">Editorial blueprint</p>
                  <h2>{latestBlueprint ? `Blueprint revision for ${selectedProject.gameTitle}` : `First blueprint for ${selectedProject.gameTitle}`}</h2>
                </div>
                <span className="muted-text">
                  {latestBlueprint ? `Next version: v${latestBlueprint.version + 1}` : 'Create v1'}
                </span>
              </div>

              <p className="support-copy">
                This is the durable editorial plan the preview and full-build pipeline will depend on. Save only what you are prepared to treat as the real structure of the guide.
              </p>

              <label className="theme-editor blueprint-plan-editor">
                <span>Chapter plans</span>
                <textarea
                  value={blueprintFormState.chapterPlanText}
                  onChange={(event) => updateBlueprintField('chapterPlanText', event.target.value)}
                  placeholder={[
                    'Opening route :: Establish the first hours, required setup, and early warnings.',
                    'Mid-game route :: Explain chapter order, optional detours, and tactical checkpoints.',
                    'Cleanup :: Capture missables, postgame cleanup, and ending requirements.',
                  ].join('\n')}
                />
              </label>

              <div className="blueprint-text-grid">
                <label>
                  <span>Terminology rules</span>
                  <textarea
                    value={blueprintFormState.terminologyRules}
                    onChange={(event) => updateBlueprintField('terminologyRules', event.target.value)}
                  />
                </label>

                <label>
                  <span>Style bible</span>
                  <textarea
                    value={blueprintFormState.styleBible}
                    onChange={(event) => updateBlueprintField('styleBible', event.target.value)}
                  />
                </label>

                <label>
                  <span>Cross-reference plan</span>
                  <textarea
                    value={blueprintFormState.crossReferencePlan}
                    onChange={(event) => updateBlueprintField('crossReferencePlan', event.target.value)}
                  />
                </label>

                <label>
                  <span>Checklist plan</span>
                  <textarea
                    value={blueprintFormState.checklistPlan}
                    onChange={(event) => updateBlueprintField('checklistPlan', event.target.value)}
                  />
                </label>

                <label>
                  <span>Visual pack intent</span>
                  <textarea
                    value={blueprintFormState.visualPackIntent}
                    onChange={(event) => updateBlueprintField('visualPackIntent', event.target.value)}
                  />
                </label>

                <label>
                  <span>Spoiler guardrails</span>
                  <textarea
                    value={blueprintFormState.spoilerGuardrails}
                    onChange={(event) => updateBlueprintField('spoilerGuardrails', event.target.value)}
                  />
                </label>
              </div>

              <div className="action-row">
                <button className="btn-primary" onClick={handleSaveBlueprint}>
                  {latestBlueprint ? 'Save blueprint revision' : 'Save blueprint'}
                </button>
                <button className="btn-ghost" onClick={() => setCenterMode('detail')}>
                  Cancel
                </button>
              </div>
            </div>
          ) : centerMode === 'knowledge' && selectedProject && knowledgeFormState ? (
            <div className="editor-column knowledge-editor-column">
              <div className="panel-heading">
                <div>
                  <p className="section-eyebrow">Knowledge registry</p>
                  <h2>{knowledgeFormState.mode === 'edit' ? `Edit ${knowledgeFormState.canonicalName}` : `Knowledge base for ${selectedProject.gameTitle}`}</h2>
                </div>
                <span className="muted-text">{knowledgeCoverageSummary(selectedKnowledgeEntities)}</span>
              </div>

              <p className="support-copy">
                Store normalized route facts here before they become chapter prose. Visible guide facts and hidden codex notes stay separate, along with provenance and conflict signals.
              </p>

              <div className="knowledge-workbench">
                <section className="knowledge-editor-panel guide-panel">
                  <div className="form-grid knowledge-form-grid">
                    <label>
                      <span>Canonical name *</span>
                      <input
                        type="text"
                        value={knowledgeFormState.canonicalName}
                        onChange={(event) => updateKnowledgeField('canonicalName', event.target.value)}
                        placeholder="e.g. Zozo clock clue"
                      />
                    </label>

                    <label>
                      <span>Category</span>
                      <select
                        value={knowledgeFormState.category}
                        onChange={(event) => updateKnowledgeField('category', event.target.value as KnowledgeEntity['category'])}
                      >
                        <option value="route-note">Route note</option>
                        <option value="quest">Quest</option>
                        <option value="system">System</option>
                        <option value="item">Item</option>
                        <option value="location">Location</option>
                        <option value="enemy">Enemy</option>
                        <option value="skill">Skill</option>
                        <option value="class">Class</option>
                        <option value="vendor">Vendor</option>
                        <option value="map-region">Map region</option>
                      </select>
                    </label>

                    <label>
                      <span>Visibility</span>
                      <select
                        value={knowledgeFormState.visibility}
                        onChange={(event) => updateKnowledgeField('visibility', event.target.value as KnowledgeEntity['visibility'])}
                      >
                        <option value="visible-guide">Visible guide</option>
                        <option value="hidden-codex">Hidden codex</option>
                      </select>
                    </label>

                    <label>
                      <span>Scope</span>
                      <select
                        value={knowledgeFormState.scope}
                        onChange={(event) => updateKnowledgeField('scope', event.target.value as KnowledgeEntity['scope'])}
                      >
                        <option value="broad">Broad knowledge</option>
                        <option value="run-specific">Run-specific</option>
                      </select>
                    </label>

                    <label>
                      <span>Confidence</span>
                      <select
                        value={knowledgeFormState.confidence}
                        onChange={(event) => updateKnowledgeField('confidence', event.target.value as KnowledgeEntity['confidence'])}
                      >
                        <option value="low">Low</option>
                        <option value="medium">Medium</option>
                        <option value="high">High</option>
                      </select>
                    </label>

                    <label>
                      <span>Aliases</span>
                      <textarea
                        value={knowledgeFormState.aliasesText}
                        onChange={(event) => updateKnowledgeField('aliasesText', event.target.value)}
                        placeholder="One alias per line"
                      />
                    </label>
                  </div>

                  <label className="theme-editor knowledge-summary-editor">
                    <span>Summary *</span>
                    <textarea
                      value={knowledgeFormState.summary}
                      onChange={(event) => updateKnowledgeField('summary', event.target.value)}
                      placeholder="Explain the fact in durable editorial language before it becomes guide prose."
                    />
                  </label>

                  <div className="knowledge-meta-grid">
                    <label className="theme-editor">
                      <span>Structured attributes</span>
                      <textarea
                        value={knowledgeFormState.attributesText}
                        onChange={(event) => updateKnowledgeField('attributesText', event.target.value)}
                        placeholder="Key :: Value"
                      />
                    </label>

                    <label className="theme-editor">
                      <span>Version tags</span>
                      <textarea
                        value={knowledgeFormState.versionTagsText}
                        onChange={(event) => updateKnowledgeField('versionTagsText', event.target.value)}
                        placeholder="One tag per line"
                      />
                    </label>
                  </div>

                  <section className="knowledge-subpanel">
                    <div className="panel-heading">
                      <div>
                        <p className="section-eyebrow">Provenance</p>
                        <h3>Primary source note</h3>
                      </div>
                      <span className="muted-text">Required while provenance is enforced</span>
                    </div>

                    <div className="knowledge-form-grid">
                      <label>
                        <span>Source type *</span>
                        <input
                          type="text"
                          value={knowledgeFormState.sourceType}
                          onChange={(event) => updateKnowledgeField('sourceType', event.target.value)}
                          placeholder="e.g. manual-entry"
                        />
                      </label>

                      <label>
                        <span>Trust classification</span>
                        <select
                          value={knowledgeFormState.sourceTrustClassification}
                          onChange={(event) => updateKnowledgeField('sourceTrustClassification', event.target.value as SourceReference['trustClassification'])}
                        >
                          <option value="primary">Primary</option>
                          <option value="secondary">Secondary</option>
                          <option value="community-verified">Community verified</option>
                          <option value="working-notes">Working notes</option>
                          <option value="uncertain">Uncertain</option>
                        </select>
                      </label>

                      <label>
                        <span>Source title *</span>
                        <input
                          type="text"
                          value={knowledgeFormState.sourceTitle}
                          onChange={(event) => updateKnowledgeField('sourceTitle', event.target.value)}
                          placeholder="e.g. SNES playthrough notes"
                        />
                      </label>

                      <label>
                        <span>Source URI</span>
                        <input
                          type="text"
                          value={knowledgeFormState.sourceUri}
                          onChange={(event) => updateKnowledgeField('sourceUri', event.target.value)}
                          placeholder="Optional URL or local note pointer"
                        />
                      </label>
                    </div>

                    <label className="theme-editor">
                      <span>Source excerpt *</span>
                      <textarea
                        value={knowledgeFormState.sourceExcerpt}
                        onChange={(event) => updateKnowledgeField('sourceExcerpt', event.target.value)}
                        placeholder="Record the exact clue, observation, or note that supports this fact."
                      />
                    </label>
                  </section>

                  <section className="knowledge-subpanel">
                    <div className="panel-heading">
                      <div>
                        <p className="section-eyebrow">Conflict signal</p>
                        <h3>Optional contradiction note</h3>
                      </div>
                      <span className="muted-text">Only fill this in when the fact is contested or unstable</span>
                    </div>

                    <label className="theme-editor">
                      <span>Conflict summary</span>
                      <textarea
                        value={knowledgeFormState.conflictSummary}
                        onChange={(event) => updateKnowledgeField('conflictSummary', event.target.value)}
                        placeholder="Describe the contradiction or ambiguity that still matters."
                      />
                    </label>

                    <div className="knowledge-form-grid">
                      <label>
                        <span>Conflict type</span>
                        <input
                          type="text"
                          value={knowledgeFormState.conflictType}
                          onChange={(event) => updateKnowledgeField('conflictType', event.target.value)}
                          placeholder="e.g. timing-disagreement"
                        />
                      </label>

                      <label>
                        <span>Severity</span>
                        <select
                          value={knowledgeFormState.conflictSeverity}
                          onChange={(event) => updateKnowledgeField('conflictSeverity', event.target.value as ConflictRecord['severity'])}
                        >
                          <option value="warning">Warning</option>
                          <option value="blocking">Blocking</option>
                        </select>
                      </label>

                      <label>
                        <span>Resolution status</span>
                        <select
                          value={knowledgeFormState.conflictResolutionStatus}
                          onChange={(event) => updateKnowledgeField('conflictResolutionStatus', event.target.value as ConflictRecord['resolutionStatus'])}
                        >
                          <option value="open">Open</option>
                          <option value="monitoring">Monitoring</option>
                          <option value="resolved">Resolved</option>
                        </select>
                      </label>
                    </div>

                    <label className="theme-editor">
                      <span>Recommended handling</span>
                      <textarea
                        value={knowledgeFormState.recommendedHandling}
                        onChange={(event) => updateKnowledgeField('recommendedHandling', event.target.value)}
                        placeholder="Explain how the guide or codex should carry this uncertainty forward."
                      />
                    </label>
                  </section>

                  <div className="action-row">
                    <button className="btn-primary" onClick={handleSaveKnowledge}>
                      {knowledgeFormState.mode === 'edit' ? 'Save knowledge update' : 'Add knowledge entry'}
                    </button>
                    <button className="btn-secondary" onClick={() => setKnowledgeFormState(createDefaultKnowledgeFormState(selectedProject, null))}>
                      New entry
                    </button>
                    <button className="btn-ghost" onClick={() => setCenterMode('detail')}>
                      Close workbench
                    </button>
                  </div>
                </section>

                <aside className="knowledge-browser-panel guide-panel">
                  <div className="panel-heading">
                    <div>
                      <p className="section-eyebrow">Stored entries</p>
                      <h3>Visible vs hidden</h3>
                    </div>
                    <span className="muted-text">{visibleKnowledgeCount} visible · {hiddenKnowledgeCount} hidden</span>
                  </div>

                  {selectedKnowledgeEntities.length === 0 ? (
                    <div className="empty-state subtle-empty">
                      <h3>No knowledge entries yet</h3>
                      <p>Add the first structured fact so the hidden codex and guide-facing truth stop living only in your head.</p>
                    </div>
                  ) : (
                    <div className="knowledge-entry-list">
                      {selectedKnowledgeEntities.map((knowledgeEntity) => (
                        <button
                          key={knowledgeEntity.id}
                          type="button"
                          className={`knowledge-entry-card ${knowledgeFormState.knowledgeId === knowledgeEntity.id ? 'selected' : ''}`}
                          onClick={() => setKnowledgeFormState(createDefaultKnowledgeFormState(selectedProject, knowledgeEntity))}
                        >
                          <div className="project-list-head">
                            <strong>{knowledgeEntity.canonicalName}</strong>
                            <span className={`status-pill ${statusClass(knowledgeEntity.visibility)}`}>
                              {knowledgeVisibilityLabel(knowledgeEntity.visibility)}
                            </span>
                          </div>
                          <p>{knowledgeEntity.summary}</p>
                          <div className="inline-meta">
                            <span>{knowledgeEntity.category}</span>
                            <span>{knowledgeEntity.confidence}</span>
                            <span>{knowledgeEntity.sourceRefs.length} source{knowledgeEntity.sourceRefs.length === 1 ? '' : 's'}</span>
                          </div>
                        </button>
                      ))}
                    </div>
                  )}
                </aside>
              </div>
            </div>
          ) : selectedProject ? (
            <div className="editor-column">
              <div className="panel-heading">
                <div>
                  <p className="section-eyebrow">Current selection</p>
                  <h2>{selectedProject.gameTitle}</h2>
                </div>
                <span className={`status-pill ${statusClass(selectedProject.projectState.toUpperCase())}`}>
                  {selectedProject.projectState.toUpperCase()}
                </span>
              </div>

              <p className="support-copy">{selectedProject.guideIntent} · {summaryLine(selectedProject)}</p>

              <dl className="detail-grid">
                <dt>Platform / version</dt>
                <dd>{selectedProject.platform.trim() || 'Unresolved'}</dd>

                <dt>Guide intent</dt>
                <dd>{selectedProject.guideIntent}</dd>

                <dt>Player goal</dt>
                <dd>{selectedProject.playerGoal.trim() || 'Unresolved'}</dd>

                <dt>Run style</dt>
                <dd>{selectedProject.runStyle}</dd>

                <dt>Spoiler policy</dt>
                <dd>{selectedProject.spoilerPolicy}</dd>

                <dt>Depth preference</dt>
                <dd>{selectedProject.depthPreference}</dd>

                <dt>Unresolved</dt>
                <dd>{unresolvedFields(selectedProject).join(', ') || 'None'}</dd>

                <dt>Validation</dt>
                <dd>
                  <span className={`status-pill ${statusClass(validationStatus)}`}>{validationStatus}</span>
                </dd>

                <dt>Validation summary</dt>
                <dd>{validationSummary}</dd>

                <dt>Last checked</dt>
                <dd>{typeof lastChecked === 'string' && lastChecked.includes('T') ? formatDate(lastChecked) : lastChecked}</dd>

                <dt>Theme CSS</dt>
                <dd>{themeSummary(selectedProject.sharedThemeCss)}</dd>

                <dt>Editorial blueprint</dt>
                <dd>{summarizeBlueprint(latestBlueprint)}</dd>

                <dt>Blueprint versions</dt>
                <dd>{selectedBlueprints.length}</dd>

                <dt>Knowledge base</dt>
                <dd>{knowledgeCoverageSummary(selectedKnowledgeEntities)}</dd>

                <dt>Visible guide facts</dt>
                <dd>{visibleKnowledgeCount}</dd>

                <dt>Hidden codex notes</dt>
                <dd>{hiddenKnowledgeCount}</dd>

                <dt>Preview slice</dt>
                <dd>{describePreview(latestPreview, latestBlueprint)}</dd>

                <dt>Preview approval</dt>
                <dd>
                  <span className={`status-pill ${statusClass(previewApprovalLabel(latestPreview))}`}>
                    {previewApprovalLabel(latestPreview)}
                  </span>
                </dd>

                <dt>Preview versions</dt>
                <dd>{selectedPreviews.length}</dd>

                <dt>Full build</dt>
                <dd>{describeFullBuild(latestBuildBundle)}</dd>

                <dt>Build validation</dt>
                <dd>
                  <span className={`status-pill ${statusClass(latestBuildBundle?.validationState ?? 'not-run')}`}>
                    {latestBuildBundle?.validationState.toUpperCase() ?? 'NOT RUN'}
                  </span>
                </dd>

                <dt>Build bundle versions</dt>
                <dd>{selectedBuildBundles.length}</dd>

                <dt>Storage backend</dt>
                <dd>Browser local storage</dd>

                <dt>Created</dt>
                <dd>{formatDate(selectedProject.createdAt)}</dd>

                <dt>Identifier</dt>
                <dd className="mono-value">{selectedProject.id}</dd>
              </dl>

              {latestBlueprint && (
                <section className="blueprint-summary guide-panel">
                  <div className="panel-heading">
                    <div>
                      <p className="section-eyebrow">Latest blueprint</p>
                      <h3>Editorial blueprint v{latestBlueprint.version}</h3>
                    </div>
                    <span className="muted-text">Updated {formatDate(latestBlueprint.updatedAt)}</span>
                  </div>

                  <div className="blueprint-chapter-list">
                    {latestBlueprint.chapterPlans.map((chapter) => (
                      <article key={`${latestBlueprint.id}-${chapter.title}`} className="blueprint-chapter-card">
                        <strong>{chapter.title}</strong>
                        <p>{chapter.purpose}</p>
                      </article>
                    ))}
                  </div>

                  <div className="blueprint-notes-grid">
                    <div>
                      <h4>Terminology rules</h4>
                      <p>{latestBlueprint.terminologyRules}</p>
                    </div>
                    <div>
                      <h4>Style bible</h4>
                      <p>{latestBlueprint.styleBible}</p>
                    </div>
                    <div>
                      <h4>Cross-reference plan</h4>
                      <p>{latestBlueprint.crossReferencePlan}</p>
                    </div>
                    <div>
                      <h4>Checklist plan</h4>
                      <p>{latestBlueprint.checklistPlan}</p>
                    </div>
                    <div>
                      <h4>Visual pack intent</h4>
                      <p>{latestBlueprint.visualPackIntent}</p>
                    </div>
                    <div>
                      <h4>Spoiler guardrails</h4>
                      <p>{latestBlueprint.spoilerGuardrails}</p>
                    </div>
                  </div>
                </section>
              )}

              {selectedKnowledgeEntities.length > 0 && (
                <section className="knowledge-summary guide-panel">
                  <div className="panel-heading">
                    <div>
                      <p className="section-eyebrow">Knowledge registry</p>
                      <h3>Structured facts and hidden codex notes</h3>
                    </div>
                    <span className="muted-text">{knowledgeCoverageSummary(selectedKnowledgeEntities)}</span>
                  </div>

                  <div className="knowledge-card-list">
                    {selectedKnowledgeEntities.map((knowledgeEntity) => (
                      <article key={knowledgeEntity.id} className="knowledge-card">
                        <div className="project-list-head">
                          <strong>{knowledgeEntity.canonicalName}</strong>
                          <span className={`status-pill ${statusClass(knowledgeEntity.visibility)}`}>
                            {knowledgeVisibilityLabel(knowledgeEntity.visibility)}
                          </span>
                        </div>
                        <p className="knowledge-card-summary">{knowledgeEntity.summary}</p>
                        <div className="inline-meta">
                          <span>{knowledgeEntity.category}</span>
                          <span>{knowledgeEntity.scope.replace(/-/g, ' ')}</span>
                          <span>{knowledgeEntity.confidence}</span>
                        </div>
                        <p className="knowledge-card-proof">{knowledgeEntitySummary(knowledgeEntity)}</p>
                      </article>
                    ))}
                  </div>
                </section>
              )}

              {latestPreview && (
                <section className="preview-summary guide-panel">
                  <div className="panel-heading">
                    <div>
                      <p className="section-eyebrow">Latest preview package</p>
                      <h3>Preview v{latestPreview.version}</h3>
                    </div>
                    <span className={`status-pill ${statusClass(previewApprovalLabel(latestPreview))}`}>
                      {previewApprovalLabel(latestPreview)}
                    </span>
                  </div>

                  <div className="preview-meta-row inline-meta">
                    <span>Generated {formatDate(latestPreview.generatedAt)}</span>
                    <span>Blueprint v{latestPreview.blueprintVersion}</span>
                    <span>{latestPreview.sampleSections.length} sample sections</span>
                  </div>

                  {!previewMatchesLatestBlueprint && latestBlueprint && (
                    <p className="preview-warning-note">
                      This preview targets blueprint v{latestPreview.blueprintVersion}; the latest blueprint is v{latestBlueprint.version}, so generate a fresh preview before trusting it.
                    </p>
                  )}

                  <div className="preview-section-list">
                    {latestPreview.sampleSections.map((section) => (
                      <article key={`${latestPreview.id}-${section.title}`} className="preview-section-card">
                        <div className="project-list-head">
                          <strong>{section.title}</strong>
                          <span className="chip">{previewSectionKindLabel(section)}</span>
                        </div>
                        <p className="preview-section-summary">{section.summary}</p>
                        <p className="preview-section-body">{section.body}</p>
                      </article>
                    ))}
                  </div>

                  <div className="preview-review-form">
                    <label>
                      <span>Review notes</span>
                      <textarea
                        value={previewReviewNotes}
                        onChange={(event) => setPreviewReviewNotes(event.target.value)}
                        placeholder="Record whether the tone, density, checklist behavior, and reference formatting are representative enough to move forward."
                      />
                    </label>

                    <div className="action-row">
                      <button className="btn-primary" onClick={() => handlePreviewReview('approved')}>
                        Approve Preview
                      </button>
                      <button className="btn-secondary" onClick={() => handlePreviewReview('changes-requested')}>
                        Request Changes
                      </button>
                    </div>
                  </div>
                </section>
              )}

              {latestBuildBundle && (
                <section className="preview-summary guide-panel">
                  <div className="panel-heading">
                    <div>
                      <p className="section-eyebrow">Latest full build bundle</p>
                      <h3>Build bundle v{latestBuildBundle.version}</h3>
                    </div>
                    <span className={`status-pill ${statusClass(latestBuildBundle.validationState)}`}>
                      {latestBuildBundle.validationState.toUpperCase()}
                    </span>
                  </div>

                  <div className="preview-meta-row inline-meta">
                    <span>Generated {formatDate(latestBuildBundle.generatedAt)}</span>
                    <span>{latestBuildBundle.units.length} bounded units</span>
                    <span>{latestBuildBundle.hiddenCodexIds.length} hidden codex note{latestBuildBundle.hiddenCodexIds.length === 1 ? '' : 's'} withheld</span>
                  </div>

                  <p className={latestBuildBundle.validationState === 'validation-failed' ? 'preview-warning-note' : 'support-copy'}>
                    {latestBuildBundle.summary}
                  </p>

                  <div className="preview-section-list">
                    {latestBuildBundle.units.map((unit) => (
                      <article key={unit.id} className="preview-section-card">
                        <div className="project-list-head">
                          <strong>{unit.title}</strong>
                          <span className="chip">{buildUnitKindLabel(unit)}</span>
                        </div>
                        <p className="preview-section-summary">{unit.purpose}</p>
                        <p className="preview-section-body">{unit.body}</p>
                      </article>
                    ))}
                  </div>
                </section>
              )}
            </div>
          ) : (
            <div className="empty-state detail-empty">
              <h2>No project selected</h2>
              <p>Select a project from the rail or create a new one to start shaping the manifest and shared theme.</p>
              <button className="btn-primary" onClick={startCreate}>Create project</button>
            </div>
          )}
        </section>

        <aside className="panel lens-panel guide-panel">
          <div className="panel-heading">
            <div>
              <p className="section-eyebrow">Project lens</p>
              <h2>Trust + theme surface</h2>
            </div>
            <span className="muted-text">Live CSS preview</span>
          </div>

          <dl className="lens-grid">
            <dt>State</dt>
            <dd>{selectedProject ? selectedProject.projectState.toUpperCase() : 'EMPTY'}</dd>

            <dt>Validation</dt>
            <dd>{selectedProject ? validationStatus : 'NOT RUN'}</dd>

            <dt>Unresolved</dt>
            <dd>{selectedProject ? unresolvedFields(selectedProject).join(', ') || 'None' : 'None'}</dd>

            <dt>Theme</dt>
            <dd>{centerMode === 'form' ? themeSummary(formState.sharedThemeCss) : themeSummary(selectedProject?.sharedThemeCss ?? '')}</dd>

            <dt>Blueprint</dt>
            <dd>{summarizeBlueprint(latestBlueprint)}</dd>

            <dt>Knowledge</dt>
            <dd>{knowledgeCoverageSummary(selectedKnowledgeEntities)}</dd>

            <dt>Preview</dt>
            <dd>{describePreview(latestPreview, latestBlueprint)}</dd>

            <dt>Approval</dt>
            <dd>{previewApprovalLabel(latestPreview)}</dd>

            <dt>Full build</dt>
            <dd>{describeFullBuild(latestBuildBundle)}</dd>

            <dt>Freeze gate</dt>
            <dd>{latestBuildBundle?.validationState.toUpperCase() ?? 'NOT RUN'}</dd>
          </dl>

          <div className="theme-preview-card guide-panel guide-sample-surface">
            <p className="section-eyebrow">Theme preview</p>
            <h3>Field Manual — Shared Editorial Surface</h3>
            <p>
              Use the custom CSS field to restyle this workspace and future guide surfaces together. Treat this card as your live proof target.
            </p>
            <div className="inline-meta">
              <span className="chip">App chrome</span>
              <span className="chip">Guide surface</span>
              <span className="chip">Shared CSS</span>
            </div>
          </div>

          <div className="css-guide guide-panel">
            <h3>Useful selectors</h3>
            <ul>
              <li><code>.guide-theme-surface</code> — outer shell shared by the web workspace</li>
              <li><code>.guide-panel</code> — cards, rails, and detail panels</li>
              <li><code>.guide-sample-surface</code> — theme preview card</li>
              <li><code>.status-pill</code> — trust-signal badges</li>
            </ul>
            <h3>Useful CSS variables</h3>
            <p className="code-hint">Try overriding <code>--app-bg</code>, <code>--panel-bg</code>, <code>--panel-border</code>, <code>--accent</code>, and <code>--text-primary</code>.</p>
          </div>
        </aside>
      </section>
    </main>
  );
}
