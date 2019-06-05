/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Bruno Schmitt, Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "../ast_context.hpp"
#include "../ast_node.hpp"
#include "../ast_node_kinds.hpp"

#include <memory>
#include <string>

namespace tweedledee {
namespace qasm {

  enum class binary_ops {
    add,
    sub,
    div,
    mult,
    pow,
    eq,
  };

  enum class unary_ops {
    sin,
    cos,
    tan,
    exp,
    ln,
    sqrt,
    minus,
    plus,
  };

  /*! \brief Base class of expressions */
  class expr : public ast_node {
  };


  /*! \brief Binary expressions */
  class expr_binary_op : public expr {
  public:

    static expr_binary_op* create(ast_context* ctx, uint32_t loc, expr* left, binary_ops op, expr* right)
    {
      return new (*ctx) expr_unary_op(loc, left, op, right);
    }

    binary_ops get_op() const
    {
      return op_;
    }

    bool is(binary_ops op) const
    {
      return op_ == op;
    }

    binary_ops get_op() 

    expr& left() const
    {
      return *(left_);
    }

    expr& right() const
    {
      return *(right_);
    }

  protected:
    binary_ops op_;
    expr* left_;
    expr* right_;

    expr_binary_op(uint32_t loc,
                   expr* left,
                   binary_ops op,
                   expr* right)
      : expr(loc)
      , left_(left)
      , op_(op)
      , right_(right)
    {}

	ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::expr_binary_op;
	}
  };


  /*! \brief Unary expressions */
  class expr_unary_op : public expr {
  public:

    static expr_unary_op* create(ast_context* ctx, uint32_t loc, unary_ops op, expr* subexpr)
    {
      return new (*ctx) expr_unary_op(loc, op, subexpr);
    }

	unary_ops op() const
	{
		return op_;
	}

	bool is(unary_ops op) const
	{
	  return op_ == op;
	}

    expr& subexpr() const
    {
      return *(subexpr_);
    }

  protected:
    unary_ops op_;
    std::shared_ptr<expr> subexpr_;

    expr_unary_op(uint32_t loc,
                   unary_ops op,
                   expr* subexpr)
      : expr(loc)
      , op_(op)
      , subexpr_(subexpr)
    {}
    
	ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::expr_unary_op;
	}
  };


  /*! \brief PI */
  class expr_pi final : public expr {
  public:
    static expr_pi* create(ast_context* ctx, uint32_t loc)
    {
      return new (*ctx) expr_pi(loc);
    }

    double evaluate() const
    {
      return M_PI;
    }

    double value() const
    {
      return M_PI;
    }

  protected:
    expr_pi(uint32_t loc)
      : expr(loc)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::expr_pi;
	}
  };


  /*! \brief Integer literal */
  class expr_integer final : public expr {

  public:
    static expr_integer* create(ast_context* ctx, uint32_t loc, int32_t value)
    {
      return new (*ctx) expr_integer(loc, value);
    }

    int32_t evaluate() const
    {
      return value_;
    }

  protected:
	int32_t value_;

    expr_integer(uint32_t loc, int32_t value)
      : expr(loc)
      , value_(value)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::expr_integer;
    }

  };


  /*! \brief Real-valued literal */
  class expr_real final : public expr {
  public:
    static expr_real* create(ast_context* ctx, uint32_t loc, double value)
    {
      return new (*ctx) expr_real(loc, value);
    }

    double evaluate() const
    {
      return value_;
    }

    double value() const
    {
      return value_;
    }

  protected:
	double value_;

    expr_real(uint32_t loc, double value)
      : expr(loc)
      , value_(value)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::expr_real;
    }

  };


  /* \brief Variable expressions */
  class expr_var final : public expr {
  public:
    static expr_var* create(ast_context* ctx, uint32_t loc, std::string_view id)
    {
      return new (*ctx) expr_var(loc, id);
    }

    std::string_view id() const
    {
      return id_;
    }

  protected:
    std::string_view id_;

    expr_var(uint32_t loc, std::string_view id)
      : expr(loc)
      , id_(id)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::expr_var;
    }

  };

  class expr_var_offset final : public expr_var {
  public:
    static expr_var_offset* create(ast_context* ctx, uint32_t loc,
                                   std::string_view id, uint32_t offset)
    {
      return new (*ctx) expr_var_offset(loc, id, offset);
    }

    uint32_t offset() const
    {
      return offset_;
    }

  protected:
    uint32_t offset_;

    expr_var_offset(uint32_t loc, std::string_view id, uint32_t offset)
      : expr_var(loc, id)
      , offset_(offset)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::expr_var_offset;
    }

  };

} // namespace qasm
} // namespace tweedledee
