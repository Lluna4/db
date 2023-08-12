#include <gtest/gtest.h>
#include "tokenize.hpp"

TEST(VectorTest, Vectorize)
{
    std::vector<std::string> res = {"hola", "buenas"};
    std::vector<std::string> tokens = tokenize("hola,,,,,                     ,,,,,,,,,,,,,\"buenas\"", ',');
    EXPECT_EQ(res, tokens);
}
TEST(VectorTest, Test2)
{
    std::vector<std::string> res = {"hello world"};
    std::vector<std::string> tokens = tokenize("\"hello world\"");
    EXPECT_EQ(res, tokens);
}

TEST(VectorTest, Test3)
{
    std::vector<std::string> res = {"apple", "banana", "orange"};
    std::vector<std::string> tokens = tokenize("apple, banana, orange", ',');
    EXPECT_EQ(res, tokens);
}
