#include "ast/expr.hpp"

using namespace synthewareQ;

int main() {
  auto loc = ast::Location(0);
  auto pi = ast::Expr_pi::create(loc);
}
