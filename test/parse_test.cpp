#include <iostream>
#include <string>

#include "parser/position.h"
#include "parser/token.h"
#include "parser/lexer.h"
#include "parser/preprocessor.h"

using namespace synthewareQ::parser;

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cerr << "Input file not specified.\n";
    return -1;
  }

  Preprocessor pp;
  pp.add_target_file(argv[1]);
  pp.print_all_tokens();
  return 1;
}
