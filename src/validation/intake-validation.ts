import type { BuildJob, ProjectManifest, ValidationReport } from '../domain/index.js';

function generateUUID(): string {
  if (typeof crypto !== 'undefined' && typeof crypto.randomUUID === 'function') {
    return crypto.randomUUID();
  }

  return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, (c) => {
    const r = (Math.random() * 16) | 0;
    const v = c === 'x' ? r : (r & 0x3) | 0x8;
    return v.toString(16);
  });
}

function currentIsoTimestamp(): string {
  return new Date().toISOString();
}

function summarizeValidation(report: ValidationReport): string {
  if (report.blockingCount === 0 && report.warningCount === 0) {
    return 'Intake check passed with no blockers or warnings.';
  }

  return `${report.blockingCount} blocker(s), ${report.warningCount} warning(s)`;
}

export function unresolvedFields(project: Pick<ProjectManifest, 'platform' | 'playerGoal'>): string[] {
  const unresolved: string[] = [];

  if (!project.platform.trim()) {
    unresolved.push('platform / version');
  }

  if (!project.playerGoal.trim()) {
    unresolved.push('player goal');
  }

  return unresolved;
}

export function createIntakeValidationArtifacts(project: ProjectManifest): {
  report: ValidationReport;
  job: BuildJob;
} {
  const createdAt = currentIsoTimestamp();

  const report: ValidationReport = {
    id: generateUUID(),
    projectId: project.id,
    targetType: 'project-manifest',
    targetId: project.id,
    checksRun: [
      'required-intake-fields',
      'platform-accuracy-signal',
      'intake-state-readiness',
      'shared-theme-presence',
    ],
    blockingFindings: [],
    warningFindings: [],
    blockingCount: 0,
    warningCount: 0,
    createdAt,
    summary: '',
  };

  if (!project.platform.trim()) {
    report.blockingFindings.push(
      'Platform or version constraints are unresolved, so accuracy-sensitive guide work should not advance yet.',
    );
  }

  if (!project.playerGoal.trim()) {
    report.warningFindings.push(
      'Player goal is unresolved, so the guide intent is still broad and may need refinement.',
    );
  }

  if (!project.sharedThemeCss.trim()) {
    report.warningFindings.push(
      'No shared theme CSS is stored yet, so the web app and future guide surfaces will keep the default editorial theme.',
    );
  }

  report.blockingCount = report.blockingFindings.length;
  report.warningCount = report.warningFindings.length;
  report.summary = summarizeValidation(report);

  const job: BuildJob = {
    id: generateUUID(),
    projectId: project.id,
    jobType: 'validate',
    status: report.blockingCount > 0 ? 'blocked' : 'completed',
    progress: 100,
    startedAt: createdAt,
    endedAt: createdAt,
    humanSummary: report.summary,
    validationReportId: report.id,
  };

  return { report, job };
}
