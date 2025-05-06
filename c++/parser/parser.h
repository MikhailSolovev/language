#ifndef TOY_LANG_PARSER
#define TOY_LANG_PARSER

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <typeinfo>
#include <sstream>
#include <type_traits>
#include <utility>
#include "../tokenizer/tokenizer.h"
#include "../ast/ast.h"

class ExprAST;
class StatementAST;
class FunctionDefAST;
class NumberAST;
class IdentifierAST;
class BinaryOpAST;
class ReturnStmtAST;
class TernaryExprAST;
class FunctionCallAST;
class AssignmentAST;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visitNumberExpr(NumberAST&) = 0;
    virtual void visitIdentifierExpr(IdentifierAST&) = 0;
    virtual void visitBinaryExpr(BinaryOpAST&) = 0;
    virtual void visitTernaryExpr(TernaryExprAST&) = 0;
    virtual void visitFunctionCallExpr(FunctionCallAST&) = 0;
    virtual void visitAssignmentStmt(AssignmentAST&) = 0;
    virtual void visitReturnStmt(ReturnStmtAST&) = 0;
    virtual void visitFunctionDef(FunctionDefAST&) = 0;
};

class NodeAST {
public:
    virtual ~NodeAST() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class ExprAST : public NodeAST {
public:
    virtual ~ExprAST() = default;
};

class StatementAST : public NodeAST {
public:
    virtual ~StatementAST() = default;
};

class NumberAST : public ExprAST {
    int value;
public:
    explicit NumberAST(int value) : value(value) {}
    void accept(Visitor& visitor) override { visitor.visitNumberExpr(*this); }
    int getValue() const { return value; }
};

class IdentifierAST : public ExprAST {
    std::string name;
public:
    explicit IdentifierAST(std::string name) : name(std::move(name)) {}
    void accept(Visitor& visitor) override { visitor.visitIdentifierExpr(*this); }
    const std::string& getName() const { return name; }
};

class BinaryOpAST : public ExprAST {
    std::string op;
    std::unique_ptr<ExprAST> left;
    std::unique_ptr<ExprAST> right;
public:
    BinaryOpAST(std::string op, std::unique_ptr<ExprAST> left, std::unique_ptr<ExprAST> right)
        : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
    void accept(Visitor& visitor) override { visitor.visitBinaryExpr(*this); }
    const std::string& getOp() const { return op; }
    ExprAST* getLeft() const { return left.get(); }
    ExprAST* getRight() const { return right.get(); }
};

class TernaryExprAST : public ExprAST {
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ExprAST> then_expr;
    std::unique_ptr<ExprAST> else_expr;
public:
    TernaryExprAST(std::unique_ptr<ExprAST> condition, 
                   std::unique_ptr<ExprAST> then_expr,
                   std::unique_ptr<ExprAST> else_expr)
        : condition(std::move(condition)), 
          then_expr(std::move(then_expr)), 
          else_expr(std::move(else_expr)) {}
    void accept(Visitor& visitor) override { visitor.visitTernaryExpr(*this); }
    ExprAST* getCondition() const { return condition.get(); }
    ExprAST* getThenExpr() const { return then_expr.get(); }
    ExprAST* getElseExpr() const { return else_expr.get(); }
};

class FunctionCallAST : public ExprAST {
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> arguments;
public:
    FunctionCallAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {}
    void accept(Visitor& visitor) override { visitor.visitFunctionCallExpr(*this); }
    const std::string& getCallee() const { return callee; }
    const std::vector<std::unique_ptr<ExprAST>>& getArguments() const { return arguments; }
};

class AssignmentAST : public StatementAST {
    std::string name;
    std::unique_ptr<ExprAST> value;
public:
    AssignmentAST(std::string name, std::unique_ptr<ExprAST> value)
        : name(std::move(name)), value(std::move(value)) {}
    void accept(Visitor& visitor) override { visitor.visitAssignmentStmt(*this); }
    const std::string& getName() const { return name; }
    ExprAST* getValue() const { return value.get(); }
};

class ReturnStmtAST : public StatementAST {
    std::unique_ptr<ExprAST> return_expr;
public:
    explicit ReturnStmtAST(std::unique_ptr<ExprAST> return_expr)
        : return_expr(std::move(return_expr)) {}
    void accept(Visitor& visitor) override { visitor.visitReturnStmt(*this); }
    ExprAST* getReturnExpr() const { return return_expr.get(); }
};

class FunctionDefAST : public StatementAST {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::unique_ptr<StatementAST>> body;
    std::unique_ptr<ExprAST> return_expr;
public:
    FunctionDefAST(std::string name, std::vector<std::string> params,
                  std::vector<std::unique_ptr<StatementAST>> body,
                  std::unique_ptr<ExprAST> return_expr)
        : name(std::move(name)), params(std::move(params)),
          body(std::move(body)), return_expr(std::move(return_expr)) {}
    void accept(Visitor& visitor) override { visitor.visitFunctionDef(*this); }
    const std::string& getName() const { return name; }
    const std::vector<std::string>& getParams() const { return params; }
    const std::vector<std::unique_ptr<StatementAST>>& getBody() const { return body; }
    ExprAST* getReturnExpr() const { return return_expr.get(); }
};

class Parser {
public:
    explicit Parser(std::istream* in);

    std::unique_ptr<Program> Parse();

private:
    Tokenizer tokenizer_;
    Token current_token_;

    void Next();

    bool match(const Token& expected);

    template<typename T>
    bool match() {
        return std::holds_alternative<T>(current_token_);
    }

    template<typename T>
    T expect() {
        if (!match<T>()) {
            std::stringstream ss;
            ss << "Expected token of type " << typeid(T).name() << " but got ";
            if (std::holds_alternative<SymbolToken>(current_token_)) {
                ss << "SymbolToken(" << std::get<SymbolToken>(current_token_).name << ")";
            } else if (std::holds_alternative<ConstantToken>(current_token_)) {
                ss << "ConstantToken(" << std::get<ConstantToken>(current_token_).value << ")";
            } else if (std::holds_alternative<EmbracingToken>(current_token_)) {
                ss << "EmbracingToken";
            } else if (std::holds_alternative<OperatorToken>(current_token_)) {
                ss << "OperatorToken";
            } else if (std::holds_alternative<UtilityTokens>(current_token_)) {
                ss << "UtilityTokens";
            }
            throw SyntaxError(ss.str());
        }
        return std::get<T>(current_token_);
    }

    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<FunctionDef> parseFunctionDef();
    std::unique_ptr<Assignment> parseAssignment();
    std::unique_ptr<Return> parseReturn();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseTernaryExpr();
    std::unique_ptr<Expression> parseLogicalExpr();
    std::unique_ptr<Expression> parseAddExpr();
    std::unique_ptr<Expression> parseMulExpr();
    std::unique_ptr<Expression> parsePrimary();
    std::unique_ptr<Expression> parseFunctionCall();
};

#endif