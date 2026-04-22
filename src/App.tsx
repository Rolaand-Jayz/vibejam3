import { useCallback, useEffect, useMemo, useState } from 'react';
import type { EditorialBlueprint, ProjectManifest } from './domain/index.js';
import { ProjectService } from './orchestration/index.js';
import { LocalStorageProjectStorage } from './storage/index.js';
import {
  blueprintSummary as summarizeBlueprint,
  formatChapterPlanText,
  parseChapterPlanText,
} from './validation/editorial-blueprint.js';
import { unresolvedFields } from './validation/intake-validation.js';

type CenterMode = 'detail' | 'form' | 'blueprint';
type FormMode = 'create' | 'edit';

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

function statusClass(status: string): string {
  return `status-${status.toLowerCase().replace(/[^a-z0-9]+/g, '-')}`;
}

export default function App() {
  const [projects, setProjects] = useState<ProjectManifest[]>([]);
  const [selectedId, setSelectedId] = useState<string | null>(null);
  const [centerMode, setCenterMode] = useState<CenterMode>('detail');
  const [formMode, setFormMode] = useState<FormMode>('create');
  const [formState, setFormState] = useState<ProjectFormState>(createDefaultFormState);
  const [blueprintFormState, setBlueprintFormState] = useState<BlueprintFormState | null>(null);
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
  const latestBlueprint = selectedBlueprints[0] ?? null;
  const selectedJobs = selectedProject ? service.listJobs(selectedProject.id) : [];
  const selectedReports = selectedProject ? service.listValidationReports(selectedProject.id) : [];
  const latestJob = selectedJobs[0] ?? null;
  const latestReport = selectedReports[0] ?? null;
  const validationStatus = latestJob?.status.toUpperCase() ?? 'NOT RUN';
  const validationSummary = latestReport?.summary ?? 'No intake validation has been recorded yet.';
  const lastChecked = latestJob?.endedAt ?? latestReport?.createdAt ?? 'Not yet';
  const activeThemeCss = centerMode === 'form' ? formState.sharedThemeCss : selectedProject?.sharedThemeCss ?? '';

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

  return (
    <main className="app-shell guide-theme-surface">
      <section className="hero-card guide-panel">
        <div className="hero-copy">
          <p className="eyebrow">MY PLAITHROUGH</p>
          <h1>Web workbench kept in step with the Qt desktop app.</h1>
          <p className="lead">
            Project manifests are now durable in the browser, intake validation runs are persisted,
            and each project can carry custom CSS so the app and guide surfaces can share a theme.
          </p>
        </div>

        <div className="hero-badge guide-panel">
          <span className="badge-label">current slice</span>
          <strong>Durable manifest · intake checks · shared CSS theme</strong>
          <span>Browser local storage now mirrors the current Qt intake workflow.</span>
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
