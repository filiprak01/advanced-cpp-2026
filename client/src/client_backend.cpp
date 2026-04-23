#include <client/client_backend.hpp>
#include <client/protocol.hpp>

#include <stdexcept>

namespace
{
    std::string formatHostFromRawIp(int rawIp)
    {
        return std::to_string((rawIp >> 24) & 0xFF) + "." +
               std::to_string((rawIp >> 16) & 0xFF) + "." +
               std::to_string((rawIp >> 8) & 0xFF) + "." +
               std::to_string(rawIp & 0xFF);
    }
} // namespace

// ---- Construction / destruction ----------------------------------------

ClientBackend::ClientBackend(ClientConfig cfg)
    : config(std::move(cfg))
{
    setupDefaultHandlers();
}

ClientBackend::~ClientBackend()
{
    disconnect();
}

// ---- Lifecycle ---------------------------------------------------------

void ClientBackend::connect()
{
    const connection::ConnectionState state = currentConnectionState.load();
    if (state == connection::ConnectionState::Connecting ||
        state == connection::ConnectionState::Connected)
    {
        return;
    }

    joinRecvThreadIfNeeded();
    currentConnectionState.store(connection::ConnectionState::Connecting);
    shouldStop = false;

    try
    {
        conn.connect(formatHostFromRawIp(config.getServerIp()), config.getServerPort());
        currentConnectionState.store(connection::ConnectionState::Connected);
        recvThread = std::thread(&ClientBackend::recvLoop, this);
    }
    catch (...)
    {
        markDisconnected();
        throw;
    }
}

void ClientBackend::disconnect()
{
    markDisconnected();
    joinRecvThreadIfNeeded();
}

bool ClientBackend::isConnected() const
{
    return currentConnectionState.load() == connection::ConnectionState::Connected &&
           conn.isConnected();
}

connection::ConnectionState ClientBackend::getConnectionState() const
{
    return currentConnectionState.load();
}

// ---- Queue drain -------------------------------------------------------

std::vector<json> ClientBackend::drainQueue()
{
    std::vector<json> out;
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!incomingQueue.empty())
    {
        out.push_back(std::move(incomingQueue.front()));
        incomingQueue.pop();
    }
    return out;
}

// ---- High-level API ----------------------------------------------------

void ClientBackend::login(const std::string &username, const std::string &password)
{
    sendJson(protocol::buildLoginRequest(username, password));
}

void ClientBackend::registerUser(const std::string &username, const std::string &password)
{
    sendJson(protocol::buildRegisterRequest(username, password));
}

void ClientBackend::sendMessage(int channelId, const std::string &content)
{
    sendJson(protocol::buildAddMessageRequest(currentUser, channelId, content));
}

void ClientBackend::editMessage(int messageId, const std::string &newContent)
{
    sendJson(protocol::buildEditMessageRequest(messageId, newContent));
}

void ClientBackend::removeMessage(int messageId, int channelId)
{
    sendJson(protocol::buildRemoveMessageRequest(messageId, channelId));
}

void ClientBackend::createChannel(const std::string &name, const std::vector<std::string> &members, bool isPrivate)
{
    sendJson(protocol::buildCreateChannelRequest(name, members, isPrivate));
}

void ClientBackend::editChannel(int channelId, const std::string &newName)
{
    sendJson(protocol::buildEditChannelRequest(channelId, newName));
}

void ClientBackend::removeChannel(int channelId)
{
    sendJson(protocol::buildRemoveChannelRequest(channelId));
}

void ClientBackend::addUserToChannel(int channelId, const std::string &userName)
{
    sendJson(protocol::buildAddUserToChannelRequest(channelId, userName));
}

void ClientBackend::synchronize()
{
    sendJson(protocol::buildSynchronizeRequest(-1));
}

void ClientBackend::sendRaw(const std::string &rawJson)
{
    sendPayload(rawJson);
}

std::vector<std::string> ClientBackend::drainStatus()
{
    std::vector<std::string> out;
    std::lock_guard<std::mutex> lock(statusMutex);
    while (!statusQueue.empty())
    {
        out.push_back(std::move(statusQueue.front()));
        statusQueue.pop();
    }
    return out;
}

void ClientBackend::pushStatus(const std::string &message)
{
    std::lock_guard<std::mutex> lock(statusMutex);
    statusQueue.push(message);
}

