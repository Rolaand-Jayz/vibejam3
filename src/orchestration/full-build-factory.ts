import type {
  EditorialBlueprint,
  GuideBuildBundle,
  GuideBuildUnit,
  KnowledgeEntity,
  PreviewPackage,
  ProjectManifest,
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

function sortKnowledge(knowledgeEntities: KnowledgeEntity[]): KnowledgeEntity[] {
  return [...knowledgeEntities].sort((left, right) => left.canonicalName.localeCompare(right.canonicalName));
}

function selectKnowledgeForChapter(
  knowledgeEntities: KnowledgeEntity[],
  chapterIndex: number,
  chapterCount: number,
): KnowledgeEntity[] {
  if (knowledgeEntities.length === 0 || chapterCount <= 0) {
    return [];
  }

  return knowledgeEntities.filter((_, index) => index % chapterCount === chapterIndex);
}

function formatKnowledgeBulletList(knowledgeEntities: KnowledgeEntity[]): string[] {
  if (knowledgeEntities.length === 0) {
    return ['- No visible guide facts are anchored to this chapter yet; freeze review should confirm the chapter still has enough guide-facing proof.'];
  }

  return knowledgeEntities.map((entity) => `- ${entity.canonicalName}: ${entity.summary}`);
}

function buildFrontMatterUnit(
  project: ProjectManifest,
  blueprint: EditorialBlueprint,
  preview: PreviewPackage,
): GuideBuildUnit {
  const platform = project.platform.trim() || 'the chosen platform';
  const playerGoal = project.playerGoal.trim() || 'the chosen run goal';
  const reviewNotes = preview.reviewNotes.trim() || 'No extra preview notes were recorded beyond the approval decision.';

  return {
    id: generateUuid(),
    title: 'Guide framing',
    kind: 'front-matter',
    purpose: 'Lock the tone, scope, and review contract before the bounded guide units begin.',
    body: [
      `${project.gameTitle} — ${project.guideIntent}`,
      '',
      `Platform focus: ${platform}`,
      `Player goal: ${playerGoal}`,
      `Run style: ${project.runStyle}`,
      `Depth target: ${project.depthPreference}`,
      `Blueprint version: v${blueprint.version}`,
      `Approved preview version: v${preview.version}`,
      '',
      `Tone contract: ${blueprint.styleBible}`,
      `Spoiler guardrails: ${blueprint.spoilerGuardrails}`,
      `Preview approval notes: ${reviewNotes}`,
    ].join('\n'),
    sourceKnowledgeIds: [],
    blockingFindings: [],
    warningFindings: [],
  };
}

function buildWalkthroughUnit(
  project: ProjectManifest,
  blueprint: EditorialBlueprint,
  chapter: EditorialBlueprint['chapterPlans'][number],
  chapterIndex: number,
  visibleKnowledge: KnowledgeEntity[],
): GuideBuildUnit {
  const chapterKnowledge = selectKnowledgeForChapter(visibleKnowledge, chapterIndex, blueprint.chapterPlans.length);

  return {
    id: generateUuid(),
    title: chapter.title,
    kind: 'walkthrough-chapter',
    purpose: chapter.purpose,
    body: [
      `Objective: ${chapter.purpose}`,
      `Route stance: Keep the ${project.runStyle} rhythm aligned with ${project.playerGoal.trim() || 'the chosen run goal'}.`,
      `Terminology discipline: ${blueprint.terminologyRules}`,
      `Checklist rule: ${blueprint.checklistPlan}`,
      `Cross-reference plan: ${blueprint.crossReferencePlan}`,
      '',
      'Guide-facing knowledge anchors:',
      ...formatKnowledgeBulletList(chapterKnowledge),
    ].join('\n'),
    sourceKnowledgeIds: chapterKnowledge.map((entity) => entity.id),
    blockingFindings: [],
    warningFindings: chapterKnowledge.length === 0
      ? ['This walkthrough chapter has no assigned visible guide facts yet.']
      : [],
  };
}

function buildReferenceFamilyUnit(visibleKnowledge: KnowledgeEntity[], blueprint: EditorialBlueprint): GuideBuildUnit {
  const referenceLines = visibleKnowledge.length === 0
    ? ['- No visible guide-facing reference entries have been recorded yet.']
    : visibleKnowledge.map((entity) => `- ${entity.canonicalName} (${entity.category}, ${entity.confidence}) • ${entity.sourceRefs.length} source(s)`);

  return {
    id: generateUuid(),
    title: 'Reference family index',
    kind: 'reference-family',
    purpose: 'Collect the current visible guide facts into one editorial reference surface.',
    body: [
      `Reference linkage: ${blueprint.crossReferencePlan}`,
      `Visual pack intent: ${blueprint.visualPackIntent}`,
      '',
      'Visible guide knowledge in this build:',
      ...referenceLines,
    ].join('\n'),
    sourceKnowledgeIds: visibleKnowledge.map((entity) => entity.id),
    blockingFindings: [],
    warningFindings: visibleKnowledge.length === 0
      ? ['The build contains no visible guide-facing reference entries yet.']
      : [],
  };
}

function buildAppendixUnit(hiddenKnowledge: KnowledgeEntity[], blueprint: EditorialBlueprint): GuideBuildUnit {
  const appendixNotes = hiddenKnowledge.length === 0
    ? ['- No hidden codex notes were withheld from the reader-facing build.']
    : [
        `- ${hiddenKnowledge.length} hidden codex note(s) were withheld from the visible guide bundle so unresolved or spoiler-heavy notes do not leak forward.`,
        '- Freeze review should confirm that any withheld notes remain intentionally excluded from reader-facing copy.',
      ];

  return {
    id: generateUuid(),
    title: 'Freeze review appendix',
    kind: 'appendix',
    purpose: 'Summarize what the freeze reviewer should confirm before a future immutable edition is allowed.',
    body: [
      `Checklist carry-forward: ${blueprint.checklistPlan}`,
      `Spoiler guardrails: ${blueprint.spoilerGuardrails}`,
      '',
      'Freeze review notes:',
      ...appendixNotes,
    ].join('\n'),
    sourceKnowledgeIds: [],
    blockingFindings: [],
    warningFindings: hiddenKnowledge.length > 0
      ? ['Hidden codex notes were intentionally withheld from the reader-facing bundle.']
      : [],
  };
}

export function buildGuideBuildBundleInput(
  project: ProjectManifest,
  blueprint: EditorialBlueprint,
  preview: PreviewPackage,
  visibleKnowledgeEntities: KnowledgeEntity[],
  hiddenKnowledgeEntities: KnowledgeEntity[],
  version: number,
): Omit<GuideBuildBundle, 'id' | 'generatedAt' | 'validationState' | 'summary'> {
  const sortedVisibleKnowledge = sortKnowledge(visibleKnowledgeEntities);
  const sortedHiddenKnowledge = sortKnowledge(hiddenKnowledgeEntities);
  const walkthroughUnits = blueprint.chapterPlans.map((chapter, chapterIndex) =>
    buildWalkthroughUnit(project, blueprint, chapter, chapterIndex, sortedVisibleKnowledge),
  );

  return {
    projectId: project.id,
    previewId: preview.id,
    blueprintVersion: blueprint.version,
    version,
    visibleKnowledgeIds: sortedVisibleKnowledge.map((entity) => entity.id),
    hiddenCodexIds: sortedHiddenKnowledge.map((entity) => entity.id),
    units: [
      buildFrontMatterUnit(project, blueprint, preview),
      ...walkthroughUnits,
      buildReferenceFamilyUnit(sortedVisibleKnowledge, blueprint),
      buildAppendixUnit(sortedHiddenKnowledge, blueprint),
    ],
  };
}
