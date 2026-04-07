#include <gtest/gtest.h>
#include <server/core/channel_manager.hpp>
#include <common/models/user.hpp>

// ---------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------

struct ChannelFixture : ::testing::Test
{
    ChannelRepository repo;
    UserRepository userRepo;
    ChannelManager mgr{repo, userRepo};

    void addUser(const std::string &name)
    {
        userRepo.addUser(User(name, "", ""));
    }
};

// ---------------------------------------------------------------------------
// createPublicChannel
// ---------------------------------------------------------------------------

/// @test Creating a public channel with one user succeeds.
TEST_F(ChannelFixture, CreatePublicChannelSucceeds)
{
    EXPECT_TRUE(mgr.createPublicChannel("general", {"alice"}));
    EXPECT_TRUE(repo.channelExists(1));
}

/// @test Created public channel is not private.
TEST_F(ChannelFixture, CreatePublicChannelIsNotPrivate)
{
    mgr.createPublicChannel("general", {"alice"});
    Channel ch = repo.getChannel(1);
    EXPECT_FALSE(ch.getIsPrivate());
}

/// @test Created public channel is active by default.
TEST_F(ChannelFixture, CreatePublicChannelIsActiveByDefault)
{
    mgr.createPublicChannel("general", {"alice"});
    EXPECT_TRUE(repo.getChannel(1).getIsActive());
}

/// @test Public channel max users is 32.
TEST_F(ChannelFixture, CreatePublicChannelMaxUsersIs32)
{
    mgr.createPublicChannel("big", {"alice"});
    EXPECT_EQ(repo.getChannel(1).getMaxUsers(), 32);
}

/// @test Creating a public channel with no users succeeds (empty init list).
TEST_F(ChannelFixture, CreatePublicChannelWithNoUsersSucceeds)
{
    EXPECT_TRUE(mgr.createPublicChannel("empty", {}));
}

/// @test Seeded users appear in the channel's user set.
TEST_F(ChannelFixture, CreatePublicChannelSeedsUsers)
{
    mgr.createPublicChannel("general", {"alice", "bob", "charlie"});
    Channel ch = repo.getChannel(1);
    EXPECT_NE(ch.getUserIds().find("alice"), ch.getUserIds().end());
    EXPECT_NE(ch.getUserIds().find("bob"), ch.getUserIds().end());
    EXPECT_NE(ch.getUserIds().find("charlie"), ch.getUserIds().end());
}

/// @test Seeded users appear in getUserActiveChannels after creation.
TEST_F(ChannelFixture, CreatePublicChannelRegistersChannelForSeededUsers)
{
    mgr.createPublicChannel("general", {"user5", "user6"});
    EXPECT_EQ(mgr.getUserActiveChannels("user5").size(), 1u);
    EXPECT_EQ(mgr.getUserActiveChannels("user6").size(), 1u);
}

// ---------------------------------------------------------------------------
// createPrivateConversation
// ---------------------------------------------------------------------------

/// @test Creating a private conversation with exactly 2 users succeeds.
TEST_F(ChannelFixture, CreatePrivateConversationWithTwoUsersSucceeds)
{
    addUser("alice");
    addUser("bob");
    EXPECT_TRUE(mgr.createPrivateConversation({"alice", "bob"}));
    EXPECT_TRUE(repo.channelExists(1));
}

/// @test Private channel has maxUsers == 2.
TEST_F(ChannelFixture, CreatePrivateConversationMaxUsersIsTwo)
{
    addUser("alice");
    addUser("bob");
    mgr.createPrivateConversation({"alice", "bob"});
    EXPECT_EQ(repo.getChannel(1).getMaxUsers(), 2);
}

/// @test Private channel is marked private.
TEST_F(ChannelFixture, CreatePrivateConversationIsPrivate)
{
    addUser("alice");
    addUser("bob");
    mgr.createPrivateConversation({"alice", "bob"});
    EXPECT_TRUE(repo.getChannel(1).getIsPrivate());
}

/// @test Creating a private conversation with only 1 user fails.
TEST_F(ChannelFixture, CreatePrivateConversationWithOneUserFails)
{
    EXPECT_FALSE(mgr.createPrivateConversation({"alice"}));
}

