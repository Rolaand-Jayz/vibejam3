import Database from 'better-sqlite3';
import { randomUUID } from 'node:crypto';
import { existsSync, mkdirSync } from 'node:fs';
import { dirname, join } from 'node:path';
import type { BuildJob, EditorialBlueprint, PreviewPackage, ProjectManifest, ValidationReport } from '../domain/index.js';
import {
  BuildJobSchema,
  EditorialBlueprintSchema,
  PreviewPackageSchema,
  ProjectManifestSchema,
  ValidationReportSchema,
} from '../domain/index.js';
import type { ProjectStorage } from './project-storage.js';

interface DbRow {
  id: string;
  project_state: string | null;
  game_title: string;
  platform: string | null;
  guide_intent: string;
  player_goal: string;
  run_style: string;
  spoiler_policy: string;
  depth_preference: string;
  provider_profile: string | null;
  policy_profile: string | null;
  template_version: string | null;
  shared_theme_css: string | null;
  created_at: string;
}

interface BuildJobRow {
  id: string;
  project_id: string;
  job_type: string;
  status: string;
  progress: number;
  started_at: string;
  ended_at: string;
  human_summary: string;
  validation_report_id: string;
}

interface EditorialBlueprintRow {
  id: string;
  project_id: string;
  version: number;
  chapter_plans: string;
  terminology_rules: string;
  style_bible: string;
  cross_reference_plan: string;
  checklist_plan: string;
  visual_pack_intent: string;
  spoiler_guardrails: string;
  created_at: string;
  updated_at: string;
}

interface PreviewPackageRow {
  id: string;
  project_id: string;
  blueprint_version: number;
  version: number;
  sample_sections: string;
  review_notes: string;
  approval_state: string;
  generated_at: string;
  updated_at: string;
}

interface ValidationReportRow {
  id: string;
  project_id: string;
  target_type: string;
  target_id: string;
  checks_run: string;
  blocking_findings: string;
  warning_findings: string;
  blocking_count: number;
  warning_count: number;
  created_at: string;
  summary: string;
}

/**
 * Real SQLite-backed storage for project manifests and validation history.
 *
 * IMPORTANT: This file imports native Node.js modules (better-sqlite3, node:crypto, node:fs, node:path).
 * It CANNOT be imported from browser code. Use only from Node.js entry points (tests, CLI, Electron main).
 */
export class SqliteProjectStorage implements ProjectStorage {
  private db: Database.Database;

  constructor(dbPath?: string) {
    const resolvedPath = dbPath ?? process.env.VIBEJAM_DB_PATH ?? join(process.cwd(), 'data', 'vibejam3.db');
    const dir = dirname(resolvedPath);
    if (!existsSync(dir)) {
      mkdirSync(dir, { recursive: true });
    }
    this.db = new Database(resolvedPath);
    this.db.pragma('journal_mode = WAL');
    this.db.pragma('foreign_keys = ON');
  }

