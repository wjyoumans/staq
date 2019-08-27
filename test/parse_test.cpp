#include <iostream>
#include <string>
#include <memory>
#include <optional>

#include "parser/position.h"
#include "ast/ast.h"
#include "parser/parser.h"
#include "tools/ast_printer.h"
#include "tools/resource_estimator.h"

using namespace synthewareQ::parser;
using namespace synthewareQ::ast;

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cerr << "Input file not specified.\n";
    return -1;
  }

  auto prog = parse_file(argv[1]);
  std::cout << *prog;

  std::cout << "\nAST:\n";
  synthewareQ::tools::print_tree(*prog);

  std::cout << "\nResource estimates:\n";
  auto count = synthewareQ::tools::estimate_resources(*prog);
  for (auto& [name, num] : count) {
    std::cout << "  " << name << ": " << num << "\n";
  }

  return 1;
}
