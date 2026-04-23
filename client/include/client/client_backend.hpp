#pragma once
#include <client/response_dispatcher.hpp>
#include <client/socket_connection.hpp>
#include <common/models/channel.hpp>
#include <common/models/message.hpp>
#include <common/utilities/config.hpp>
#include <client/connection_state.hpp>
#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * @brief Core client component that owns the socket, recv thread, and local caches.
 *
 * The UI layer calls the high-level API (login, sendMessage, …) and polls
 * drainQueue() on each render tick to retrieve any server-pushed updates.
 *
 * Thread safety
 * -------------
 * - incomingQueue is protected by queueMutex (recv thread writes, UI reads).
 * - Local caches (channels, messages) are written only by the UI thread after
 *   it pops items from incomingQueue, so no second lock is required.
 */
class ClientBackend
{
public:
    explicit ClientBackend(ClientConfig config);
    ~ClientBackend();

    // Non-copyable
    ClientBackend(const ClientBackend &) = delete;
    ClientBackend &operator=(const ClientBackend &) = delete;

    // ---- Lifecycle ---------------------------------------------------------

    /**
     * @brief Connects to the server specified in the config and starts the recv thread.
     * @throws std::runtime_error on connection failure.
     */
    void connect();

    /// @brief Disconnects and stops the recv thread.
    void disconnect();

    /// @brief Returns @c true while the socket is open.
    bool isConnected() const;
    connection::ConnectionState getConnectionState() const;

    // ---- Incoming message queue --------------------------------------------

    /**
     * @brief Moves all pending server messages out of the queue.
     *
     * Call this from the UI thread on each render tick.
     * Each returned JSON object is a fully parsed server response.
     */
    std::vector<json> drainQueue();

    // ---- High-level API (called by the UI) ---------------------------------

    void login(const std::string &username, const std::string &password);
    void registerUser(const std::string &username, const std::string &password);

    void sendMessage(int channelId, const std::string &content);
    void editMessage(int messageId, const std::string &newContent);
    void removeMessage(int messageId, int channelId);

    void createChannel(const std::string &name, const std::vector<std::string> &members, bool isPrivate);
    void editChannel(int channelId, const std::string &newName);
    void removeChannel(int channelId);
    void addUserToChannel(int channelId, const std::string &userName);

    /// @brief Requests full state (channels + messages) from the server.
    void synchronize();

    /**
     * @brief Sends a raw JSON string directly to the server.
     * @param rawJson A complete JSON string to transmit.
     */
    void sendRaw(const std::string &rawJson);

    /**
     * @brief Drains status messages created by server error responses.
     */
    std::vector<std::string> drainStatus();

    // ---- Local state (read by the UI) --------------------------------------

    const std::unordered_map<int, Channel> &getChannels() const { return channels; }
    const std::unordered_map<int, Message> &getMessages() const { return messages; }
    const std::string &getCurrentUser() const { return currentUser; }

    // ---- Response handler registration ------------------------------------

    /**
     * @brief Provides access to the dispatcher so the UI can register
     *        presentation-layer handlers (e.g. repaint callbacks).
     */
    ResponseDispatcher &dispatcher() { return responseDispatcher; }

private:
    void sendJson(const json &msg);
    void sendPayload(const std::string &payload);
    void recvLoop();
    void setupDefaultHandlers();
    void pushStatus(const std::string &message);
    void markDisconnected();
    void markConnectionLost();
    void markConnected();
    void joinRecvThreadIfNeeded();

    ClientConfig config;
    SocketConnection conn;
    ResponseDispatcher responseDispatcher;

    std::queue<json> incomingQueue;
    std::mutex queueMutex;
    std::queue<std::string> statusQueue;
    std::mutex statusMutex;

    std::thread recvThread;
    std::atomic<bool> shouldStop{false};

    // Local caches — written only from the UI thread
    std::unordered_map<int, Channel> channels;
    std::unordered_map<int, Message> messages;
    std::string currentUser;

    // Connection state can be updated from the recv thread and the UI thread.
    std::atomic<connection::ConnectionState> currentConnectionState{
        connection::ConnectionState::Disconnected};
};
