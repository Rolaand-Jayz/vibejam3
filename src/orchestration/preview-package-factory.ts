import type { EditorialBlueprint, PreviewPackage, ProjectManifest } from '../domain/index.js';

function requirePrimaryChapter(blueprint: EditorialBlueprint): EditorialBlueprint['chapterPlans'][number] {
  const chapter = blueprint.chapterPlans[0];
  if (!chapter) {
    throw new Error('Preview generation requires at least one chapter plan in the editorial blueprint.');
  }

  return chapter;
}

function buildFrontMatterSection(project: ProjectManifest, blueprint: EditorialBlueprint): PreviewPackage['sampleSections'][number] {
  const platform = project.platform.trim() || 'the chosen platform';
  const playerGoal = project.playerGoal.trim() || 'the chosen run goal';

  return {
    title: 'Preview frame',
    kind: 'front-matter',
    summary: 'Tone, scope, and spoiler contract for the review slice.',
    body: [
      `${project.gameTitle} — ${project.guideIntent}`,
      '',
      `Platform focus: ${platform}`,
      `Player goal: ${playerGoal}`,
      `Run style: ${project.runStyle}`,
      `Depth target: ${project.depthPreference}`,
      '',
      `Tone contract: ${blueprint.styleBible}`,
      `Spoiler guardrails: ${blueprint.spoilerGuardrails}`,
    ].join('\n'),
  };
}

function buildWalkthroughSection(project: ProjectManifest, blueprint: EditorialBlueprint): PreviewPackage['sampleSections'][number] {
  const primaryChapter = requirePrimaryChapter(blueprint);
  const supportingChapter = blueprint.chapterPlans[1] ?? primaryChapter;
  const playerGoal = project.playerGoal.trim() || 'the chosen run goal';

  return {
    title: primaryChapter.title,
    kind: 'walkthrough',
    summary: primaryChapter.purpose,
    body: [
      `Objective: ${primaryChapter.purpose}`,
      `Route stance: Guide the player toward ${playerGoal} with a ${project.runStyle} rhythm.`,
      `Supporting chapter: ${supportingChapter.title} — ${supportingChapter.purpose}`,
      '',
      `Cross-reference behavior: ${blueprint.crossReferencePlan}`,
      `Terminology discipline: ${blueprint.terminologyRules}`,
    ].join('\n'),
  };
}

function buildChecklistSection(project: ProjectManifest, blueprint: EditorialBlueprint): PreviewPackage['sampleSections'][number] {
  const firstChapter = requirePrimaryChapter(blueprint);
  const secondChapter = blueprint.chapterPlans[1] ?? firstChapter;
  const playerGoal = project.playerGoal.trim() || 'the chosen run goal';

  return {
    title: 'Checklist cadence sample',
    kind: 'checklist',
    summary: 'How route chapters end with explicit checks before progress continues.',
    body: [
      `Before leaving ${firstChapter.title}:`,
      `- Confirm the route still serves ${playerGoal}.`,
      '- Capture irreversible warnings in a short tactical list.',
      `- Call out optional detours that should link forward to ${secondChapter.title}.`,
      '',
      `Checklist rule: ${blueprint.checklistPlan}`,
    ].join('\n'),
  };
}

function buildReferenceSection(project: ProjectManifest, blueprint: EditorialBlueprint): PreviewPackage['sampleSections'][number] {
  return {
    title: 'Reference formatting sample',
    kind: 'reference',
    summary: 'How terminology, visual notes, and cross-references should read in the guide.',
    body: [
      `Reference note for ${project.gameTitle}`,
      `Preferred terminology: ${blueprint.terminologyRules}`,
      `Visual direction: ${blueprint.visualPackIntent}`,
      `Reference linkage: ${blueprint.crossReferencePlan}`,
      `Spoiler policy in effect: ${project.spoilerPolicy}`,
    ].join('\n'),
  };
}

export function buildPreviewPackageInput(
  project: ProjectManifest,
  blueprint: EditorialBlueprint,
  version: number,
): Omit<PreviewPackage, 'id' | 'generatedAt' | 'updatedAt'> {
  return {
    projectId: project.id,
    blueprintVersion: blueprint.version,
    version,
    sampleSections: [
      buildFrontMatterSection(project, blueprint),
      buildWalkthroughSection(project, blueprint),
      buildChecklistSection(project, blueprint),
      buildReferenceSection(project, blueprint),
    ],
    reviewNotes: '',
    approvalState: 'requires-review',
  };
}
