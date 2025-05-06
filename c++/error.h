#ifndef TOY_LANG_ERROR
#define TOY_LANG_ERROR

#include <stdexcept>
#include <string>
#include <variant>
#include <type_traits>
#include <utility>

class SyntaxError : public std::runtime_error {
public:
    explicit SyntaxError(const std::string& message) : std::runtime_error(message) {}
};

struct RuntimeError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct NameError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

#endif