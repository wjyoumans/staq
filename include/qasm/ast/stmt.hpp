/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "../ast_context.hpp"
#include "../ast_node.hpp"
#include "../ast_node_kinds.hpp"

#include <memory>
#include <string>

namespace tweedledee {
namespace qasm {

  /*! \brief Statement base class */
  class stmt : public ast_node {
  public:
    stmt* create(ast_context* ctx, uint32_t location)
    {
      return new (*ctx) stmt(location);
    }

  protected:
    stmt(uint32_t location)
      : ast_node(location)
    {}

  };


  /*! \brief Conditional statement */
  class stmt_if : public stmt {
  public:
    stmt_if* create(ast_context* ctx, uint32_t location, expr* cond, stmt* then)
    {
      return new (*ctx) stmt_if(location, cond, then);
    }

    expr& cond()
    {
      return *(cond_);
    }

    stmt& then()
    {
      return *(then_);
    }

  protected:
    expr* cond_;
    stmt* then_;

    stmt_if(uint32_t location, expr* cond, stmt* then)
      : ast_node(location)
      , cond_(cond)
      , then_(then)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_if;
	}
  };


  /*! \brief Measurement statement */
  class stmt_measure : public stmt {
  public:
    stmt_measure* create(ast_context* ctx, uint32_t location, expr_var* src, expr_var* dest)
    {
      return new (*ctx) stmt_measure(location, src, dest);
    }

    expr& source()
    {
      return *(src_);
    }

    stmt& dest()
    {
      return *(dest_);
    }

  protected:
    expr_var* src_;
    expr_var* dest_;

    stmt_measure(uint32_t location, expr_var* src, expr_var* dest)
      : ast_node(location)
      , src_(src)
      , dest_(dest)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_measure;
	}
  };


  /*! \brief Reset statement */
  class stmt_reset : public stmt {
  public:
    stmt_reset* create(ast_context* ctx, uint32_t location, expr_var* arg)
    {
      return new (*ctx) stmt_reset(location, arg);
    }

    expr& arg()
    {
      return *(arg_);
    }

  protected:
    expr_var* arg_;

    stmt_reset(uint32_t location, expr_var* arg)
      : ast_node(location)
      , arg_(arg)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_reset;
	}
  };


  /*! \brief Unitary sub-class */
  class stmt_unitary : public stmt {
  public:
    stmt_unitary* create(ast_context* ctx, uint32_t location)
    {
      return new (*ctx) stmt_unitary(location);
    }

  protected:
    stmt_unitary(uint32_t location)
      : stmt(location)
    {}

  };


  /*! \brief Barrier */
  class stmt_barrier : public stmt_unitary {
  public:
    stmt_barrier* create(ast_context* ctx, uint32_t location, vector<expr_var*>& args)
    {
      return new (*ctx) stmt_barrier(location);
    }

    vector<expr_var*>& args const
    {
      return args_;
    }

  protected:
    vector<expr_var*> args_;
    
    stmt_barrier(uint32_t location, vector<expr_var*>& args)
      : stmt_unitary(location)
      , args_(args)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_barrier;
	}
  };


  /*! \brief Single-qubit unitary */
  class stmt_u : public stmt_unitary {
  public:
    stmt_u* create(ast_context* ctx, uint32_t location,
                   expr* theta, expr* phi, expr* lambda, expr_var* arg)
    {
      return new (*ctx) stmt_u(location, theta, phi, lambda, arg);
    }

    expr& theta const
    {
      return *(theta_);
    }

    expr& phi const
    {
      return *(phi_);
    }

    expr& lambda const
    {
      return *(lambda_);
    }

    expr_var& arg const
    {
      return *(arg_);
    }

  protected:
    expr* theta_;
    expr* phi_;
    expr* lambda_;
    expr_var* arg_;
    
    stmt_u(uint32_t location, expr* theta, expr* phi, expr* lambda, expr_var* arg)
      : stmt_unitary(location)
      , theta_(theta)
      , phi_(phi)
      , lambda_(lambda)
      , arg_(arg)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_unitary;
	}
  };


  /*! \brief CNOT gate */
  class stmt_cnot : public stmt_unitary {
  public:
    stmt_cnot* create(ast_context* ctx, uint32_t location,
                   expr_var* ctrl, expr_var* tgt)
    {
      return new (*ctx) stmt_cnot(location, ctrl, tgt);
    }

    expr_var& control const
    {
      return *(ctrl_);
    }

    expr_var& target const
    {
      return *(tgt_);
    }

  protected:
    expr_var* ctrl_;
    expr_var* tgt_;
    
    stmt_cnot(uint32_t location, expr_var* ctrl, expr_var* tgt)
      : stmt_unitary(location)
      , ctrl_(ctrl)
      , tgt_(tgt)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_cnot;
	}
  };


  /*! \brief Defined gate */
  class stmt_gate : public stmt_unitary {
  public:
    stmt_gate* create(ast_context* ctx, uint32_t location, vector<expr*>& cargs, vector<expr_var*>& qargs)
    {
      return new (*ctx) stmt_gate(location, cargs, qargs);
    }

    vector<expr*>& cargs const
    {
      return cargs_;
    }

    vector<expr_var*>& qargs const
    {
      return qargs_;
    }

  protected:
    vector<expr*> cargs_;
    vector<expr_var*> qargs;
    
    stmt_gate(uint32_t location, vector<expr*>& cargs, vector<expr_var*>& qargs)
      : stmt_unitary(location)
      , cargs_(cargs)
      , qargs_(qargs)
    {}

    ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::stmt_gate;
	}
  };

} // namespace qasm
} // namespace tweedledee
