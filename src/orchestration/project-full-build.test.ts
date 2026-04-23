import { describe, expect, it } from 'vitest';
import { InMemoryProjectStorage } from '../storage/index.js';
import { ProjectService } from './project-service.js';

describe('ProjectService full build bundles', () => {
  function createService(): ProjectService {
    const storage = new InMemoryProjectStorage();
    const service = new ProjectService(storage);
    service.initialize();
    return service;
  }

  function createApprovedPreviewProject(service: ProjectService): string {
    const projectResult = service.createProject({
      gameTitle: 'Final Fantasy VI',
      platform: 'SNES',
      guideIntent: 'balanced route with trusted reference notes',
      playerGoal: 'finish the story and keep key missables safe',
      runStyle: 'casual',
      spoilerPolicy: 'minimal',
      depthPreference: 'standard',
      sharedThemeCss: '.guide-theme-surface { --accent: #d6a55f; }',
    });

    expect(projectResult.ok).toBe(true);
    if (!projectResult.ok) {
      throw new Error('project creation failed unexpectedly');
    }

    const blueprintResult = service.createBlueprint(projectResult.data.id, {
      chapterPlans: [
        { title: 'Opening route', purpose: 'Establish the early route, setup, and irreversible warnings.' },
        { title: 'World of Balance core path', purpose: 'Lay out the mid-game route and party checkpoints.' },
        { title: 'World of Ruin cleanup', purpose: 'Track optional reunions and final preparation.' },
      ],
      terminologyRules: 'Use official location, boss, and esper names.',
      styleBible: 'Keep the tone calm, tactical, and book-like.',
      crossReferencePlan: 'Tie route warnings back to reference notes and missable callouts.',
      checklistPlan: 'End each chapter with a short irreversible-check list.',
      visualPackIntent: 'Warm field-manual cards with compact tactical density.',
      spoilerGuardrails: 'Keep plot reveals out of headings and route summaries.',
    });

    expect(blueprintResult.ok).toBe(true);
    if (!blueprintResult.ok) {
      throw new Error('blueprint creation failed unexpectedly');
    }

    const visibleKnowledge = service.createKnowledgeEntity(projectResult.data.id, {
      canonicalName: 'Zozo clock clue',
      category: 'quest',
      visibility: 'visible-guide',
      scope: 'broad',
      confidence: 'high',
      aliases: ['Clock puzzle'],
      summary: 'The Zozo clock puzzle should be quoted with the solved time instead of paraphrasing the merchant clue chain.',
      structuredAttributes: [
        { key: 'Correct answer', value: '6:10:50' },
        { key: 'Guide usage', value: 'Visible route note and missable checkpoint callout' },
      ],
      sourceRefs: [
        {
          sourceType: 'manual-entry',
          sourceTitle: 'SNES route notebook',
          sourceUri: 'https://example.com/ff6-route-notes',
          excerpt: 'Clock clue resolves to 6:10:50 once the merchant hints are combined correctly.',
          retrievalDate: new Date().toISOString(),
          trustClassification: 'community-verified',
        },
      ],
      versionTags: ['snes'],
    });

    expect(visibleKnowledge.ok).toBe(true);
    if (!visibleKnowledge.ok) {
      throw new Error('visible knowledge creation failed unexpectedly');
    }

    const hiddenKnowledge = service.createKnowledgeEntity(projectResult.data.id, {
      canonicalName: 'Sketch glitch caution',
      category: 'system',
      visibility: 'hidden-codex',
      scope: 'run-specific',
      confidence: 'medium',
      aliases: ['Sketch exploit'],
      summary: 'Hidden codex note about unstable sketch behavior that should stay out of visible guide copy.',
      structuredAttributes: [
        { key: 'Risk', value: 'Potential save corruption on some versions' },
      ],
      sourceRefs: [
        {
          sourceType: 'manual-entry',
          sourceTitle: 'Internal safety notes',
          sourceUri: '',
          excerpt: 'Keep this in the hidden codex until a clean repro path is documented.',
          retrievalDate: new Date().toISOString(),
          trustClassification: 'working-notes',
        },
      ],
      versionTags: ['snes'],
    });

    expect(hiddenKnowledge.ok).toBe(true);
    if (!hiddenKnowledge.ok) {
      throw new Error('hidden knowledge creation failed unexpectedly');
    }

    const previewResult = service.generatePreview(projectResult.data.id);
    expect(previewResult.ok).toBe(true);
    if (!previewResult.ok) {
      throw new Error('preview generation failed unexpectedly');
    }

    const approvePreview = service.reviewPreview(previewResult.data.id, {
      approvalState: 'approved',
      reviewNotes: 'Representative enough to continue into the first bounded full build.',
    });

    expect(approvePreview.ok).toBe(true);
    if (!approvePreview.ok) {
      throw new Error('preview approval failed unexpectedly');
    }

    return projectResult.data.id;
  }

  it('refuses full build until the latest preview is explicitly approved', () => {
    const service = createService();
    const projectId = createApprovedPreviewProject(service);

    const latestPreview = service.listPreviewPackages(projectId)[0];
    expect(latestPreview).toBeDefined();
    if (!latestPreview) {
      throw new Error('latest preview was unexpectedly missing');
    }

    const rewindPreview = service.reviewPreview(latestPreview.id, {
      approvalState: 'changes-requested',
      reviewNotes: 'Need one more pass before the full build should be allowed.',
    });
    expect(rewindPreview.ok).toBe(true);
    if (!rewindPreview.ok) {
      throw new Error('preview rewind failed unexpectedly');
    }

    const fullBuildResult = service.runFullBuild(projectId);
    expect(fullBuildResult.ok).toBe(false);
    if (fullBuildResult.ok) {
      throw new Error('full build should not succeed without an approved preview');
    }

    expect(fullBuildResult.error).toMatch(/preview/i);
  });

  it('creates a durable build bundle, validation report, and reviewable job when the preview is approved', () => {
    const service = createService();
    const projectId = createApprovedPreviewProject(service);

    const buildResult = service.runFullBuild(projectId);
    expect(buildResult.ok).toBe(true);
    if (!buildResult.ok) {
      throw new Error('full build failed unexpectedly');
    }

    expect(buildResult.data.bundle.version).toBe(1);
    expect(buildResult.data.bundle.units.length).toBeGreaterThanOrEqual(5);
    expect(buildResult.data.bundle.hiddenCodexIds.length).toBe(1);
    expect(buildResult.data.job.jobType).toBe('full-build');
    expect(buildResult.data.job.status).toBe('requires-review');
    expect(buildResult.data.report.blockingCount).toBe(0);
    expect(buildResult.data.report.warningCount).toBeGreaterThanOrEqual(1);

    const bundles = service.listGuideBuildBundles(projectId);
    expect(bundles).toHaveLength(1);
    expect(bundles[0]?.validationState).toBe('freeze-pending');

    const bundleText = bundles[0]?.units.map((unit) => unit.body).join('\n\n') ?? '';
    expect(bundleText).toContain('Zozo clock clue');
    expect(bundleText).not.toContain('Sketch glitch caution');

    const updatedProject = service.getProject(projectId);
    expect(updatedProject.ok).toBe(true);
    if (!updatedProject.ok) {
      throw new Error('project lookup failed unexpectedly');
    }
    expect(updatedProject.data.projectState).toBe('freeze-pending');
  });

  it('marks the project validation-failed when visible guide knowledge still carries an unresolved blocking conflict', () => {
    const service = createService();
    const projectId = createApprovedPreviewProject(service);

    const visibleKnowledge = service.listKnowledgeEntities(projectId).find((entity) => entity.visibility === 'visible-guide');
    expect(visibleKnowledge).toBeDefined();
    if (!visibleKnowledge) {
      throw new Error('visible knowledge was unexpectedly missing');
    }

    const updateResult = service.updateKnowledgeEntity(visibleKnowledge.id, {
      canonicalName: visibleKnowledge.canonicalName,
      category: visibleKnowledge.category,
      visibility: visibleKnowledge.visibility,
      scope: visibleKnowledge.scope,
      confidence: visibleKnowledge.confidence,
      aliases: visibleKnowledge.aliases,
      summary: visibleKnowledge.summary,
      structuredAttributes: visibleKnowledge.structuredAttributes,
      sourceRefs: visibleKnowledge.sourceRefs.map((sourceRef) => ({
        sourceType: sourceRef.sourceType,
        sourceTitle: sourceRef.sourceTitle,
        sourceUri: sourceRef.sourceUri,
        excerpt: sourceRef.excerpt,
        retrievalDate: sourceRef.retrievalDate,
        trustClassification: sourceRef.trustClassification,
      })),
      conflictMarkers: [
        {
          conflictSummary: 'Two route notes disagree on whether the clock solution should be delayed until after the first detour.',
          conflictType: 'route-order-disagreement',
          severity: 'blocking',
          resolutionStatus: 'open',
          recommendedHandling: 'Do not freeze the guide until the route order is re-verified against a clean save.',
        },
      ],
      versionTags: visibleKnowledge.versionTags,
    });

    expect(updateResult.ok).toBe(true);
    if (!updateResult.ok) {
      throw new Error('knowledge update failed unexpectedly');
    }

    const buildResult = service.runFullBuild(projectId);
    expect(buildResult.ok).toBe(true);
    if (!buildResult.ok) {
      throw new Error('full build failed unexpectedly');
    }

    expect(buildResult.data.job.status).toBe('blocked');
    expect(buildResult.data.report.blockingCount).toBe(1);
    expect(buildResult.data.bundle.validationState).toBe('validation-failed');

    const updatedProject = service.getProject(projectId);
    expect(updatedProject.ok).toBe(true);
    if (!updatedProject.ok) {
      throw new Error('project lookup failed unexpectedly');
    }
    expect(updatedProject.data.projectState).toBe('validation-failed');
  });
});
