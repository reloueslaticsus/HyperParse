#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>

typedef unsigned long long ull;

enum class State {

  trash_state = 0,
  req_start,
  // request type
  req_start_G,
  req_start_GE,
  req_start_GET,
  req_start_P,
  req_start_PU,
  req_start_PUT,
  req_start_PO,
  req_start_POS,
  req_start_POST,

  space,

  // uri
  req_uri_start,
  req_uri_scheme_start,
  req_uri_scheme_colon,
  req_uri_relative_path,
  req_uri_relative_noscheme_or_empty,
  req_uri_relative_path_slash,
  req_uri_rootless_or_empty,
  req_uri_rootless_or_empty_path_slash,
  req_uri_slash_slash_or_relative_path,
  req_uri_authority_info,
  req_uri_port,
  req_uri_query,
  req_uri_fragment,
  req_uri_path_segment_slash,
  req_uri_path_segment,

  // http version
  req_http_version_H,
  req_http_verison_T,
  req_http_version_T,
  req_http_version_P,
  req_http_version_slash,
  req_http_version_major,
  req_http_version_dot,
  req_http_version_minor,

};

struct Error {
  State err;
  long long idx;
};

struct URIInfo {
  ull scheme_index = -1;
  ull authority_index = -1;
  ull path_index = -1;
};

class Parser {
public:
  Parser();
  void parse(const std::string &data);
  void printState() {
    switch (parserState) {
    case State::req_uri_start:
      std::cout << "start\n";
      break;
    case State::req_uri_scheme_start:
      std::cout << "schema start\n";
      break;
    case State::req_uri_relative_path:
      std::cout << "relative path\n";
      break;
    case State::req_uri_relative_path_slash:
      std::cout << "relative path slash\n";
      break;
    case State::req_uri_relative_noscheme_or_empty:
      std::cout << "relative no scheme or empty\n";
      break;
    case State::req_uri_slash_slash_or_relative_path:
      std::cout << "slash slash or relative path\n";
      break;
    case State::req_uri_rootless_or_empty:
      std::cout << "rootless or empty\n";
      break;
    case State::req_uri_rootless_or_empty_path_slash:
      std::cout << "rootless or empty path slash\n";
      break;
    case State::req_uri_fragment:
      std::cout << "fragment\n";
      break;
    case State::req_uri_query:
      std::cout << "query\n";
      break;
    case State::req_uri_authority_info:
      std::cout << "authority info\n";
      break;
    case State::req_uri_path_segment:
      std::cout << "path segment\n";
      break;
    case State::req_uri_path_segment_slash:
      std::cout << "path segment slash\n";
      break;
    case State::req_uri_port:
      std::cout << "port\n";
      break;
    case State::req_uri_scheme_colon:
      std::cout << "scheme colon\n";
      break;
    default:
      std::cout << "unkown state\n";
      break;
    }
  }

private:
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

  inline void transitionState(State nextState) { parserState = nextState; }

  inline void setError() {
    error.err = parserState;
    error.idx = idx;
  }

  void parseInput();

  std::string data;
  Error error{State::req_start, -1};
  State parserState;
  ull idx;
};

#endif