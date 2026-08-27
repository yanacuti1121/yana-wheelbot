---
name: yana-command-generate-coverage
description: "Yana AI /generate-coverage command adapter. Run the real project coverage script if configured. Fake coverage reports are forbidden."
---

# Yana AI Command: /generate-coverage

Invoke this workflow explicitly as `$yana-command-generate-coverage`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

This command only runs a real `test:coverage` script from `package.json`.
It does not generate simulated coverage.

```bash
.claude/scripts/generate-coverage-report.sh
```

If no coverage script exists, add a real coverage tool first.