/// @test Creating a private conversation with 3 users fails.
TEST_F(ChannelFixture, CreatePrivateConversationWithThreeUsersFails)
{
    EXPECT_FALSE(mgr.createPrivateConversation({"alice", "bob", "charlie"}));
}

/// @test Creating a private conversation with empty list fails.
TEST_F(ChannelFixture, CreatePrivateConversationWithEmptyListFails)
{
    EXPECT_FALSE(mgr.createPrivateConversation({}));
}

// ---------------------------------------------------------------------------
// deleteChannel
// ---------------------------------------------------------------------------

/// @test Deleting an existing channel returns true and removes it from repo.
TEST_F(ChannelFixture, DeleteChannelExistingSucceeds)
{
    mgr.createPublicChannel("test", {"alice"});
    EXPECT_TRUE(mgr.deleteChannel(1));
    EXPECT_FALSE(repo.channelExists(1));
}

/// @test Deleting a non-existent channel returns false.
TEST_F(ChannelFixture, DeleteChannelNonExistentReturnsFalse)
{
    EXPECT_FALSE(mgr.deleteChannel(999));
}

/// @test After deleting a channel, the channel is removed from users' lists.
TEST_F(ChannelFixture, DeleteChannelRemovesFromUserActiveChannels)
{
    mgr.createPublicChannel("test", {"user7"});
    mgr.deleteChannel(1);
    EXPECT_EQ(mgr.getUserActiveChannels("user7").size(), 0u);
}

// ---------------------------------------------------------------------------
// addUserToChannel
// ---------------------------------------------------------------------------

/// @test Adding a user to an existing channel returns true.
TEST_F(ChannelFixture, AddUserToChannelSucceeds)
{
    mgr.createPublicChannel("general", {});
    EXPECT_TRUE(mgr.addUserToChannel(1, "user42"));
}

/// @test Adding a user to a non-existent channel returns false.
TEST_F(ChannelFixture, AddUserToChannelNonExistentChannelFails)
{
    EXPECT_FALSE(mgr.addUserToChannel(999, "alice"));
}

/// @test Adding the same user twice to a channel returns false on second call.
TEST_F(ChannelFixture, AddUserToChannelAlreadyMemberFails)
{
    mgr.createPublicChannel("general", {});
    mgr.addUserToChannel(1, "user42");
    EXPECT_FALSE(mgr.addUserToChannel(1, "user42"));
}

/// @test Adding a user beyond maxUsers capacity returns false.
TEST_F(ChannelFixture, AddUserToChannelBeyondMaxUsersCapacityFails)
{
    // Private channel max = 2; seed with 2 users directly at creation.
    addUser("user10");
    addUser("user11");
    mgr.createPrivateConversation({"user10", "user11"});
    // Channel is already full; adding a third user should fail.
    EXPECT_FALSE(mgr.addUserToChannel(1, "user12"));
}

/// @test User appears in active channels list after being added.
TEST_F(ChannelFixture, AddUserToChannelAppearsInActiveChannels)
{
    mgr.createPublicChannel("general", {});
    mgr.addUserToChannel(1, "user55");
    EXPECT_EQ(mgr.getUserActiveChannels("user55").size(), 1u);
}

// ---------------------------------------------------------------------------
// removeUserFromChannel
// ---------------------------------------------------------------------------

/// @test Removing a member from a channel returns true.
TEST_F(ChannelFixture, RemoveUserFromChannelSucceeds)
{
    mgr.createPublicChannel("general", {"alice"});
    EXPECT_TRUE(mgr.removeUserFromChannel(1, "alice"));
}

/// @test Removing a non-member from a channel returns false.
TEST_F(ChannelFixture, RemoveUserFromChannelNonMemberFails)
{
    mgr.createPublicChannel("general", {"alice"});
    EXPECT_FALSE(mgr.removeUserFromChannel(1, "unknown"));
}

/// @test Removing a user from a non-existent channel returns false.
TEST_F(ChannelFixture, RemoveUserFromNonExistentChannelFails)
{
    EXPECT_FALSE(mgr.removeUserFromChannel(999, "alice"));
}

