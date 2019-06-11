/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast/ast.hpp"

namespace synthewareQ {
namespace ast {

  // Header-only makes c++ forward decls a nightmare
  class Translation_unit;
  class Type_register;
  class Type_qubit;
  class Type_cbit;
  class Expr_bop;
  class Expr_uop;
  class Expr_pi;
  class Expr_int;
  class Expr_real;
  class Expr_var;
  class Expr_offset;
  class Stmt_if;
  class Stmt_measure;
  class Stmt_reset;
  class Stmt_decl;
  class Stmt_barrier;
  class Stmt_unitary;
  class Stmt_cnot;
  class Stmt_gate;
  class Decl_var;
  class Decl_gate;
  class Decl_opaque;
  class Decl_oracle;
  

  /*! \brief Base visitor interface 
   *  
   *  Classic visitor style via (virtual) double dispatch.
   *  Traversal to sub-nodes is handled by the particular visitor, not the
   *  node class
   */
  class Visitor {
  public:
    virtual void visit(Translation_unit *) = 0;
    // Types
    virtual void visit(Type_register *) = 0;
    virtual void visit(Type_qubit *) = 0;
    virtual void visit(Type_cbit *) = 0;
    // Expressions
    virtual void visit(Expr_bop *) = 0;
    virtual void visit(Expr_uop *) = 0;
    virtual void visit(Expr_pi *) = 0;
    virtual void visit(Expr_int *) = 0;
    virtual void visit(Expr_real *) = 0;
    // Access paths
    virtual void visit(Expr_var *) = 0;
    virtual void visit(Expr_offset *) = 0;
    // Statements
    virtual void visit(Stmt_if *) = 0;
    virtual void visit(Stmt_measure *) = 0;
    virtual void visit(Stmt_reset *) = 0;
    virtual void visit(Stmt_decl *) = 0;
    virtual void visit(Stmt_barrier *) = 0;
    virtual void visit(Stmt_unitary *) = 0;
    virtual void visit(Stmt_cnot *) = 0;
    virtual void visit(Stmt_gate *) = 0;
    // Declarations
    virtual void visit(Decl_var *) = 0;
    virtual void visit(Decl_gate *) = 0;
    virtual void visit(Decl_opaque *) = 0;
    virtual void visit(Decl_oracle *) = 0;
  };

}
}
