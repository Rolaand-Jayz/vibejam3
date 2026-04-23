import type {
  EditorialBlueprint,
  GuideBuildBundle,
  KnowledgeEntity,
  PreviewPackage,
  ProjectManifest,
  ValidationReport,
} from '../domain/index.js';

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

export function createFullBuildValidationArtifacts(input: {
  project: ProjectManifest;
  blueprint: EditorialBlueprint;
  preview: PreviewPackage;
  bundle: GuideBuildBundle;
  visibleKnowledge: KnowledgeEntity[];
  hiddenKnowledge: KnowledgeEntity[];
}): {
  report: ValidationReport;
  nextProjectState: ProjectManifest['projectState'];
  jobStatus: 'blocked' | 'requires-review';
  summary: string;
} {
  const { project, blueprint, preview, bundle, visibleKnowledge, hiddenKnowledge } = input;

  const checksRun = [
    'approved-preview-gate',
    'bounded-guide-unit-coverage',
    'hidden-codex-separation',
    'knowledge-conflict-scan',
  ];

  const blockingFindings: string[] = [];
  const warningFindings: string[] = [];

  if (preview.approvalState !== 'approved') {
    blockingFindings.push('The latest preview is not approved, so the full build cannot be trusted for freeze review.');
  }

  if (preview.blueprintVersion !== blueprint.version) {
    blockingFindings.push('The approved preview does not match the latest blueprint version.');
  }

  const walkthroughUnitCount = bundle.units.filter((unit) => unit.kind === 'walkthrough-chapter').length;
  if (walkthroughUnitCount !== blueprint.chapterPlans.length) {
    blockingFindings.push('The build does not cover every planned walkthrough chapter as a bounded unit.');
  }

  if (visibleKnowledge.length === 0) {
    warningFindings.push('No visible guide-facing knowledge entries exist yet, so the build relies on blueprint structure alone.');
  }

  for (const unit of bundle.units) {
    warningFindings.push(...unit.warningFindings);
    blockingFindings.push(...unit.blockingFindings);
  }

  for (const entity of visibleKnowledge) {
    for (const conflict of entity.conflictMarkers) {
      if (conflict.severity === 'blocking' && conflict.resolutionStatus !== 'resolved') {
        blockingFindings.push(`${entity.canonicalName} still carries an unresolved blocking conflict: ${conflict.conflictSummary}`);
      } else if (conflict.severity === 'warning' && conflict.resolutionStatus !== 'resolved') {
        warningFindings.push(`${entity.canonicalName} carries an unresolved warning conflict: ${conflict.conflictSummary}`);
      }
    }

    if (entity.confidence === 'low') {
      warningFindings.push(`${entity.canonicalName} is still marked low-confidence and should be reviewed before freeze.`);
    }
  }

  for (const hiddenEntity of hiddenKnowledge) {
    const leaked = bundle.units.some((unit) => unit.body.includes(hiddenEntity.canonicalName));
    if (leaked) {
      blockingFindings.push(`Hidden codex note “${hiddenEntity.canonicalName}” leaked into the visible build bundle.`);
    }
  }

  if (hiddenKnowledge.length > 0) {
    warningFindings.push(`${hiddenKnowledge.length} hidden codex note(s) were withheld from the visible guide bundle.`);
  }

  const dedupedBlockingFindings = Array.from(new Set(blockingFindings));
  const dedupedWarningFindings = Array.from(new Set(warningFindings));
  const nextProjectState = dedupedBlockingFindings.length > 0 ? 'validation-failed' : 'freeze-pending';
  const summary = dedupedBlockingFindings.length > 0
    ? `${project.gameTitle} full build v${bundle.version} produced ${bundle.units.length} bounded units but ${dedupedBlockingFindings.length} blocker(s) still prevent freeze.`
    : `${project.gameTitle} full build v${bundle.version} produced ${bundle.units.length} bounded units and is ready for freeze review.`;

  const report: ValidationReport = {
    id: generateUuid(),
    projectId: project.id,
    targetType: 'guide-build-bundle',
    targetId: bundle.id,
    checksRun,
    blockingFindings: dedupedBlockingFindings,
    warningFindings: dedupedWarningFindings,
    blockingCount: dedupedBlockingFindings.length,
    warningCount: dedupedWarningFindings.length,
    createdAt: new Date().toISOString(),
    summary,
  };

  return {
    report,
    nextProjectState,
    jobStatus: dedupedBlockingFindings.length > 0 ? 'blocked' : 'requires-review',
    summary,
  };
}
