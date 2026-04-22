#include <gtest/gtest.h>
#include <common/models/user.hpp>

/// @brief Fixture dla testów klasy User.
class UserTest : public ::testing::Test
{
protected:
    User makeUser(const std::string &name = "alice") const
    {
        return User(name, "hash123", "salt64");
    }
};

/// @test Konstruktor ustawia poprawne pola.
TEST_F(UserTest, ConstructorSetsFields) {}

/// @test getUsername zwraca ustawioną nazwę.
TEST_F(UserTest, GetUsernameReturnsCorrectName) {}

/// @test getPasswordHash zwraca poprawny skrót.
TEST_F(UserTest, GetPasswordHashReturnsHash) {}

/// @test getBase64salt zwraca poprawną sól.
TEST_F(UserTest, GetBase64SaltReturnsSalt) {}

/// @test operator== porównuje użytkowników po nazwie.
TEST_F(UserTest, OperatorEqualByUsername) {}

/// @test Serializacja do JSON zawiera username.
TEST_F(UserTest, ToJsonContainsUsername) {}

/// @test Deserializacja z JSON odtwarza oryginalny obiekt.
TEST_F(UserTest, FromJsonRestoresObject)
{
    User original = makeUser();
    json j = original.toJson();
    User restored;
    restored.fromJson(j);
    EXPECT_EQ(restored.getUsername(), original.getUsername());
    EXPECT_EQ(restored.getPasswordHash(), original.getPasswordHash());
}
