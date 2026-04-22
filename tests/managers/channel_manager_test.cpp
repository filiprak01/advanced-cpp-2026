#include <gtest/gtest.h>
#include <server/core/channel_manager.hpp>
#include <common/models/user.hpp>
#include "domain_result_expect.hpp"

struct ChannelFixture : ::testing::Test
{
    ChannelRepository repo;
    UserRepository userRepo;
    ChannelManager mgr{repo, userRepo};

    void addUser(const std::string &name)
    {
        userRepo.addUser(User(name, "", ""));
    }

    void SetUp() override
    {
        addUser("alice");
        addUser("bob");
        addUser("charlie");
    }
};

TEST_F(ChannelFixture, CreatePublicChannelSucceeds)
{
    EXPECT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"alice"}), success::Code::channel_created);
    EXPECT_TRUE(repo.channelExists(1));
}

TEST_F(ChannelFixture, CreatePublicChannelIncludesRequestor)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {}), success::Code::channel_created);
    Channel channel = repo.getChannel(1);
    EXPECT_NE(channel.getUserIds().find("alice"), channel.getUserIds().end());
}

TEST_F(ChannelFixture, CreatePublicChannelSeedsUsers)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob", "charlie"}), success::Code::channel_created);
    Channel channel = repo.getChannel(1);
    EXPECT_NE(channel.getUserIds().find("alice"), channel.getUserIds().end());
    EXPECT_NE(channel.getUserIds().find("bob"), channel.getUserIds().end());
    EXPECT_NE(channel.getUserIds().find("charlie"), channel.getUserIds().end());
}

TEST_F(ChannelFixture, CreatePublicChannelRegistersAllMembers)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_EQ(mgr.getUserActiveChannels("alice").size(), 1u);
    EXPECT_EQ(mgr.getUserActiveChannels("bob").size(), 1u);
}

TEST_F(ChannelFixture, CreatePrivateConversationWithRequestorAndPeerSucceeds)
{
    EXPECT_RESULT_SUCCESS(mgr.createPrivateConversation("alice", {"bob"}), success::Code::channel_created);
    EXPECT_TRUE(repo.channelExists(1));
}

TEST_F(ChannelFixture, CreatePrivateConversationIsPrivate)
{
    ASSERT_RESULT_SUCCESS(mgr.createPrivateConversation("alice", {"bob"}), success::Code::channel_created);
    EXPECT_TRUE(repo.getChannel(1).getIsPrivate());
    EXPECT_EQ(repo.getChannel(1).getMaxUsers(), 2);
}

TEST_F(ChannelFixture, CreatePrivateConversationWithTooManyDistinctUsersFails)
{
    EXPECT_RESULT_FORMAT_ERROR(mgr.createPrivateConversation("alice", {"bob", "charlie"}), errors::Code::invalid_payload);
}

TEST_F(ChannelFixture, CreatePrivateConversationWithoutPeerFails)
{
    EXPECT_RESULT_FORMAT_ERROR(mgr.createPrivateConversation("alice", {}), errors::Code::invalid_payload);
}

TEST_F(ChannelFixture, DeleteChannelRequiresRequestorMembership)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.deleteChannel("charlie", 1), errors::Code::forbidden);
    EXPECT_TRUE(repo.channelExists(1));
}

TEST_F(ChannelFixture, DeleteChannelExistingSucceeds)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_SUCCESS(mgr.deleteChannel("alice", 1), success::Code::channel_removed);
    EXPECT_FALSE(repo.channelExists(1));
}

TEST_F(ChannelFixture, DeleteChannelRemovesFromUserLists)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    ASSERT_RESULT_SUCCESS(mgr.deleteChannel("alice", 1), success::Code::channel_removed);
    EXPECT_TRUE(mgr.getUserActiveChannels("alice").empty());
    EXPECT_TRUE(mgr.getUserActiveChannels("bob").empty());
}

TEST_F(ChannelFixture, AddUserToChannelRequiresRequestorMembership)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {}), success::Code::channel_created);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.addUserToChannel("bob", 1, "charlie"), errors::Code::forbidden);
}