  initialize(): void {
    this.db.exec(`
      CREATE TABLE IF NOT EXISTS project_manifests (
        id TEXT PRIMARY KEY NOT NULL,
        project_state TEXT NOT NULL DEFAULT 'intake',
        game_title TEXT NOT NULL,
        platform TEXT,
        guide_intent TEXT NOT NULL,
        player_goal TEXT NOT NULL,
        run_style TEXT NOT NULL,
        spoiler_policy TEXT NOT NULL,
        depth_preference TEXT NOT NULL,
        provider_profile TEXT,
        policy_profile TEXT NOT NULL,
        template_version TEXT NOT NULL,
        shared_theme_css TEXT NOT NULL DEFAULT '',
        created_at TEXT NOT NULL
      );

      CREATE TABLE IF NOT EXISTS build_jobs (
        id TEXT PRIMARY KEY NOT NULL,
        project_id TEXT NOT NULL,
        job_type TEXT NOT NULL,
        status TEXT NOT NULL,
        progress INTEGER NOT NULL,
        started_at TEXT NOT NULL,
        ended_at TEXT NOT NULL,
        human_summary TEXT NOT NULL,
        validation_report_id TEXT NOT NULL,
        FOREIGN KEY (project_id) REFERENCES project_manifests(id) ON DELETE CASCADE
      );

      CREATE TABLE IF NOT EXISTS editorial_blueprints (
        id TEXT PRIMARY KEY NOT NULL,
        project_id TEXT NOT NULL,
        version INTEGER NOT NULL,
        chapter_plans TEXT NOT NULL,
        terminology_rules TEXT NOT NULL,
        style_bible TEXT NOT NULL,
        cross_reference_plan TEXT NOT NULL,
        checklist_plan TEXT NOT NULL,
        visual_pack_intent TEXT NOT NULL,
        spoiler_guardrails TEXT NOT NULL,
        created_at TEXT NOT NULL,
        updated_at TEXT NOT NULL,
        FOREIGN KEY (project_id) REFERENCES project_manifests(id) ON DELETE CASCADE
      );

      CREATE TABLE IF NOT EXISTS preview_packages (
        id TEXT PRIMARY KEY NOT NULL,
        project_id TEXT NOT NULL,
        blueprint_version INTEGER NOT NULL,
        version INTEGER NOT NULL,
        sample_sections TEXT NOT NULL,
        review_notes TEXT NOT NULL,
        approval_state TEXT NOT NULL,
        generated_at TEXT NOT NULL,
        updated_at TEXT NOT NULL,
        FOREIGN KEY (project_id) REFERENCES project_manifests(id) ON DELETE CASCADE
      );

      CREATE TABLE IF NOT EXISTS validation_reports (
        id TEXT PRIMARY KEY NOT NULL,
        project_id TEXT NOT NULL,
        target_type TEXT NOT NULL,
        target_id TEXT NOT NULL,
        checks_run TEXT NOT NULL,
        blocking_findings TEXT NOT NULL,
        warning_findings TEXT NOT NULL,
        blocking_count INTEGER NOT NULL,
        warning_count INTEGER NOT NULL,
        created_at TEXT NOT NULL,
        summary TEXT NOT NULL,
        FOREIGN KEY (project_id) REFERENCES project_manifests(id) ON DELETE CASCADE
      );
    `);

    this.ensureColumn('project_manifests', 'project_state', "TEXT NOT NULL DEFAULT 'intake'");
    this.ensureColumn('project_manifests', 'shared_theme_css', "TEXT NOT NULL DEFAULT ''");
  }

  createProject(input: Omit<ProjectManifest, 'id' | 'createdAt'>): ProjectManifest {
    const manifest: ProjectManifest = {
      ...input,
      id: randomUUID(),
      createdAt: new Date().toISOString(),
    };

    const validated = ProjectManifestSchema.parse(manifest);

    this.db.prepare(`
      INSERT INTO project_manifests (
        id, project_state, game_title, platform, guide_intent, player_goal, run_style,
        spoiler_policy, depth_preference, provider_profile, policy_profile,
        template_version, shared_theme_css, created_at
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `).run(
      validated.id,
      validated.projectState,
      validated.gameTitle,
      validated.platform || null,
      validated.guideIntent,
      validated.playerGoal,
      validated.runStyle,
      validated.spoilerPolicy,
      validated.depthPreference,
      validated.providerProfile ? JSON.stringify(validated.providerProfile) : null,
      JSON.stringify(validated.policyProfile),
      validated.templateVersion,
      validated.sharedThemeCss,
      validated.createdAt,
    );

    return validated;
  }

  getProject(id: string): ProjectManifest | null {
    const row = this.db.prepare(`
      SELECT * FROM project_manifests WHERE id = ?
    `).get(id) as DbRow | undefined;

    if (!row) {
      return null;
    }

    return this.rowToManifest(row);
  }

  listProjects(): ProjectManifest[] {
    const rows = this.db.prepare(`
      SELECT * FROM project_manifests ORDER BY created_at DESC
    `).all() as DbRow[];

    return rows.map((row) => this.rowToManifest(row));
  }

  updateProject(id: string, patch: Partial<ProjectManifest>): ProjectManifest | null {
    const existing = this.getProject(id);
    if (!existing) {
      return null;
    }

    const updated: ProjectManifest = {
      ...existing,
      ...patch,
      id: existing.id,
      createdAt: existing.createdAt,
    };

    const validated = ProjectManifestSchema.parse(updated);

    this.db.prepare(`
      UPDATE project_manifests SET
        project_state = ?,
        game_title = ?,
        platform = ?,
        guide_intent = ?,
        player_goal = ?,
        run_style = ?,
        spoiler_policy = ?,
        depth_preference = ?,
        provider_profile = ?,
        policy_profile = ?,
        template_version = ?,
        shared_theme_css = ?
      WHERE id = ?
    `).run(
      validated.projectState,
      validated.gameTitle,
      validated.platform || null,
      validated.guideIntent,
      validated.playerGoal,
      validated.runStyle,
      validated.spoilerPolicy,
      validated.depthPreference,
      validated.providerProfile ? JSON.stringify(validated.providerProfile) : null,
      JSON.stringify(validated.policyProfile),
      validated.templateVersion,
      validated.sharedThemeCss,
      validated.id,
    );

    return validated;
  }

