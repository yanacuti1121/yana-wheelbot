---
name: yana-command-route
description: "Yana AI /route command adapter. Choose the right specialist agent using the v10 routing map. Usage: /route <task description>"
---

# Yana AI Command: /route

Invoke this workflow explicitly as `$yana-command-route`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

Use `.claude/agent-routing-map.json` to pick the best agent for `$ARGUMENTS`.

Steps:
1. Read `.claude/agent-routing-map.json`.
2. Match the task description against the map.
3. Output exactly:

```markdown
## Route
Primary agent: @[agent]
Verification agent: @[agent]
Reason: [matched words / rule]
Files to read first: [if known]
Do not call: [agents that look similar but are wrong]
```

If no rule matches, use the fallback agent from the routing map.
