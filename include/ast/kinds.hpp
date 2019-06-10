/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>
#include <string>

/*! \brief Provides tags for AST nodes to support RTTI */

namespace synthewareQ {
namespace ast {

  enum class ast_nodes {
    // Basic translation unit
    trans_unit,
    // Types
    type_register,
    type_qbit,
    type_cbit,
    // Statements
    stmt_if,
    stmt_measure,
    stmt_reset,
    stmt_decl,
    stmt_list,
    // Unitary statements
    stmt_barrier,
    stmt_unitary,
    stmt_cnot,
    stmt_gate,
    // Declarations
    decl_var,
    decl_gate,
    decl_opaque,
    decl_oracle,
    // Expressions
    expr_bop,
    expr_uop,
    expr_pi,
    expr_int,
    expr_real,
    // Access path expressions
    expr_var,
    expr_offset,
  };

}
}