// ---- Internal ----------------------------------------------------------

void ClientBackend::sendJson(const json &msg)
{
    sendPayload(msg.dump());
}

void ClientBackend::sendPayload(const std::string &payload)
{
    if (!isConnected())
    {
        markDisconnected();
        throw std::runtime_error("ClientBackend: cannot send while disconnected");
    }

    try
    {
        conn.send(payload);
    }
    catch (...)
    {
        markDisconnected();
        joinRecvThreadIfNeeded();
        throw;
    }
}

void ClientBackend::recvLoop()
{
    while (!shouldStop)
    {
        std::string raw;
        try
        {
            if (!conn.recv(raw))
            {
                markConnectionLost();
                break;
            }
        }
        catch (const std::exception &)
        {
            markConnectionLost();
            break;
        }

        try
        {
            json msg = json::parse(raw);
            responseDispatcher.dispatch(msg);
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                incomingQueue.push(std::move(msg));
            }
        }
        catch (const json::parse_error &)
        {
            markConnectionLost();
        }
    }
}

void ClientBackend::markDisconnected()
{
    shouldStop = true;
    currentConnectionState.store(connection::ConnectionState::Disconnected);
    conn.disconnect();
}

void ClientBackend::markConnected()
{
    shouldStop = false;
    currentConnectionState.store(connection::ConnectionState::Connected);
}

void ClientBackend::markConnectionLost()
{
    shouldStop = true;
    currentConnectionState.store(connection::ConnectionState::ConnectionLost);
    pushStatus("connection_lost");
}

void ClientBackend::joinRecvThreadIfNeeded()
{
    if (recvThread.joinable() && recvThread.get_id() != std::this_thread::get_id())
    {
        recvThread.join();
    }
}

void ClientBackend::setupDefaultHandlers()
{
    responseDispatcher.registerHandler("login_response", [this](const json &msg)
                                       {
        if (msg.value("status", "") == "ok" && msg.contains("payload"))
            currentUser = msg["payload"].value("userName", ""); });

    responseDispatcher.registerHandler("new_message", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        const auto &p = msg["payload"];
        int id = p.value("messageId", -1);
        if (id < 0)
            return;
        Message m;
        m.fromJson(p);
        messages[id] = std::move(m);
        int limit = config.getMessageCacheSize();
        if (limit > 0 && static_cast<int>(messages.size()) > limit)
            messages.erase(messages.begin()); });

    responseDispatcher.registerHandler("message_edited", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        const auto &p = msg["payload"];
        int id = p.value("messageId", -1);
        auto it = messages.find(id);
        if (it != messages.end())
            it->second = it->second.withContent(p.value("newContent", "")); });

    responseDispatcher.registerHandler("message_removed", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        messages.erase(msg["payload"].value("messageId", -1)); });

    responseDispatcher.registerHandler("channel_created", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        Channel ch;
        ch.fromJson(msg["payload"]);
        channels[ch.getChannelId()] = std::move(ch); });

    responseDispatcher.registerHandler("user_joined_channel", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        Channel ch;
        ch.fromJson(msg["payload"]);
        channels[ch.getChannelId()] = std::move(ch); });

    responseDispatcher.registerHandler("channel_edited", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        const auto &p = msg["payload"];
        int id = p.value("channelId", -1);
        auto it = channels.find(id);
        if (it != channels.end())
            it->second = it->second.withName(p.value("newName", "")); });

    responseDispatcher.registerHandler("channel_removed", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        channels.erase(msg["payload"].value("channelId", -1)); });

    responseDispatcher.registerHandler("sync_response", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        const auto &p = msg["payload"];
        if (p.contains("channels"))
        {
            channels.clear();
            for (const auto &cj : p["channels"])
            {
                Channel ch;
                ch.fromJson(cj);
                channels[ch.getChannelId()] = std::move(ch);
            }
        }
        if (p.contains("messages"))
        {
            messages.clear();
            for (const auto &mj : p["messages"])
            {
                Message m;
                m.fromJson(mj);
                messages[m.getId()] = std::move(m);
            }
        } });

    responseDispatcher.registerHandler("error", [this](const json &msg)
                                       {
        if (!msg.contains("payload"))
            return;
        pushStatus(msg["payload"].value("reason", "unknown_error")); });
}
