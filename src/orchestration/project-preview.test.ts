import { describe, expect, it } from 'vitest';
import { InMemoryProjectStorage } from '../storage/index.js';
import { ProjectService } from './project-service.js';

describe('ProjectService preview packages', () => {
  function createService(): ProjectService {
    const storage = new InMemoryProjectStorage();
    const service = new ProjectService(storage);
    service.initialize();
    return service;
  }

  function createBlueprintReadyProject(service: ProjectService): string {
    const projectResult = service.createProject({
      gameTitle: 'Chrono Trigger',
      platform: 'SNES',
      guideIntent: 'boss order and hidden loot route',
      playerGoal: 'see every ending',
      runStyle: 'casual',
      spoilerPolicy: 'minimal',
      depthPreference: 'standard',
      sharedThemeCss: '.guide-theme-surface { --accent: #d2a25a; }',
    });

    expect(projectResult.ok).toBe(true);
    if (!projectResult.ok) {
      throw new Error('project creation failed unexpectedly');
    }

    const blueprintResult = service.createBlueprint(projectResult.data.id, {
      chapterPlans: [
        { title: 'Opening route', purpose: 'Establish the first route locks and early warnings.' },
        { title: 'Core progression', purpose: 'Lay out the main route, power spikes, and detours.' },
        { title: 'Cleanup', purpose: 'Track missables and ending requirements.' },
      ],
      terminologyRules: 'Use official boss, location, and item names.',
      styleBible: 'Keep the tone calm, tactical, and spoiler-aware.',
      crossReferencePlan: 'Tie route warnings back to optional detours and reference notes.',
      checklistPlan: 'End each chapter with a short irreversible-check list.',
      visualPackIntent: 'Warm field-manual cards with restrained accents.',
      spoilerGuardrails: 'Keep plot reveals out of headings and summaries.',
    });

    expect(blueprintResult.ok).toBe(true);
    if (!blueprintResult.ok) {
      throw new Error('blueprint creation failed unexpectedly');
    }

    return projectResult.data.id;
  }

  it('refuses preview generation until a blueprint exists', () => {
    const service = createService();
    const projectResult = service.createProject({
      gameTitle: 'Final Fantasy VI',
      platform: 'SNES',
      guideIntent: 'balanced route',
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

    const previewResult = service.generatePreview(projectResult.data.id);
    expect(previewResult.ok).toBe(false);
    if (previewResult.ok) {
      throw new Error('preview generation should not succeed without a blueprint');
    }

    expect(previewResult.error).toMatch(/blueprint/i);
  });

  it('creates a durable preview package and advances the project to preview-ready', () => {
    const service = createService();
    const projectId = createBlueprintReadyProject(service);

    const previewResult = service.generatePreview(projectId);
    expect(previewResult.ok).toBe(true);
    if (!previewResult.ok) {
      throw new Error('preview generation failed unexpectedly');
    }

    expect(previewResult.data.version).toBe(1);
    expect(previewResult.data.approvalState).toBe('requires-review');
    expect(previewResult.data.sampleSections.length).toBeGreaterThanOrEqual(4);

    const previews = service.listPreviewPackages(projectId);
    expect(previews).toHaveLength(1);
    expect(previews[0]?.approvalState).toBe('requires-review');

    const updatedProject = service.getProject(projectId);
    expect(updatedProject.ok).toBe(true);
    if (!updatedProject.ok) {
      throw new Error('project lookup failed unexpectedly');
    }

    expect(updatedProject.data.projectState).toBe('preview-ready');
  });

  it('records explicit preview review decisions and versioned regeneration', () => {
    const service = createService();
    const projectId = createBlueprintReadyProject(service);

    const firstPreview = service.generatePreview(projectId);
    expect(firstPreview.ok).toBe(true);
    if (!firstPreview.ok) {
      throw new Error('first preview generation failed unexpectedly');
    }

    const requestChanges = service.reviewPreview(firstPreview.data.id, {
      approvalState: 'changes-requested',
      reviewNotes: 'The route sample needs denser checkpoint language before full build.',
    });
    expect(requestChanges.ok).toBe(true);
    if (!requestChanges.ok) {
      throw new Error('preview review failed unexpectedly');
    }

    expect(requestChanges.data.approvalState).toBe('changes-requested');
    expect(requestChanges.data.reviewNotes).toMatch(/checkpoint language/i);

    const changedProject = service.getProject(projectId);
    expect(changedProject.ok).toBe(true);
    if (!changedProject.ok) {
      throw new Error('project lookup failed unexpectedly');
    }
    expect(changedProject.data.projectState).toBe('revision-requested');

    const secondPreview = service.generatePreview(projectId);
    expect(secondPreview.ok).toBe(true);
    if (!secondPreview.ok) {
      throw new Error('second preview generation failed unexpectedly');
    }
    expect(secondPreview.data.version).toBe(2);

    const approvePreview = service.reviewPreview(secondPreview.data.id, {
      approvalState: 'approved',
      reviewNotes: 'This preview is representative enough to move forward later.',
    });
    expect(approvePreview.ok).toBe(true);
    if (!approvePreview.ok) {
      throw new Error('preview approval failed unexpectedly');
    }
    expect(approvePreview.data.approvalState).toBe('approved');

    const approvedProject = service.getProject(projectId);
    expect(approvedProject.ok).toBe(true);
    if (!approvedProject.ok) {
      throw new Error('project lookup failed unexpectedly');
    }
    expect(approvedProject.data.projectState).toBe('preview-ready');

    const previews = service.listPreviewPackages(projectId);
    expect(previews.map((entry) => entry.version)).toEqual([2, 1]);
    expect(previews[0]?.approvalState).toBe('approved');
  });
});
