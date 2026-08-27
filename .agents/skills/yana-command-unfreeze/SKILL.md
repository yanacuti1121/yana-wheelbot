---
name: yana-command-unfreeze
description: "Yana AI /unfreeze command adapter. Clear the directory restriction set by /freeze, allowing edits everywhere again for this session."
---

# Yana AI Command: /unfreeze

Invoke this workflow explicitly as `$yana-command-unfreeze`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

Run:

```bash
bash core/scripts/freeze-scope.sh clear
```

Report the script's own output to the user verbatim. If no freeze was active, the script says so plainly — report that as-is rather than implying something was cleared when it wasn't.
