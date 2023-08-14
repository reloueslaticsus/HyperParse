#include "parser.h"

Parser::Parser() { std::cout << "Created HTTP parser!!!\n"; }

void Parser::parse(const std::string &data) {
  this->data = std::move(data);
  this->idx = 0;
  this->parserState = State::req_start;
  parseInput();
}

void Parser::parseInput() {

  while (idx < data.length()) {
    auto c = data[idx];
    std::cout << c;

    switch (parserState) {

    case State::req_start:
      if (c == 'G') {
        transitionState(State::req_start_G);
      } else if (c == 'P') {
        transitionState(State::req_start_P);
      } else {
        setError();
      }
      break;

    case State::req_start_G:
      if (c == 'E') {
        transitionState(State::req_start_GE);
      } else {
        setError();
      }
      break;

    case State::req_start_GE:
      if (c == 'T') {
        transitionState(State::req_start_GET);
      } else {
        setError();
      }
      break;

    case State::req_start_P:
      if (c == 'U') {
        transitionState(State::req_start_PU);
      } else if (c == 'O') {
        transitionState(State::req_start_PO);
      } else {
        setError();
      }
      break;

    case State::req_start_PU:
      if (c == 'T') {
        transitionState(State::req_start_PUT);
      } else {
        setError();
      }
      break;

    case State::req_start_PO:
      if (c == 'S') {
        transitionState(State::req_start_POS);
      } else {
        setError();
      }
      break;

    case State::req_start_POS:
      if (c == 'T') {
        transitionState(State::req_start_POST);
      } else {
        setError();
      }
      break;

    case State::req_start_GET:
    case State::req_start_POST:
    case State::req_start_PUT:
    case State::space:
      if (c == ' ') {
        transitionState(State::space);
      } else {
        transitionState(State::req_uri_start);
      }
      break;

    case State::req_uri_start:
      if (isAlpha(c)) {
        transitionState(State::req_uri_scheme_start);
      } else if (pCharNoColon(c)) {
        transitionState(State::req_uri_relative_noscheme_or_empty);
      } else if (c == '/') {
        transitionState(State::req_uri_relative_path);
      } else if (c == '?') {
        transitionState(State::req_uri_query);
      } else if (c == '#') {
        transitionState(State::req_uri_fragment);
      } else {
        setError();
      }
      break;

    case State::req_uri_relative_noscheme_or_empty:
      if (pCharNoColon(c)) {
        transitionState(State::req_uri_relative_noscheme_or_empty);
      } else if (c == '/') {
        transitionState(State::req_uri_relative_path_slash);
      } else {
        setError();
      }
      break;

    case State::req_uri_rootless_or_empty:
      if (pChar(c)) {
        transitionState(State::req_uri_rootless_or_empty);
      } else if (c == '/') {
        transitionState(State::req_uri_rootless_or_empty_path_slash);
      } else {
        setError();
      }
      break;

    case State::req_uri_scheme_start:
      if ((isAlphaDigit(c) || c == '+' || c == '-' || c == '.')) {
        transitionState(State::req_uri_scheme_start);
      } else if (c == ':') {
        info.scheme_start_index = 0;
        info.scheme_end_index = idx - 1;
        transitionState(State::req_uri_scheme_colon);
      } else if (pCharNoColon(c)) {
        transitionState(State::req_uri_relative_noscheme_or_empty);
      } else if (c == '/') {
        transitionState(State::req_uri_slash_slash_or_relative_path);
      } else {
        setError();
      }
      break;

    case State::req_uri_scheme_colon:
      if (c == '/') {
        transitionState(State::req_uri_slash_slash_or_relative_path);
      } else if (pChar(c)) {
        transitionState(State::req_uri_rootless_or_empty);
      } else if (c == '?') {
        transitionState(State::req_uri_query);
      } else if (c == '#') {
        transitionState(State::req_uri_fragment);
      } else {
        setError();
      }
      break;

    case State::req_uri_relative_path:
    case State::req_uri_slash_slash_or_relative_path:
      if (c == '/') {
        authority_start_index = idx + 1;
        transitionState(State::req_uri_authority_info);
      } else if (pChar(c)) {
        transitionState(State::req_uri_path_segment);
      } else {
        setError();
      }
      break;

    case State::req_uri_authority_info:
      // we let the user of the parser further parse out authority
      if (isUnreserved(c) || subDelims(c) || isEscaped(c) || c == ':' ||
          c == '@' || c == '[' || c == ']') {
        transitionState(State::req_uri_authority_info);
      } else if (c == '?') {
        authority_end_index = idx - 1;
        transitionState(State::req_uri_query);
      } else if (c == '#') {
        authority_end_index = idx - 1;
        transitionState(State::req_uri_fragment);
      } else if (c == '/') {
        authority_end_index = idx - 1;
        transitionState(State::req_uri_path_segment_slash);
      } else {
        setError();
      }
      break;

    case State::req_uri_relative_path_slash:
    case State::req_uri_path_segment_slash:
    case State::req_uri_rootless_or_empty_path_slash:
      if (pChar(c)) {
        transitionState(State::req_uri_path_segment);
      } else {
        setError();
      }
      break;

    case State::req_uri_path_segment:
      if (pChar(c)) {
        transitionState(State::req_uri_path_segment);
      } else if (c == '/') {
        transitionState(State::req_uri_path_segment_slash);
      } else if (c == '?') {
        transitionState(State::req_uri_query);
      } else if (c == '#') {
        transitionState(State::req_uri_fragment);
      } else {
        setError();
      }
      break;

    case State::req_uri_query:
      if (c == '?' || c == '/' || pChar(c)) {
        transitionState(State::req_uri_query);
      } else if (c == '#') {
        transitionState(State::req_uri_fragment);
      } else {
        setError();
      }
      break;

    case State::req_uri_fragment:
      if (c == '?' || c == '/' || pChar(c)) {
        transitionState(State::req_uri_fragment);
      } else {
        setError();
      }
    default:
      break;
    }

    if (error.idx != -1) {
      std::cout << "had error at index: " << error.idx << std::endl;
      std::cout << "Error in state: ";
      printState();
      break;
    }

    idx++;
  }
}