TEST_F(ChannelFixture, AddUserToChannelSucceedsForMember)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {}), success::Code::channel_created);
    EXPECT_RESULT_SUCCESS(mgr.addUserToChannel("alice", 1, "charlie"), success::Code::user_joined_channel);
}

TEST_F(ChannelFixture, AddUserToChannelAppearsInActiveChannels)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {}), success::Code::channel_created);
    ASSERT_RESULT_SUCCESS(mgr.addUserToChannel("alice", 1, "charlie"), success::Code::user_joined_channel);
    EXPECT_EQ(mgr.getUserActiveChannels("charlie").size(), 1u);
}

TEST_F(ChannelFixture, AddUserToChannelAlreadyMemberFails)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.addUserToChannel("alice", 1, "bob"), errors::Code::user_already_in_channel);
}

TEST_F(ChannelFixture, AddUserToChannelBeyondCapacityFails)
{
    ASSERT_RESULT_SUCCESS(mgr.createPrivateConversation("alice", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.addUserToChannel("alice", 1, "charlie"), errors::Code::channel_full);
}

TEST_F(ChannelFixture, RemoveUserFromChannelRequiresRequestorMembership)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.removeUserFromChannel("charlie", 1, "bob"), errors::Code::forbidden);
}

TEST_F(ChannelFixture, RemoveUserFromChannelSucceeds)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_SUCCESS(mgr.removeUserFromChannel("alice", 1, "bob"), success::Code::user_removed_from_channel);
}

TEST_F(ChannelFixture, RemoveUserFromChannelDisappearsFromActiveList)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    ASSERT_RESULT_SUCCESS(mgr.removeUserFromChannel("alice", 1, "bob"), success::Code::user_removed_from_channel);
    EXPECT_TRUE(mgr.getUserActiveChannels("bob").empty());
}

TEST_F(ChannelFixture, EditChannelNameRequiresRequestorMembership)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "old", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_DOMAIN_ERROR(mgr.editChannelName("charlie", 1, "new"), errors::Code::forbidden);
}

TEST_F(ChannelFixture, EditChannelNameExistingSucceeds)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "old", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_SUCCESS(mgr.editChannelName("alice", 1, "new"), success::Code::channel_edited);
}

TEST_F(ChannelFixture, EditChannelNamePersistsInRepo)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "old", {}), success::Code::channel_created);
    ASSERT_RESULT_SUCCESS(mgr.editChannelName("alice", 1, "newname"), success::Code::channel_edited);
    EXPECT_EQ(repo.getChannel(1).getName(), "newname");
}

TEST_F(ChannelFixture, DeactivatePrivateChannelTogglesActive)
{
    ASSERT_RESULT_SUCCESS(mgr.createPrivateConversation("alice", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_SUCCESS(mgr.deactivateChannel(1), success::Code::channel_deactivated);
    EXPECT_FALSE(repo.getChannel(1).getIsActive());
}

TEST_F(ChannelFixture, DeactivatePublicChannelReturnsTrueButStaysActive)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "general", {"bob"}), success::Code::channel_created);
    EXPECT_RESULT_SUCCESS(mgr.deactivateChannel(1), success::Code::channel_deactivated);
    EXPECT_TRUE(repo.getChannel(1).getIsActive());
}

TEST_F(ChannelFixture, GetUserActiveChannelsEmptyForUnknownUser)
{
    EXPECT_TRUE(mgr.getUserActiveChannels("nobody").empty());
}

TEST_F(ChannelFixture, GetAllUserChannelsIncludesDeactivated)
{
    ASSERT_RESULT_SUCCESS(mgr.createPrivateConversation("alice", {"bob"}), success::Code::channel_created);
    ASSERT_RESULT_SUCCESS(mgr.deactivateChannel(1), success::Code::channel_deactivated);
    EXPECT_TRUE(mgr.getUserActiveChannels("alice").empty());
    EXPECT_EQ(mgr.getAllUserChannels("alice").size(), 1u);
}

TEST_F(ChannelFixture, UserActiveChannelsCountsAllJoined)
{
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "ch1", {}), success::Code::channel_created);
    ASSERT_RESULT_SUCCESS(mgr.createPublicChannel("alice", "ch2", {"bob"}), success::Code::channel_created);
    EXPECT_EQ(mgr.getUserActiveChannels("alice").size(), 2u);
}
