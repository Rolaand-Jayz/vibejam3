import type {
  BuildJob,
  EditorialBlueprint,
  GuideBuildBundle,
  KnowledgeEntity,
  PreviewPackage,
  ProjectManifest,
  ValidationReport,
} from '../domain/index.js';
import {
  BuildJobSchema,
  EditorialBlueprintSchema,
  GuideBuildBundleSchema,
  KnowledgeEntitySchema,
  PreviewPackageSchema,
  ProjectManifestSchema,
  ValidationReportSchema,
} from '../domain/index.js';

interface StorageSnapshot {
  projects: ProjectManifest[];
  blueprints: EditorialBlueprint[];
  buildBundles: GuideBuildBundle[];
  knowledgeEntities: KnowledgeEntity[];
  previews: PreviewPackage[];
  jobs: BuildJob[];
  validationReports: ValidationReport[];
}

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

function createEmptySnapshot(): StorageSnapshot {
  return {
    projects: [],
    blueprints: [],
    buildBundles: [],
    knowledgeEntities: [],
    previews: [],
    jobs: [],
    validationReports: [],
  };
}

function browserStorage(): Storage | null {
  if (typeof window === 'undefined') {
    return null;
  }

  try {
    return window.localStorage;
  } catch {
    return null;
  }
}

