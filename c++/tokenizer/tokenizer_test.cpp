#include "tokenizer.h"
#include <gtest/gtest.h>
#include <sstream>
#include "../error.h"

class TokenizerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TokenizerTest, ParseNumber) {
    std::stringstream ss("123");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<ConstantToken>(token));
    EXPECT_EQ(std::get<ConstantToken>(token).value, 123);
    EXPECT_FALSE(tokenizer.IsEnd());
}

TEST_F(TokenizerTest, ParseIdentifier) {
    std::stringstream ss("variable_name");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<SymbolToken>(token));
    EXPECT_EQ(std::get<SymbolToken>(token).name, "variable_name");
    EXPECT_FALSE(tokenizer.IsEnd());
}

TEST_F(TokenizerTest, ParseKeywords) {
    std::stringstream ss("def return if then else");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::DEF);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::RETURN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::IF);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::THEN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::ELSE);
}

TEST_F(TokenizerTest, ParseOperators) {
    std::stringstream ss("+ - * / == != < =");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::PLUS);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::MINUS);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::MULTIPLY);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::DIVIDE);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::EQ_EQ);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::NOT_EQ);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::LESS);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::EQ);
}

TEST_F(TokenizerTest, ParseParenthesesAndComma) {
    std::stringstream ss("( ) ,");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::LPAREN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::RPAREN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::COMMA);
}

TEST_F(TokenizerTest, ParseNewline) {
    std::stringstream ss("\n");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::NEWLINE);
}

TEST_F(TokenizerTest, ParseEOF) {
    std::stringstream ss("");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::EOFT);
    EXPECT_TRUE(tokenizer.IsEnd());
}

TEST_F(TokenizerTest, ParseComplexExpression) {
    std::stringstream ss("def foo(a, b)\n    return a + b * 2");
    Tokenizer tokenizer(&ss);
    
    auto token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::DEF);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<SymbolToken>(token));
    EXPECT_EQ(std::get<SymbolToken>(token).name, "foo");
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::LPAREN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<SymbolToken>(token));
    EXPECT_EQ(std::get<SymbolToken>(token).name, "a");
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::COMMA);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<SymbolToken>(token));
    EXPECT_EQ(std::get<SymbolToken>(token).name, "b");
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<EmbracingToken>(token));
    EXPECT_EQ(std::get<EmbracingToken>(token), EmbracingToken::RPAREN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::NEWLINE);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<UtilityTokens>(token));
    EXPECT_EQ(std::get<UtilityTokens>(token), UtilityTokens::RETURN);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<SymbolToken>(token));
    EXPECT_EQ(std::get<SymbolToken>(token).name, "a");
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::PLUS);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<SymbolToken>(token));
    EXPECT_EQ(std::get<SymbolToken>(token).name, "b");
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<OperatorToken>(token));
    EXPECT_EQ(std::get<OperatorToken>(token), OperatorToken::MULTIPLY);
    
    tokenizer.Next();
    token = tokenizer.GetToken();
    ASSERT_TRUE(std::holds_alternative<ConstantToken>(token));
    EXPECT_EQ(std::get<ConstantToken>(token).value, 2);
}

TEST_F(TokenizerTest, ParseErrorCases) {
    {
        std::stringstream ss("@");
        EXPECT_THROW(Tokenizer tokenizer(&ss), SyntaxError);
    }
    
    {
        std::stringstream ss("\"unterminated");
        EXPECT_THROW(Tokenizer tokenizer(&ss), SyntaxError);
    }
}