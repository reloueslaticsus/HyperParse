#include "parser.h"

int main(int argc, char **argv) {
  Parser parser;
  if (parser.parse(argv[1])) {
    std::cout << "is correct" << std::endl;
  } else {
    std::cout << "is incorrect" << std::endl;
  }
}