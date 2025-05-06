#ifndef TOY_LANG_TOKENIZER
#define TOY_LANG_TOKENIZER

#include <istream>
#include <string>
#include <variant>
#include <memory>
#include <type_traits>
#include <utility>
#include "../error.h"

struct SymbolToken {
  std::string name;
  bool operator==(const SymbolToken& other) const { return name == other.name; }
};

struct ConstantToken {
  int value;
  bool operator==(const ConstantToken& other) const { return value == other.value; }
};

enum class EmbracingToken {
    LPAREN,
    RPAREN,
    COMMA,
    IF,
    THEN,
    ELSE
};

enum class OperatorToken {
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  EQ_EQ,
  NOT_EQ,
  LESS,
  EQ
};

enum class UtilityTokens {
    DEF,
    RETURN,
    NEWLINE,
    EOFT
};

using Token = std::variant<
    SymbolToken,
    ConstantToken,
    EmbracingToken,
    OperatorToken,
    UtilityTokens
>;

class Tokenizer {
public:
  explicit Tokenizer(std::istream* in);

  bool IsEnd();

  void Next();

  Token GetToken();

private:
  std::istream* in_;
  Token current_token_;
};

#endif // TOY_LANG_TOKENIZER