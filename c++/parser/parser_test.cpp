#include <gtest/gtest.h>
#include <sstream>
#include "parser.h"

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ParserTest, ParseNumber) {
    std::stringstream ss("x = 42\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<NumberExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->value, 42);
}

TEST_F(ParserTest, ParseIdentifier) {
    std::stringstream ss("x = x\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<VariableExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->name, "x");
}

TEST_F(ParserTest, ParseBinaryOperation) {
    std::stringstream ss("x = 1 + 2\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<BinaryExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->op, OperatorToken::PLUS);
    auto left = dynamic_cast<NumberExpr*>(expr->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value, 1);
    auto right = dynamic_cast<NumberExpr*>(expr->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->value, 2);
}

TEST_F(ParserTest, ParseTernaryExpression) {
    std::stringstream ss("x = if 1 then 2 else 3\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<TernaryExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    auto cond = dynamic_cast<NumberExpr*>(expr->cond.get());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->value, 1);
    auto then_expr = dynamic_cast<NumberExpr*>(expr->then_expr.get());
    ASSERT_NE(then_expr, nullptr);
    EXPECT_EQ(then_expr->value, 2);
    auto else_expr = dynamic_cast<NumberExpr*>(expr->else_expr.get());
    ASSERT_NE(else_expr, nullptr);
    EXPECT_EQ(else_expr->value, 3);
}

TEST_F(ParserTest, ParseFunctionDefinition) {
    std::stringstream ss("def add(x, y)\n    x = x + y\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto func = dynamic_cast<FunctionDef*>(program->statements[0].get());
    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->name, "add");
    ASSERT_EQ(func->params.size(), 2);
    EXPECT_EQ(func->params[0], "x");
    EXPECT_EQ(func->params[1], "y");
    auto body = dynamic_cast<Assignment*>(func->body.get());
    ASSERT_NE(body, nullptr);
    EXPECT_EQ(body->name, "x");
    auto expr = dynamic_cast<BinaryExpr*>(body->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->op, OperatorToken::PLUS);
    auto left = dynamic_cast<VariableExpr*>(expr->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->name, "x");
    auto right = dynamic_cast<VariableExpr*>(expr->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->name, "y");
}

TEST_F(ParserTest, ParseAssignment) {
    std::stringstream ss("x = 42\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<NumberExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->value, 42);
}

TEST_F(ParserTest, ParseFunctionCall) {
    std::stringstream ss("x = add(1, 2)\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<CallExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->callee, "add");
    ASSERT_EQ(expr->args.size(), 2);
    auto arg1 = dynamic_cast<NumberExpr*>(expr->args[0].get());
    ASSERT_NE(arg1, nullptr);
    EXPECT_EQ(arg1->value, 1);
    auto arg2 = dynamic_cast<NumberExpr*>(expr->args[1].get());
    ASSERT_NE(arg2, nullptr);
    EXPECT_EQ(arg2->value, 2);
}

TEST_F(ParserTest, ParseComplexExpression) {
    std::stringstream ss("x = if x < 10 then x + 1 else x - 1\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name, "x");
    auto expr = dynamic_cast<TernaryExpr*>(stmt->value.get());
    ASSERT_NE(expr, nullptr);
    auto cond = dynamic_cast<BinaryExpr*>(expr->cond.get());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->op, OperatorToken::LESS);
    auto then_expr = dynamic_cast<BinaryExpr*>(expr->then_expr.get());
    ASSERT_NE(then_expr, nullptr);
    EXPECT_EQ(then_expr->op, OperatorToken::PLUS);
    auto else_expr = dynamic_cast<BinaryExpr*>(expr->else_expr.get());
    ASSERT_NE(else_expr, nullptr);
    EXPECT_EQ(else_expr->op, OperatorToken::MINUS);
}

TEST_F(ParserTest, ParseNestedFunction) {
    std::stringstream ss("def outer(x)\n    def inner(y)\n        x = x + y\n");
    Parser parser(&ss);
    auto program = parser.Parse();
    ASSERT_EQ(program->statements.size(), 1);
    auto outer = dynamic_cast<FunctionDef*>(program->statements[0].get());
    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->name, "outer");
    ASSERT_EQ(outer->params.size(), 1);
    EXPECT_EQ(outer->params[0], "x");
    auto inner = dynamic_cast<FunctionDef*>(outer->body.get());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->name, "inner");
    ASSERT_EQ(inner->params.size(), 1);
    EXPECT_EQ(inner->params[0], "y");
    auto body = dynamic_cast<Assignment*>(inner->body.get());
    ASSERT_NE(body, nullptr);
    EXPECT_EQ(body->name, "x");
    auto expr = dynamic_cast<BinaryExpr*>(body->value.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->op, OperatorToken::PLUS);
    auto left = dynamic_cast<VariableExpr*>(expr->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->name, "x");
    auto right = dynamic_cast<VariableExpr*>(expr->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->name, "y");
} 