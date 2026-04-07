#pragma once
#include <server/auth/auth_manager.hpp>
#include <server/auth/registration_manager.hpp>
#include <server/core/connection_manager.hpp>
#include <server/core/session_manager.hpp>
#include <server/core/channel_manager.hpp>
#include <server/core/message_manager.hpp>

/**
 * @brief Kontekst przekazywany do zdarzeń serwera.
 *
 * Gromadzi referencje do wszystkich menedżerów w jednej strukturze,
 * umożliwiając zdarzeniom dostęp do dowolnego menedżera bez
 * konieczności przekazywania wielu argumentów.
 */
struct ManagerContext
{
  AuthManager &authenticationManager;       ///< Menedżer uwierzytelniania.
  RegistrationManager &registrationManager; ///< Menedżer rejestracji użytkowników.
  ConnectionManager &connectionManager;     ///< Menedżer połączeń sieciowych.
  SessionManager &sessionManager;           ///< Menedżer sesji.
  ChannelManager &channelManager;           ///< Menedżer kanałów.
  MessageManager &messageManager;           ///< Menedżer wiadomości.

  /**
   * @brief Tworzy kontekst na podstawie referencji do menedżerów.
   * @param authMgr   Menedżer uwierzytelniania.
   * @param regMgr    Menedżer rejestracji.
   * @param connMgr   Menedżer połączeń.
   * @param sessMgr   Menedżer sesji.
   * @param chanMgr   Menedżer kanałów.
   * @param msgMgr    Menedżer wiadomości.
   */
  ManagerContext(AuthManager &authMgr, RegistrationManager &regMgr, ConnectionManager &connMgr,
                 SessionManager &sessMgr, ChannelManager &chanMgr, MessageManager &msgMgr)
      : authenticationManager(authMgr), registrationManager(regMgr), connectionManager(connMgr),
        sessionManager(sessMgr), channelManager(chanMgr), messageManager(msgMgr) {}
};
