---
name: "source-command-unfreeze"
description: "Clear the directory restriction set by /freeze, allowing edits everywhere again for this session."
---

# source-command-unfreeze

Use this skill when the user asks to run the migrated source command `unfreeze`.

## Command Template

Run:

```bash
bash core/scripts/freeze-scope.sh clear
```

Report the script's own output to the user verbatim. If no freeze was active, the script says so plainly — report that as-is rather than implying something was cleared when it wasn't.
