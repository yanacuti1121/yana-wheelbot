---
name: yana-command-resume
description: "Yana AI /resume command adapter. Rebuild working context after token/session exhaustion using local checkpoint, memory index, brain dump, git state, and recent handoffs. Usage: /resume [optional topic]"
---

# Yana AI Command: /resume

Invoke this workflow explicitly as `$yana-command-resume`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

You are resuming work after a context or usage interruption.

## Phase 1 — Create a fresh checkpoint artifact

Run:

```bash
node .claude/scripts/session-checkpoint.js
```

Then read:

1. `.claude/state/SESSION_CHECKPOINT.md`
2. `BRAIN_DUMP.md` if present
3. `MEMORY.md`
4. `TODO.md`
5. Recent handoffs in `docs/handoff/` if present

If `$ARGUMENTS` is provided, also run:

```bash
node .claude/scripts/memory-router.js "$ARGUMENTS"
```

## Phase 2 — Identify the resume target

Report:

```markdown
## Resume target
- Topic: ...
- Current branch: ...
- Uncommitted files: ...
- Most relevant memory files: ...
- Files to read before editing: ...
```

## Phase 3 — Continue safely

Before editing:
- Read the exact files you plan to modify.
- Use `.claude/agent-routing-map.json` to select the specialist.
- If the work is ambiguous, ask one focused question or propose the safest next step.

Do not pretend to remember anything not found in the checkpoint, memory, git history, or current conversation.
