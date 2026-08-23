#ifndef WEBSOCKET_CONTROL_SERVER_H
#define WEBSOCKET_CONTROL_SERVER_H

#include <esp_http_server.h>
#include <cJSON.h>
#include <functional>
#include <map>
#include <string>

// Local LAN control (see this board's README "Control protocol" section)
// requires a pairing token as a query param on the handshake URL:
// ws://<device-ip>:8080/ws?token=<token>. The token is generated once on
// first boot (6-digit numeric, same convention as this ecosystem's existing
// xiaozhi.me pairing codes) and persisted in NVS ("ws_control" namespace,
// key "token"). It's retrievable/rotatable only through an already-
// authenticated channel (the cloud/voice MCP session, or an existing valid
// local WS session) via self.local_control.get_token / rotate_token --
// without this gate, any device on the same Wi-Fi network could drive the
// robot, remap its motor GPIOs, or disable the anti-fall sensor with zero
// authentication (see PR discussion / independent review for how this was
// found).
class WebSocketControlServer {
public:
    WebSocketControlServer();
    ~WebSocketControlServer();

    bool Start(int port = 8080);

    void Stop();

    size_t GetClientCount() const;

    void BroadcastMessage(const std::string& message);

    // Invoked when the last local-WS client disconnects (close frame or
    // otherwise). A remote move command can request up to 30s of run time
    // (self.wheelbot.move_forward's duration_ms range) -- without this, a
    // client that drops Wi-Fi mid-move leaves the motor running unattended
    // until either that duration elapses or the anti-fall sensor catches
    // it, instead of stopping the moment nobody is holding the controls
    // anymore. Set by the board once it owns both this server and the
    // WheelbotController to call EmergencyStop() on.
    void SetOnAllClientsDisconnected(std::function<void()> callback) {
        on_all_clients_disconnected_ = std::move(callback);
    }

private:
    httpd_handle_t server_handle_;
    std::map<int, httpd_req_t*> clients_;
    std::string token_;
    std::function<void()> on_all_clients_disconnected_;

    static esp_err_t ws_handler(httpd_req_t* req);
    bool IsAuthorized(httpd_req_t* req) const;
    void LoadOrGenerateToken();
    void RegisterMcpTools();

    void HandleMessage(httpd_req_t* req, const char* data, size_t len);
    void AddClient(httpd_req_t* req);
    void RemoveClient(httpd_req_t* req);
    static WebSocketControlServer* instance_;
};

#endif  // WEBSOCKET_CONTROL_SERVER_H
