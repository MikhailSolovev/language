#ifndef TOY_LANG_AST
#define TOY_LANG_AST

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <type_traits>
#include <utility>
#include "../tokenizer/tokenizer.h"

class Expression {
public:
    virtual ~Expression() = default;
};

class Statement {
public:
    virtual ~Statement() = default;
};

class Program {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

class NumberExpr : public Expression {
public:
    explicit NumberExpr(int value) : value(value) {}
    int value;
};

class VariableExpr : public Expression {
public:
    explicit VariableExpr(std::string name) : name(std::move(name)) {}
    std::string name;
};

class BinaryExpr : public Expression {
public:
    BinaryExpr(OperatorToken op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    OperatorToken op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class CallExpr : public Expression {
public:
    CallExpr(std::string callee, std::vector<std::unique_ptr<Expression>> args)
        : callee(std::move(callee)), args(std::move(args)) {}
    std::string callee;
    std::vector<std::unique_ptr<Expression>> args;
};

class TernaryExpr : public Expression {
public:
    TernaryExpr(std::unique_ptr<Expression> cond, std::unique_ptr<Expression> then_expr, std::unique_ptr<Expression> else_expr)
        : cond(std::move(cond)), then_expr(std::move(then_expr)), else_expr(std::move(else_expr)) {}
    std::unique_ptr<Expression> cond;
    std::unique_ptr<Expression> then_expr;
    std::unique_ptr<Expression> else_expr;
};

class Assignment : public Statement {
public:
    Assignment(std::string name, std::unique_ptr<Expression> value)
        : name(std::move(name)), value(std::move(value)) {}
    std::string name;
    std::unique_ptr<Expression> value;
};

class Return : public Statement {
public:
    explicit Return(std::unique_ptr<Expression> value) : value(std::move(value)) {}
    std::unique_ptr<Expression> value;
};

class FunctionDef : public Statement {
public:
    FunctionDef(std::string name, std::vector<std::string> params, std::unique_ptr<Statement> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<Statement> body;
};

#endif // TOY_LANG_AST 