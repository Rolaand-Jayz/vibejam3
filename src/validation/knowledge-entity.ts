import type { KnowledgeAttribute, KnowledgeEntity } from '../domain/index.js';

export type KnowledgeAttributeParseResult =
  | { ok: true; data: KnowledgeAttribute[] }
  | { ok: false; error: string };

export function parseKnowledgeListText(rawText: string): string[] {
  return rawText
    .split(/[\n,]/g)
    .map((entry) => entry.trim())
    .filter((entry, index, values) => entry.length > 0 && values.indexOf(entry) === index);
}

export function formatKnowledgeListText(values: string[]): string {
  return values.join('\n');
}

export function parseKnowledgeAttributeText(rawText: string): KnowledgeAttributeParseResult {
  const lines = rawText
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line.length > 0);

  if (lines.length === 0) {
    return { ok: true, data: [] };
  }

  const attributes: KnowledgeAttribute[] = [];
  for (const [index, line] of lines.entries()) {
    const separatorIndex = line.includes('::') ? line.indexOf('::') : line.indexOf(':');
    if (separatorIndex === -1) {
      return {
        ok: false,
        error: `Knowledge attribute line ${index + 1} must use “Key :: Value”.`,
      };
    }

    const key = line.slice(0, separatorIndex).trim();
    const value = line.slice(separatorIndex + (line.includes('::') ? 2 : 1)).trim();
    if (!key || !value) {
      return {
        ok: false,
        error: `Knowledge attribute line ${index + 1} needs both a key and a value.`,
      };
    }

    attributes.push({ key, value });
  }

  return { ok: true, data: attributes };
}

export function formatKnowledgeAttributeText(attributes: KnowledgeAttribute[]): string {
  return attributes.map((attribute) => `${attribute.key} :: ${attribute.value}`).join('\n');
}

export function knowledgeEntitySummary(entity: KnowledgeEntity): string {
  const sourceCount = entity.sourceRefs.length;
  const conflictCount = entity.conflictMarkers.length;
  return `${entity.visibility.replace(/-/g, ' ')} • ${sourceCount} source${sourceCount === 1 ? '' : 's'} • ${conflictCount} conflict${conflictCount === 1 ? '' : 's'} • ${entity.confidence}`;
}
