#include "tokenizer.h"
#include "../error.h"
#include <cctype>
#include <sstream>
#include <variant>
#include <type_traits>
#include <utility>
#include <stdexcept>

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return std::holds_alternative<UtilityTokens>(current_token_) && 
           std::get<UtilityTokens>(current_token_) == UtilityTokens::EOFT;
}

void Tokenizer::Next() {
    char c;
    while (in_->get(c) && std::isspace(c)) {
        if (c == '\n') {
            current_token_ = UtilityTokens::NEWLINE;
            return;
        }
    }
    
    if (in_->eof()) {
        current_token_ = UtilityTokens::EOFT;
        return;
    }
    
    if (std::isdigit(c)) {
        int value = c - '0';
        while (in_->get(c) && std::isdigit(c)) {
            value = value * 10 + (c - '0');
        }
        if (in_->good()) {
            in_->unget();
        }
        current_token_ = ConstantToken{value};
        return;
    }
    
    if (std::isalpha(c)) {
        std::string name;
        name += c;
        while (in_->get(c) && (std::isalnum(c) || c == '_')) {
            name += c;
        }
        if (in_->good()) {
            in_->unget();
        }
        
        if (name == "def") {
            current_token_ = UtilityTokens::DEF;
        } else if (name == "return") {
            current_token_ = UtilityTokens::RETURN;
        } else if (name == "if") {
            current_token_ = EmbracingToken::IF;
        } else if (name == "then") {
            current_token_ = EmbracingToken::THEN;
        } else if (name == "else") {
            current_token_ = EmbracingToken::ELSE;
        } else {
            current_token_ = SymbolToken{name};
        }
        return;
    }
    
    switch (c) {
        case '(':
            current_token_ = EmbracingToken::LPAREN;
            break;
        case ')':
            current_token_ = EmbracingToken::RPAREN;
            break;
        case ',':
            current_token_ = EmbracingToken::COMMA;
            break;
        case '+':
            current_token_ = OperatorToken::PLUS;
            break;
        case '-':
            current_token_ = OperatorToken::MINUS;
            break;
        case '*':
            current_token_ = OperatorToken::MULTIPLY;
            break;
        case '/':
            current_token_ = OperatorToken::DIVIDE;
            break;
        case '=':
            if (in_->get(c) && c == '=') {
                current_token_ = OperatorToken::EQ_EQ;
            } else {
                if (in_->good()) {
                    in_->unget();
                }
                current_token_ = OperatorToken::EQ;
            }
            break;
        case '!':
            if (in_->get(c) && c == '=') {
                current_token_ = OperatorToken::NOT_EQ;
            } else {
                if (in_->good()) {
                    in_->unget();
                }
                throw SyntaxError("Unexpected character after '!'");
            }
            break;
        case '<':
            current_token_ = OperatorToken::LESS;
            break;
        default:
            throw SyntaxError("Unexpected character: " + std::string(1, c));
    }
}

Token Tokenizer::GetToken() {
    return current_token_;
}