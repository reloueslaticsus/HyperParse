#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>

typedef unsigned long long ull;

enum class State {
  start = 0,

  scheme_or_noscheme_or_rootless,
  noscheme_or_rootless,
  scheme_colon_or_rootless,
  slash,
  slash_slash_or_path,
  authority_start,
  authority_user_info,
  host,
  port,
  query,
  fragment,
  path_segment_slash,
  path_segment

};

struct Error {
  State err;
  long long idx;
};

class Parser {
public:
  Parser();
  bool parse(const std::string &data);

private:
  inline bool match(const char &f, const char &s) { return f == s; }

  inline bool isLowerAlpha(const char &f) { return f >= 97 && f <= 122; }

  inline bool isUpperAlpha(const char &f) { return f >= 65 && f <= 90; }

  inline bool isDigit(const char &f) { return f >= 48 && f <= 57; }

  inline bool isAlpha(const char &f) {
    return isLowerAlpha(f) || isUpperAlpha(f);
  }

  inline bool isAlphaDigit(const char &f) { return isAlpha(f) || isDigit(f); }

  inline bool isHex(const char &f) {
    return isDigit(f) || (f >= 97 && f <= 102) || (f >= 65 && f <= 70);
  }

  inline bool isUnreserved(const char &f) {
    return f == '-' || f == '_' || f == '.' || f == '~' || isAlphaDigit(f);
  }

  inline bool isReserved(const char &f) {
    return f == ';' || f == '/' || f == '?' || f == ':' || f == '@' ||
           f == '&' || f == '=' || f == '+' || f == '$' || f == ',';
  }

  inline bool isEscaped(const char &f) {
    return f == '%' && isHex(data[idx + 1]) && isHex(data[idx + 2]);
  }

  inline bool uric(const char &f) {
    return isReserved(f) || isUnreserved(f) || isEscaped(f);
  }

  inline bool uricNoSlash(const char &f) {
    return isUnreserved(f) || isEscaped(f) || f == ';' || f == '?' ||
           f == ':' || f == '@' || f == '&' || f == '=' || f == '+' ||
           f == '$' || f == ',';
  }

  inline bool subDelims(const char &f) {
    return f == '!' || f == '$' || f == '&' || f == '\'' || f == '(' ||
           f == ')' || f == '*' || f == '+' || f == ',' || f == ';' || f == '=';
  }

  inline bool pChar(const char &f) {
    return isUnreserved(f) || isEscaped(f) || subDelims(f) || f == ':' ||
           f == '@';
  }

  inline bool pCharNoColon(const char &f) { return pChar(f) && f != ':'; }

  void printState() {
    switch (parserState) {
    case State::start:
      std::cout << "start\n";
      break;
    case State::scheme_colon_or_rootless:
      std::cout << "schema colon or rootless\n";
      break;
    case State::scheme_or_noscheme_or_rootless:
      std::cout << "scheme, noscheme, or rootless\n";
      break;
    case State::fragment:
      std::cout << "fragment\n";
      break;
    case State::query:
      std::cout << "query\n";
      break;
    case State::host:
      std::cout << "host\n";
      break;
    case State::slash:
      std::cout << "slash\n";
      break;
    case State::slash_slash_or_path:
      std::cout << "slash slash or path\n";
      break;
    case State::authority_start:
      std::cout << "authority start\n";
      break;
    case State::authority_user_info:
      std::cout << "authority user info\n";
      break;
    case State::path_segment:
      std::cout << "path segment\n";
      break;
    case State::path_segment_slash:
      std::cout << "path segment slash\n";
      break;
    case State::port:
      std::cout << "port\n";
      break;
    case State::noscheme_or_rootless:
      std::cout << "noscheme or rootless\n";
      break;
    default:
      std::cout << "unkown state\n";
      break;
    }
  }

  void transitionState(State s2) {
    parserState = s2;
    printState();
  }

  inline void setError() {
    error.err = parserState;
    error.idx = idx;
  }

  bool parseInput();

  std::string data;
  Error error{State::start, -1};
  State parserState;
  ull idx;
};

#endif