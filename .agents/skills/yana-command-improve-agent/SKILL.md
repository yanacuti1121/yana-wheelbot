---
name: yana-command-improve-agent
description: "Yana AI /improve-agent command adapter. Improve an existing agent based on recent performance:"
---

# Yana AI Command: /improve-agent

Invoke this workflow explicitly as `$yana-command-improve-agent`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

Improve an existing agent based on recent performance:

1. Analyze recent uses of: $ARGUMENTS
2. Identify patterns in:
   - Failed tasks
   - User corrections
   - Suboptimal outputs
3. Update the agent's prompt with:
   - New examples
   - Clarified instructions
   - Additional constraints
4. Test on recent scenarios
5. Save improved version
