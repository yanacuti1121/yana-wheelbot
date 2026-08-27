---
name: yana-command-langchain-agent
description: "Yana AI /langchain-agent command adapter. Create a production-ready LangChain/LangGraph agent for: $ARGUMENTS"
---

# Yana AI Command: /langchain-agent

Invoke this workflow explicitly as `$yana-command-langchain-agent`.
Treat text supplied with the invocation as `$ARGUMENTS` wherever the source workflow references it.
Follow the source workflow without weakening its approval, scope, safety, or verification requirements.

# LangChain/LangGraph Agent Scaffold

Create a production-ready LangChain/LangGraph agent for: $ARGUMENTS

Implement a complete agent system including:

1. **Agent Architecture**:
   - LangGraph state machine
   - Tool selection logic
   - Memory management
   - Context window optimization
   - Multi-agent coordination

2. **Tool Implementation**:
   - Custom tool creation
   - Tool validation
   - Error handling in tools
   - Tool composition
   - Async tool execution

3. **Memory Systems**:
   - Short-term memory
   - Long-term storage (vector DB)
   - Conversation summarization
   - Entity tracking
   - Memory retrieval strategies

4. **Prompt Engineering**:
   - System prompts
   - Few-shot examples
   - Chain-of-thought reasoning
   - Output formatting
   - Prompt templates

5. **RAG Integration**:
   - Document loading pipeline
   - Chunking strategies
   - Embedding generation
   - Vector store setup
   - Retrieval optimization

6. **Production Features**:
   - Streaming responses
   - Token counting
   - Cost tracking
   - Rate limiting
   - Fallback strategies

7. **Observability**:
   - LangSmith integration
   - Custom callbacks
   - Performance metrics
   - Decision tracking
   - Debug mode

Include error handling, testing strategies, and deployment considerations. Use the latest LangChain/LangGraph best practices.
