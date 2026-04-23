#include <gtest/gtest.h>
#include <server/core/message_manager.hpp>
#include "domain_result_expect.hpp"

namespace
{
constexpr int kValidFd = 201;
constexpr int kGhostFd = 999;
constexpr int kOtherFd = 202;
} // namespace

struct MessageFixture : ::testing::Test
{
    MessageRepository msgRepo;
    UserRepository userRepo;
    ChannelRepository chRepo;
    ConnectionManager connectionManager{-1};
    MessageManager mgr{connectionManager, msgRepo, userRepo, chRepo, 1, {}};

    const std::string validUser{"alice"};
    const int channelId{1};

    void bindUser(int fd, const std::string &userName)
    {
        ASSERT_TRUE(connectionManager.registerConnection(fd, userName));
    }

    void SetUp() override
    {
        bindUser(kValidFd, validUser);
        bindUser(kOtherFd, "bob");
        userRepo.addUser(User{validUser, "hash", "salt"});
        userRepo.addUser(User{"bob", "hash", "salt"});
        chRepo.addChannel(Channel{channelId, "general", {}, {validUser, "bob"}, false});
    }
};

TEST_F(MessageFixture, SendMessageSucceeds)
{
    EXPECT_RESULT_SUCCESS(mgr.sendMessage("hello", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
}

TEST_F(MessageFixture, SendMessageStoresInRepo)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("hello", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_TRUE(msgRepo.messageExists(1));
}

TEST_F(MessageFixture, SendMessageContentIsPreserved)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("exact content", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_EQ(msgRepo.getMessage(1).getContent(), "exact content");
}

TEST_F(MessageFixture, SendMessageSenderNameIsPreserved)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("hi", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_EQ(msgRepo.getMessage(1).getSenderName(), validUser);
}

TEST_F(MessageFixture, SendMessageIncrementsIds)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("msg1", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("msg2", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_TRUE(msgRepo.messageExists(1));
    EXPECT_TRUE(msgRepo.messageExists(2));
}

TEST_F(MessageFixture, SendMessageUnknownUserFails)
{
    bindUser(kGhostFd, "ghost");
    EXPECT_RESULT_DOMAIN_ERROR(mgr.sendMessage("hi", kGhostFd, channelId, std::chrono::steady_clock::now()), errors::Code::user_not_found);
}

TEST_F(MessageFixture, SendMessageUnknownChannelFails)
{
    EXPECT_RESULT_DOMAIN_ERROR(mgr.sendMessage("hi", kValidFd, 999, std::chrono::steady_clock::now()), errors::Code::channel_not_found);
}

TEST_F(MessageFixture, SendMessageEmptyContentSucceeds)
{
    EXPECT_RESULT_SUCCESS(mgr.sendMessage("", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
}

TEST_F(MessageFixture, SendMessageVeryLongContentSucceeds)
{
    std::string huge(100000, 'x');
    EXPECT_RESULT_SUCCESS(mgr.sendMessage(huge, kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
}

TEST_F(MessageFixture, DeleteMessageExistingSucceeds)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("bye", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_RESULT_SUCCESS(mgr.deleteMessage(validUser, 1), success::Code::message_removed);
}

TEST_F(MessageFixture, DeleteMessageRemovesFromRepo)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("bye", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.deleteMessage(validUser, 1), success::Code::message_removed);
    EXPECT_FALSE(msgRepo.messageExists(1));
}

TEST_F(MessageFixture, DeleteMessageNonExistentFails)
{
    EXPECT_RESULT_DOMAIN_ERROR(mgr.deleteMessage(validUser, 999), errors::Code::message_not_found);
}

TEST_F(MessageFixture, DeleteMessageTwiceReturnsFalseOnSecond)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("once", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.deleteMessage(validUser, 1), success::Code::message_removed);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.deleteMessage(validUser, 1), errors::Code::message_not_found);
}

TEST_F(MessageFixture, DeleteMessageDoesNotAppearInChannelMessages)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("gone", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.deleteMessage(validUser, 1), success::Code::message_removed);
    auto messages = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(messages.has_value());
    EXPECT_TRUE(messages->empty());
}

TEST_F(MessageFixture, DeleteMessageByOtherUserFails)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("owned", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.deleteMessage("bob", 1), errors::Code::forbidden);
    EXPECT_TRUE(msgRepo.messageExists(1));
}

TEST_F(MessageFixture, EditMessageExistingSucceeds)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("original", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_RESULT_SUCCESS(mgr.editMessage(validUser, 1, "edited"), success::Code::message_edited);
}

TEST_F(MessageFixture, EditMessageUpdatesContent)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("original", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.editMessage(validUser, 1, "new content"), success::Code::message_edited);
    EXPECT_EQ(msgRepo.getMessage(1).getContent(), "new content");
}

TEST_F(MessageFixture, EditMessagePreservesSenderName)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("original", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.editMessage(validUser, 1, "changed"), success::Code::message_edited);
    EXPECT_EQ(msgRepo.getMessage(1).getSenderName(), validUser);
}

