---
name: vibejam3-release-readiness
description: "Project-local release readiness gate tuned to vibejam3's guide production system. Professional go/no-go for shipping."
argument-hint: "Release scope, release type, or focus areas such as packaging, docs, tests, CI/CD, or blockers"
user-invocable: true
disable-model-invocation: false
---

# vibejam3 Release Readiness

Use this skill when the question is not "does `npm run build` pass?" but "is this professionally ready to ship?"

This is a project-specialized version of `shared-release-readiness`. It checks against vibejam3's actual product surfaces instead of generic criteria.

## Non-bypassable no-stub rule

- Do **not** write, recommend, preserve, or disguise stubs, placeholder behavior, fake-success paths, mock-complete flows, TODO-backed user paths, or dead UI affordances.
- If real implementation is blocked, stop at the last truthful state, state the blocker plainly, and leave no fake completion behind.

## What this skill does

- Audits vibejam3 for professional release readiness.
- Checks build, tests, docs, packaging, versioning, runtime, and operational readiness.
- Produces a clear verdict: ready / ready-with-conditions / not-ready.

## Required workflow

1. Determine the release scope (which milestone, which slice).
2. Check build and type safety.
3. Check test coverage against `18-test-strategy.md`.
4. Check acceptance criteria from `19-acceptance-criteria-matrix.md`.
5. Check documentation:
   - does `AGENTS.md` reflect the current state?
   - are PRD docs still accurate?
   - is there a user-facing README or changelog?
6. Check operational readiness:
   - does the app start and run correctly?
   - are error states handled gracefully?
   - are provider fallbacks documented?
7. Check packaging and distribution:
   - is the build output clean?
   - are dependencies locked?
   - is the version correct?
8. Check trust surfaces:
   - edition immutability preserved
   - provider adapters working
   - validation coverage adequate
   - rights and provenance rules followed
9. Produce the verdict.

## Vibejam3-specific release checks

- **Guide production pipeline**: can the system actually produce a guide end to end?
- **Provider BYOK**: do all supported provider adapters work with real keys?
- **Storage integrity**: are SQLite databases created and read correctly?
- **Filesystem artifacts**: are guide assets stored and retrieved correctly?
- **Edition management**: are frozen editions truly immutable?
- **Validation coverage**: do all consistency validators from `12-consistency-validation.md` pass?

## Hard bans

- No vague readiness impression without a verdict.
- No calling the project ready while unresolved blockers remain.
- No reducing release quality to only compile success.
- No hiding professionalism gaps behind optimistic wording.

## Output required

Return all of these:

- verdict
- release scope checked
- strengths
- blockers
- conditions if any
- concrete next actions

## Completion standard

This skill is complete only when vibejam3's release surface has a documented verdict, concrete blockers or strengths, and actionable next steps.
