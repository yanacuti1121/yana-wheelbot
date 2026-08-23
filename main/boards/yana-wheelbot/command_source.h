#ifndef COMMAND_SOURCE_H
#define COMMAND_SOURCE_H

// Tracks which transport is currently driving an MCP tool call, so
// WheelbotController can tag each queued move with where it came from and
// expose "who is currently in control" via self.wheelbot.get_control_status.
//
// This is a plain global, not thread-local: the call chain from a
// transport's own message handler down through McpServer::ParseMessage() to
// a tool's callback lambda runs synchronously on that transport's own task,
// so setting this right before ParseMessage() and resetting it right after
// correctly reflects the source for every tool call made inside that one
// synchronous call. It is NOT safe to read outside of a tool callback (i.e.
// there is no meaningful "current" value between calls -- it always reflects
// whichever transport called last, not a live session state).
//
// Deliberately NOT touching mcp_server.{cc,h} or application.cc (shared by
// every board) to add this -- only this board's own websocket_control_server.cc
// (which sets it) and wheelbot_controller.cc (which reads it) know this file
// exists. The cloud/voice path (main/application.cc's ParseMessage() call)
// never touches this file, so it implicitly stays at the default.
enum class CommandSource {
    kCloudVoice,  // default -- the cloud/voice MCP channel (application.cc)
    kLocalWs,     // this board's local LAN WebSocket control endpoint
};

inline CommandSource g_yana_command_source = CommandSource::kCloudVoice;

inline const char* CommandSourceName(CommandSource source) {
    switch (source) {
        case CommandSource::kLocalWs:
            return "local_ws";
        case CommandSource::kCloudVoice:
        default:
            return "cloud_voice";
    }
}

// RAII guard: sets g_yana_command_source for the duration of one synchronous
// McpServer::ParseMessage() call, restoring the previous value afterward.
class ScopedCommandSource {
public:
    explicit ScopedCommandSource(CommandSource source) : previous_(g_yana_command_source) {
        g_yana_command_source = source;
    }
    ~ScopedCommandSource() { g_yana_command_source = previous_; }

private:
    CommandSource previous_;
};

#endif  // COMMAND_SOURCE_H
