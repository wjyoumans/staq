#include "ast/decl.hpp"

using namespace synthewareQ;

int main() {
  auto loc = ast::Location(0);
  auto e1 = ast::Expr_int::create(loc, 5);
  auto e2 = ast::Expr_pi::create(loc);
  auto e3 = ast::Expr_bop::create(loc, e1, ast::bop::add, e2);
  auto e4 = ast::Expr_bop::create(loc, e3, ast::bop::add, ast::Expr_real::create(loc, 1.0));

  delete e4;
}
