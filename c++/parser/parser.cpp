#include "parser.h"
#include "../error.h"
#include "../tokenizer/tokenizer.h"
#include <stdexcept>
#include <sstream>
#include <variant>
#include <type_traits>
#include <utility>

Parser::Parser(std::istream* in) : tokenizer_(in) {
    Next();
}

void Parser::Next() {
    if (!tokenizer_.IsEnd()) {
        tokenizer_.Next();
        current_token_ = tokenizer_.GetToken();
    }
}

bool Parser::match(const Token& expected) {
    if (current_token_.index() != expected.index()) {
        return false;
    }
    
    if (std::holds_alternative<UtilityTokens>(current_token_)) {
        return std::get<UtilityTokens>(current_token_) == std::get<UtilityTokens>(expected);
    }
    if (std::holds_alternative<EmbracingToken>(current_token_)) {
        return std::get<EmbracingToken>(current_token_) == std::get<EmbracingToken>(expected);
    }
    if (std::holds_alternative<OperatorToken>(current_token_)) {
        return std::get<OperatorToken>(current_token_) == std::get<OperatorToken>(expected);
    }
    if (std::holds_alternative<SymbolToken>(current_token_)) {
        return std::get<SymbolToken>(current_token_).name == std::get<SymbolToken>(expected).name;
    }
    if (std::holds_alternative<ConstantToken>(current_token_)) {
        return std::get<ConstantToken>(current_token_).value == std::get<ConstantToken>(expected).value;
    }
    return false;
}

