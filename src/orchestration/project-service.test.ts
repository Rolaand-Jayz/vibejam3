import { describe, expect, it } from 'vitest';
import { InMemoryProjectStorage } from '../storage/index.js';
import { ProjectService } from './project-service.js';

describe('ProjectService editorial blueprints', () => {
  function createService(): ProjectService {
    const storage = new InMemoryProjectStorage();
    const service = new ProjectService(storage);
    service.initialize();
    return service;
  }

  it('creates a first blueprint and advances the project to blueprint-ready', () => {
    const service = createService();
    const projectResult = service.createProject({
      gameTitle: 'Chrono Cross',
      platform: 'PS1',
      guideIntent: 'true ending route',
      playerGoal: 'recruit key characters',
      runStyle: 'optimized',
      spoilerPolicy: 'moderate',
      depthPreference: 'comprehensive',
      sharedThemeCss: '.guide-theme-surface { --accent: #d4ab68; }',
    });

    expect(projectResult.ok).toBe(true);
    if (!projectResult.ok) {
      throw new Error('project creation failed unexpectedly');
    }

    const blueprintResult = service.createBlueprint(projectResult.data.id, {
      chapterPlans: [
        { title: 'Opening route', purpose: 'Set expectations and early missables.' },
        { title: 'Branch logic', purpose: 'Explain recruit splits and route commitments.' },
      ],
      terminologyRules: 'Use location names exactly as they appear in-game.',
      styleBible: 'Keep tactical guidance compact, calm, and spoiler-aware.',
      crossReferencePlan: 'Link recruit notes back to route commitment callouts.',
      checklistPlan: 'End each chapter with irreversible checks.',
      visualPackIntent: 'Warm field-manual cards with restrained emphasis.',
      spoilerGuardrails: 'Avoid late-game identity reveals in headings.',
    });

    expect(blueprintResult.ok).toBe(true);
    if (!blueprintResult.ok) {
      throw new Error('blueprint creation failed unexpectedly');
    }

    expect(blueprintResult.data.version).toBe(1);
    expect(blueprintResult.data.chapterPlans).toHaveLength(2);

    const updatedProject = service.getProject(projectResult.data.id);
    expect(updatedProject.ok).toBe(true);
    if (!updatedProject.ok) {
      throw new Error('project lookup failed unexpectedly');
    }

    expect(updatedProject.data.projectState).toBe('blueprint-ready');

    const blueprints = service.listBlueprints(projectResult.data.id);
    expect(blueprints).toHaveLength(1);
    expect(blueprints[0]?.version).toBe(1);
  });

  it('versions later blueprints newest-first per project', () => {
    const service = createService();
    const projectResult = service.createProject({
      gameTitle: 'Vagrant Story',
      platform: 'PS1',
      guideIntent: 'boss strategy notebook',
      playerGoal: 'finish the story',
      runStyle: 'casual',
      spoilerPolicy: 'minimal',
      depthPreference: 'standard',
      sharedThemeCss: '',
    });

    expect(projectResult.ok).toBe(true);
    if (!projectResult.ok) {
      throw new Error('project creation failed unexpectedly');
    }

    const first = service.createBlueprint(projectResult.data.id, {
      chapterPlans: [{ title: 'Workshop opening', purpose: 'Cover first systems and weapon basics.' }],
      terminologyRules: 'Prefer official boss names.',
      styleBible: 'Use short tactical paragraphs.',
      crossReferencePlan: 'Tie material notes back to weapon families.',
      checklistPlan: 'Track workshop exits and key pickups.',
      visualPackIntent: 'Dense reference cards with low ornament.',
      spoilerGuardrails: 'Keep late dungeon names out of early summaries.',
    });
    expect(first.ok).toBe(true);

    const second = service.createBlueprint(projectResult.data.id, {
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
    expect(second.ok).toBe(true);
    if (!second.ok) {
      throw new Error('second blueprint creation failed unexpectedly');
    }

    const blueprints = service.listBlueprints(projectResult.data.id);
    expect(blueprints).toHaveLength(2);
    expect(blueprints.map((entry) => entry.version)).toEqual([2, 1]);
    expect(blueprints[0]?.chapterPlans).toHaveLength(2);
  });
});
