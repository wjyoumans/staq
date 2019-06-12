#include "ast/ast.hpp"
#include "ast/visitors/generic.hpp"

#include "parser/lexer.hpp"

#include <iostream>

using namespace std;
using namespace synthewareQ::ast;

class Expr_printer : public Node_visitor {
  public:
    void visit(Expr_bop * node) {
      cout << "(";
      node->get_left()->accept(*this);
      cout << "_";
      node->get_right()->accept(*this);
      cout << ")";
    }

    void visit(Expr_uop * node) {
      cout << "(_";
      node->get_subexpr()->accept(*this);
    }

    void visit(Expr_pi * node) {
      cout << "pi";
    }

    void visit(Expr_int * node) {
      cout << node->numeric_value();
    }

    void visit(Expr_real * node) {
      cout << node->numeric_value();
    }
};



int main() {
  Expr_printer pretty;

  auto loc = Location(0);
  auto e1 = Expr_int::create(loc, 5);
  auto e2 = Expr_pi::create(loc);
  auto e3 = Expr_bop::create(loc, e1, bop::add, e2);
  auto e4 = Expr_bop::create(loc, e3, bop::add, Expr_real::create(loc, 1.0));

  e4->accept(pretty);

  delete e4;
}
