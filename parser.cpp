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
        transitionState(State::scheme_start);
      } else if (pCharNoColon(c)) {
        transitionState(State::relative_noscheme_or_empty);
      } else if (match('/', c)) {
        transitionState(State::relative_path);
      } else if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
      break;

    case State::relative_noscheme_or_empty:
      if (pCharNoColon(c)) {
        transitionState(State::relative_noscheme_or_empty);
      } else if (match('/', c)) {
        transitionState(State::relative_path_slash);
      } else {
        setError();
      }
      break;

    case State::rootless_or_empty:
      if (pChar(c)) {
        transitionState(State::rootless_or_empty);
      } else if (match('/', c)) {
        transitionState(State::rootless_or_empty_path_slash);
      } else {
        setError();
      }
      break;

    case State::scheme_start:
      if ((isAlphaDigit(c) || match('+', c) || match('-', c) ||
           match('.', c))) {
        transitionState(State::scheme_start);
      } else if (match(':', c)) {
        transitionState(State::scheme_colon);
      } else if (pCharNoColon(c)) {
        transitionState(State::relative_noscheme_or_empty);
      } else if (match('/', c)) {
        transitionState(State::slash_slash_or_relative_path);
      } else {
        setError();
      }
      break;

    case State::scheme_colon:
      if (match('/', c)) {
        transitionState(State::slash_slash_or_relative_path);
      } else if (pChar(c)) {
        transitionState(State::rootless_or_empty);
      } else if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else {
        setError();
      }
      break;

    case State::relative_path:
    case State::slash_slash_or_relative_path:
      if (match('/', c)) {
        transitionState(State::authority_info);
      } else if (pChar(c)) {
        transitionState(State::path_segment);
      } else {
        setError();
      }
      break;

    case State::authority_info:
      // eat everything for now, but this should be broken out.
      if (isUnreserved(c) || subDelims(c) || isEscaped(c) || match(':', c) ||
          match('@', c) || match('[', c) || match(']', c)) {
        transitionState(State::authority_info);
      } else if (match('?', c)) {
        transitionState(State::query);
      } else if (match('#', c)) {
        transitionState(State::fragment);
      } else if (match('/', c)) {
        transitionState(State::path_segment_slash);
      } else {
        setError();
      }
      break;

    case State::relative_path_slash:
    case State::path_segment_slash:
    case State::rootless_or_empty_path_slash:
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