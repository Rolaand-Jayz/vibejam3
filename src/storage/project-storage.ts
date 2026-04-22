import type { BuildJob, EditorialBlueprint, PreviewPackage, ProjectManifest, ValidationReport } from '../domain/index.js';
import {
  BuildJobSchema,
  EditorialBlueprintSchema,
  PreviewPackageSchema,
  ProjectManifestSchema,
  ValidationReportSchema,
} from '../domain/index.js';

interface StorageSnapshot {
  projects: ProjectManifest[];
  blueprints: EditorialBlueprint[];
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

    const jobs = Array.isArray(parsed.jobs)
      ? parsed.jobs
          .map((item) => BuildJobSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const blueprints = Array.isArray(parsed.blueprints)
      ? parsed.blueprints
          .map((item) => EditorialBlueprintSchema.safeParse(item))
          .filter((result) => result.success)
          .map((result) => result.data)
      : [];

    const previews = Array.isArray(parsed.previews)
      ? parsed.previews
          .map((item) => PreviewPackageSchema.safeParse(item))
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
      previews: Array.from(this.previews.values()),
      jobs: Array.from(this.jobs.values()),
      validationReports: Array.from(this.validationReports.values()),
    };
  }

  protected hydrate(snapshot: StorageSnapshot): void {
    this.projects = new Map(snapshot.projects.map((project) => [project.id, project]));
    this.blueprints = new Map(snapshot.blueprints.map((blueprint) => [blueprint.id, blueprint]));
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

    for (const preview of Array.from(this.previews.values())) {
      if (preview.projectId === id) {
        this.previews.delete(preview.id);
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
