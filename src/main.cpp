#include <array>
#include <cstdlib>
#include <expected>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class lex_error {};

enum class lex_types { WORD, INT, FLOAT, SYMBOL, MULTI_CHARACTER_SYMBOL };

auto multi_character_symbols =
    std::to_array({"+=", "*="}); // sort by longest -> shortest in length

class token {
public:
  enum lex_types type;
  std::string value;

  token(enum lex_types type, std::string value) : type(type), value(value) {}
};

std::expected<std::vector<token>, lex_error>
lex_code(const std::string &buffer) {
  std::vector<token> result;

  for (unsigned int i = 0; i < buffer.length(); ++i) {
    enum lex_types type;
    std::string value = "";

    if (buffer.substr(i, 2) == "//") {
      while (i != buffer.length() && buffer[i] != '\n')
        ++i;
      continue;
    }

    if (buffer.substr(i, 2) == "/*") {
      while (i != buffer.length() && buffer.substr(i, 2) != "*/")
        ++i;
      continue;
    }

    if ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.') {
      while (i != buffer.length() &&
             ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.')) {
        value += buffer[i];
        ++i;
      }
      if (value != ".") { // so that "." will go continue to the symbol
                          // definition section
        type = lex_types::INT;
        for (const char c : value)
          if (c == '.')
            type = lex_types::FLOAT;

        result.push_back({type, value});
        continue;
      }
    }

    if ((buffer[i] >= '0' && buffer[i] <= '9') ||
        (buffer[i] >= 'A' && buffer[i] <= 'Z') ||
        (buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] == '_')) {
      while (i != buffer.length() &&
             ((buffer[i] >= '0' && buffer[i] <= '9') ||
              (buffer[i] >= 'A' && buffer[i] <= 'Z') ||
              (buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] == '_'))) {
        value += buffer[i];
        ++i;
      }

      type = lex_types::WORD;
      result.push_back({type, value});
      continue;
    }

    bool found_matching_multi_character_symbol = false;
    for (std::string n : multi_character_symbols) {
      if (n == buffer.substr(i, n.length())) {
        type = lex_types::MULTI_CHARACTER_SYMBOL;
        value = n;
        result.push_back({type, value});
        found_matching_multi_character_symbol = true;
        break;
      }
    }

    if (found_matching_multi_character_symbol)
      continue;

    type = lex_types::SYMBOL;
    value += buffer[i];
    result.push_back({type, value});
  }

  return result;
}

class Node {
public:
  std::unique_ptr<Node> back;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;

  Node(std::unique_ptr<Node> back, std::unique_ptr<Node> left,
       std::unique_ptr<Node> right)
      : back(std::move(back)), left(std::move(left)), right(std::move(right)) {}
};

enum class parse_error {};

std::expected<std::unique_ptr<Node>, parse_error>
parse(const std::vector<token> &buffer_lex) {
  std::unique_ptr<Node> root =
      std::make_unique<Node>(nullptr, nullptr, nullptr);
  for (int i = 0; i < buffer_lex.size(); ++i) {
    const token &current = buffer_lex[i];
  }

  return std::move(root);
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cout << "ERR: You must provide a file as your first CLI argument!\n";
    return EXIT_FAILURE;
  }

  std::ifstream file(argv[1]);
  std::string buffer{std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>()};
  std::vector<token> lexed_code;
  auto buffer_lex = lex_code(buffer);
  if (!buffer_lex.has_value()) {
    std::cout << "ERR: lexing interrupted by error: " << (int)buffer_lex.error()
              << '\n';
    return EXIT_FAILURE;
  }

  auto tree_root = parse(*buffer_lex);
  if (!tree_root.has_value()) {
    std::cout << "ERR: parsing interrupted by error: "
              << (int)buffer_lex.error() << '\n';
    return EXIT_FAILURE;
  }

  // evaluate starting from *tree_root node

  return EXIT_SUCCESS;
}
