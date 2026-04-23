#pragma once

namespace connection
{
    enum class ConnectionState
    {
        Disconnected,
        Connecting,
        Connected,
        ConnectionLost
    };
}