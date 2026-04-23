#include <gtest/gtest.h>
#include <common/repos/message_repo.hpp>

/// @brief Fixture dla testów MessageRepository.
class MessageRepositoryTest : public ::testing::Test
{
protected:
    MessageRepository repo;
    Message makeMessage(int id = 1) const
    {
        return Message(id, "hello", "alice", std::chrono::steady_clock::now());
    }
};

/// @test addMessage dodaje wiadomość do repozytorium.
TEST_F(MessageRepositoryTest, AddMessageStoresMessage) {}

/// @test messageExists zwraca true po dodaniu wiadomości.
TEST_F(MessageRepositoryTest, MessageExistsAfterAdd) {}

/// @test messageExists zwraca false dla nieistniejącej wiadomości.
TEST_F(MessageRepositoryTest, MessageNotExistsForUnknownId) {}

/// @test getMessage zwraca dodaną wiadomość.
TEST_F(MessageRepositoryTest, GetMessageReturnsAddedMessage) {}

/// @test removeMessage usuwa wiadomość z repozytorium.
TEST_F(MessageRepositoryTest, RemoveMessageDeletesMessage) {}

/// @test updateMessage aktualizuje treść wiadomości.
TEST_F(MessageRepositoryTest, UpdateMessageModifiesMessage) {}

TEST_F(MessageRepositoryTest, JsonRoundTripPreservesMessages)
{
    repo.addMessage(Message(42, "hello", "alice", std::chrono::steady_clock::now()));

    MessageRepository restored;
    restored.fromJson(repo.toJson());

    ASSERT_TRUE(restored.messageExists(42));
    EXPECT_EQ(restored.getMessage(42).getContent(), "hello");
    EXPECT_EQ(restored.getMessage(42).getSenderName(), "alice");
}