std::unique_ptr<Program> Parser::Parse() {
    auto program = std::make_unique<Program>();
    while (!tokenizer_.IsEnd()) {
        if (std::holds_alternative<UtilityTokens>(current_token_) && 
            std::get<UtilityTokens>(current_token_) == UtilityTokens::NEWLINE) {
            Next();
            continue;
        }
        program->statements.push_back(parseStatement());
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (std::holds_alternative<UtilityTokens>(current_token_) && 
        std::get<UtilityTokens>(current_token_) == UtilityTokens::DEF) {
        return parseFunctionDef();
    }
    if (std::holds_alternative<UtilityTokens>(current_token_) && 
        std::get<UtilityTokens>(current_token_) == UtilityTokens::RETURN) {
        return parseReturn();
    }
    return parseAssignment();
}

std::unique_ptr<FunctionDef> Parser::parseFunctionDef() {
    if (!std::holds_alternative<UtilityTokens>(current_token_) || 
        std::get<UtilityTokens>(current_token_) != UtilityTokens::DEF) {
        throw SyntaxError("Expected 'def' keyword");
    }
    Next();
    
    if (!std::holds_alternative<SymbolToken>(current_token_)) {
        throw SyntaxError("Expected function name");
    }
    auto name = std::get<SymbolToken>(current_token_).name;
    Next();
    
    if (!std::holds_alternative<EmbracingToken>(current_token_) || 
        std::get<EmbracingToken>(current_token_) != EmbracingToken::LPAREN) {
        throw SyntaxError("Expected '(' after function name");
    }
    Next();
    
    std::vector<std::string> params;
    if (!std::holds_alternative<EmbracingToken>(current_token_) || 
        std::get<EmbracingToken>(current_token_) != EmbracingToken::RPAREN) {
        do {
            if (!std::holds_alternative<SymbolToken>(current_token_)) {
                throw SyntaxError("Expected parameter name");
            }
            params.push_back(std::get<SymbolToken>(current_token_).name);
            Next();
            
            if (std::holds_alternative<EmbracingToken>(current_token_) && 
                std::get<EmbracingToken>(current_token_) == EmbracingToken::RPAREN) {
                break;
            }
            
            if (!std::holds_alternative<EmbracingToken>(current_token_) || 
                std::get<EmbracingToken>(current_token_) != EmbracingToken::COMMA) {
                throw SyntaxError("Expected ',' or ')' after parameter");
            }
            Next();
        } while (true);
    }
    
    if (!std::holds_alternative<EmbracingToken>(current_token_) || 
        std::get<EmbracingToken>(current_token_) != EmbracingToken::RPAREN) {
        throw SyntaxError("Expected ')' after parameters");
    }
    Next();
    
    if (std::holds_alternative<UtilityTokens>(current_token_) && 
        std::get<UtilityTokens>(current_token_) == UtilityTokens::NEWLINE) {
        Next();
    }
    
    auto body = parseStatement();
    
    return std::make_unique<FunctionDef>(name, std::move(params), std::move(body));
}

std::unique_ptr<Assignment> Parser::parseAssignment() {
    if (!std::holds_alternative<SymbolToken>(current_token_)) {
        throw SyntaxError("Expected variable name");
    }
    auto name = std::get<SymbolToken>(current_token_).name;
    Next();
    
    if (!std::holds_alternative<OperatorToken>(current_token_) || 
        std::get<OperatorToken>(current_token_) != OperatorToken::EQ) {
        throw SyntaxError("Expected '=' after variable name");
    }
    Next();
    
    auto value = parseExpression();
    
    if (std::holds_alternative<UtilityTokens>(current_token_) && 
        std::get<UtilityTokens>(current_token_) == UtilityTokens::NEWLINE) {
        Next();
    }
    
    return std::make_unique<Assignment>(name, std::move(value));
}

std::unique_ptr<Return> Parser::parseReturn() {
    if (!std::holds_alternative<UtilityTokens>(current_token_) || 
        std::get<UtilityTokens>(current_token_) != UtilityTokens::RETURN) {
        throw SyntaxError("Expected 'return' keyword");
    }
    Next();
    
    auto value = parseExpression();
    
    if (std::holds_alternative<UtilityTokens>(current_token_) && 
        std::get<UtilityTokens>(current_token_) == UtilityTokens::NEWLINE) {
        Next();
    }
    
    return std::make_unique<Return>(std::move(value));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseTernaryExpr();
}

std::unique_ptr<Expression> Parser::parseTernaryExpr() {
    if (std::holds_alternative<EmbracingToken>(current_token_) && 
        std::get<EmbracingToken>(current_token_) == EmbracingToken::IF) {
        Next();
        
        auto cond = parseLogicalExpr();
        
        if (!std::holds_alternative<EmbracingToken>(current_token_) || 
            std::get<EmbracingToken>(current_token_) != EmbracingToken::THEN) {
            throw SyntaxError("Expected 'then' after condition");
        }
        Next();
        
        auto then_expr = parseExpression();
        
        if (!std::holds_alternative<EmbracingToken>(current_token_) || 
            std::get<EmbracingToken>(current_token_) != EmbracingToken::ELSE) {
            throw SyntaxError("Expected 'else' after then expression");
        }
        Next();
        
        auto else_expr = parseExpression();
        
        return std::make_unique<TernaryExpr>(std::move(cond), std::move(then_expr), std::move(else_expr));
    }
    
    return parseLogicalExpr();
}

std::unique_ptr<Expression> Parser::parseLogicalExpr() {
    auto expr = parseAddExpr();
    
    while (std::holds_alternative<OperatorToken>(current_token_)) {
        auto op = std::get<OperatorToken>(current_token_);
        if (op != OperatorToken::EQ_EQ && op != OperatorToken::NOT_EQ && op != OperatorToken::LESS) {
            break;
        }
        Next();
        
        auto right = parseAddExpr();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseAddExpr() {
    auto expr = parseMulExpr();
    
    while (std::holds_alternative<OperatorToken>(current_token_)) {
        auto op = std::get<OperatorToken>(current_token_);
        if (op != OperatorToken::PLUS && op != OperatorToken::MINUS) {
            break;
        }
        Next();
        
        auto right = parseMulExpr();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseMulExpr() {
    auto expr = parsePrimary();
    
    while (std::holds_alternative<OperatorToken>(current_token_)) {
        auto op = std::get<OperatorToken>(current_token_);
        if (op != OperatorToken::MULTIPLY && op != OperatorToken::DIVIDE) {
            break;
        }
        Next();
        
        auto right = parsePrimary();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (std::holds_alternative<ConstantToken>(current_token_)) {
        auto value = std::get<ConstantToken>(current_token_).value;
        Next();
        return std::make_unique<NumberExpr>(value);
    }
    
    if (std::holds_alternative<SymbolToken>(current_token_)) {
        auto name = std::get<SymbolToken>(current_token_).name;
        Next();
        
        if (std::holds_alternative<EmbracingToken>(current_token_) && 
            std::get<EmbracingToken>(current_token_) == EmbracingToken::LPAREN) {
            Next();
            std::vector<std::unique_ptr<Expression>> args;
            if (!std::holds_alternative<EmbracingToken>(current_token_) || 
                std::get<EmbracingToken>(current_token_) != EmbracingToken::RPAREN) {
                do {
                    args.push_back(parseExpression());
                    
                    if (std::holds_alternative<EmbracingToken>(current_token_) && 
                        std::get<EmbracingToken>(current_token_) == EmbracingToken::RPAREN) {
                        break;
                    }
                    
                    if (!std::holds_alternative<EmbracingToken>(current_token_) || 
                        std::get<EmbracingToken>(current_token_) != EmbracingToken::COMMA) {
                        throw SyntaxError("Expected ',' or ')' after argument");
                    }
                    Next();
                } while (true);
            }
            
            if (!std::holds_alternative<EmbracingToken>(current_token_) || 
                std::get<EmbracingToken>(current_token_) != EmbracingToken::RPAREN) {
                throw SyntaxError("Expected ')' after arguments");
            }
            Next();
            
            return std::make_unique<CallExpr>(name, std::move(args));
        }
        
        return std::make_unique<VariableExpr>(name);
    }
    
    if (std::holds_alternative<EmbracingToken>(current_token_) && 
        std::get<EmbracingToken>(current_token_) == EmbracingToken::LPAREN) {
        Next();
        auto expr = parseExpression();
        if (!std::holds_alternative<EmbracingToken>(current_token_) || 
            std::get<EmbracingToken>(current_token_) != EmbracingToken::RPAREN) {
            throw SyntaxError("Expected ')' after expression");
        }
        Next();
        return expr;
    }
    
    throw SyntaxError("Unexpected token in primary expression");
} 