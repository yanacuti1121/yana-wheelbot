---
name: "source-command-generate-coverage"
description: "Run the real project coverage script if configured. Fake coverage reports are forbidden."
---

# source-command-generate-coverage

Use this skill when the user asks to run the migrated source command `generate-coverage`.

## Command Template

This command only runs a real `test:coverage` script from `package.json`.
It does not generate simulated coverage.

```bash
.Codex/scripts/generate-coverage-report.sh
```

If no coverage script exists, add a real coverage tool first.
