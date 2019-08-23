#include <iostream>
#include <string>
#include <memory>
#include <optional>

#include "parser/position.h"
#include "ast/ast.h"

using namespace synthewareQ::parser;
using namespace synthewareQ::ast;

int main(int argc, char** argv) {

  /*
  if (argc < 2) {
    std::cerr << "Input file not specified.\n";
    return -1;
  }

  Preprocessor pp;
  pp.add_target_file(argv[1]);
  pp.print_all_tokens();
  */

  Position pos;

  // AST
  auto exp1 = std::make_unique<BExpr>(BExpr(pos, 
                        ptr<Expr>(new IntExpr(pos, 2)),
                        BinaryOp::Plus,
                        ptr<Expr>(new VarExpr(pos, "x"))));
  auto exp2 = new BExpr(pos,
                        std::move(exp1),
                        BinaryOp::Plus,
                        ptr<Expr>(new PiExpr(pos)));

  std::cout << *exp2 << "\n";
  return 1;
}
