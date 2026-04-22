import type { BlueprintChapterPlan, EditorialBlueprint } from '../domain/index.js';

export type ChapterPlanParseResult =
  | { ok: true; data: BlueprintChapterPlan[] }
  | { ok: false; error: string };

export function parseChapterPlanText(rawText: string): ChapterPlanParseResult {
  const lines = rawText
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line.length > 0);

  if (lines.length === 0) {
    return {
      ok: false,
      error: 'Add at least one chapter plan line using “Chapter title :: purpose”.',
    };
  }

  const chapterPlans: BlueprintChapterPlan[] = [];

  for (let index = 0; index < lines.length; index += 1) {
    const line = lines[index];
    if (line === undefined) {
      continue;
    }

    const separatorIndex = line.indexOf('::');
    if (separatorIndex === -1) {
      return {
        ok: false,
        error: `Chapter plan line ${index + 1} must use “Chapter title :: purpose”.`,
      };
    }

    const title = line.slice(0, separatorIndex).trim();
    const purpose = line.slice(separatorIndex + 2).trim();
    if (!title || !purpose) {
      return {
        ok: false,
        error: `Chapter plan line ${index + 1} needs both a title and a purpose.`,
      };
    }

    chapterPlans.push({ title, purpose });
  }

  return { ok: true, data: chapterPlans };
}

export function formatChapterPlanText(chapterPlans: BlueprintChapterPlan[]): string {
  return chapterPlans.map((chapter) => `${chapter.title} :: ${chapter.purpose}`).join('\n');
}

export function blueprintSummary(blueprint: EditorialBlueprint | null): string {
  if (!blueprint) {
    return 'No editorial blueprint drafted yet.';
  }

  return `v${blueprint.version} · ${blueprint.chapterPlans.length} chapter plan${blueprint.chapterPlans.length === 1 ? '' : 's'}`;
}
