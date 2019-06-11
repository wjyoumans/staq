/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast/ast.hpp"
#include "ast/visitor.hpp"

namespace synthewareQ {
namespace ast {

  /*! \brief Pass-through visitor
   *
   *  Touches every nodes in the tree by default  
   *  Usage: override visitors for just the nodes
   *  desired. Note that visitation for children
   *  of overriden visitors is not automatic
   */
  class Node_visitor : public Visitor {
  public:
    void visit(Translation_unit * node) {
      for (auto& stmt : *(node->get_body())) {
        stmt.accept(*this);
      }
    }

    // Types
    void visit(Type_register * node) {
      node->get_base()->accept(*this);
    }
    void visit(Type_qubit * node) {}
    void visit(Type_cbit * node) {}

    // Expressions
    void visit(Expr_bop * node) {
      node->get_left()->accept(*this);
      node->get_right()->accept(*this);
    }
    void visit(Expr_uop * node) {
      node->get_subexpr()->accept(*this);
    }
    void visit(Expr_pi * node) {}
    void visit(Expr_int * node) {}
    void visit(Expr_real * node) {}
    // Access paths
    void visit(Expr_var * node) {
      node->get_type()->accept(*this);
    }
    void visit(Expr_offset * node) {
      node->get_var()->accept(*this);
    }

    // Statements
    void visit(Stmt_if * node) {
      node->get_cond()->accept(*this);
      node->get_then()->accept(*this);
    }
    void visit(Stmt_measure * node) {
      node->get_src()->accept(*this);
      node->get_dest()->accept(*this);
    }
    void visit(Stmt_reset * node) {
      node->get_arg()->accept(*this);
    }
    void visit(Stmt_decl * node) {
      node->get_decl()->accept(*this);
    }
    void visit(Stmt_barrier * node) {
      for (auto& ap : *(node->get_args())) {
        ap.accept(*this);
      }
    }
    void visit(Stmt_unitary * node) {
      node->get_theta()->accept(*this);
      node->get_phi()->accept(*this);
      node->get_lambda()->accept(*this);
      node->get_arg()->accept(*this);
    }
    void visit(Stmt_cnot * node) {
      node->get_control()->accept(*this);
      node->get_target()->accept(*this);
    }
    void visit(Stmt_gate * node) {
      for (auto& expr : *(node->get_cargs())) {
        expr.accept(*this);
      }
      for (auto& ap : *(node->get_qargs())) {
        ap.accept(*this);
      }
    }
    
    // Declarations
    void visit(Decl_var * node) {
      node->get_type()->accept(*this);
    }
    void visit(Decl_gate * node) {
      for (auto& decl : *(node->get_params())) {
        decl.accept(*this);
      }
      for (auto& decl : *(node->get_formals())) {
        decl.accept(*this);
      }
      for (auto& stmt : *(node->get_body())) {
        stmt.accept(*this);
      }
    }
    void visit(Decl_opaque * node) {
      for (auto& decl : *(node->get_params())) {
        decl.accept(*this);
      }
      for (auto& decl : *(node->get_formals())) {
        decl.accept(*this);
      }
    }
    void visit(Decl_oracle * node) {
      for (auto& decl : *(node->get_params())) {
        decl.accept(*this);
      }
      for (auto& decl : *(node->get_formals())) {
        decl.accept(*this);
      }
    }
  };

}
}