function loadSnapshot(rawValue: string | null): StorageSnapshot {
  if (!rawValue) {
    return createEmptySnapshot();
  }

  try {
    const parsed = JSON.parse(rawValue) as {
      projects?: unknown[];
      blueprints?: unknown[];
      buildBundles?: unknown[];
      knowledgeEntities?: unknown[];
      previews?: unknown[];
      jobs?: unknown[];
      validationReports?: unknown[];
    };

    const projects = Array.isArray(parsed.projects)
      ? parsed.projects
          .map((item) => ProjectManifestSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const blueprints = Array.isArray(parsed.blueprints)
      ? parsed.blueprints
          .map((item) => EditorialBlueprintSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const buildBundles = Array.isArray(parsed.buildBundles)
      ? parsed.buildBundles
          .map((item) => GuideBuildBundleSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const knowledgeEntities = Array.isArray(parsed.knowledgeEntities)
      ? parsed.knowledgeEntities
          .map((item) => KnowledgeEntitySchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const previews = Array.isArray(parsed.previews)
      ? parsed.previews
          .map((item) => PreviewPackageSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const jobs = Array.isArray(parsed.jobs)
      ? parsed.jobs
          .map((item) => BuildJobSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const validationReports = Array.isArray(parsed.validationReports)
      ? parsed.validationReports
          .map((item) => ValidationReportSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    return {
      projects,
      blueprints,
      buildBundles,
      knowledgeEntities,
      previews,
      jobs,
      validationReports,
    };
  } catch {
    return createEmptySnapshot();
  }
}

/**
 * Storage adapter interface.
 * Defines the contract any storage backend must fulfill.
 * PRD reference: COPILOT-PRD-vibe1/08-storage-and-infrastructure-strategy.md
 */
export interface ProjectStorage {
  initialize(): void;
  createProject(manifest: Omit<ProjectManifest, 'id' | 'createdAt'>): ProjectManifest;
  getProject(id: string): ProjectManifest | null;
  listProjects(): ProjectManifest[];
  updateProject(id: string, patch: Partial<ProjectManifest>): ProjectManifest | null;
  deleteProject(id: string): boolean;
  createBlueprint(blueprint: Omit<EditorialBlueprint, 'id' | 'createdAt' | 'updatedAt'>): EditorialBlueprint;
  listBlueprints(projectId: string): EditorialBlueprint[];
  listGuideBuildBundles(projectId: string): GuideBuildBundle[];
  saveGuideBuildBundle(bundle: GuideBuildBundle): GuideBuildBundle;
  createKnowledgeEntity(entity: Omit<KnowledgeEntity, 'id' | 'createdAt' | 'updatedAt'>): KnowledgeEntity;
  getKnowledgeEntity(id: string): KnowledgeEntity | null;
  updateKnowledgeEntity(id: string, patch: Partial<KnowledgeEntity>): KnowledgeEntity | null;
  listKnowledgeEntities(projectId: string): KnowledgeEntity[];
  createPreview(preview: Omit<PreviewPackage, 'id' | 'generatedAt' | 'updatedAt'>): PreviewPackage;
  getPreview(id: string): PreviewPackage | null;
  updatePreview(id: string, patch: Partial<PreviewPackage>): PreviewPackage | null;
  listPreviewPackages(projectId: string): PreviewPackage[];
  listJobs(projectId: string): BuildJob[];
  saveJob(job: BuildJob): BuildJob;
  listValidationReports(projectId: string): ValidationReport[];
  saveValidationReport(report: ValidationReport): ValidationReport;
  shutdown(): void;
}

/**
 * In-memory storage for testing and isolated execution.
 */
export class InMemoryProjectStorage implements ProjectStorage {
  protected projects = new Map<string, ProjectManifest>();
  protected blueprints = new Map<string, EditorialBlueprint>();
  protected buildBundles = new Map<string, GuideBuildBundle>();
  protected knowledgeEntities = new Map<string, KnowledgeEntity>();
  protected previews = new Map<string, PreviewPackage>();
  protected jobs = new Map<string, BuildJob>();
  protected validationReports = new Map<string, ValidationReport>();

  initialize(): void {
    // No-op for in-memory storage
  }

  protected snapshot(): StorageSnapshot {
    return {
      projects: Array.from(this.projects.values()),
      blueprints: Array.from(this.blueprints.values()),
      buildBundles: Array.from(this.buildBundles.values()),
      knowledgeEntities: Array.from(this.knowledgeEntities.values()),
      previews: Array.from(this.previews.values()),
      jobs: Array.from(this.jobs.values()),
      validationReports: Array.from(this.validationReports.values()),
    };
  }

  protected hydrate(snapshot: StorageSnapshot): void {
    this.projects = new Map(snapshot.projects.map((project) => [project.id, project]));
    this.blueprints = new Map(snapshot.blueprints.map((blueprint) => [blueprint.id, blueprint]));
    this.buildBundles = new Map(snapshot.buildBundles.map((bundle) => [bundle.id, bundle]));
    this.knowledgeEntities = new Map(snapshot.knowledgeEntities.map((entity) => [entity.id, entity]));
    this.previews = new Map(snapshot.previews.map((preview) => [preview.id, preview]));
    this.jobs = new Map(snapshot.jobs.map((job) => [job.id, job]));
    this.validationReports = new Map(snapshot.validationReports.map((report) => [report.id, report]));
  }

  protected persist(): void {
    // No-op for in-memory storage
  }

  createProject(input: Omit<ProjectManifest, 'id' | 'createdAt'>): ProjectManifest {
    const manifest: ProjectManifest = {
      ...input,
      id: generateUUID(),
      createdAt: new Date().toISOString(),
    };

    const validated = ProjectManifestSchema.parse(manifest);
    this.projects.set(validated.id, validated);
    this.persist();
    return validated;
  }

  getProject(id: string): ProjectManifest | null {
    return this.projects.get(id) ?? null;
  }

  listProjects(): ProjectManifest[] {
    return Array.from(this.projects.values()).sort(
      (a, b) => new Date(b.createdAt).getTime() - new Date(a.createdAt).getTime(),
    );
  }

  updateProject(id: string, patch: Partial<ProjectManifest>): ProjectManifest | null {
    const existing = this.projects.get(id);
    if (!existing) {
      return null;
    }

    const updated = ProjectManifestSchema.parse({
      ...existing,
      ...patch,
      id: existing.id,
      createdAt: existing.createdAt,
    });

    this.projects.set(id, updated);
    this.persist();
    return updated;
  }

  deleteProject(id: string): boolean {
    const deleted = this.projects.delete(id);
    if (!deleted) {
      return false;
    }

    for (const blueprint of Array.from(this.blueprints.values())) {
      if (blueprint.projectId === id) {
        this.blueprints.delete(blueprint.id);
      }
    }

    for (const bundle of Array.from(this.buildBundles.values())) {
      if (bundle.projectId === id) {
        this.buildBundles.delete(bundle.id);
      }
    }

    for (const preview of Array.from(this.previews.values())) {
      if (preview.projectId === id) {
        this.previews.delete(preview.id);
      }
    }

    for (const entity of Array.from(this.knowledgeEntities.values())) {
      if (entity.projectId === id) {
        this.knowledgeEntities.delete(entity.id);
      }
    }

    for (const job of Array.from(this.jobs.values())) {
      if (job.projectId === id) {
        this.jobs.delete(job.id);
      }
    }

    for (const report of Array.from(this.validationReports.values())) {
      if (report.projectId === id) {
        this.validationReports.delete(report.id);
      }
    }

    this.persist();
    return true;
  }

  createBlueprint(input: Omit<EditorialBlueprint, 'id' | 'createdAt' | 'updatedAt'>): EditorialBlueprint {
    const now = new Date().toISOString();
    const blueprint: EditorialBlueprint = {
      ...input,
      id: generateUUID(),
      createdAt: now,
      updatedAt: now,
    };

    const validated = EditorialBlueprintSchema.parse(blueprint);
    this.blueprints.set(validated.id, validated);
    this.persist();
    return validated;
  }

  listBlueprints(projectId: string): EditorialBlueprint[] {
    return Array.from(this.blueprints.values())
      .filter((blueprint) => blueprint.projectId === projectId)
      .sort((a, b) => {
        if (b.version !== a.version) {
          return b.version - a.version;
        }

        return new Date(b.updatedAt).getTime() - new Date(a.updatedAt).getTime();
      });
  }

  listGuideBuildBundles(projectId: string): GuideBuildBundle[] {
    return Array.from(this.buildBundles.values())
      .filter((bundle) => bundle.projectId === projectId)
      .sort((a, b) => {
        if (b.version !== a.version) {
          return b.version - a.version;
        }

        return new Date(b.generatedAt).getTime() - new Date(a.generatedAt).getTime();
      });
  }

  saveGuideBuildBundle(bundle: GuideBuildBundle): GuideBuildBundle {
    const validated = GuideBuildBundleSchema.parse(bundle);
    this.buildBundles.set(validated.id, validated);
    this.persist();
    return validated;
  }

  createKnowledgeEntity(input: Omit<KnowledgeEntity, 'id' | 'createdAt' | 'updatedAt'>): KnowledgeEntity {
    const now = new Date().toISOString();
    const entity: KnowledgeEntity = {
      ...input,
      id: generateUUID(),
      createdAt: now,
      updatedAt: now,
    };

    const validated = KnowledgeEntitySchema.parse(entity);
    this.knowledgeEntities.set(validated.id, validated);
    this.persist();
    return validated;
  }

  getKnowledgeEntity(id: string): KnowledgeEntity | null {
    return this.knowledgeEntities.get(id) ?? null;
  }

  updateKnowledgeEntity(id: string, patch: Partial<KnowledgeEntity>): KnowledgeEntity | null {
    const existing = this.knowledgeEntities.get(id);
    if (!existing) {
      return null;
    }

    const updated = KnowledgeEntitySchema.parse({
      ...existing,
      ...patch,
      id: existing.id,
      projectId: existing.projectId,
      createdAt: existing.createdAt,
      updatedAt: patch.updatedAt ?? new Date().toISOString(),
    });

    this.knowledgeEntities.set(id, updated);
    this.persist();
    return updated;
  }

  listKnowledgeEntities(projectId: string): KnowledgeEntity[] {
    return Array.from(this.knowledgeEntities.values())
      .filter((entity) => entity.projectId === projectId)
      .sort((a, b) => new Date(b.updatedAt).getTime() - new Date(a.updatedAt).getTime());
  }

  createPreview(input: Omit<PreviewPackage, 'id' | 'generatedAt' | 'updatedAt'>): PreviewPackage {
    const now = new Date().toISOString();
    const preview: PreviewPackage = {
      ...input,
      id: generateUUID(),
      generatedAt: now,
      updatedAt: now,
    };

    const validated = PreviewPackageSchema.parse(preview);
    this.previews.set(validated.id, validated);
    this.persist();
    return validated;
  }

  getPreview(id: string): PreviewPackage | null {
    return this.previews.get(id) ?? null;
  }

  updatePreview(id: string, patch: Partial<PreviewPackage>): PreviewPackage | null {
    const existing = this.previews.get(id);
    if (!existing) {
      return null;
    }

    const updated = PreviewPackageSchema.parse({
      ...existing,
      ...patch,
      id: existing.id,
      projectId: existing.projectId,
      generatedAt: existing.generatedAt,
      updatedAt: patch.updatedAt ?? new Date().toISOString(),
    });

    this.previews.set(id, updated);
    this.persist();
    return updated;
  }

  listPreviewPackages(projectId: string): PreviewPackage[] {
    return Array.from(this.previews.values())
      .filter((preview) => preview.projectId === projectId)
      .sort((a, b) => {
        if (b.version !== a.version) {
          return b.version - a.version;
        }

        return new Date(b.updatedAt).getTime() - new Date(a.updatedAt).getTime();
      });
  }

  listJobs(projectId: string): BuildJob[] {
    return Array.from(this.jobs.values())
      .filter((job) => job.projectId === projectId)
      .sort((a, b) => new Date(b.startedAt).getTime() - new Date(a.startedAt).getTime());
  }

  saveJob(job: BuildJob): BuildJob {
    const validated = BuildJobSchema.parse(job);
    this.jobs.set(validated.id, validated);
    this.persist();
    return validated;
  }

  listValidationReports(projectId: string): ValidationReport[] {
    return Array.from(this.validationReports.values())
      .filter((report) => report.projectId === projectId)
      .sort((a, b) => new Date(b.createdAt).getTime() - new Date(a.createdAt).getTime());
  }

  saveValidationReport(report: ValidationReport): ValidationReport {
    const validated = ValidationReportSchema.parse(report);
    this.validationReports.set(validated.id, validated);
    this.persist();
    return validated;
  }

  shutdown(): void {
    this.projects.clear();
    this.blueprints.clear();
    this.buildBundles.clear();
    this.knowledgeEntities.clear();
    this.previews.clear();
    this.jobs.clear();
    this.validationReports.clear();
  }
}

/**
 * Durable browser storage backed by localStorage.
 * Persists projects, validation jobs, and validation reports across reloads.
 */
export class LocalStorageProjectStorage extends InMemoryProjectStorage {
  constructor(private readonly storageKey = 'my-plaithrough.web-store.v1') {
    super();
  }

  override initialize(): void {
    const storage = browserStorage();
    if (storage == null) {
      return;
    }

    this.hydrate(loadSnapshot(storage.getItem(this.storageKey)));
  }

  protected override persist(): void {
    const storage = browserStorage();
    if (storage == null) {
      return;
    }

    storage.setItem(this.storageKey, JSON.stringify(this.snapshot()));
  }

  override shutdown(): void {
    // Durable browser state should survive app shutdown.
  }
}
