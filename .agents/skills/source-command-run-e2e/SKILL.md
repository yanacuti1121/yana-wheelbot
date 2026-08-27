---
name: "source-command-run-e2e"
description: "Run real Playwright E2E commands through the safe Yana AI wrapper. Defaults to list-only to avoid accidental full-suite burns."
---

# source-command-run-e2e

Use this skill when the user asks to run the migrated source command `run-e2e`.

## Command Template

Run real E2E checks only. This command never creates simulated reports.

Recommended low-cost modes:

```bash
.Codex/scripts/run-e2e-tests.sh list
.Codex/scripts/run-e2e-tests.sh spec tests/e2e/korean-learning.spec.ts --workers=1
.Codex/scripts/run-e2e-tests.sh grep "login|announcement" --workers=1
```

Full suite is blocked in Codespaces unless intentionally allowed:

```bash
YANA_ALLOW_FULL_E2E=1 .Codex/scripts/run-e2e-tests.sh full
```