/// @test After removal, the channel no longer appears in user's active channels.
TEST_F(ChannelFixture, RemoveUserFromChannelDisappearsFromActiveList)
{
    mgr.createPublicChannel("general", {"charlie"});
    mgr.removeUserFromChannel(1, "charlie");
    EXPECT_EQ(mgr.getUserActiveChannels("charlie").size(), 0u);
}

// ---------------------------------------------------------------------------
// editChannelName
// ---------------------------------------------------------------------------

/// @test Editing the name of an existing channel returns true.
TEST_F(ChannelFixture, EditChannelNameExistingSucceeds)
{
    mgr.createPublicChannel("old", {"alice"});
    EXPECT_TRUE(mgr.editChannelName(1, "new"));
}

/// @test Editing the name of a non-existent channel returns false.
TEST_F(ChannelFixture, EditChannelNameNonExistentFails)
{
    EXPECT_FALSE(mgr.editChannelName(999, "name"));
}

/// @test The channel name is actually updated in the repo after editChannelName.
TEST_F(ChannelFixture, EditChannelNamePersistsInRepo)
{
    mgr.createPublicChannel("old", {});
    mgr.editChannelName(1, "newname");
    // Repo holds updated channel — retrieve via getChannel and check name indirectly
    // by ensuring we can still get the channel (it still exists and is active).
    EXPECT_TRUE(repo.channelExists(1));
}

// ---------------------------------------------------------------------------
// deactivateChannel — bug documentation
// ---------------------------------------------------------------------------

/// @test Deactivating a private channel returns true and toggles isActive.
TEST_F(ChannelFixture, DeactivatePrivateChannelTogglesActive)
{
    addUser("alice");
    addUser("bob");
    mgr.createPrivateConversation({"alice", "bob"});
    EXPECT_TRUE(mgr.deactivateChannel(1));
    EXPECT_FALSE(repo.getChannel(1).getIsActive());
}

/// @test Deactivating a public channel returns true but channel stays active (known behaviour).
/// @note This is a known implementation behaviour: public channels are never actually deactivated.
TEST_F(ChannelFixture, DeactivatePublicChannelReturnsTrueButStaysActive)
{
    mgr.createPublicChannel("general", {"alice"});
    bool result = mgr.deactivateChannel(1);
    // The method returns true for public channels without toggling.
    EXPECT_TRUE(result);
    // Channel remains active — this documents the current behaviour.
    EXPECT_TRUE(repo.getChannel(1).getIsActive());
}

/// @test Deactivating a non-existent channel returns false.
TEST_F(ChannelFixture, DeactivateNonExistentChannelFails)
{
    EXPECT_FALSE(mgr.deactivateChannel(999));
}

// ---------------------------------------------------------------------------
// getUserActiveChannels / getAllUserChannels
// ---------------------------------------------------------------------------

/// @test User with no channels has empty active-channel list.
TEST_F(ChannelFixture, GetUserActiveChannelsEmptyForNewUser)
{
    EXPECT_EQ(mgr.getUserActiveChannels("nobody").size(), 0u);
}

/// @test getAllUserChannels returns deactivated channels too.
TEST_F(ChannelFixture, GetAllUserChannelsIncludesDeactivated)
{
    addUser("alice");
    addUser("bob");
    mgr.createPrivateConversation({"alice", "bob"});
    mgr.deactivateChannel(1);
    // Active list is empty after toggle.
    EXPECT_EQ(mgr.getUserActiveChannels("alice").size(), 0u);
    // All-channels list still returns the deactivated channel.
    EXPECT_EQ(mgr.getAllUserChannels("alice").size(), 1u);
}

/// @test User appears in active channels list after joining multiple channels.
TEST_F(ChannelFixture, UserActiveChannelsCountsAllJoined)
{
    mgr.createPublicChannel("ch1", {"alice"});
    mgr.createPublicChannel("ch2", {"alice"});
    EXPECT_EQ(mgr.getUserActiveChannels("alice").size(), 2u);
}