  deleteProject(id: string): boolean {
    const result = this.db.prepare('DELETE FROM project_manifests WHERE id = ?').run(id);
    return result.changes > 0;
  }

  createBlueprint(input: Omit<EditorialBlueprint, 'id' | 'createdAt' | 'updatedAt'>): EditorialBlueprint {
    const now = new Date().toISOString();
    const blueprint = EditorialBlueprintSchema.parse({
      ...input,
      id: randomUUID(),
      createdAt: now,
      updatedAt: now,
    });

    this.db.prepare(`
      INSERT INTO editorial_blueprints (
        id, project_id, version, chapter_plans, terminology_rules, style_bible,
        cross_reference_plan, checklist_plan, visual_pack_intent, spoiler_guardrails,
        created_at, updated_at
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `).run(
      blueprint.id,
      blueprint.projectId,
      blueprint.version,
      JSON.stringify(blueprint.chapterPlans),
      blueprint.terminologyRules,
      blueprint.styleBible,
      blueprint.crossReferencePlan,
      blueprint.checklistPlan,
      blueprint.visualPackIntent,
      blueprint.spoilerGuardrails,
      blueprint.createdAt,
      blueprint.updatedAt,
    );

    return blueprint;
  }

  listBlueprints(projectId: string): EditorialBlueprint[] {
    const rows = this.db.prepare(`
      SELECT * FROM editorial_blueprints WHERE project_id = ? ORDER BY version DESC, updated_at DESC
    `).all(projectId) as EditorialBlueprintRow[];

    return rows.map((row) => EditorialBlueprintSchema.parse({
      id: row.id,
      projectId: row.project_id,
      version: row.version,
      chapterPlans: JSON.parse(row.chapter_plans) as EditorialBlueprint['chapterPlans'],
      terminologyRules: row.terminology_rules,
      styleBible: row.style_bible,
      crossReferencePlan: row.cross_reference_plan,
      checklistPlan: row.checklist_plan,
      visualPackIntent: row.visual_pack_intent,
      spoilerGuardrails: row.spoiler_guardrails,
      createdAt: row.created_at,
      updatedAt: row.updated_at,
    }));
  }

  createPreview(input: Omit<PreviewPackage, 'id' | 'generatedAt' | 'updatedAt'>): PreviewPackage {
    const now = new Date().toISOString();
    const preview = PreviewPackageSchema.parse({
      ...input,
      id: randomUUID(),
      generatedAt: now,
      updatedAt: now,
    });

    this.db.prepare(`
      INSERT INTO preview_packages (
        id, project_id, blueprint_version, version, sample_sections,
        review_notes, approval_state, generated_at, updated_at
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    `).run(
      preview.id,
      preview.projectId,
      preview.blueprintVersion,
      preview.version,
      JSON.stringify(preview.sampleSections),
      preview.reviewNotes,
      preview.approvalState,
      preview.generatedAt,
      preview.updatedAt,
    );

    return preview;
  }

  getPreview(id: string): PreviewPackage | null {
    const row = this.db.prepare(`
      SELECT * FROM preview_packages WHERE id = ?
    `).get(id) as PreviewPackageRow | undefined;

    if (!row) {
      return null;
    }

    return PreviewPackageSchema.parse({
      id: row.id,
      projectId: row.project_id,
      blueprintVersion: row.blueprint_version,
      version: row.version,
      sampleSections: JSON.parse(row.sample_sections) as PreviewPackage['sampleSections'],
      reviewNotes: row.review_notes,
      approvalState: row.approval_state,
      generatedAt: row.generated_at,
      updatedAt: row.updated_at,
    });
  }

  updatePreview(id: string, patch: Partial<PreviewPackage>): PreviewPackage | null {
    const existing = this.getPreview(id);
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

    this.db.prepare(`
      UPDATE preview_packages SET
        review_notes = ?,
        approval_state = ?,
        updated_at = ?
      WHERE id = ?
    `).run(
      updated.reviewNotes,
      updated.approvalState,
      updated.updatedAt,
      updated.id,
    );

    return updated;
  }

  listPreviewPackages(projectId: string): PreviewPackage[] {
    const rows = this.db.prepare(`
      SELECT * FROM preview_packages WHERE project_id = ? ORDER BY version DESC, updated_at DESC
    `).all(projectId) as PreviewPackageRow[];

    return rows.map((row) => PreviewPackageSchema.parse({
      id: row.id,
      projectId: row.project_id,
      blueprintVersion: row.blueprint_version,
      version: row.version,
      sampleSections: JSON.parse(row.sample_sections) as PreviewPackage['sampleSections'],
      reviewNotes: row.review_notes,
      approvalState: row.approval_state,
      generatedAt: row.generated_at,
      updatedAt: row.updated_at,
    }));
  }

