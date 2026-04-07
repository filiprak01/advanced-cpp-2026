#include <common/models/connection.hpp>

bool Connection::operator==(const Connection &other) const
{
    return socket == other.getSocket();
}

json Connection::toJson() const
{
    json j;
    j["socket"] = socket;
    j["session"] = session.toJson();
    return j;
}

Connection Connection::fromJson(const json &j)
{
    std::string socket = j.at("socket").get<std::string>();
    Session session = Session::fromJson(j.at("session"));
    return Connection(socket, session);
}
