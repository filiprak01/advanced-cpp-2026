#include <gtest/gtest.h>
#include <server/core/message_manager.hpp>

// ---------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------

struct MessageFixture : ::testing::Test
{
    MessageRepository msgRepo;
    UserRepository userRepo;
    ChannelRepository chRepo;
    MessageManager mgr{msgRepo, userRepo, chRepo, 1, {}};

    const std::string validUser{"alice"};
    const int channelId{1};

    void SetUp() override
    {
        // Seed a user and a channel that most tests need.
        userRepo.addUser(User{validUser, "hash", "salt"});
        chRepo.addChannel(Channel{channelId, "general", {}, {}, false});
    }
};

// ---------------------------------------------------------------------------
// sendMessage — happy path
// ---------------------------------------------------------------------------

/// @test Sending a message from an existing user to an existing channel succeeds.
TEST_F(MessageFixture, SendMessageSucceeds)
{
    EXPECT_TRUE(mgr.sendMessage("hello", validUser, channelId, std::chrono::steady_clock::now()));
}

/// @test After a successful send, the message exists in the repo.
TEST_F(MessageFixture, SendMessageStoresInRepo)
{
    mgr.sendMessage("hello", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_TRUE(msgRepo.messageExists(1));
}

/// @test Message content is stored verbatim.
TEST_F(MessageFixture, SendMessageContentIsPreserved)
{
    mgr.sendMessage("exact content", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_EQ(msgRepo.getMessage(1).getContent(), "exact content");
}

/// @test Sender name is stored verbatim.
TEST_F(MessageFixture, SendMessageSenderNameIsPreserved)
{
    mgr.sendMessage("hi", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_EQ(msgRepo.getMessage(1).getSenderName(), validUser);
}

/// @test Sending multiple messages assigns incrementing ids.
TEST_F(MessageFixture, SendMessageIncrementsIds)
{
    mgr.sendMessage("msg1", validUser, channelId, std::chrono::steady_clock::now());
    mgr.sendMessage("msg2", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_TRUE(msgRepo.messageExists(1));
    EXPECT_TRUE(msgRepo.messageExists(2));
}

// ---------------------------------------------------------------------------
// sendMessage — error paths
// ---------------------------------------------------------------------------

/// @test Sending a message from a non-existent user returns false.
TEST_F(MessageFixture, SendMessageUnknownUserFails)
{
    EXPECT_FALSE(mgr.sendMessage("hi", "ghost", channelId, std::chrono::steady_clock::now()));
}

/// @test Sending a message to a non-existent channel returns false.
TEST_F(MessageFixture, SendMessageUnknownChannelFails)
{
    EXPECT_FALSE(mgr.sendMessage("hi", validUser, 999, std::chrono::steady_clock::now()));
}

/// @test Sending a message with empty content still succeeds (no content validation).
TEST_F(MessageFixture, SendMessageEmptyContentSucceeds)
{
    EXPECT_TRUE(mgr.sendMessage("", validUser, channelId, std::chrono::steady_clock::now()));
}

/// @test Sending a very long message succeeds (no length limit in business logic).
TEST_F(MessageFixture, SendMessageVeryLongContentSucceeds)
{
    std::string huge(100000, 'x');
    EXPECT_TRUE(mgr.sendMessage(huge, validUser, channelId, std::chrono::steady_clock::now()));
}

// ---------------------------------------------------------------------------
// deleteMessage
// ---------------------------------------------------------------------------

/// @test Deleting an existing message returns true.
TEST_F(MessageFixture, DeleteMessageExistingSucceeds)
{
    mgr.sendMessage("bye", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_TRUE(mgr.deleteMessage(1));
}

/// @test After deletion, the message no longer exists in the repo.
TEST_F(MessageFixture, DeleteMessageRemovesFromRepo)
{
    mgr.sendMessage("bye", validUser, channelId, std::chrono::steady_clock::now());
    mgr.deleteMessage(1);
    EXPECT_FALSE(msgRepo.messageExists(1));
}

/// @test Deleting a non-existent message returns false.
TEST_F(MessageFixture, DeleteMessageNonExistentFails)
{
    EXPECT_FALSE(mgr.deleteMessage(999));
}

/// @test Double deletion: second call returns false.
TEST_F(MessageFixture, DeleteMessageTwiceReturnsFalseOnSecond)
{
    mgr.sendMessage("once", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_TRUE(mgr.deleteMessage(1));
    EXPECT_FALSE(mgr.deleteMessage(1));
}

/// @test After deleting a message it no longer appears in getChannelMessages.
TEST_F(MessageFixture, DeleteMessageDoesNotAppearInChannelMessages)
{
    mgr.sendMessage("gone", validUser, channelId, std::chrono::steady_clock::now());
    mgr.deleteMessage(1);
    auto msgs = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(msgs.has_value());
    EXPECT_TRUE(msgs->empty());
}

// ---------------------------------------------------------------------------
// editMessage
// ---------------------------------------------------------------------------

/// @test Editing an existing message returns true.
TEST_F(MessageFixture, EditMessageExistingSucceeds)
{
    mgr.sendMessage("original", validUser, channelId, std::chrono::steady_clock::now());
    EXPECT_TRUE(mgr.editMessage(1, "edited"));
}

/// @test After editing, the updated content is readable from the repo.
TEST_F(MessageFixture, EditMessageUpdatesContent)
{
    mgr.sendMessage("original", validUser, channelId, std::chrono::steady_clock::now());
    mgr.editMessage(1, "new content");
    EXPECT_EQ(msgRepo.getMessage(1).getContent(), "new content");
}

/// @test Editing a message preserves the original sender name.
TEST_F(MessageFixture, EditMessagePreservesSenderName)
{
    mgr.sendMessage("original", validUser, channelId, std::chrono::steady_clock::now());
    mgr.editMessage(1, "changed");
    EXPECT_EQ(msgRepo.getMessage(1).getSenderName(), validUser);
}

/// @test Editing a non-existent message — documents the current (unsafe) behaviour.
/// @note editMessage does NOT check existence first; it operates on a default-constructed Message.
///       This test documents the known behaviour: it should NOT crash (it may return false though).
TEST_F(MessageFixture, EditMessageNonExistentDoesNotCrash)
{
    // We just verify no exception/crash escapes; the return value may be false.
    EXPECT_NO_FATAL_FAILURE(mgr.editMessage(999, "new"));
}

// ---------------------------------------------------------------------------
// getChannelMessages
// ---------------------------------------------------------------------------

/// @test Getting messages for a channel with no messages returns an empty vector.
TEST_F(MessageFixture, GetChannelMessagesEmptyChannelReturnsEmptyVector)
{
    auto msgs = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(msgs.has_value());
    EXPECT_TRUE(msgs->empty());
}

/// @test Getting messages for a non-existent channel returns nullopt.
TEST_F(MessageFixture, GetChannelMessagesNonExistentChannelReturnsNullopt)
{
    EXPECT_FALSE(mgr.getChannelMessages(999).has_value());
}

/// @test After sending one message, getChannelMessages returns exactly one message.
TEST_F(MessageFixture, GetChannelMessagesReturnsOneAfterOneSend)
{
    mgr.sendMessage("hello", validUser, channelId, std::chrono::steady_clock::now());
    auto msgs = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(msgs.has_value());
    EXPECT_EQ(msgs->size(), 1u);
}

/// @test Messages sent to different channels are not mixed together.
TEST_F(MessageFixture, GetChannelMessagesDoesNotMixChannels)
{
    chRepo.addChannel(Channel{2, "other", {}, {}, false});
    mgr.sendMessage("ch1 msg", validUser, channelId, std::chrono::steady_clock::now());
    mgr.sendMessage("ch2 msg", validUser, 2, std::chrono::steady_clock::now());

    auto msgs1 = mgr.getChannelMessages(channelId);
    auto msgs2 = mgr.getChannelMessages(2);
    ASSERT_TRUE(msgs1.has_value());
    ASSERT_TRUE(msgs2.has_value());
    EXPECT_EQ(msgs1->size(), 1u);
    EXPECT_EQ(msgs2->size(), 1u);
    EXPECT_EQ(msgs1->at(0).getContent(), "ch1 msg");
    EXPECT_EQ(msgs2->at(0).getContent(), "ch2 msg");
}

/// @test Sending multiple messages to a channel — all are returned.
TEST_F(MessageFixture, GetChannelMessagesReturnsAllMessages)
{
    for (int i = 0; i < 5; ++i)
        mgr.sendMessage("msg " + std::to_string(i), validUser, channelId,
                        std::chrono::steady_clock::now());
    auto msgs = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(msgs.has_value());
    EXPECT_EQ(msgs->size(), 5u);
}
