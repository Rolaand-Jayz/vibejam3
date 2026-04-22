---
description: "Use when working on any vibejam3 code, planning, design, or review task. Enforces the rule that the agent must always load and follow the matching project-local skill before acting. Covers skill matching, loading order, gate enforcement, and what to do when no skill matches."
applyTo: "**"
---

# Skill enforcement — mandatory skill loading

These rules apply to ALL work in the vibejam3 project. They are non-negotiable.

## Step 1: Match before acting

Before writing any code, making any plan, producing any design, or generating any output, scan the project-local skills listed in `AGENTS.md` and the global shared skills for a match.

A skill matches when:
- Its description keywords overlap with the task at hand
- Its trigger phrases match what the user asked for
- The task falls within its documented scope

## Step 2: Load the skill file

When a skill matches, `read_file` the corresponding `SKILL.md` as your **first action** — before generating output, before making a plan, before writing code.

If multiple skills match:
1. Load the most specific project-local skill first
2. Load secondary skills only when they add clear value to the current step
3. Do not drag skills through phases where they are no longer needed

## Step 3: Follow the skill's procedure

After loading, follow the skill's step-by-step procedure exactly.
- Do not skip steps
- Do not summarize steps away
- Do not replace the skill's workflow with generic behavior
- If the skill says "load X first" or "run Y before proceeding", do exactly that

## Step 4: Respect gates

Phase gates are hard stops, not suggestions:

| Gate | When | Skill |
|------|------|-------|
| Environment check | Before any new phase | `vibejam3-phase0-environment-inspector` |
| Readiness check | Before a slice enters coding | `vibejam3-readiness-gate` |
| Design check | Any UI/visual work | `vibejam3-app-design` |
| Completion check | Before claiming a phase is done | `vibejam3-completion-gate` |
| Release check | Before shipping | `vibejam3-release-readiness` |

A gate failure means **stop and fix**, not note and continue.

## Step 5: Missing skills

If a skill should exist for the current task but is not listed, do NOT silently proceed without it. State explicitly:
- Which skill is missing
- What gap it creates
- Ask whether to proceed without it or stop

## For autonomous / end-to-end work

When asked to "implement Phase N" or run autonomously:
1. Load `vibejam3-prd-accelerator` first
2. Or load `vibejam3-autonomy-wrapper` / `vibejam3-e2e-stack` for full autonomous execution
3. These route through the full skill chain automatically
