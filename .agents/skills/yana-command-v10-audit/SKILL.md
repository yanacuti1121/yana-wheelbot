---
name: yana-command-v10-audit
description: "Yana AI /v10-audit command adapter. Run the v10 reliability audit: verify pack, hook health, routing, memory retrieval, and fake-claim protection. Usage: /v10-audit [optional topic]"
---

# Yana AI Command: /v10-audit

Invoke this workflow explicitly as `$yana-command-v10-audit`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

Run these checks in order:

```bash
.claude/scripts/verify-claude-pack.sh
```

If `$ARGUMENTS` is provided, run:

```bash
node .claude/scripts/memory-router.js "$ARGUMENTS"
```

Then inspect:
- `.claude/agent-routing-map.json`
- `.claude/hook-budget.json`
- `.claude/agents/prompt-firewall.md`
- `.claude/agents/token-guard.md`
- `.claude/agents/agent-gardener.md`

Report:

```markdown
## V10 Audit Result
Pack integrity: pass/fail
Hook health: pass/fail
Routing precision: pass/warn/fail
Memory retrieval: pass/warn/fail
Fake-claim protection: pass/warn/fail

Top fixes needed:
1. ...
2. ...
3. ...
```

Do not make fixes unless the user asks for `/v10-fix`.