TEST_F(MessageFixture, EditMessageByOtherUserFails)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("original", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.editMessage("bob", 1, "changed"), errors::Code::forbidden);
    EXPECT_EQ(msgRepo.getMessage(1).getContent(), "original");
}

TEST_F(MessageFixture, EditMessageNonExistentDoesNotCrash)
{
    EXPECT_RESULT_DOMAIN_ERROR(mgr.editMessage(validUser, 999, "new"), errors::Code::message_not_found);
}

TEST_F(MessageFixture, GetChannelMessagesEmptyChannelReturnsEmptyVector)
{
    auto messages = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(messages.has_value());
    EXPECT_TRUE(messages->empty());
}

TEST_F(MessageFixture, GetChannelMessagesNonExistentChannelReturnsNullopt)
{
    EXPECT_FALSE(mgr.getChannelMessages(999).has_value());
}

TEST_F(MessageFixture, GetChannelMessagesReturnsOneAfterOneSend)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("hello", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    auto messages = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(messages.has_value());
    EXPECT_EQ(messages->size(), 1u);
}

TEST_F(MessageFixture, GetChannelMessagesDoesNotMixChannels)
{
    chRepo.addChannel(Channel{2, "other", {}, {validUser}, false});

    ASSERT_RESULT_SUCCESS(mgr.sendMessage("ch1 msg", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("ch2 msg", kValidFd, 2, std::chrono::steady_clock::now()), success::Code::message_added);

    auto channelOneMessages = mgr.getChannelMessages(channelId);
    auto channelTwoMessages = mgr.getChannelMessages(2);
    ASSERT_TRUE(channelOneMessages.has_value());
    ASSERT_TRUE(channelTwoMessages.has_value());
    EXPECT_EQ(channelOneMessages->size(), 1u);
    EXPECT_EQ(channelTwoMessages->size(), 1u);
    EXPECT_EQ(channelOneMessages->at(0).getContent(), "ch1 msg");
    EXPECT_EQ(channelTwoMessages->at(0).getContent(), "ch2 msg");
}

TEST_F(MessageFixture, GetChannelMessagesReturnsAllMessages)
{
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_RESULT_SUCCESS(mgr.sendMessage("msg " + std::to_string(i), kValidFd, channelId,
                                             std::chrono::steady_clock::now()),
                              success::Code::message_added);
    }

    auto messages = mgr.getChannelMessages(channelId);
    ASSERT_TRUE(messages.has_value());
    EXPECT_EQ(messages->size(), 5u);
}

TEST_F(MessageFixture, RebuildIndexRestoresMessageChannelLinks)
{
    ASSERT_RESULT_SUCCESS(mgr.sendMessage("persist me", kValidFd, channelId, std::chrono::steady_clock::now()), success::Code::message_added);

    ChannelRepository restoredChannels;
    MessageRepository restoredMessages;
    restoredChannels.fromJson(chRepo.toJson());
    restoredMessages.fromJson(msgRepo.toJson());

    ConnectionManager restoredConnections{-1};
    MessageManager restoredMgr{restoredConnections, restoredMessages, userRepo, restoredChannels, 1, {}};
    restoredMgr.rebuildIndexFromRepositories();

    ASSERT_TRUE(restoredMgr.getChannelIdFromMessage(1).has_value());
    EXPECT_EQ(restoredMgr.getChannelIdFromMessage(1).value(), channelId);

    auto messages = restoredMgr.getChannelMessages(channelId);
    ASSERT_TRUE(messages.has_value());
    ASSERT_EQ(messages->size(), 1u);
    EXPECT_EQ(messages->at(0).getContent(), "persist me");
}
