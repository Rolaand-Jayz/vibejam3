import { describe, expect, it } from 'vitest';
import { InMemoryProjectStorage } from '../storage/index.js';
import { ProjectService } from './project-service.js';

describe('ProjectService knowledge entities', () => {
  function createService(): ProjectService {
    const storage = new InMemoryProjectStorage();
    const service = new ProjectService(storage);
    service.initialize();
    return service;
  }

  function createProject(service: ProjectService, overrides?: Partial<Parameters<ProjectService['createProject']>[0]>): string {
    const result = service.createProject({
      gameTitle: 'Chrono Trigger',
      platform: 'SNES',
      guideIntent: 'boss order and hidden loot route',
      playerGoal: 'see every ending',
      runStyle: 'casual',
      spoilerPolicy: 'minimal',
      depthPreference: 'standard',
      sharedThemeCss: '.guide-theme-surface { --accent: #d2a25a; }',
      ...overrides,
    });

    expect(result.ok).toBe(true);
    if (!result.ok) {
      throw new Error('project creation failed unexpectedly');
    }

    return result.data.id;
  }

  function createPreviewReadyProject(service: ProjectService): string {
    const projectId = createProject(service, {
      gameTitle: 'Vagrant Story',
      platform: 'PS1',
      guideIntent: 'boss strategy notebook',
      playerGoal: 'finish the story',
      runStyle: 'optimized',
      spoilerPolicy: 'moderate',
      depthPreference: 'comprehensive',
    });

    const blueprintResult = service.createBlueprint(projectId, {
      chapterPlans: [
        { title: 'Workshop opening', purpose: 'Cover first systems and weapon basics.' },
        { title: 'Catacombs loop', purpose: 'Explain optional detours and risk/reward.' },
      ],
      terminologyRules: 'Prefer official boss names and weapon class labels.',
      styleBible: 'Use short tactical paragraphs with one caution callout per chapter.',
      crossReferencePlan: 'Tie material notes back to weapon families and break arts.',
      checklistPlan: 'Track workshop exits, key pickups, and affinity prep.',
      visualPackIntent: 'Dense reference cards with low ornament and warmer highlights.',
      spoilerGuardrails: 'Keep late dungeon names out of early summaries.',
    });

    expect(blueprintResult.ok).toBe(true);
    if (!blueprintResult.ok) {
      throw new Error('blueprint creation failed unexpectedly');
    }

    const previewResult = service.generatePreview(projectId);
    expect(previewResult.ok).toBe(true);
    if (!previewResult.ok) {
      throw new Error('preview generation failed unexpectedly');
    }

    return projectId;
  }

  it('requires provenance when the project policy demands it', () => {
    const service = createService();
    const projectId = createProject(service);

    const result = service.createKnowledgeEntity(projectId, {
      canonicalName: 'Ayla charm route',
      category: 'system',
      visibility: 'visible-guide',
      scope: 'run-specific',
      confidence: 'high',
      aliases: ['Charm path'],
      summary: 'Charm priority changes the weapon and accessory order for this route.',
      structuredAttributes: [
        { key: 'Best window', value: 'Pre-Black Omen side content' },
      ],
      sourceRefs: [],
      conflictMarkers: [],
      versionTags: ['snes'],
    });

    expect(result.ok).toBe(false);
    if (result.ok) {
      throw new Error('knowledge creation should fail without provenance');
    }

    expect(result.error).toMatch(/provenance|source/i);
  });

  it('creates structured knowledge and advances intake projects into knowledge-building', () => {
    const service = createService();
    const projectId = createProject(service, {
      gameTitle: 'Final Fantasy VI',
      platform: 'SNES',
      guideIntent: 'balanced route',
      playerGoal: 'finish the story',
    });

    const result = service.createKnowledgeEntity(projectId, {
      canonicalName: 'Zozo clock clue',
      category: 'quest',
      visibility: 'visible-guide',
      scope: 'broad',
      confidence: 'medium',
      aliases: ['Clock puzzle'],
      summary: 'The Zozo clock puzzle unlocks the story-critical route forward once the clue text is decoded correctly.',
      structuredAttributes: [
        { key: 'Correct answer', value: '6:10:50' },
        { key: 'Gating value', value: 'Required for Terra branch progress' },
      ],
      sourceRefs: [
        {
          sourceType: 'manual-entry',
          sourceTitle: 'SNES playthrough notes',
          sourceUri: 'https://example.com/ff6-notes',
          excerpt: 'Clock clue leads to the correct setting once the merchant hints are combined.',
          retrievalDate: '2026-04-22T00:00:00.000Z',
          trustClassification: 'community-verified',
        },
      ],
      conflictMarkers: [
        {
          conflictSummary: 'Some fan notes swap the minute and second hands when paraphrasing the clue.',
          conflictType: 'instruction-ambiguity',
          severity: 'warning',
          resolutionStatus: 'open',
          recommendedHandling: 'Quote the final solved time exactly instead of paraphrasing the hint chain.',
        },
      ],
      versionTags: ['snes', 'pixel-remaster'],
    });

    expect(result.ok).toBe(true);
    if (!result.ok) {
      throw new Error('knowledge creation failed unexpectedly');
    }

    expect(result.data.visibility).toBe('visible-guide');
    expect(result.data.sourceRefs).toHaveLength(1);
    expect(result.data.conflictMarkers).toHaveLength(1);

    const project = service.getProject(projectId);
    expect(project.ok).toBe(true);
    if (!project.ok) {
      throw new Error('project lookup failed unexpectedly');
    }
    expect(project.data.projectState).toBe('knowledge-building');

    const entities = service.listKnowledgeEntities(projectId);
    expect(entities).toHaveLength(1);
    expect(entities[0]?.canonicalName).toBe('Zozo clock clue');
  });

  it('updates hidden-codex knowledge without regressing preview-ready projects', () => {
    const service = createService();
    const projectId = createPreviewReadyProject(service);

    const created = service.createKnowledgeEntity(projectId, {
      canonicalName: 'Dragon reach breakpoint',
      category: 'system',
      visibility: 'hidden-codex',
      scope: 'run-specific',
      confidence: 'medium',
      aliases: ['Break arts threshold'],
      summary: 'Hidden codex note tracking when dragon-affinity reach starts to outscale generic safety setups.',
      structuredAttributes: [
        { key: 'Risk profile', value: 'High if gear affinity lags behind' },
      ],
      sourceRefs: [
        {
          sourceType: 'manual-entry',
          sourceTitle: 'Workshop test file',
          sourceUri: '',
          excerpt: 'Damage stability changes once dragon reach and affinity prep line up.',
          retrievalDate: '2026-04-22T00:00:00.000Z',
          trustClassification: 'working-notes',
        },
      ],
      conflictMarkers: [],
      versionTags: ['ps1'],
    });

    expect(created.ok).toBe(true);
    if (!created.ok) {
      throw new Error('knowledge creation failed unexpectedly');
    }

    const updated = service.updateKnowledgeEntity(created.data.id, {
      canonicalName: 'Dragon reach breakpoint',
      category: 'system',
      visibility: 'hidden-codex',
      scope: 'run-specific',
      confidence: 'high',
      aliases: ['Break arts threshold', 'Dragon affinity breakpoint'],
      summary: 'Hidden codex note confirming the safer breakpoint for dragon reach scaling after affinity prep is locked in.',
      structuredAttributes: [
        { key: 'Risk profile', value: 'Stable once affinity prep is complete' },
        { key: 'Guide usage', value: 'Reference only; do not surface in spoiler-light chapter copy' },
      ],
      sourceRefs: [
        {
          sourceType: 'manual-entry',
          sourceTitle: 'Workshop test file',
          sourceUri: '',
          excerpt: 'Confirmed breakpoint after repeated affinity setup runs.',
          retrievalDate: '2026-04-22T00:00:00.000Z',
          trustClassification: 'working-notes',
        },
      ],
      conflictMarkers: [
        {
          conflictSummary: 'Older community notes cite the breakpoint one chapter earlier.',
          conflictType: 'timing-disagreement',
          severity: 'warning',
          resolutionStatus: 'monitoring',
          recommendedHandling: 'Keep this codex-only until the route note is validated against another clean save.',
        },
      ],
      versionTags: ['ps1', 'route-b'],
    });

    expect(updated.ok).toBe(true);
    if (!updated.ok) {
      throw new Error('knowledge update failed unexpectedly');
    }

    expect(updated.data.confidence).toBe('high');
    expect(updated.data.conflictMarkers).toHaveLength(1);

    const project = service.getProject(projectId);
    expect(project.ok).toBe(true);
    if (!project.ok) {
      throw new Error('project lookup failed unexpectedly');
    }
    expect(project.data.projectState).toBe('preview-ready');

    const entities = service.listKnowledgeEntities(projectId);
    expect(entities).toHaveLength(1);
    expect(entities[0]?.visibility).toBe('hidden-codex');
    expect(entities[0]?.aliases).toContain('Dragon affinity breakpoint');
  });
});
