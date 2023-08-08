#include "parser.h"

Parser::Parser() { std::cout << "Created HTTP parser!!!\n"; }

bool Parser::parse(const std::string &data) {
  this->data = std::move(data);
  this->idx = 0;
  this->parserState = State::start;
  return parseInput();
}

bool Parser::parseInput() {

  while (idx < data.length()) {
    auto c = data[idx];
    std::cout << c << std::endl;

    switch (parserState) {

    case State::start:
      if (isAlpha(c)) {
        transitionState(State::scheme_or_noscheme_or_rootless);
      } else if (pChar(c)) {
        transitionState(State::noscheme_or_rootless);
      } else if (match('/', c)) {
        transitionState(State::slash_slash_or_path);
      } else if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
      break;

    case State::noscheme_or_rootless:
      if (pChar(c) || pCharNoColon(c)) {
        transitionState(State::noscheme_or_rootless);
      } else if (match('/', c)) {
        transitionState(State::slash_slash_or_path);
      } else {
        setError();
      }
      break;

    case State::scheme_or_noscheme_or_rootless:
      if ((isAlphaDigit(c) || match('+', c) || match('-', c) ||
           match('.', c))) {
        transitionState(State::scheme_or_noscheme_or_rootless);
      } else if (match(':', c)) {
        transitionState(State::scheme_colon_or_rootless);
      } else {
        setError();
      }
      break;

    case State::scheme_colon_or_rootless:
      if (match('/', c)) {
        transitionState(State::slash_slash_or_path);
      } else if (pChar(c)) {
        transitionState(State::noscheme_or_rootless);
      } else if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
      break;

    case State::slash_slash_or_path:
      if (match('/', c)) {
        transitionState(State::authority_start);
      } else if (pChar(c)) {
        transitionState(State::path_segment);
      } else {
        setError();
      }
      break;

    case State::authority_start:
      if (isUnreserved(c) || subDelims(c) || match(':', c) || isEscaped(c)) {
        transitionState(State::authority_user_info);
      } else if (match('[', c)) {
        transitionState(State::host);
      } else {
        setError();
      }
      break;

    case State::authority_user_info:
      if (isUnreserved(c) || subDelims(c) || match(':', c) || isEscaped(c)) {
        transitionState(State::authority_user_info);
      } else if (match('@', c)) {
        transitionState(State::host);
      } else {
        setError();
      }
      break;

    case State::host:
      if (match(']', c)) {
        transitionState(State::port);
      }
      // eat everything in here for now...
      break;

    case State::port:
      if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else if (isDigit(c) || match(':', c)) {
        transitionState(State::port);
      } else if (match('/', c)) {
        transitionState(State::path_segment_slash);
      } else {
        setError();
      }
      break;

    case State::path_segment_slash:
      if (pChar(c)) {
        transitionState(State::path_segment);
      } else {
        setError();
      }
      break;

    case State::path_segment:
      if (pChar(c)) {
        transitionState(State::path_segment);
      } else if (match('/', c)) {
        transitionState(State::path_segment_slash);
      } else if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
      break;

    case State::query:
      if (match('?', c) || match('/', c) || pChar(c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
      break;

    case State::fragment:
      if (match('?', c) || match('/', c) || pChar(c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
    default:
      break;
    }

    if (error.idx != -1) {
      std::cout << "had error at index: " << error.idx << std::endl;
    }
    idx++;
  }
  return error.err == State::start;
}