  listJobs(projectId: string): BuildJob[] {
    const rows = this.db.prepare(`
      SELECT * FROM build_jobs WHERE project_id = ? ORDER BY started_at DESC
    `).all(projectId) as BuildJobRow[];

    return rows.map((row) => BuildJobSchema.parse({
      id: row.id,
      projectId: row.project_id,
      jobType: row.job_type,
      status: row.status,
      progress: row.progress,
      startedAt: row.started_at,
      endedAt: row.ended_at,
      humanSummary: row.human_summary,
      validationReportId: row.validation_report_id,
    }));
  }

  saveJob(job: BuildJob): BuildJob {
    const validated = BuildJobSchema.parse(job);

    this.db.prepare(`
      INSERT INTO build_jobs (
        id, project_id, job_type, status, progress, started_at, ended_at, human_summary, validation_report_id
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
      ON CONFLICT(id) DO UPDATE SET
        project_id = excluded.project_id,
        job_type = excluded.job_type,
        status = excluded.status,
        progress = excluded.progress,
        started_at = excluded.started_at,
        ended_at = excluded.ended_at,
        human_summary = excluded.human_summary,
        validation_report_id = excluded.validation_report_id
    `).run(
      validated.id,
      validated.projectId,
      validated.jobType,
      validated.status,
      validated.progress,
      validated.startedAt,
      validated.endedAt,
      validated.humanSummary,
      validated.validationReportId,
    );

    return validated;
  }

  listValidationReports(projectId: string): ValidationReport[] {
    const rows = this.db.prepare(`
      SELECT * FROM validation_reports WHERE project_id = ? ORDER BY created_at DESC
    `).all(projectId) as ValidationReportRow[];

    return rows.map((row) => ValidationReportSchema.parse({
      id: row.id,
      projectId: row.project_id,
      targetType: row.target_type,
      targetId: row.target_id,
      checksRun: JSON.parse(row.checks_run) as string[],
      blockingFindings: JSON.parse(row.blocking_findings) as string[],
      warningFindings: JSON.parse(row.warning_findings) as string[],
      blockingCount: row.blocking_count,
      warningCount: row.warning_count,
      createdAt: row.created_at,
      summary: row.summary,
    }));
  }

  saveValidationReport(report: ValidationReport): ValidationReport {
    const validated = ValidationReportSchema.parse(report);

    this.db.prepare(`
      INSERT INTO validation_reports (
        id, project_id, target_type, target_id, checks_run, blocking_findings,
        warning_findings, blocking_count, warning_count, created_at, summary
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
      ON CONFLICT(id) DO UPDATE SET
        project_id = excluded.project_id,
        target_type = excluded.target_type,
        target_id = excluded.target_id,
        checks_run = excluded.checks_run,
        blocking_findings = excluded.blocking_findings,
        warning_findings = excluded.warning_findings,
        blocking_count = excluded.blocking_count,
        warning_count = excluded.warning_count,
        created_at = excluded.created_at,
        summary = excluded.summary
    `).run(
      validated.id,
      validated.projectId,
      validated.targetType,
      validated.targetId,
      JSON.stringify(validated.checksRun),
      JSON.stringify(validated.blockingFindings),
      JSON.stringify(validated.warningFindings),
      validated.blockingCount,
      validated.warningCount,
      validated.createdAt,
      validated.summary,
    );

    return validated;
  }

  shutdown(): void {
    this.db.close();
  }

  private rowToManifest(row: DbRow): ProjectManifest {
    return ProjectManifestSchema.parse({
      id: row.id,
      projectState: row.project_state ?? 'intake',
      gameTitle: row.game_title,
      platform: row.platform ?? '',
      guideIntent: row.guide_intent,
      playerGoal: row.player_goal,
      runStyle: row.run_style,
      spoilerPolicy: row.spoiler_policy,
      depthPreference: row.depth_preference,
      providerProfile: row.provider_profile ? JSON.parse(row.provider_profile) : undefined,
      policyProfile: row.policy_profile ? JSON.parse(row.policy_profile) : undefined,
      templateVersion: row.template_version ?? '0.1.0',
      sharedThemeCss: row.shared_theme_css ?? '',
      createdAt: row.created_at,
    });
  }

  private ensureColumn(tableName: string, columnName: string, definition: string): void {
    const columns = this.db.prepare(`PRAGMA table_info(${tableName})`).all() as Array<{ name: string }>;
    if (columns.some((column) => column.name === columnName)) {
      return;
    }

    this.db.exec(`ALTER TABLE ${tableName} ADD COLUMN ${columnName} ${definition}`);
  }
}
