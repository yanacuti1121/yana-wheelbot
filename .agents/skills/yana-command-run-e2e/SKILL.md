---
name: yana-command-run-e2e
description: "Yana AI /run-e2e command adapter. Run real Playwright E2E commands through the safe Yana AI wrapper. Defaults to list-only to avoid accidental full-suite burns."
---

# Yana AI Command: /run-e2e

Invoke this workflow explicitly as `$yana-command-run-e2e`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

Run real E2E checks only. This command never creates simulated reports.

Recommended low-cost modes:

```bash
.claude/scripts/run-e2e-tests.sh list
.claude/scripts/run-e2e-tests.sh spec tests/e2e/korean-learning.spec.ts --workers=1
.claude/scripts/run-e2e-tests.sh grep "login|announcement" --workers=1
```

Full suite is blocked in Codespaces unless intentionally allowed:

```bash
YANA_ALLOW_FULL_E2E=1 .claude/scripts/run-e2e-tests.sh full
```
